#ifndef _MEDIA_PRIV_H_
#define _MEDIA_PRIV_H_
#include "Media.h"
#include "MediaFrame.h"
#include "RKnpu.h"
#include "Logger.h"
#include "RKrga.h"
// Media内部参数
typedef struct
{
    BOOL                    algLoad;
    /*linux启动完成*/
    UINT32                  SysStart;	
}MEDIA_INNER_PARAM_T;

#ifdef MEDIARKMPP
RKrga * Media_GetRkrga(void);
RKnpu * Media_GetRknpu(void);
#endif

#endif