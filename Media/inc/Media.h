#ifndef _HALMEDIA_H_
#define _HALMEDIA_H_
#include "common_type.h"
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

/*日期、时间*/
#ifndef _DATE_TIME_
#define _DATE_TIME_
typedef struct
{
    short year;
    short month;
    short dayOfWeek;        /*0:星期日-6:星期六*/
    short day;
    short hour;
    short minute;
    short second;
    short milliSecond;
} DATE_TIME_T;
#endif

/*特殊应用*/
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

//#define BEEP_MAX_SOUND_NUM  (64)
#define BEEP_MAX_BUF_SIZE   (BEEP_MAX_SOUND_NUM*64000*2) /* 最多支持的BEEP缓存大小 */
#define BEEP_LOOP_FOREVER   (0)
typedef enum
{
    BEEP_INIT_STATUS,
    BEEP_WORKING_STATUS,
    BEEP_OVER_STATUS,
	BEEP_STATUS_MAX,
}BEEP_WORK_STATUS_E;

typedef struct
{
    UINT16  beepNumber;     /* BEEP声音个数, 0-BEEP_MAX_SOUND_NUM */
    UINT16  sampleRate;     /* 采样率, 目前只支持16000 */
    UINT16  chanMode;       /* 通道数, 1:单通道 2:双通道 默认单通道 */
    UINT16  reserved[5];    /* 保留 */
    UINT32  dataOffset[BEEP_MAX_SOUND_NUM]; /* 数据偏移, 需4字节对齐 */
    UINT32  dataLength[BEEP_MAX_SOUND_NUM]; /* 数据大小, 需4字节对齐 */
    UINT8   data[BEEP_MAX_BUF_SIZE];        /* PCM数据 */
    /*BEEP 当前状态 DSP TO APP*/
    BEEP_WORK_STATUS_E  beepStatus;
	UINT8	res[4];
} BEEP_SOUND_BUF_T;

#define ALG_OSD_MAX_NUM	(128)  /* 最多支持的OSD 个数 */
#define ALG_OSD_MAX_BUF_SIZE	(32 * 128) /* 最多支持的OSD 缓存大小 */

typedef struct
{
    UINT32  OsdNumber;                   	/* OSD个数, 0-BEEP_MAX_SOUND_NUM */
    UINT32  DataOffset[ALG_OSD_MAX_NUM]; 	/* 数据偏移, 需4字节对齐，报警事件字符串在OsdData中的偏移量 */
    UINT32  DataLength[ALG_OSD_MAX_NUM]; 	/* 数据大小, 需4字节对齐，报警事件字符串的长度*/
    UINT32  OsdData[ALG_OSD_MAX_BUF_SIZE];  /* OSD数据，所有报警事件字符串的存放位置 */
    UINT16  startX[ALG_OSD_MAX_NUM];        /* ALG OSD数据  startX,叠加osd的起始位置*/
    UINT16  startY[ALG_OSD_MAX_NUM];        /* ALG OSD数据  startY，叠加osd的起始位置*/
	UINT8	res[4];
} ALG_OSD_BUF_T; 


typedef struct
{
    CPU_BITS    addr[MAX_SHARE_ADDR];   /*多核/多进程访问地址 */
    volatile    UINT32    len;        /*缓冲长度*/
    volatile    UINT32    rIdx;       /*缓冲写索引*/
    volatile    UINT32    wIdx;       /*缓冲读索引*/
	volatile    UINT8	  res[4];
}MEDIA_SHAREDATA_T;


typedef struct 
{
    /*时间信息*/
    UINT16            	time_type;             //0 无效 1 白天 2 晚上 3黎明黄昏
	  /*人脸检测配置参数*/
	float               faceScoreThred;        //人脸分数，范围0~1，默认0.2
	UINT8               faceConfidThred;       //人脸置信度，范围0~100，默认25
	UINT16              faceResultSkip;        //人脸检测结果选择传输
    /*预留信息*/
    UINT8              	reserved[16];          //保留字节(20)
}SMART_DETECT_PARAM_T;



/******************************编码打包定义***********************************/
/*定义分通道上传，录像存储缓冲区I帧信息存储最大数目*/
#define REC_IFRM_INFO_NUM   (200)
#define MAX_PACKET_IN_POOL  (4096)

typedef struct
{
    volatile UINT32 idx;                /* index in recBuf */
    volatile UINT32  stdTime;
    DATE_TIME_T mdiaAbsTime;
    volatile UINT32 len;
    volatile UINT32 frameType;
}STREAM_IFRAME_INFO_T;

typedef struct 
{
    STREAM_IFRAME_INFO_T ifInfo[REC_IFRM_INFO_NUM];
    volatile UINT32     rIdx;
    volatile UINT32     wIdx;
}IFRAME_INFO_ARRAY_T;


