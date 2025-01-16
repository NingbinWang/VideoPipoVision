#ifndef _MEDIA_FRAME_H_
#define _MEDIA_FRAME_H_
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
    MEDIA_FORMAT_RGGB = 10,                 //RGB   
    MEDIA_FORMAT_BGRA,                      //BGRA  

    /********YUV********/
    //YUV平面
    MEDIA_FORMAT_YUV_PLANAR_444 = 30,       //YYYYUUUUVVVV...
    MEDIA_FORMAT_YUV_PLANAR_422,            //YYYYUUVV...
    MEDIA_FORMAT_YUV_PLANAR_420,            //YYYYUV...
    MEDIA_FORMAT_YVU_PLANAR_444,            //YYYYVVVVUUUU...
    MEDIA_FORMAT_YVU_PLANAR_422,            //YYYYVVUU...
    MEDIA_FORMAT_YVU_PLANAR_420,            //YYYYVU...

    //YUV交织
    MEDIA_FORMAT_YUV_PACK_444,              //YUV...
    MEDIA_FORMAT_YUV_PACK_422,              //YUYV...

    MEDIA_FORMAT_YVU_PACK_444,              //YVU...
    MEDIA_FORMAT_YVU_PACK_422,              //YVYU...

    MEDIA_FORMAT_UYVY_PACK_422,             //UYVY...
    MEDIA_FORMAT_VYUY_PACK_422,             //VYUY...


    //Y平面UVUV交织
    MEDIA_FORMAT_Y_PLANAR_UV_PACK_444,      //YYYYUVUVUVUV...
    MEDIA_FORMAT_Y_PLANAR_UV_PACK_422,      //YYYYUVUV...
    MEDIA_FORMAT_Y_PLANAR_UV_PACK_420,      //YYYYUV...

    //Y平面VUVU交织
    MEDIA_FORMAT_Y_PLANAR_VU_PACK_444,      //YYYYVUVUVUVU...
    MEDIA_FORMAT_Y_PLANAR_VU_PACK_422,      //YYYYVUVU...
    MEDIA_FORMAT_Y_PLANAR_VU_PACK_420,      //YYYYVU...

    //only Y
    MEDIA_FORMAT_Y_ONLY,

	MEDIA_FORMAT_BGR_PACK_Y,                // [BGRBGR...][YYYY...]

    MEDIA_FORMAT_MAX,
}MEDIA_FORMAT_E;

typedef struct
{
    unsigned char    *virAddr[4];    //addr[0] 帧数据的起始地址。Addr[1]，UV分量的地址  虚拟地址
    unsigned char    *phyAddr[4];    //addr[0] 帧数据的起始地址。Addr[1]，UV分量的地址  物理地址
    unsigned int    stride[4];  //stride[0]Y分量 lineoffset stride[1]UV分量   lineoffset
}MEDIA_YUV_FRAME_T;

//帧头信息
typedef struct
{
    unsigned int           image_width;          //图像宽度
    unsigned int           image_height;         //图像高度
    unsigned int           frameNum;             //帧号
    MEDIA_FORMAT_E   format;               //YUV格式
    unsigned long           pts;                  //时标
    unsigned int           tagProc;              //< 标记frame处理过程
    unsigned int           tagAttr;              //< 标记frame属性

    void*            privt[2];
    unsigned int           poolId;
    unsigned int           modId;
}MEDIA_VIDEO_HEADER_T;

//视频输入帧
typedef struct
{
     MEDIA_VIDEO_HEADER_T   video_header;    //帧头信息
     MEDIA_YUV_FRAME_T      yuv_frame;       //YUV数据指针
}MEDIA_VIDEO_FRAME_T;





#ifdef __cplusplus
}
#endif/*__cplusplus*/
#endif