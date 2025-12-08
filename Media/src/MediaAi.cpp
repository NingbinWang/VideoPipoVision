#include "MediaPriv.h"
#include "MppEncoder.h"
#include "drawing.h"
#include "PostProcess.h"

RKnpu * npu = nullptr;
RknnConText_T*  npu_ctx;
RKrga *mediarga = nullptr;


int MediaAi_Init(unsigned char *model_data,int model_data_size,const char *labels_nale_txt_path)
{
     npu_ctx = (RknnConText_T*)malloc(sizeof(RknnConText_T));
     memset(npu_ctx, 0, sizeof(RknnConText_T));
     npu = Media_GetRknpu();
     npu->Init_Model(model_data,model_data_size,npu_ctx);
     mediarga = Media_GetRkrga();
     init_post_process(labels_nale_txt_path);
     return 0;
}


int MediaAi_VideoReport(IMAGE_FRAME_T* img,DETECT_RESULT_GROUP_T *results)
{
    bool ret;
    img->format = RK_FORMAT_YCbCr_420_SP;
 //   void *mppbuffer = MediaEncGetInputFrame();
  //  IMAGE_FRAME_T srcimg;
   // srcimg.format = RK_FORMAT_RGB_888;
   // srcimg.height = img->height;
  //  srcimg.height_stride = img->height_stride;
   // srcimg.width = img->width;
   // srcimg.width_stride = img->width_stride;
   // srcimg.virt_addr = (char *)MediaEncGetInputFrameBufferAddr(mppbuffer);
    //srcimg.fd = MediaEncGetInputFrameBufferFd(mppbuffer);

    //mediarga->img_cvtcolor_virt((IMAGE_T *)img,(IMAGE_T *)&srcimg);

    IMAGE_FRAME_T dstimg= {0};
    void *resize_buf = nullptr;
    dstimg.format = RK_FORMAT_RGB_888;
    dstimg.height = npu_ctx->model_height;
    //dstimg.height_stride = npu_ctx->model_height;
    dstimg.width = npu_ctx->model_width;
   // dstimg.width_stride = npu_ctx->model_width;
    resize_buf = malloc((dstimg.height) * (dstimg.width) * (npu_ctx->model_channel));
    dstimg.virt_addr = (char* )resize_buf;
    mediarga->img_resize_ai_virt((IMAGE_T *)img,(IMAGE_T *)&dstimg);
    ret = npu->Inference_Model(resize_buf,img->width,img->height,(RESULT_GROUP_T*)results,npu_ctx);
    if(resize_buf != nullptr)
    {
        free(resize_buf);
    }
    if(ret == true){
        return 0;
    }else{
        return -1;
    } 
}

int MediaAi_VideoDrawRect(IMAGE_FRAME_T* img,DETECT_RESULT_GROUP_T *detect_result)
{
    img->format = RK_FORMAT_YCbCr_420_SP;
    if(detect_result->count < 1)
    {
        return 0;
    }
    OSD_RECT_T osdrect[detect_result->count] = {0};
    for (int i = 0; i < detect_result->count; i++)
    {
        DETECT_RESULT_T *det_result = &(detect_result->results[i]);
       // LOG_DEBUG("face @ (%d %d %d %d) %.3f\n",det_result->box.left, det_result->box.top,det_result->box.right, det_result->box.bottom,det_result->prop);
        int x1 = det_result->box.left;
        int y1 = det_result->box.top;
        int x2 = det_result->box.right;
        int y2 = det_result->box.bottom;
        //OSD_RECT_T osdrect = {0};
        osdrect[i].x_pos = AI_ALIGN(x1,2);
        osdrect[i].y_pos = AI_ALIGN(y1,2);
        osdrect[i].witdh = AI_ALIGN((x2 - x1),2);
        osdrect[i].height = AI_ALIGN((y2 - y1),2);
       // mediarga->img_fillrectangle_virt((IMAGE_T *)img,osdrect);
    }
    mediarga->img_fillrectangle_array_virt((IMAGE_T *)img,osdrect,detect_result->count);
    return 0;
}


/*

int MediaAi_VideoDrawobj(IMAGE_FRAME_T* img,DETECT_RESULT_GROUP_T *detect_result,void * mppbuffer)
{
    img->format = RK_FORMAT_YCbCr_420_SP;
    IMAGE_FRAME_T outimg;
    outimg.format = RK_FORMAT_YCbCr_420_SP;
    outimg.height = img->height;
    outimg.height_stride = img->height_stride;
    outimg.width = img->width;
    outimg.width_stride = img->width_stride;
    outimg.virt_addr = (char *)MediaEncGetInputFrameBufferAddr(mppbuffer);
    outimg.fd = MediaEncGetInputFrameBufferFd(mppbuffer);
    mediarga->img_copy_fd((IMAGE_T *)img,(IMAGE_T *)&outimg);
     // Draw objects
    for (int i = 0; i < detect_result->count; i++)
   {
      RESULT_T *det_result =(RESULT_T *) &(detect_result->results[i]);
      //LOG_DEBUG("%s @ (%d %d %d %d) %f\n", det_result->name, det_result->box.left, det_result->box.top, det_result->box.right, det_result->box.bottom, det_result->prop);
    int x1 = det_result->box.left;
    int y1 = det_result->box.top;
    int x2 = det_result->box.right;
    int y2 = det_result->box.bottom;
    draw_rectangle_yuv420sp((unsigned char *)outimg.virt_addr, outimg.width_stride, outimg.height_stride, x1, y1, x2 - x1 + 1, y2 - y1 + 1, 0x00FF0000, 4);
  }
  return 0;

}
  */