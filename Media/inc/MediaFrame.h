#ifndef _MEDIA_FRAME_H_
#define _MEDIA_FRAME_H_
#include "Common.h"
#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/

#define MAX_NALU_NUM                    (16)
typedef struct
{
    UINT32 	            nalu_type;		   /*5: IDR，6: SEI，7: SPS，8: PPS，9: AUD*/
    UINT32   	        nalu_len;          /* nalu的长度*/
    UINT8 	            *nalu_ptr;         /* nalu的起始地址*/
}NALU_T;

typedef enum
{
    /********RAW********/
    MEDIA_FORMAT_RAW8 = 0,
    MEDIA_FORMAT_RAW10,
    MEDIA_FORMAT_RAW12,
    MEDIA_FORMAT_RAW14,
    MEDIA_FORMAT_RAW16,
    /********RGB********/
    MEDIA_FORMAT_RGB565 = 10,           // 16-bit RGB            
    MEDIA_FORMAT_BGR565,           // 16-bit RGB            
    MEDIA_FORMAT_RGB555,           // 15-bit RGB             
    MEDIA_FORMAT_BGR555,           // 15-bit RGB            
    MEDIA_FORMAT_RGB444,           // 12-bit RGB               
    MEDIA_FORMAT_BGR444,           // 12-bit RGB              
    MEDIA_FORMAT_RGB888,           // 24-bit RGB             
    MEDIA_FORMAT_BGR888,           // 24-bit RGB               
    MEDIA_FORMAT_RGB101010,        // 30-bit RGB              
    MEDIA_FORMAT_BGR101010,        // 30-bit RGB               
    MEDIA_FORMAT_ARGB8888,         // 32-bit RGB               
    MEDIA_FORMAT_ABGR8888,         // 32-bit RGB              
    MEDIA_FORMAT_BGRA8888,         // 32-bit RGB               
    MEDIA_FORMAT_RGBA8888,         // 32-bit RGB               
    MEDIA_FORMAT_RGB_BUTT,
    /********YUV********/
    MEDIA_FORMAT_YUV420SP = 30,    //YYYY... UV... (NV12)
    MEDIA_FORMAT_YUV420SP_10BIT,           
    MEDIA_FORMAT_YUV422SP,         //YYYY... UVUV... (NV16)
    MEDIA_FORMAT_YUV422SP_10BIT,   //Not part of ABI
    MEDIA_FORMAT_YUV420P,          //YYYY... U...V...  (I420)
    MEDIA_FORMAT_YUV420SP_VU,      //YYYY... VUVUVU... (NV21)
    MEDIA_FORMAT_YUV422P,          //YYYY... UU...VV...(422P
	MEDIA_FORMAT_YUV422SP_VU,      // YYYY... VUVUVU... (NV61)
    MEDIA_FORMAT_YUV422_YUYV,      // YUYVYUYV... (YUY2)       
    MEDIA_FORMAT_YUV422_YVYU,      // YVYUYVYU... (YVY2)       
    MEDIA_FORMAT_YUV422_UYVY,      // UYVYUYVY... (UYVY)       
    MEDIA_FORMAT_YUV422_VYUY,      // VYUYVYUY... (VYUY)       
    MEDIA_FORMAT_YUV400,           // YYYY...                  
    MEDIA_FORMAT_YUV440SP,         // YYYY... UVUV...          
    MEDIA_FORMAT_YUV411SP,         // YYYY... UV...            
    MEDIA_FORMAT_YUV444SP,        //YYYY... UVUVUVUV...      
    MEDIA_FORMAT_YUV444P,          // YYYY... UUUU... VVVV...  
    MEDIA_FORMAT_YUV444SP_10BIT,   //
    MEDIA_FORMAT_YUV_BUTT,
    //only Y
    MEDIA_FORMAT_Y_ONLY = 50,
	MEDIA_FORMAT_BGR_PACK_Y,                // [BGRBGR...][YYYY...]
    MEDIA_FORMAT_MAX,
}MEDIA_FORMAT_TYPE_E;



typedef struct
{
   void*    pVirAddr;    //虚拟地址
   void*     pPhyAddr;    //物理地址
    UINT32     u32Stride;  //stride[0]Y分量 lineoffset stride[1]UV分量   lineoffset
}MEDIA_YUV_FRAME_T;

//帧头信息
typedef struct
{
    UINT32                 u32ImageWidth;         //图像宽度
    UINT32                 u32ImageHeight;         //图像高度
    INT32                  iframeNum;             //帧号
    UINT                   uIndex;             //V4L2的buf号
    size_t                 sSize;             //一帧的大小
    MEDIA_FORMAT_TYPE_E    eFormatType;               //YUV格式
    void*            privt[2];
}MEDIA_VIDEO_HEADER_T;

