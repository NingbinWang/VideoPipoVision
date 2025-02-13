#include "Media.h"
#include "RKnpu.h"
#include "Logger.h"
#include "MppEncoder.h"


RKnpu * npu = nullptr;
RknnConText_T*  npu_ctx;
RKrga *mediarga = nullptr;


int MediaAi_Init(unsigned char *model_data,int model_data_size,const char *labels_nale_txt_path)
{
    size_t size = 0;
     npu_ctx = (RknnConText_T*)malloc(sizeof(RknnConText_T));
     memset(npu_ctx, 0, sizeof(RknnConText_T));
     npu = new RKnpu();
     npu->Init_Model(model_data,model_data_size,npu_ctx);
     mediarga = npu->getrga();
     size = strlen(labels_nale_txt_path);
     if(size > 256)
        size = 256;
     memcpy(npu_ctx->labels_nale_txt_path,labels_nale_txt_path,size);
     return 0;
}


int MediaAi_Report(IMAGE_FRAME_T* img,DETECT_RESULT_GROUP_T *results)
{
    bool ret;
    img->format = RK_FORMAT_YCbCr_420_SP;
    void *mppbuffer = MediaEncGetInputFrame();
    IMAGE_FRAME_T srcimg;

    srcimg.format = RK_FORMAT_RGB_888;
    srcimg.height = img->height;
    srcimg.height_stride = img->height_stride;
    srcimg.width = img->width;
    srcimg.width_stride = img->width_stride;
    srcimg.virt_addr = (char *)MediaEncGetInputFrameBufferAddr(mppbuffer);
    srcimg.fd = MediaEncGetInputFrameBufferFd(mppbuffer);
    mediarga->img_imcvtcolor_virt((IMAGE_T *)img,(IMAGE_T *)&srcimg);
    IMAGE_FRAME_T dstimg;
    void *resize_buf = nullptr;
    dstimg.format = RK_FORMAT_RGB_888;
    dstimg.height = npu_ctx->model_height;
    dstimg.height_stride = npu_ctx->model_height;
    dstimg.width = npu_ctx->model_width;
    dstimg.width_stride = npu_ctx->model_width;
    resize_buf = malloc((dstimg.height) * (dstimg.width) * (npu_ctx->model_channel));
    dstimg.virt_addr = (char* )resize_buf;
    mediarga->img_resize_virt((IMAGE_T *)&srcimg,(IMAGE_T *)&dstimg);
    ret = npu->Inference_Model(resize_buf,img->width,img->height,(RESULT_GROUP_T*)results,npu_ctx);
    if(ret == true){
        return 0;
    }else{
        return -1;
    }
    

}