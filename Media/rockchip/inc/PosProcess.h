#ifndef _RKNN_POSPROCESS_H_
#define _RKNN_POSPROCESS_H_
#include <stdint.h>
#include <vector>
#define NMS_THRESH 0.45
#define BOX_THRESH 0.25
#define OBJECT_NAME_MAX_SIZE 16
#define OBJECT_NUMB_MAX_SIZE 64
#define OBJECT_CLASS_NUM 80
#define PROP_BOX_SIZE (5 + OBJECT_CLASS_NUM)
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

int post_process(int8_t *input0, int8_t *input1, int8_t *input2, int model_in_h, int model_in_w,
    float conf_threshold, float nms_threshold, BOX_T pads, float scale_w, float scale_h,
    std::vector<int32_t> &qnt_zps, std::vector<float> &qnt_scales,
    RESULT_GROUP_T *group,const char *labels_nale_txt_path);


#endif