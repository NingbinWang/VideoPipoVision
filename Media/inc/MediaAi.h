#ifndef _MEDIA_AI_H_
#define _MEDIA_AI_H_
#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/
#define AI_ALIGN(x, a)         (((x)+(a)-1)&~((a)-1))
#include "MediaFrame.h"
//yolo
#define OBJ_NUMB_MAX_SIZE 128

typedef struct
{
    int left;
    int top;
    int right;
    int bottom;
} DETECT_BOX_T;


typedef struct {
    DETECT_BOX_T box;
    float prop;
    int cls_id;
} DETECT_RESULT_T;

typedef struct {
    int id;
    int count;
    int costtime;
    DETECT_RESULT_T results[OBJ_NUMB_MAX_SIZE];
} DETECT_RESULT_GROUP_T;
/*
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
*/




int MediaAi_Init(unsigned char *model_data,int model_data_size,const char *labels_nale_txt_path);
int MediaAi_VideoReport(IMAGE_FRAME_T* img,DETECT_RESULT_GROUP_T *results);
//int MediaAi_VideoDrawobj(IMAGE_FRAME_T* img,DETECT_RESULT_GROUP_T *detect_result,void * mppbuffer);
int MediaAi_VideoDrawRect(IMAGE_FRAME_T* img,DETECT_RESULT_GROUP_T *detect_result);

#ifdef __cplusplus
}
#endif/*__cplusplus*/


#endif