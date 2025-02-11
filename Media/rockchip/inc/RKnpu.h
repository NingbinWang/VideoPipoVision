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


typedef struct
{
  rknn_context rknn_ctx;
  rknn_input_output_num io_num;
  rknn_tensor_attr *input_attrs;
  rknn_tensor_attr *output_attrs;
  int model_channel;
  int model_width;
  int model_height;
} RknnConText_T;



#define NMS_THRESH 0.45
#define BOX_THRESH 0.25
#define OBJECT_NAME_MAX_SIZE 16
#define OBJECT_NUMB_MAX_SIZE 64
typedef struct 
{
    int left;
    int right;
    int top;
    int bottom;
} BOX_T;

typedef struct
{
    char name[OBJECT_NAME_MAX_SIZE];
    BOX_T box;
    float prop;
} RESULT_T;

typedef struct
{
    int id;
    int count;
    RESULT_T results[OBJECT_NUMB_MAX_SIZE];
} RESULT_GROUP_T;

class RKnpu {
  public:
    RKnpu();
    ~RKnpu();
  bool Init_Model(unsigned char *model_data,int model_data_size,RknnConText_T* app_ctx);
  bool Inference_Model(IMAGE_T *img, RESULT_GROUP_T *detect_result,RknnConText_T *app_ctx);

private:
 void dump_tensor_attr(rknn_tensor_attr *attr);
 double __get_us(struct timeval t);


};



#endif