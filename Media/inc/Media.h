#ifndef _HALMEDIA_H_
#define _HALMEDIA_H_
#include "Common.h"
#include "MediaConfig.h"
#include "MediaVi.h"
#include "MediaVo.h"
#include "MediaEnc.h"
#include "MediaDec.h"
#include "MediaFrame.h"
#include "MediaAi.h"
#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/

//缓冲区的大小
typedef struct
{
    void*    			  addr[MAX_SHARE_ADDR];        //多核/多进程访问地址 0为虚拟地址 1为物理地址
    UINT32    	  u32len;      //缓冲区长度
    UINT32        rIdx;        //缓冲写索引
    UINT32        wIdx;        //缓冲读索引
}AUDIO_POOL_INFO_T;

typedef enum 
{
	AUCODEC_MAX,
} AUDIO_CODEC_TYPE_E;

typedef struct
{
    AUDIO_CODEC_TYPE_E aCodecType;
    /*audio codec type*/
    UINT32 res[3];
    /**< 预留*/
}AUDIO_CFG_PARAM_T;

/******************************编码打包定义***********************************/

/*录像码流共享缓冲结构*/
typedef struct
{
	MEDIA_VIDEO_HEADER_T      stVideoHeader;
    void *          addr[MAX_SHARE_ADDR];  //多核/多进程访问地址 0为虚拟地址 1为物理地址
    UINT32 wIdx;
    UINT32 rIdx;
    UINT32 totalLen;
   UINT32 wErrTime;
    UINT64 lastFrameStdTime;
    UINT32 vFrmCounter;
   UINT32 streamType;     /*码流类型等*/
    UINT32 packetType;     /*封装类型 STREAM_PACKET_TYPE*/
   UINT32 maxPackLen;     /*包长度*/
    DATE_TIME_T lastFrameAbsTime;
}REC_POOL_INFO_T;

typedef struct
{
	MEDIA_VIDEO_HEADER_T      stVideoHeader;
    void *     			  addr[MAX_SHARE_ADDR];        //多核/多进程访问地址 0为虚拟地址 1为物理地址
    UINT32    	  u32len;      //缓冲区长度
    UINT32        rIdx;        //缓冲写索引
    UINT32        wIdx;        //缓冲读索引
}RAW_POOL_INFO_T;



typedef struct
{
	//接入
    UINT32                             u32ViChanCnt; //接入通道个数
    VI_CFG_PARAM_T                     astViCfgParam[MAX_VI_CHAN_SOC];//视频输入初始化参数
    //裸流
	RAW_POOL_INFO_T                    aRawStreamPool[MAX_VI_CHAN_SOC];//码流缓冲地址
    //编码
    UINT32                             u32EncChanCnt;//编码主通道个数-接几路sensor就是几路编码数(只计算主码流)
    MEDIA_ENC_PARAM_T		           astEncCfgParam[MAX_ENC_CHAN_SOC];//视频编码初始化参数
    MEDIA_ENCODER_STATUS_T             stEncoderStatus; //编码器的状态状态信息
	/*
     1）若recPool[0].bufLen==0, 为每个缓冲区分配一个默认大小的缓冲；
     2）若recPool[0].bufLen!=0, 所有缓冲区都按照应用指定的大小分配
     3）总的大小不要超过32MB
     */
    REC_POOL_INFO_T                    astRecPool[MAX_ENC_CHAN_SOC + MAX_USB_CHAN_SOC];
    // 音频初始化参数
    AUDIO_CFG_PARAM_T                  stAudioCfgParam;
	//音频
    AUDIO_POOL_INFO_T                  stAudioStreamPool;  //音频码流缓冲地址

    
    //LOGO
	UINT8                              u8logoAddr[OSD_LOGO_LEN]; //logo图像
    //解码通道个数
    UINT32                    u32DecChanCnt;         
    //视频输出通道个数
    UINT32                    u32VoChanCnt;
    // 视频输出 初始化参数
    VO_CFG_PARAM_T            astVoCfgParam[MAX_VO_CHAN_SOC];
     /*预留每个通道的多路码流都可以PS网传
     通道映射，在只有2个通道的输入的情况下
     索引0 1 分别为通道0和1的第一路码流，索引2 3分别为通道0和1的第二路码流
     */
     /*解码码流共享缓存区地址*/
    DEC_SHARE_BUF_T           decShareBuf[MAX_DEC_CHAN_SOC];/*解码码流下载*/
     
     /*图片回放下载码流*/
    DEC_SHARE_BUF_T           jpegDecShareBuf[MAX_DEC_CHAN_SOC];/*jpeg回放下载地址*/
     
     /* 音频回放共享缓冲区地址 */
    DEC_SHARE_BUF_T           audioDecShareBuf[MAX_DEC_CHAN_SOC];/*音频回放下载地址*/

    /* 状态缓存区 */
    DEC_STATUS_T              decStatus[MAX_DEC_CHAN_SOC];      /*dec Status  */
    
	//当前系统的日期、时间
    DATE_TIME_T               stNow;  
}MEDIA_PARAM_T;

INT32 MediaInit(MEDIA_PARAM_T* pParam);

#ifdef __cplusplus
}
#endif/*__cplusplus*/


#endif