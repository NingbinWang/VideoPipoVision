#pragma once
#include "MediaConfig.h"
#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/
/*编码通道运行状态*/
typedef struct
{
    volatile UINT32   viFps;          /*采集帧率*/
    volatile UINT32   osdStatus;      /*osd是否叠加*/
    volatile UINT32   viFrm;          /*采集帧数*/
    volatile UINT32   viLostFrm;      /*采集丢帧数目累加*/
    volatile UINT32   viW;            /*采集的宽高*/
    volatile UINT32   viH;            /*采集的宽高*/
    volatile UINT32   encFps;         /*编码帧率*/
    volatile UINT32   encFrm;         /*已编码帧数*/
    volatile UINT32   encLostFrm;     /*编码丢帧数目累加*/
    volatile UINT32   encBitRate;     /*编码比特率*/
    volatile UINT32   encW;           /*编码宽高*/
    volatile UINT32   encH;           /*编码宽高*/
    volatile UINT32   audioFrm;       /*音频已编码帧数*/
    volatile UINT32   audioLostFrm;   /*音频编码丢帧数目累加*/
    volatile UINT32   enable;         /*通道是否使能*/
    volatile UINT32   stremType;      /*视频流类型*/
    volatile UINT32   bHaveSingal;      /*是否有信号*/
    volatile UINT32   RecPoolFrmLost;   /*视频录像丢帧数目累加*/
    volatile UINT32   NetPoolFrmLost;   /*网传视频录像丢帧数目累加*/
    volatile UINT32   PsNetPoolFrmLost; /* PS网传视频录像丢帧数目累加*/
    volatile UINT32   res[5];
} ENC_STATUS_T;
#ifdef __cplusplus
}

class MediaEnc
{

};





#endif/*__cplusplus*/

