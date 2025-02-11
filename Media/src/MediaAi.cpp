#include "MediaAi.h"
#include "RKnpu.h"

RKnpu * npu = nullptr;
RknnConText_T*  ctx;
int MediaAI_Init(unsigned char *model_data,int model_data_size)
{
    
     RknnConText_T npu_ctx;
     memset(&npu_ctx, 0, sizeof(RknnConText_T));
     npu = new RKnpu();
     npu->Init_Model(model_data,model_data_size,&npu_ctx);
     ctx = &npu_ctx;
     return 0;
}

int MediaAI_Report(IMAGE_FRAME_T* img,DETECT_RESULT_GROUP_T *results)
{
    bool ret;
    ret = npu->Inference_Model((IMAGE_T*)img,(RESULT_GROUP_T*)results,ctx);
    if(ret == true){
        return 0;
    }else{
        return -1;
    }
    
}