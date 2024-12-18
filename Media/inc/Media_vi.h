#ifndef _MEDIA_VI_H_
#define _MEDIA_VI_H_
#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/
#define MAX_VI_CHAN_SOC 8
#define MAX_ENC_CHAN_SOC  16
#define MAX_ENC_CHAN      16

//视频输入配置参数
typedef enum
{
    VIEW_NATURAL = 0,              //不镜像
    VIEW_90FLIP,                   //90度旋转
    VIEW_180FLIP,                  //180度旋转或中心镜像
    VIEW_270FLIP,                  //270度旋转
    VIEW_VERTICAL_MIRROR,          //垂直镜像，即图像左右翻转
    VIEW_HORIZONTAL_MIRROR,        //水平镜像，即图像上下翻转
    VIEW_90FLIP_VERTICAL_MIRROR,   //90度旋转加垂直镜像
    VIEW_90FLIP_HORIZONTAL_MIRROR, //90度旋转加水平镜像
    VIEW_FLIP_MAX,
} VI_VIEW_FLIP_E;

typedef enum
{
    CMOS_OV_OS05A20 = 0,
    CMOS_OV_OS04C10 = 1,
    CMOS_OV_9284    = 2,
    CMOS_YUV422_TVI = 3,
	VI_SENSOR_TYPE_MAX,
}VI_SENSOR_TYPE_E;

typedef enum
{
	LENS_TYPE_MAX,
} VIDEO_LENS_TYPE_E;


typedef struct
{
	unsigned int dayNightMode; //白天黑夜模式切换 0:白天 1:夜晚 2：自动
	unsigned char sensitive; //灵敏度0-7
	unsigned char filtime;	//过滤时间0-120
} VI_DATNIGHT_INFO_T;

typedef struct
{
	unsigned char NRMode; //降噪开关 0:关闭 1:普通 2:专家
	unsigned char NRLeve; //降噪等级（普通降噪等级）0-100
	unsigned char SpaNRLeve; //降噪等级（空域降噪等级）0-100
	unsigned char TemNRLeve; //降噪等级（时域降噪等级）0-100
} VI_NR_INFO_T;

typedef struct
{
	unsigned char uWhiteBalanceMode; //白平衡模式 0:手动 1:自动白平衡1 3:锁定白平衡 6:日光灯14：白炽灯15：暖光灯16：自然光
	unsigned char R_GAIN; //白平衡等级（手动模式生效）0-100
	unsigned char B_GAIN;
} VI_WHITEBALANCE_INFO_T;

typedef struct
{
	unsigned char FogMode; //透雾模式
    unsigned char FogLeavel; //透雾等级
} VI_FOG_INFO_T;

typedef struct
{
    unsigned int x; //图像中心水平坐标
    unsigned int y; //图像中心垂直坐标
    unsigned int w; //图像窗口宽最大不能超过VIDEO_PARAM_CFG结构体中image_viW, 最小64
    unsigned int h; //图像窗口高最大不能超过VIDEO_PARAM_CFG结构体中image_viH, 最小32
} VI_RECT_T;

typedef struct
{
	unsigned int idx;//区域id
    unsigned int x; //图像中心水平坐标
    unsigned int y; //图像中心垂直坐标
    unsigned int w; //图像窗口宽最大不能超过VIDEO_PARAM_CFG结构体中image_viW, 最小64
    unsigned int h; //图像窗口高最大不能超过VIDEO_PARAM_CFG结构体中image_viH, 最小32
} VI_Cover_T;

typedef struct
{
	unsigned char BLCMode;//背光补偿模式 0:关闭 1:上 2:下 3：左，4：右5：中6：自定义
    VI_RECT_T	blc_window_rec; //补偿区域（自定义模式生效）
} VI_BLC_INFO_T;

typedef struct
{
	// 遮挡块数 
	unsigned char CoverNum;
	// 遮挡区域（自定义模式生效） 
    VI_Cover_T	cover_window_rec[4];
} VI_COVER_INFO_T;

typedef struct
{
    unsigned int                          enable;            //是否进行配置 只管数组0上面的值
    unsigned int                          ctlid;         //Media中的配置 只管数组0上面的值
    VI_VIEW_FLIP_E                        viewMirror;   // 镜像选择方式 默认为0
    unsigned int                                image_viW; //前端原始图像有效宽
    unsigned int                                image_viH; //前端原始图像有效高
    unsigned int                                frame_rate; //前端帧率
    VI_SENSOR_TYPE_E                      vSensorType; //Sensor类型
    VIDEO_LENS_TYPE_E                     vLensType; //镜头类型
    VI_DATNIGHT_INFO_T                    dayNightInfo; //日夜模式   	0-day 1-night 2-自动模式 默认为2
    unsigned char                                 enWdr;//宽动态开关 0:关闭 1:打开2：自动
    unsigned char                                 enWdrLevel; //宽动态等级（只有在宽动态开启时生效）默认为50
    unsigned char                                 uBrightnessLevel;//亮度0-100> 默认为50
    unsigned char                                 uStaturationLevel;// 饱和度0-100 默认为50
    unsigned char                                 uContrastLevel;//对比度0-100 默认为50
    unsigned char                                 bEnableDCE;//畸变校正 默认为0
    unsigned char                                 uSharpnessLevel;  //锐度0-100 默认为50
    unsigned int                                uExposure;//曝光时间 [10,1000000] 微妙，默认为1/帧率
    unsigned char                                 uIrisType;  //光圈类型0-DC 1-P     暂不支持此接口
    VI_WHITEBALANCE_INFO_T                uWhiteBalanceInfo; //白平衡模式 0-手动白平衡；1-自动白平衡1；3-锁定白平衡；6-日光灯；14-白炽灯；15-暖光灯；16-自然光 默认为1
    VI_NR_INFO_T                          uNRInfo; //数字降噪	  0-关闭  1-普通 2-专家 默认为1
    VI_FOG_INFO_T                         uFogInfo; //透雾模式 0 关 1 开启 2自动
    unsigned char                                 uElesharkMode;//电子防抖开关
    VI_BLC_INFO_T                         uBLCInfo;//背光补偿 默认关
    unsigned char                                 uHLCMode;//强光抑制 默认关
    unsigned char                                 enHLCLevel; //强光抑制等级（只有在强光抑制开启时生效）默认为50    
    VI_COVER_INFO_T                       uCoverMode; //视频遮挡 默认关   
	unsigned char                                 ulightover;//防补光过爆0关 1开 默认为0
	unsigned char                                 ugrayscale;//灰度范围0[0-255] 1[16-235]默认为0
    unsigned char                                 res[7];
} VI_CFG_PARAM_T;


VI_CFG_PARAM_T *Media_Get_ViParam(void);

#define MEDIA_VI_MAX_DEV_NUM	4
#define MEDIA_VI_MAX_CHN_NUM	4
#define MEDIA_VI_MAX_PATH_NUM	8

#define MEDIA_VI_MAX_CHIP_CNT			2           /*芯片内部VI 硬件chip个数*/
#define MEDIA_VI_MAX_VCAP_PER_CHIP	1           /*每个VI硬件chip内部包含的VCAP个数*/
#define MEDIA_VI_MAX_VI_PER_VCAP		4           /*每个VCAP内部包含的VI设备个数*/
#define MEDIA_VI_MAX_CHN_PER_VI		4           /*每个VI设备内部包含的通道个数*/


#ifdef __cplusplus
}
#endif/*__cplusplus*/


#endif