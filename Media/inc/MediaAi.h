#ifndef _MEDIA_AI_H_
#define _MEDIA_AI_H_
#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/


//yolo
#define OBJ_NAME_MAX_SIZE 16
#define OBJ_NUMB_MAX_SIZE 64
typedef struct 
{
    int left;
    int right;
    int top;
    int bottom;
} BOX_DETECT_T;

typedef struct
{
    char name[OBJ_NAME_MAX_SIZE];
    BOX_DETECT_T box;
    float prop;
} DETECT_RESULT_T;

typedef struct
{
    int id;
    int count;
    DETECT_RESULT_T results[OBJ_NUMB_MAX_SIZE];
} DETECT_RESULT_GROUP_T;

typedef struct
{
  int width;
  int height;
  int width_stride;
  int height_stride;
  int format;
  char *virt_addr;
  int fd;
} IMAGE_FRAME_T;



int MediaAI_Init(unsigned char *model_data,int model_data_size);
int MediaAI_Report(IMAGE_FRAME_T* img,DETECT_RESULT_GROUP_T *results);

#ifdef __cplusplus
}
#endif/*__cplusplus*/


#endif