#ifndef __MPP_ENC_H__
#define __MPP_ENC_H__

#include "rockchip/mpp_frame.h"
#include "rockchip/rk_mpi.h"
#include <pthread.h>
#include <string.h>

typedef void (*MppEncoderFrameCallback)(void* userdata, const char* data, int size);

typedef struct
{
    RK_U32 width;//表示输入图像水平方向像素数
    RK_U32 height;//表示输入图像垂直方向像素数
    RK_U32 hor_stride;//表示输入图像垂直方向相应两行之间的距离，单位为byte
    RK_U32 ver_stride;//表示输入图像分量之间的以行数间隔数，单位为1
    MppFrameFormat fmt;//表示输入图像色彩空间格式以及内存排布方式
    MppCodingType type;//表示MppEncCodecCFG对应的协议类型，需要与MppCtx初始化函数mpp_init的参数一致

    RK_U32 osd_enable;//osd的开关
    RK_U32 osd_mode;
    RK_U32 split_mode;//裁切
    RK_U32 split_arg;
    RK_U32 split_out;

    RK_U32 user_data_enable; //添加用户自定义数据
    RK_U32 roi_enable;

    // rate control runtime parameter
    RK_S32 fps_in_flex;
    RK_S32 fps_in_den;
    RK_S32 fps_in_num;
    RK_S32 fps_out_flex;
    RK_S32 fps_out_den;
    RK_S32 fps_out_num;
    RK_S32 bps;
    RK_S32 bps_max;
    RK_S32 bps_min;
    RK_S32 rc_mode;//表示码率控制模式 CBR固定码率模式 VBR 可变码率模式
    RK_S32 gop_mode;
    RK_S32 gop_len;
    RK_S32 vi_len;

    /* general qp control */
    RK_S32 qp_init;
    RK_S32 qp_max;
    RK_S32 qp_max_i;
    RK_S32 qp_min;
    RK_S32 qp_min_i;
    RK_S32 qp_max_step; /* delta qp between each two P frame */
    RK_S32 qp_delta_ip; /* delta qp between I and P */
    RK_S32 qp_delta_vi; /* delta qp between vi and P */

    RK_U32 constraint_set;
    RK_U32 rotation;//表示输入图像的旋转属性
    RK_U32 mirroring;//表示输入图像的镜像属性
    RK_U32 flip;

    MppEncHeaderMode header_mode;
    MppEncSeiMode sei_mode;
} MppEncoderParams;

class MppEncoder {
  public:
    MppEncoder();
    ~MppEncoder();
    int Init(MppEncoderParams& params, void* userdata);
    int SetCallback(MppEncoderFrameCallback callback);
    int Encode(void* mpp_buf, char* enc_buf, int max_size);
    int GetHeader(char* enc_buf, int max_size);
    int Reset();
    void* ImportBuffer(int index, size_t size, int fd, int type);
    size_t GetFrameSize();
    void* GetInputFrameBuffer();
    int GetInputFrameBufferFd(void* mpp_buffer);
    void* GetInputFrameBufferAddr(void* mpp_buffer);
  private:
    struct DataCrc {
      RK_U32          len;
      RK_U32          sum_cnt;
      RK_ULONG        *sum;
      RK_U32          vor; // value of the xor
    };
    int InitParams(MppEncoderParams& params);
    int SetupEncCfg();
    //int EncWidthDefaultStride(int width, MppFrameFormat fmt);

    MppCtx mpp_ctx = NULL;
    MppApi* mpp_mpi = NULL;
    RK_S32 chn = 0;

    MppEncoderFrameCallback callback = NULL;

    // global flow control flag
    // RK_U32 frm_eos = 0;
    // RK_U32 pkt_eos = 0;
    // RK_U32 frm_pkt_cnt = 0;
    // RK_S32 frame_num = 0;
    // RK_S32 frame_count = 0;
    // RK_U64 stream_size = 0;

    /* encoder config set */
    MppEncCfg cfg = NULL;
    MppEncPrepCfg prep_cfg;
    MppEncRcCfg rc_cfg;
    MppEncCodecCfg codec_cfg;
    MppEncSliceSplit split_cfg;
    MppEncOSDPltCfg osd_plt_cfg;
    MppEncOSDPlt osd_plt;
    MppEncOSDData osd_data;
    // RoiRegionCfg    roi_region;
    MppEncROICfg roi_cfg;

    // input / output
    MppBufferGroup buf_grp = NULL;
    MppBuffer frm_buf = NULL;
    MppBuffer pkt_buf = NULL;
    MppBuffer md_info = NULL;

    // MppEncRoiCtx roi_ctx;

    // resources
    size_t header_size;
    size_t frame_size;
    size_t mdinfo_size;
    /* NOTE: packet buffer may overflow */
    size_t packet_size;

    MppEncoderParams enc_params;

    void* userdata = NULL;
};

#endif  //__MPP_ENCODER_H__
