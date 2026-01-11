#ifndef _MEDIA_PRIV_H_
#define _MEDIA_PRIV_H_
#include "Media.h"
#include "MediaFrame.h"
#include "RKnpu.h"
#include "Logger.h"
#include "Common.h"
#include "RKrga.h"
// Media内部参数
typedef struct
{
    BOOL                    algLoad;
	MediaVi*                pVi[MAX_VI_CHAN_SOC];
	MediaStream*            pStream[MAX_VI_CHAN_SOC];
}MEDIA_INNER_PARAM_T;

MEDIA_INNER_PARAM_T* Media_Get_InnerParam(void);

#ifdef MEDIARKMPP
RKrga * Media_GetRkrga(void);
RKnpu * Media_GetRknpu(void);
#endif

#endif