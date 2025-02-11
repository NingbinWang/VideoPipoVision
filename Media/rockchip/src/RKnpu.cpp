#include "RKnpu.h"
#include "MppEncoder.h"
#include "Logger.h"

RKrga *rga = nullptr;
RKnpu::RKnpu()
{
  rga = new RKrga();
}

RKnpu::~RKnpu() 
{
	
}


void RKnpu::dump_tensor_attr(rknn_tensor_attr *attr)
{
  LOG_DEBUG("  index=%d, name=%s, n_dims=%d, dims=[%d, %d, %d, %d], n_elems=%d, size=%d, fmt=%s, type=%s, qnt_type=%s, "
         "zp=%d, scale=%f\n",
         attr->index, attr->name, attr->n_dims, attr->dims[0], attr->dims[1], attr->dims[2], attr->dims[3],
         attr->n_elems, attr->size, get_format_string(attr->fmt), get_type_string(attr->type),
         get_qnt_type_string(attr->qnt_type), attr->zp, attr->scale);
}

double RKnpu::__get_us(struct timeval t) { return (t.tv_sec * 1000000 + t.tv_usec); }


bool RKnpu::Init_Model(unsigned char *model_data,int model_data_size,RknnConText_T* app_ctx)
{
   int ret;
   rknn_context ctx;
   rknn_sdk_version version;
   /* Create the neural network */
   LOG_DEBUG("Loading mode...\n");
   if (model_data == NULL)
   {
      LOG_ERROR("not found model data\n");
      return false;
   }
   ret = rknn_init(&ctx, model_data, model_data_size, 0, NULL);
    if (ret < 0)
   {
      LOG_ERROR("rknn_init error ret=%d\n", ret);
      return false;
   }
   ret = rknn_query(ctx, RKNN_QUERY_SDK_VERSION, &version, sizeof(rknn_sdk_version));
   if (ret < 0)
   {
      LOG_ERROR("rknn_query RKNN_QUERY_SDK_VERSION error ret=%d\n", ret);
      return false;
   }
   LOG_DEBUG("sdk version: %s driver version: %s\n", version.api_version, version.drv_version);

  ret = rknn_query(ctx, RKNN_QUERY_IN_OUT_NUM, &app_ctx->io_num, sizeof(rknn_input_output_num));
  if (ret < 0)
  {
    LOG_ERROR("rknn_query RKNN_QUERY_IN_OUT_NUM error ret=%d\n", ret);
    return false;
  }
  LOG_DEBUG("model input num: %d, output num: %d\n", app_ctx->io_num.n_input, app_ctx->io_num.n_output);

  rknn_tensor_attr *input_attrs = (rknn_tensor_attr *)malloc(app_ctx->io_num.n_input * sizeof(rknn_tensor_attr));
  memset(input_attrs, 0, app_ctx->io_num.n_input * sizeof(rknn_tensor_attr));
  for (unsigned int i = 0; i < app_ctx->io_num.n_input; i++)
  {
    input_attrs[i].index = i;
    ret = rknn_query(ctx, RKNN_QUERY_INPUT_ATTR, &(input_attrs[i]), sizeof(rknn_tensor_attr));
    if (ret < 0)
    {
      LOG_ERROR("rknn_query RKNN_QUERY_INPUT_ATTR error ret=%d\n", ret);
      return false;
    }
    dump_tensor_attr(&(input_attrs[i]));
  }

  rknn_tensor_attr *output_attrs = (rknn_tensor_attr *)malloc(app_ctx->io_num.n_output * sizeof(rknn_tensor_attr));
  memset(output_attrs, 0, app_ctx->io_num.n_output * sizeof(rknn_tensor_attr));
  for (unsigned int i = 0; i < app_ctx->io_num.n_output; i++)
  {
    output_attrs[i].index = i;
    ret = rknn_query(ctx, RKNN_QUERY_OUTPUT_ATTR, &(output_attrs[i]), sizeof(rknn_tensor_attr));
    if (ret < 0)
    {
      LOG_ERROR("rknn_query RKNN_QUERY_OUTPUT_ATTR error ret=%d\n", ret);
      return false;
    }
    dump_tensor_attr(&(output_attrs[i]));
  }

  app_ctx->input_attrs = input_attrs;
  app_ctx->output_attrs = output_attrs;
  app_ctx->rknn_ctx = ctx;

  if (input_attrs[0].fmt == RKNN_TENSOR_NCHW)
  {
    LOG_DEBUG("model is NCHW input fmt\n");
    app_ctx->model_channel = input_attrs[0].dims[1];
    app_ctx->model_height = input_attrs[0].dims[2];
    app_ctx->model_width = input_attrs[0].dims[3];
  }
  else
  {
    LOG_DEBUG("model is NHWC input fmt\n");
    app_ctx->model_height = input_attrs[0].dims[1];
    app_ctx->model_width = input_attrs[0].dims[2];
    app_ctx->model_channel = input_attrs[0].dims[3];
  }
  LOG_DEBUG("model input height=%d, width=%d, channel=%d\n", app_ctx->model_height, app_ctx->model_width, app_ctx->model_channel);

  return true;
}

