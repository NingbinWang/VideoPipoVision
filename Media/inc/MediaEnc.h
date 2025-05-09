#ifndef _MEDIA_ENC_H_
#define _MEDIA_ENC_H_
#include "MediaConfig.h"
#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/
/*编码通道运行状态*/
typedef struct
{
    unsigned int   viFps;          /*采集帧率*/
    unsigned int   osdStatus;      /*osd是否叠加*/
    unsigned int   viFrm;          /*采集帧数*/
    unsigned int   viLostFrm;      /*采集丢帧数目累加*/
    unsigned int   viW;            /*采集的宽高*/
    unsigned int   viH;            /*采集的宽高*/
    unsigned int   encFps;         /*编码帧率*/
    unsigned int   encFrm;         /*已编码帧数*/
    unsigned int   encLostFrm;     /*编码丢帧数目累加*/
    unsigned int   encBitRate;     /*编码比特率*/
    unsigned int   encW;           /*编码宽高*/
    unsigned int   encH;           /*编码宽高*/
    unsigned int   audioFrm;       /*音频已编码帧数*/
    unsigned int   audioLostFrm;   /*音频编码丢帧数目累加*/
    unsigned int   enable;         /*通道是否使能*/
    unsigned int   stremType;      /*视频流类型*/
    unsigned int   bHaveSingal;      /*是否有信号*/
    unsigned int   RecPoolFrmLost;   /*视频录像丢帧数目累加*/
    unsigned int   NetPoolFrmLost;   /*网传视频录像丢帧数目累加*/
    unsigned int   PsNetPoolFrmLost; /* PS网传视频录像丢帧数目累加*/
    unsigned int   res[5];
} ENC_STATUS_T;


typedef void (*MediaEncCallback)(void* userdata, const char* data, int size);

int MediaEncInit(const ENC_STATUS_T* status);
int MediaEncEncode(void* mpp_buf, char* enc_buf, int max_size);
int MediaEncGetHeader(char* enc_buf, int max_size);
int MediaEncSetCallback(MediaEncCallback callback );
void MediaEncDeInit();
size_t MediaEncGetFrameSize();
void* MediaEncGetInputFrame();
int MediaEncGetInputFrameBufferFd(void * source);
void* MediaEncGetInputFrameBufferAddr(void * source);

#ifdef __cplusplus
}
#endif/*__cplusplus*/

#endif
