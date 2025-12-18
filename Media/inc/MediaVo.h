#ifndef _MEDIA_VO_H_
#define _MEDIA_VO_H_
#include "sys_common.h"
#include "MediaConfig.h"
#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/

/*解码通道个数*/
#define MAX_DEC_CHAN_SOC  0
/*显示通道个数*/
#define MAX_VO_CHAN_SOC   0
/*USB摄像头通道个数*/
#define MAX_USB_CHAN_SOC  0

/*****************************LOGO参数**********************************/
/*LOGO配置参数*/
#define LOGO_MAX_W		(512)     /*宽度*/
#define LOGO_MAX_H		(256)     /*高度*/
#define OSD_LOGO_LEN		(LOGO_MAX_W * LOGO_MAX_H * 2)

typedef struct
{
    UINT32                  bHaveVo;
    /*菜单共享缓存*/
    UINT32                  res[62];
    /**< 预留*/
}VO_CFG_PARAM_T;






#ifdef __cplusplus
}
#endif/*__cplusplus*/


#endif