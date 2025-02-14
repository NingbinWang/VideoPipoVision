#ifndef _MEDIA_AI_H_
#define _MEDIA_AI_H_
#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/

#include "MediaFrame.h"
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





int MediaAi_Init(unsigned char *model_data,int model_data_size,const char *labels_nale_txt_path);
int MediaAi_VideoReport(IMAGE_FRAME_T* img,DETECT_RESULT_GROUP_T *results);
int MediaAi_VideoDrawobj(IMAGE_FRAME_T* img,DETECT_RESULT_GROUP_T *detect_result,void * mppbuffer);

#ifdef __cplusplus
}
#endif/*__cplusplus*/


#endif