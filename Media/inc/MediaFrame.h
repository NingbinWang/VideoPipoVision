#ifndef _MEDIA_FRAME_H_
#define _MEDIA_FRAME_H_
#include "Common.h"
#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/

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



#ifdef __cplusplus
}
#endif/*__cplusplus*/
#endif