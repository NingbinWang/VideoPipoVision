#ifndef _MEDIA_PRIV_H_
#define _MEDIA_PRIV_H_
#include "Media.h"
#include "MediaFrame.h"
#include "Logger.h"
#include "Common.h"
#include "MediaViInner.h"
#include "MediaEncInner.h"
#include "MediaStream.h"
#include "MppEncoder.h"
#include "MppDecoder.h"
#include "RKrga.h"
#include "RKnpu.h"

// Media内部参数
typedef struct
{
    BOOL                    algLoad;
	MediaStream*            pStream;
	MediaVi*                apVi[MAX_VI_CHAN_SOC+ MAX_USB_CHAN_SOC];//这里有多少路硬件路就有多少个Vi
	MediaEnc*               apEnc[MAX_ENC_CHAN_SOC];
	MediaEncCallback        AICallback;
}MEDIA_INNER_PARAM_T;

MEDIA_INNER_PARAM_T* Media_Get_InnerParam(void);
MEDIA_FORMAT_TYPE_E MediaForccFrame(char * strFormate);


#ifdef MEDIARKMPP
RKrga * Media_GetRkrga(void);
RKnpu * Media_GetRknpu(void);
MppEncoder * Media_Getmppencoder(void);
MppDecoder *Media_Getmppdecoder(void);

#endif

#endif