//视频输入帧
typedef struct
{
     MEDIA_VIDEO_HEADER_T   stVideoHeader;    //帧头信息
     MEDIA_YUV_FRAME_T      stYuvframe;       //YUV数据指针
}MEDIA_VIDEO_FRAME_T;

typedef struct
{
  int width;
  int height;
  int width_stride;
  int height_stride;
  int format;
  char *virt_addr;
  int fd;
} IMAGE_FRAME_T;



typedef enum
{
    /* 音视频相关 */
    STREAM_ELEMENT_VIDEO_I              =0x10,        /*视频I帧*/
    STREAM_ELEMENT_VIDEO_P              =0x11,        /*视频P帧*/
    STREAM_ELEMENT_VIDEO_B              =0x12,        /*视频B帧*/
    STREAM_ELEMENT_AUDIO_F              =0x13,         /*音频帧*/

    /* 抓图相关 */
    STREAM_ELEMENT_JPEG_IMG             =0x20,       /*抓图数据JPEG*/
    STREAM_ELEMENT_JPEG_THM             =0x21,       /*抓图数据JPEG 缩略图图片*/
    STREAM_ELEMENT_JPEG_ADAS            =0x22,       /*智能算法抓图*/
    STREAM_ELEMENT_JPEG_FR              =0x23,       /*人脸算法抓图*/
    STREAM_ELEMENT_JPEG_FACE_REG        =0x24,       /*人脸注册*/
    STREAM_ELEMENT_YUV_IMG              =0x25,       /*抓图数据YUV*/
	STREAM_ELEMENT_JPEG_PLAYB_IMG		=0x26,		 /*回放抓图*/
	STREAM_ELEMENT_JPEG_PLAYB_THM		=0x27,		 /*回放抓缩略图*/

    /* 智能算法结果相关 */
    STREAM_ELEMENT_ADAS_RESULT          =0x30,       /*ADAS检测结果*/
    STREAM_ELEMENT_VCA_PACKET           =0x31,       /*智能检测结果*/

    /* 智能算法POS相关 */
    STREAM_ELEMENT_FD_POS               =0x50,       /*人脸检测调试信息*/
    /* 其他 */
    STREAM_ELEMENT_HEARTBEAT            =0x71,       /*心跳信息*/
    STREAM_ELEMENT_MAX  = INT_MAXI,
}MEDIA_STREAM_TYPE_E;



/* 码流信息魔术数定义 */
#define STREAM_ELEMENT_MAGIC 0x68616c6f

typedef struct 
{
    UINT32    videoStreamType;/* 编码类型 */
    UINT32    width;          /* 宽 */
    UINT32    height;         /* 高 */
    UINT32    fps;            /* 帧率 */
    UINT32    bps;            /* 码率 */
    UINT32    IFrameInterval; /* I帧间隔 */
    BOOL      bFristFrm;      /* 标记开启编码后的第一帧 */
	UINT8	  res[4];
}MEDIA_VIDEO_INFO_T;

typedef struct
{
    UINT32    audioStreamType;/* 编码类型 */
    UINT32    frame_len;      /* 帧长 */
    UINT32    sample_rate;    /* 采样率 */
    UINT32    bit_rate;       /* 比特率 */
    UINT32    audio_num;      /* 通道数 */
	UINT8	  res[4];
}MEDIA_AUDIO_INFO_T;

/* 每个码流的附加信息 */
typedef struct
{
    UINT32     		u32Magic;           /* 常数，供定位 */
    UINT8    		u8Id;              /* 数据信息类型 */
    UINT8    		u8Chan;            /* 通道号 */
    UINT16   		reserve;         /* 填充预留，保持4字节对其*/
    MEDIA_STREAM_TYPE_E     eType;            /* 码流类型 I/P/A帧*/
	UINT8	 		res[4];
    DATE_TIME_T     stAbsTime;         /* 绝对时间 */
    UINT32     		u32TimeStamp;       /* 时间戳 1K时标 */
    UINT32     		u32StreamType;      /* 流类型 复合流/视频流/音频流*/
    UINT32     		u32dataLen;         /* 除该信息外的帧的长度 */
    UINT32          u32JpgId;             /* JPEG抓拍uId */
    MEDIA_AUDIO_INFO_T      stVideoInfo;
    MEDIA_VIDEO_INFO_T      stAudioInfo;
}MEDIA_STREAM_ELEMENT;



#ifdef __cplusplus
}
#endif/*__cplusplus*/
#endif
