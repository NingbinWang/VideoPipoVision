#pragma once
#include "MediaConfig.h"
#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/
/*编码通道运行状态*/
typedef struct
{
    volatile unsigned int   viFps;          /*采集帧率*/
    volatile unsigned int   osdStatus;      /*osd是否叠加*/
    volatile unsigned int   viFrm;          /*采集帧数*/
    volatile unsigned int   viLostFrm;      /*采集丢帧数目累加*/
    volatile unsigned int   viW;            /*采集的宽高*/
    volatile unsigned int   viH;            /*采集的宽高*/
    volatile unsigned int   encFps;         /*编码帧率*/
    volatile unsigned int   encFrm;         /*已编码帧数*/
    volatile unsigned int   encLostFrm;     /*编码丢帧数目累加*/
    volatile unsigned int   encBitRate;     /*编码比特率*/
    volatile unsigned int   encW;           /*编码宽高*/
    volatile unsigned int   encH;           /*编码宽高*/
    volatile unsigned int   audioFrm;       /*音频已编码帧数*/
    volatile unsigned int   audioLostFrm;   /*音频编码丢帧数目累加*/
    volatile unsigned int   enable;         /*通道是否使能*/
    volatile unsigned int   stremType;      /*视频流类型*/
    volatile unsigned int   bHaveSingal;      /*是否有信号*/
    volatile unsigned int   RecPoolFrmLost;   /*视频录像丢帧数目累加*/
    volatile unsigned int   NetPoolFrmLost;   /*网传视频录像丢帧数目累加*/
    volatile unsigned int   PsNetPoolFrmLost; /* PS网传视频录像丢帧数目累加*/
    volatile unsigned int   res[5];
} ENC_PARAM_T;
#ifdef __cplusplus
}

class MediaEnc
{
public:		
     MediaEnc(const ENC_PARAM_T &params);
    ~MediaEnc();
protected:
    ENC_PARAM_T m_params;
};





#endif/*__cplusplus*/