/*录像码流共享缓冲结构*/
typedef struct
{
    CPU_BITS        addr[MAX_SHARE_ADDR];   /*多核/多进程访问地址 */
    volatile UINT32 wIdx;
    volatile UINT32 rIdx;
    volatile UINT32 totalLen;
    volatile UINT32 wErrTime;
    volatile UINT32 lastFrameStdTime;
    volatile UINT32 vFrmCounter;
    volatile UINT32 streamType;     /*码流类型，视频流、复合流等*/
    volatile UINT32 packetType;     /*封装类型 STREAM_PACKET_TYPE*/
    volatile UINT32 maxPackLen;     /*包长度*/
    volatile UINT32 extParam;       /*扩展参数，用来扩展一些定制功能*/
    IFRAME_INFO_ARRAY_T ifInfo;	
    DATE_TIME_T lastFrameAbsTime;
}REC_POOL_INFO_T;

/*网传码流共享缓冲结构*/
typedef struct
{
    CPU_BITS        addr[MAX_SHARE_ADDR];   /*多核/多进程访问地址 */
    volatile UINT32 totalLen;       /*内存池总长度，仅DSP使用，A8无需关心*/
    volatile UINT32 lastFrmIdx;      /**/
    volatile UINT32 packetWIdx;     /*包的写指针，DSP更新*/
    volatile UINT32 packetRIdx;     /*包的读指针，A8更新*/
    volatile UINT32 links;
    volatile UINT32 vFrmCounter;
    volatile UINT32 streamType;     /*码流类型，视频流、复合流等*/
    volatile UINT32 packetType;     /*封装类型 STREAM_PACKET_TYPE*/
    volatile UINT32 maxPackLen;     /*包长度*/
    volatile UINT32 extParam;       /*扩展参数，用来扩展一些定制功能*/
    volatile UINT32 packetOffset[MAX_PACKET_IN_POOL];
}NET_POOL_INFO_T;

typedef struct
{
    UINT32                    productid;         //产品ID
    UINT32                    encChanCnt;         //编码主通道个数-接几路sensor就是几路编码数(只计算主码流)
    UINT32                    decChanCnt;         //解码通道个数
    UINT32                    VoChanCnt;        //视频输出通道个数
    DATE_TIME_T               now;               //当前系统的日期、时间
    //LOGO
	UINT8                     logoAddr[OSD_LOGO_LEN]; //logo图像
    // 视频输出 初始化参数
    VO_CFG_PARAM_T            voCfgParam[MAX_VO_CHAN_SOC];
    // 视频输入初始化参数
    VI_CFG_PARAM_T            viCfgParam[MAX_ENC_CHAN_SOC];
    // 音频初始化参数
    AUDIO_CFG_PARAM_T         audioCfgParam;
    //beep 共享缓存
    BEEP_SOUND_BUF_T          audioBeepShare;
    //alg osd 共享缓存
    ALG_OSD_BUF_T               algOsdShare;
    //码流缓冲地址
    MEDIA_SHAREDATA_T         streamShareBuf;
    //音频码流缓冲地址
    MEDIA_SHAREDATA_T         audiostreamShareBuf;
    //智能信息缓冲地址
    MEDIA_SHAREDATA_T         smartShareBuf;          
	// 智能检测相关参数
	SMART_DETECT_PARAM_T	  smartParam;
    //分通道码流上传地址
     /*
     1）若recPool[0].bufLen==0, 为每个缓冲区分配一个默认大小的缓冲；
     2）若recPool[0].bufLen!=0, 所有缓冲区都按照应用指定的大小分配
     3）总的大小不要超过32MB
     */
    REC_POOL_INFO_T           RecPool[MAX_ENC_CHAN + MAX_USB_CHAN_SOC];
     /*预留每个通道的多路码流都可以录像
     通道映射，在只有2个通道的输入的情况下
     0 1 为前二个通道的第一路码流，2 3分别为通道0和1的第二路码流
     */
    NET_POOL_INFO_T           NetPool[MAX_ENC_CHAN + MAX_USB_CHAN_SOC];
     /*预留每个通道的多路码流都可以RTP网传
     通道映射，在只有2个通道的输入的情况下
     索引0 1 分别为通道0和1的第一路码流，索引2 3分别为通道0和1的第二路码流
     */
    NET_POOL_INFO_T           PsNetPool[MAX_ENC_CHAN + MAX_USB_CHAN_SOC];
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
    ENC_STATUS_T              encStatus[MAX_ENC_CHAN];
}MEDIA_PARAM_T;



#ifdef __cplusplus
}
#endif/*__cplusplus*/


#endif