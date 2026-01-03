#include "RKnpu.h"
#include "MppEncoder.h"
#include "Logger.h"


RKnpu::RKnpu()
{

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
   // 调用rknn_init接口初始化rknn_context，加载RKNN模型
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
// Get Model Input Output Info
  ret = rknn_query(ctx, RKNN_QUERY_IN_OUT_NUM, &app_ctx->io_num, sizeof(rknn_input_output_num));
  if (ret < 0)
  {
    LOG_ERROR("rknn_query RKNN_QUERY_IN_OUT_NUM error ret=%d\n", ret);
    return false;
  }
  LOG_DEBUG("model input num: %d, output num: %d\n", app_ctx->io_num.n_input, app_ctx->io_num.n_output);
   // Get Model Input Info
 // 调用rknn_query接口，查询原始的输入tensor属性，输出的tensor属性，放到对应rknn_tensor_attr结构体对象
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
  // Get Model Output Info
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

  if (output_attrs[0].qnt_type == RKNN_TENSOR_QNT_AFFINE_ASYMMETRIC && output_attrs[0].type != RKNN_TENSOR_FLOAT16)
  {
      app_ctx->is_quant = true;
  }
  else
  {
      app_ctx->is_quant = false;
  }
  // 调用rknn_query接口，查询 RKNN 模型里面的用户自定义字符串信息
  //rknn_custom_string custom_string;
 // ret = rknn_query(ctx, RKNN_QUERY_CUSTOM_STRING, &custom_string, sizeof(custom_string));
 // if (ret != RKNN_SUCC) {
 //   LOG_ERROR("rknn_query fail! ret=%d\n", ret);
 //         return -1;
 // }
 //LOG_DEBUG("custom string: %s\n", custom_string.string);

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

bool  RKnpu::Inference_Model(void *srcbuf,int srcwidth,int srcheight, RESULT_GROUP_T *detect_result,RknnConText_T *app_ctx)
{
  int ret;
  rknn_context ctx = app_ctx->rknn_ctx;
  int model_width = app_ctx->model_width;
  int model_height = app_ctx->model_height;
  int model_channel = app_ctx->model_channel;
  struct timeval start_time, stop_time;

  // You may not need resize when src resulotion equals to dst resulotion
 
  app_ctx->scale_w = (float)model_width / srcwidth;
  app_ctx->scale_h = (float)model_height / srcheight;

  if(app_ctx->nms_threshold == 0 )
  {
      app_ctx->nms_threshold = NMS_THRESH;
  }
  if(app_ctx->box_conf_threshold == 0 )
  {
      app_ctx->box_conf_threshold = BOX_THRESH;
  }

   // Set Input Data
  rknn_input inputs[1];
  memset(inputs, 0, sizeof(inputs));
  inputs[0].index = 0;
  inputs[0].type = RKNN_TENSOR_UINT8;
  inputs[0].size = model_width * model_height * model_channel;
  inputs[0].fmt = RKNN_TENSOR_NHWC;
  inputs[0].pass_through = 0;
  //LOG_DEBUG("model input height=%d, width=%d, channel=%d\n", app_ctx->model_height, app_ctx->model_width, app_ctx->model_channel);
  inputs[0].buf = srcbuf;

  gettimeofday(&start_time, NULL);
  ret =  rknn_inputs_set(ctx, app_ctx->io_num.n_input, inputs);
  if (ret < 0)
  {
      LOG_ERROR("rknn_input_set fail! ret=%d\n", ret);
      return false;
  }
  ret = rknn_run(ctx, NULL);
  if (ret < 0)
  {
     LOG_ERROR("rknn_run fail! ret=%d\n", ret);
      return false;
  }

  rknn_output outputs[app_ctx->io_num.n_output];
  memset(outputs, 0, sizeof(outputs));
  for (unsigned int i = 0; i < app_ctx->io_num.n_output; i++)
  {
    outputs[i].index = i;
    outputs[i].want_float = (!app_ctx->is_quant);
  }
 
  ret = rknn_outputs_get(ctx, app_ctx->io_num.n_output, outputs, NULL);
  if (ret < 0)
  {
      LOG_ERROR("rknn_outputs_get fail! ret=%d\n", ret);
      return false;
  }
  gettimeofday(&stop_time, NULL);
  detect_result->costtime = (__get_us(stop_time) - __get_us(start_time)) / 1000;
 // LOG_DEBUG("post process config: box_conf_threshold = %.2f, nms_threshold = %.2f\n", box_conf_threshold, nms_threshold);

 // Post Process
  post_process(app_ctx, outputs, detect_result);

  ret = rknn_outputs_release(ctx, app_ctx->io_num.n_output, outputs);
  if (ret < 0)
  {
      LOG_ERROR("rknn_outputs_release fail! ret=%d\n", ret);
      return false;
  }
  return true;
}


