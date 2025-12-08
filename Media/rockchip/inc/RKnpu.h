#ifndef _RKNPU_H_
#define _RKNPU_H_

#include <pthread.h>
#include <string.h>
#include <sys/time.h>
#include <set>
#include <vector>
#include <math.h>
#include "RKrga.h"
#include "PostProcess.h"


class RKnpu {
  public:
    RKnpu();
    ~RKnpu();
  bool Init_Model(unsigned char *model_data,int model_data_size,RknnConText_T* app_ctx);
  bool Inference_Model(void *srcbuf,int srcwidth,int srcheight, RESULT_GROUP_T *detect_result,RknnConText_T *app_ctx);

private:
 void dump_tensor_attr(rknn_tensor_attr *attr);
 double __get_us(struct timeval t);


};



#endif