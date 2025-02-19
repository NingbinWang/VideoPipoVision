#ifndef _RKNN_POSPROCESS_H_
#define _RKNN_POSPROCESS_H_
#include <stdint.h>
#include <vector>
#include "RKrga.h"
#include "rknn_api.h"
typedef struct
{
  rknn_context rknn_ctx;
  rknn_input_output_num io_num;

  rknn_tensor_attr *input_attrs;
  rknn_tensor_attr *output_attrs;

  rknn_tensor_mem **input_mems;
  rknn_tensor_mem **output_mems;

  int model_channel;
  int model_width;
  int model_height;

  int x_pad;
  int y_pad;
  float scale_w;
  float scale_h;
  

  float nms_threshold;
  float box_conf_threshold;


  bool is_quant;
} RknnConText_T;

#define NMS_THRESH 0.45
#define BOX_THRESH 0.25
#define OBJECT_NAME_MAX_SIZE 64
#define OBJECT_NUMB_MAX_SIZE 128
#define OBJECT_CLASS_NUM 80
#define PROP_BOX_SIZE (5 + OBJECT_CLASS_NUM)

typedef struct {
    int left;
    int top;
    int right;
    int bottom;
} IMG_RECT_t;

typedef struct {
    IMG_RECT_t box;
    float prop;
    int cls_id;
} RESULT_T;

typedef struct {
    int id;
    int count;
    int costtime;
    RESULT_T results[OBJECT_NUMB_MAX_SIZE];
} RESULT_GROUP_T;

int init_post_process(const char *labels_nale_txt_path);
char *coco_cls_to_name(int cls_id);
void deinit_post_process();

int post_process(RknnConText_T *app_ctx, void *outputs, RESULT_GROUP_T *od_results);


#endif