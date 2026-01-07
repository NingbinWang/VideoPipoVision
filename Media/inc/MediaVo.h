#ifndef _MEDIA_VO_H_
#define _MEDIA_VO_H_
#include "Common.h"
#include "MediaConfig.h"
#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/



/*****************************LOGO参数**********************************/
/*LOGO配置参数*/
#define LOGO_MAX_W		(512)     /*宽度*/
#define LOGO_MAX_H		(256)     /*高度*/
#define OSD_LOGO_LEN		(LOGO_MAX_W * LOGO_MAX_H * 2)

typedef struct
{
    UINT32                  bHaveVo;
    UINT32                  res[62];
    /**< 预留*/
}VO_CFG_PARAM_T;






#ifdef __cplusplus
}
#endif/*__cplusplus*/


#endif