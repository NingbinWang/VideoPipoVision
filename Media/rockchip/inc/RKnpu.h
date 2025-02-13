#ifndef _RKNPU_H_
#define _RKNPU_H_

#include "rknn_api.h"
#include "RKrga.h"
#include <pthread.h>
#include <string.h>
#include <sys/time.h>
#include <set>
#include <vector>
#include <math.h>
#include "PosProcess.h"


typedef struct
{
  rknn_context rknn_ctx;
  rknn_input_output_num io_num;
  rknn_tensor_attr *input_attrs;
  rknn_tensor_attr *output_attrs;
  int model_channel;
  int model_width;
  int model_height;
  char labels_nale_txt_path[256];
} RknnConText_T;





class RKnpu {
  public:
    RKnpu();
    ~RKnpu();
  bool Init_Model(unsigned char *model_data,int model_data_size,RknnConText_T* app_ctx);
  bool Inference_Model(void *srcbuf,int srcwidth,int srcheight, RESULT_GROUP_T *detect_result,RknnConText_T *app_ctx);
  RKrga * getrga();

private:
 void dump_tensor_attr(rknn_tensor_attr *attr);
 double __get_us(struct timeval t);


};



#endif