bool  RKnpu::Inference_Model(IMAGE_T *img, RESULT_GROUP_T *detect_result,RknnConText_T *app_ctx)
{
  int ret;
  img->format = RK_FORMAT_YCbCr_420_SP;
  rknn_context ctx = app_ctx->rknn_ctx;
  int model_width = app_ctx->model_width;
  int model_height = app_ctx->model_height;
  int model_channel = app_ctx->model_channel;

  struct timeval start_time, stop_time;
  const float nms_threshold = NMS_THRESH;
  const float box_conf_threshold = BOX_THRESH;
  // You may not need resize when src resulotion equals to dst resulotion
  void *resize_buf = nullptr;
  // init rga context
  rga_buffer_t src;
  rga_buffer_t dst;
  im_rect src_rect;
  im_rect dst_rect;
  memset(&src_rect, 0, sizeof(src_rect));
  memset(&dst_rect, 0, sizeof(dst_rect));
  memset(&src, 0, sizeof(src));
  memset(&dst, 0, sizeof(dst));

  LOG_DEBUG("input image %dx%d stride %dx%d format=%d\n", img->width, img->height, img->width_stride, img->height_stride, img->format);

  //float scale_w = (float)model_width / img->width;
  //float scale_h = (float)model_height / img->height;

  rknn_input inputs[1];
  memset(inputs, 0, sizeof(inputs));
  inputs[0].index = 0;
  inputs[0].type = RKNN_TENSOR_UINT8;
  inputs[0].size = model_width * model_height * model_channel;
  inputs[0].fmt = RKNN_TENSOR_NHWC;
  inputs[0].pass_through = 0;

  LOG_DEBUG("resize with RGA!\n");
  resize_buf = malloc(model_width * model_height * model_channel);
  memset(resize_buf, 0, model_width * model_height * model_channel);

  src = wrapbuffer_virtualaddr((void *)img->virt_addr, img->width, img->height, img->format, img->width_stride, img->height_stride);
  dst = wrapbuffer_virtualaddr((void *)resize_buf, model_width, model_height, RK_FORMAT_RGB_888);
  ret = imcheck(src, dst, src_rect, dst_rect);
  if (IM_STATUS_NOERROR != ret)
  {
    LOG_ERROR("%d, check error! %s \n", __LINE__, imStrError((IM_STATUS)ret));
    return false;
  }
  //IM_STATUS STATUS = imresize(src, dst);
  imresize(src, dst);
  inputs[0].buf = resize_buf;

  gettimeofday(&start_time, NULL);
  rknn_inputs_set(ctx, app_ctx->io_num.n_input, inputs);

  rknn_output outputs[app_ctx->io_num.n_output];
  memset(outputs, 0, sizeof(outputs));
  for (unsigned int i = 0; i < app_ctx->io_num.n_output; i++)
  {
    outputs[i].index = i;
    outputs[i].want_float = 0;
  }

  ret = rknn_run(ctx, NULL);
  ret = rknn_outputs_get(ctx, app_ctx->io_num.n_output, outputs, NULL);
  gettimeofday(&stop_time, NULL);
  LOG_DEBUG("once run use %f ms\n", (__get_us(stop_time) - __get_us(start_time)) / 1000);

  LOG_DEBUG("post process config: box_conf_threshold = %.2f, nms_threshold = %.2f\n", box_conf_threshold, nms_threshold);

  std::vector<float> out_scales;
  std::vector<int32_t> out_zps;
  for (unsigned int i = 0; i < app_ctx->io_num.n_output; ++i)
  {
    out_scales.push_back(app_ctx->output_attrs[i].scale);
    out_zps.push_back(app_ctx->output_attrs[i].zp);
  }
 // BOX_T pads;
//  memset(&pads, 0, sizeof(BOX_RECT));

 // post_process((int8_t *)outputs[0].buf, (int8_t *)outputs[1].buf, (int8_t *)outputs[2].buf, model_height, model_width,
 //              box_conf_threshold, nms_threshold, pads, scale_w, scale_h, out_zps, out_scales, detect_result);
  ret = rknn_outputs_release(ctx, app_ctx->io_num.n_output, outputs);

  if (resize_buf)
  {
    free(resize_buf);
  }
  return 0;
}