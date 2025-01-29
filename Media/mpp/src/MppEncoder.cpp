#include "MppEncoder.h"
#include <stdio.h>
#include <string.h>
#include "rockchip/mpp_buffer.h"
#include "Logger.h"
#define FILEOUT 1
#if FILEOUT
#include <fstream>
#include <iostream>

 FILE* fp_mpp = nullptr;
 #endif

#define MPP_ALIGN(x, a)         (((x)+(a)-1)&~((a)-1))
#define SZ_4K 4096

static RK_S32 aq_thd_smart[16] = {
    1,  3,  3,  3,  3,  3,  5,  5,
    8,  8,  8, 15, 15, 20, 25, 28
};

static RK_S32 aq_step_smart[16] = {
    -8, -7, -6, -5, -4, -3, -2, -1,
    0,  1,  2,  3,  4,  6,  8, 10
};

static RK_S32 aq_thd[16] = {
    0,  0,  0,  0,
    3,  3,  5,  5,
    8,  8,  8,  15,
    15, 20, 25, 25
};
static RK_S32 aq_step_i_ipc[16] = {
    -8, -7, -6, -5,
    -4, -3, -2, -1,
    0,  1,  2,  3,
    5,  7,  7,  8,
};

static RK_S32 aq_step_p_ipc[16] = {
    -8, -7, -6, -5,
    -4, -2, -1, -1,
    0,  2,  3,  4,
    6,  8,  9,  10,
};



/*
int MppEncoder::EncWidthDefaultStride(int width, MppFrameFormat fmt)
{
    int stride = 0;

    switch (fmt & MPP_FRAME_FMT_MASK) {
    case MPP_FMT_YUV400 :
    case MPP_FMT_YUV420SP :
    case MPP_FMT_YUV420SP_VU : {
        stride = MPP_ALIGN(width, 8);
    } break;
    case MPP_FMT_YUV420P : {
        // NOTE: 420P need to align to 16 so chroma can align to 8 
        stride = MPP_ALIGN(width, 16);
    } break;
    case MPP_FMT_YUV422P:
    case MPP_FMT_YUV422SP:
    case MPP_FMT_YUV422SP_VU: {
        // NOTE: 422 need to align to 8 so chroma can align to 16 
        stride = MPP_ALIGN(width, 8);
    } break;
    case MPP_FMT_YUV444SP :
    case MPP_FMT_YUV444P : {
        stride = MPP_ALIGN(width, 8);
    } break;
    case MPP_FMT_RGB565:
    case MPP_FMT_BGR565:
    case MPP_FMT_RGB555:
    case MPP_FMT_BGR555:
    case MPP_FMT_RGB444:
    case MPP_FMT_BGR444:
    case MPP_FMT_YUV422_YUYV :
    case MPP_FMT_YUV422_YVYU :
    case MPP_FMT_YUV422_UYVY :
    case MPP_FMT_YUV422_VYUY : {
        // NOTE: for vepu limitation 
        stride = MPP_ALIGN(width, 8) * 2;
    } break;
    case MPP_FMT_RGB888 :
    case MPP_FMT_BGR888 : {
       // NOTE: for vepu limitation 
        stride = MPP_ALIGN(width, 8) * 3;
    } break;
    case MPP_FMT_RGB101010 :
    case MPP_FMT_BGR101010 :
    case MPP_FMT_ARGB8888 :
    case MPP_FMT_ABGR8888 :
    case MPP_FMT_BGRA8888 :
    case MPP_FMT_RGBA8888 : {
        // NOTE: for vepu limitation 
        stride = MPP_ALIGN(width, 8) * 4;
    } break;
    default : {
        LOG_ERROR("do not support type %d\n", fmt);
    } break;
    }

    return stride;
}
*/

int MppEncoder::InitParams(MppEncoderParams& params)
{
    memcpy(&enc_params, &params, sizeof(MppEncoderParams));

    if (enc_params.rc_mode == MPP_ENC_RC_MODE_BUTT){
         enc_params.rc_mode = (enc_params.type == MPP_VIDEO_CodingMJPEG) ? MPP_ENC_RC_MODE_FIXQP : MPP_ENC_RC_MODE_VBR;
    }
    
    if (enc_params.type == MPP_VIDEO_CodingUnused) {
        if (enc_params.width <= 0 || enc_params.height <= 0 ||
            enc_params.hor_stride <= 0 || enc_params.ver_stride <= 0) {
            LOG_ERROR("invalid w:h [%d:%d] stride [%d:%d]\n",enc_params.width, enc_params.height, enc_params.hor_stride, enc_params.ver_stride);
            return -1;
        }
    }
   
    
    if (enc_params.rc_mode == MPP_ENC_RC_MODE_FIXQP) {
        if (!enc_params.qp_init) {
            if (enc_params.type == MPP_VIDEO_CodingAVC ||
                enc_params.type == MPP_VIDEO_CodingHEVC)
                enc_params.qp_init = 26;
        }
    }

    // get paramter from cmd
    if (enc_params.hor_stride == 0) {
        enc_params.hor_stride = (MPP_ALIGN(enc_params.width, 16));
    }
    if (enc_params.ver_stride == 0) {
        enc_params.ver_stride = (MPP_ALIGN(enc_params.height, 16));
    }
    
    this->mdinfo_size  = (MPP_VIDEO_CodingHEVC == enc_params.type) ?
                      (MPP_ALIGN(enc_params.hor_stride, 32) >> 5) *
                      (MPP_ALIGN(enc_params.ver_stride, 32) >> 5) * 16 :
                      (MPP_ALIGN(enc_params.hor_stride, 64) >> 6) *
                      (MPP_ALIGN(enc_params.ver_stride, 16) >> 4) * 16;

    // update resource parameter
    switch (enc_params.fmt & MPP_FRAME_FMT_MASK) {
    case MPP_FMT_YUV420SP:
    case MPP_FMT_YUV420P: {
        this->frame_size = MPP_ALIGN(enc_params.hor_stride, 64) * MPP_ALIGN(enc_params.ver_stride, 64) * 3 / 2;
    } break;

    case MPP_FMT_YUV422_YUYV :
    case MPP_FMT_YUV422_YVYU :
    case MPP_FMT_YUV422_UYVY :
    case MPP_FMT_YUV422_VYUY :
    case MPP_FMT_YUV422P :
    case MPP_FMT_YUV422SP : {
        this->frame_size = MPP_ALIGN(enc_params.hor_stride, 64) * MPP_ALIGN(enc_params.ver_stride, 64) * 2;
    } break;
    case MPP_FMT_RGB444 :
    case MPP_FMT_BGR444 :
    case MPP_FMT_RGB555 :
    case MPP_FMT_BGR555 :
    case MPP_FMT_RGB565 :
    case MPP_FMT_BGR565 :
    case MPP_FMT_RGB888 :
    case MPP_FMT_BGR888 :
    case MPP_FMT_RGB101010 :
    case MPP_FMT_BGR101010 :
    case MPP_FMT_ARGB8888 :
    case MPP_FMT_ABGR8888 :
    case MPP_FMT_BGRA8888 :
    case MPP_FMT_RGBA8888 : {
        this->frame_size = MPP_ALIGN(enc_params.hor_stride, 64) * MPP_ALIGN(enc_params.ver_stride, 64);
    } break;

    default: {
        this->frame_size = MPP_ALIGN(enc_params.hor_stride, 64) * MPP_ALIGN(enc_params.ver_stride, 64) * 4;
    } break;
    }

    if (MPP_FRAME_FMT_IS_FBC(enc_params.fmt)) {
        if ((enc_params.fmt & MPP_FRAME_FBC_MASK) == MPP_FRAME_FBC_AFBC_V1)
            this->header_size = MPP_ALIGN(MPP_ALIGN(enc_params.width, 16) * MPP_ALIGN(enc_params.height, 16) / 16, SZ_4K);
        else
            this->header_size = MPP_ALIGN(enc_params.width, 16) * MPP_ALIGN(enc_params.height, 16) / 16;
    } else {
        this->header_size = 0;
    }
/*
    if (enc_params.fps_in_den == 0)
        enc_params.fps_in_den = 1;
    if (enc_params.fps_in_num == 0)
        enc_params.fps_in_num = 30;
    if (enc_params.fps_out_den == 0)
        enc_params.fps_out_den = 1;
    if (enc_params.fps_out_num == 0)
        enc_params.fps_out_num = 30;

    if (!enc_params.bps)
        enc_params.bps = enc_params.width * enc_params.height / 8 * (enc_params.fps_out_num / enc_params.fps_out_den);
*/


    return 0;
}

int MppEncoder::SetupEncCfg()
{
    MPP_RET ret;
   

    ret = mpp_enc_cfg_init(&cfg);
    if (ret) {
        LOG_ERROR("mpp_enc_cfg_init failed ret %d\n", ret);
        return -1;
    }
    
    ret = mpp_mpi->control(mpp_ctx, MPP_ENC_GET_CFG, cfg);
    if (ret) {
        LOG_ERROR("get enc cfg failed ret %d\n", ret);
         return -1;
    }

    /* setup default parameter */
    if (enc_params.fps_in_den == 0)
        enc_params.fps_in_den = 1;
    if (enc_params.fps_in_num == 0)
        enc_params.fps_in_num = 30;
    if (enc_params.fps_out_den == 0)
        enc_params.fps_out_den = 1;
    if (enc_params.fps_out_num == 0)
        enc_params.fps_out_num = 30;


        //tune
    if (enc_params.anti_flicker_str == 0){
        enc_params.anti_flicker_str = 0;
        enc_params.atr_str_i = 0; 
        enc_params.atr_str_p = 0;
        enc_params.atl_str = 0;
        enc_params.sao_str_i = 0;
        enc_params.sao_str_p = 0;
    }
    //scene_mode
    if (enc_params.scene_mode == 0){
        enc_params.deblur_en = 0;
        enc_params.deblur_str = 0;
        enc_params.rc_container = 0;
    }


    if (!enc_params.bps)
        enc_params.bps = enc_params.width * enc_params.height / 8 * (enc_params.fps_out_num / enc_params.fps_out_den);

    if (enc_params.rc_mode == MPP_ENC_RC_MODE_SMTRC) {
        mpp_enc_cfg_set_st(cfg, "hw:aq_thrd_i", aq_thd_smart);
        mpp_enc_cfg_set_st(cfg, "hw:aq_thrd_p", aq_thd_smart);
        mpp_enc_cfg_set_st(cfg, "hw:aq_step_i", aq_step_smart);
        mpp_enc_cfg_set_st(cfg, "hw:aq_step_p", aq_step_smart);
    } else {
        mpp_enc_cfg_set_st(cfg, "hw:aq_thrd_i", aq_thd);
        mpp_enc_cfg_set_st(cfg, "hw:aq_thrd_p", aq_thd);
        mpp_enc_cfg_set_st(cfg, "hw:aq_step_i", aq_step_i_ipc);
        mpp_enc_cfg_set_st(cfg, "hw:aq_step_p", aq_step_p_ipc);
    }

    mpp_enc_cfg_set_s32(cfg, "rc:max_reenc_times", 0);
    mpp_enc_cfg_set_s32(cfg, "rc:cu_qp_delta_depth", enc_params.cu_qp_delta_depth);
    mpp_enc_cfg_set_s32(cfg, "tune:anti_flicker_str", enc_params.anti_flicker_str);
    mpp_enc_cfg_set_s32(cfg, "tune:atr_str_i", enc_params.atr_str_i);
    mpp_enc_cfg_set_s32(cfg, "tune:atr_str_p", enc_params.atr_str_p);
    mpp_enc_cfg_set_s32(cfg, "tune:atl_str", enc_params.atl_str);
    mpp_enc_cfg_set_s32(cfg, "tune:sao_str_i", enc_params.sao_str_i);
    mpp_enc_cfg_set_s32(cfg, "tune:sao_str_p", enc_params.sao_str_p);

    mpp_enc_cfg_set_s32(cfg, "tune:scene_mode", enc_params.scene_mode);
    mpp_enc_cfg_set_s32(cfg, "tune:deblur_en", enc_params.deblur_en);
    mpp_enc_cfg_set_s32(cfg, "tune:deblur_str", enc_params.deblur_str);
    mpp_enc_cfg_set_s32(cfg, "tune:rc_container", enc_params.rc_container);
    mpp_enc_cfg_set_s32(cfg, "tune:vmaf_opt", 0);
    mpp_enc_cfg_set_s32(cfg, "hw:qbias_en", 1);
    mpp_enc_cfg_set_s32(cfg, "hw:qbias_i", enc_params.bias_i);
    mpp_enc_cfg_set_s32(cfg, "hw:qbias_p", enc_params.bias_p);
    mpp_enc_cfg_set_s32(cfg, "hw:skip_bias_en", 0);
    mpp_enc_cfg_set_s32(cfg, "hw:skip_bias", 4);
    mpp_enc_cfg_set_s32(cfg, "hw:skip_sad", 8);

    mpp_enc_cfg_set_s32(cfg, "prep:width", enc_params.width);
    mpp_enc_cfg_set_s32(cfg, "prep:height", enc_params.height);
    mpp_enc_cfg_set_s32(cfg, "prep:hor_stride", enc_params.hor_stride);
    mpp_enc_cfg_set_s32(cfg, "prep:ver_stride", enc_params.ver_stride);
    mpp_enc_cfg_set_s32(cfg, "prep:format", enc_params.fmt);
     mpp_enc_cfg_set_s32(cfg, "prep:range", MPP_FRAME_RANGE_JPEG);


    mpp_enc_cfg_set_s32(cfg, "rc:mode", enc_params.rc_mode);
    mpp_enc_cfg_set_u32(cfg, "rc:max_reenc_times", 0);
    mpp_enc_cfg_set_u32(cfg, "rc:super_mode", 0);

    /* fix input / output frame rate */
    mpp_enc_cfg_set_s32(cfg, "rc:fps_in_flex", enc_params.fps_in_flex);
    mpp_enc_cfg_set_s32(cfg, "rc:fps_in_num", enc_params.fps_in_num);
    mpp_enc_cfg_set_s32(cfg, "rc:fps_in_denorm", enc_params.fps_in_den);
    mpp_enc_cfg_set_s32(cfg, "rc:fps_out_flex", enc_params.fps_out_flex);
    mpp_enc_cfg_set_s32(cfg, "rc:fps_out_num", enc_params.fps_out_num);
    mpp_enc_cfg_set_s32(cfg, "rc:fps_out_denorm", enc_params.fps_out_den);
   // mpp_enc_cfg_set_s32(cfg, "rc:gop", enc_params.gop_len ? enc_params.gop_len : enc_params.fps_out_num * 2);

    /* drop frame or not when bitrate overflow */
    mpp_enc_cfg_set_u32(cfg, "rc:drop_mode", MPP_ENC_RC_DROP_FRM_DISABLED);
    mpp_enc_cfg_set_u32(cfg, "rc:drop_thd", 20);        /* 20% of max bps */
    mpp_enc_cfg_set_u32(cfg, "rc:drop_gap", 1);         /* Do not continuous drop frame */

    /* setup bitrate for different rc_mode */
    mpp_enc_cfg_set_s32(cfg, "rc:bps_target", enc_params.bps);
    switch (enc_params.rc_mode) {
    case MPP_ENC_RC_MODE_FIXQP : {
        /* do not setup bitrate on FIXQP mode */
    } break;
    case MPP_ENC_RC_MODE_CBR : {
        /* CBR mode has narrow bound */
        mpp_enc_cfg_set_s32(cfg, "rc:bps_max", enc_params.bps_max ? enc_params.bps_max : enc_params.bps * 17 / 16);
        mpp_enc_cfg_set_s32(cfg, "rc:bps_min", enc_params.bps_min ? enc_params.bps_min : enc_params.bps * 15 / 16);
    } break;
    case MPP_ENC_RC_MODE_VBR :
    case MPP_ENC_RC_MODE_AVBR : {
        /* VBR mode has wide bound */
        mpp_enc_cfg_set_s32(cfg, "rc:bps_max", enc_params.bps_max ? enc_params.bps_max : enc_params.bps * 17 / 16);
        mpp_enc_cfg_set_s32(cfg, "rc:bps_min", enc_params.bps_min ? enc_params.bps_min : enc_params.bps * 1 / 16);
    } break;
    default : {
        /* default use CBR mode */
        mpp_enc_cfg_set_s32(cfg, "rc:bps_max", enc_params.bps_max ? enc_params.bps_max : enc_params.bps * 17 / 16);
        mpp_enc_cfg_set_s32(cfg, "rc:bps_min", enc_params.bps_min ? enc_params.bps_min : enc_params.bps * 15 / 16);
    } break;
    }

    /* setup qp for different codec and rc_mode */
    switch (enc_params.type) {
    case MPP_VIDEO_CodingAVC :
    case MPP_VIDEO_CodingHEVC : {
        switch (enc_params.rc_mode) {
        case MPP_ENC_RC_MODE_FIXQP : {
            RK_S32 fix_qp = enc_params.qp_init;

            mpp_enc_cfg_set_s32(cfg, "rc:qp_init", fix_qp);
            mpp_enc_cfg_set_s32(cfg, "rc:qp_max", fix_qp);
            mpp_enc_cfg_set_s32(cfg, "rc:qp_min", fix_qp);
            mpp_enc_cfg_set_s32(cfg, "rc:qp_max_i", fix_qp);
            mpp_enc_cfg_set_s32(cfg, "rc:qp_min_i", fix_qp);
            mpp_enc_cfg_set_s32(cfg, "rc:qp_ip", 0);
            mpp_enc_cfg_set_s32(cfg, "rc:fqp_min_i", fix_qp);
            mpp_enc_cfg_set_s32(cfg, "rc:fqp_max_i", fix_qp);
            mpp_enc_cfg_set_s32(cfg, "rc:fqp_min_p", fix_qp);
            mpp_enc_cfg_set_s32(cfg, "rc:fqp_max_p", fix_qp);
        } break;
        case MPP_ENC_RC_MODE_CBR :
        case MPP_ENC_RC_MODE_VBR :
        case MPP_ENC_RC_MODE_AVBR : {
            mpp_enc_cfg_set_s32(cfg, "rc:qp_init", enc_params.qp_init ? enc_params.qp_init : -1);
            mpp_enc_cfg_set_s32(cfg, "rc:qp_max", enc_params.qp_max ? enc_params.qp_max : 51);
            mpp_enc_cfg_set_s32(cfg, "rc:qp_min", enc_params.qp_min ? enc_params.qp_min : 10);
            mpp_enc_cfg_set_s32(cfg, "rc:qp_max_i", enc_params.qp_max_i ? enc_params.qp_max_i : 51);
            mpp_enc_cfg_set_s32(cfg, "rc:qp_min_i", enc_params.qp_min_i ? enc_params.qp_min_i : 10);
            mpp_enc_cfg_set_s32(cfg, "rc:qp_ip", 2);
            mpp_enc_cfg_set_s32(cfg, "rc:fqp_min_i", enc_params.fqp_min_i ? enc_params.fqp_min_i : 10);
            mpp_enc_cfg_set_s32(cfg, "rc:fqp_max_i", enc_params.fqp_max_i ? enc_params.fqp_max_i : 45);
            mpp_enc_cfg_set_s32(cfg, "rc:fqp_min_p", enc_params.fqp_min_p ? enc_params.fqp_min_p : 10);
            mpp_enc_cfg_set_s32(cfg, "rc:fqp_max_p", enc_params.fqp_max_p ? enc_params.fqp_max_p : 45);
        } break;
        default : {
            LOG_ERROR("unsupport encoder rc mode %d\n", enc_params.rc_mode);
        } break;
        }
    } break;
    case MPP_VIDEO_CodingVP8 : {
        /* vp8 only setup base qp range */
        mpp_enc_cfg_set_s32(cfg, "rc:qp_init", enc_params.qp_init ? enc_params.qp_init : 40);
        mpp_enc_cfg_set_s32(cfg, "rc:qp_max",  enc_params.qp_max ? enc_params.qp_max : 127);
        mpp_enc_cfg_set_s32(cfg, "rc:qp_min",  enc_params.qp_min ? enc_params.qp_min : 0);
        mpp_enc_cfg_set_s32(cfg, "rc:qp_max_i", enc_params.qp_max_i ? enc_params.qp_max_i : 127);
        mpp_enc_cfg_set_s32(cfg, "rc:qp_min_i", enc_params.qp_min_i ? enc_params.qp_min_i : 0);
        mpp_enc_cfg_set_s32(cfg, "rc:qp_ip", 6);
    } break;
    case MPP_VIDEO_CodingMJPEG : {
        /* jpeg use special codec config to control qtable */
        mpp_enc_cfg_set_s32(cfg, "jpeg:q_factor", enc_params.qp_init ? enc_params.qp_init : 80);
        mpp_enc_cfg_set_s32(cfg, "jpeg:qf_max", enc_params.qp_max ? enc_params.qp_max : 99);
        mpp_enc_cfg_set_s32(cfg, "jpeg:qf_min", enc_params.qp_min ? enc_params.qp_min : 1);
    } break;
    default : {
    } break;
    }

    /* setup codec  */
    mpp_enc_cfg_set_s32(cfg, "codec:type", enc_params.type);
    switch (enc_params.type) {
    case MPP_VIDEO_CodingAVC : {
        RK_U32 constraint_set = enc_params.constraint_set;

        /*
         * H.264 profile_idc parameter
         * 66  - Baseline profile
         * 77  - Main profile
         * 100 - High profile
         */
        mpp_enc_cfg_set_s32(cfg, "h264:profile", 100);
        /*
         * H.264 level_idc parameter
         * 10 / 11 / 12 / 13    - qcif@15fps / cif@7.5fps / cif@15fps / cif@30fps
         * 20 / 21 / 22         - cif@30fps / half-D1@@25fps / D1@12.5fps
         * 30 / 31 / 32         - D1@25fps / 720p@30fps / 720p@60fps
         * 40 / 41 / 42         - 1080p@30fps / 1080p@30fps / 1080p@60fps
         * 50 / 51 / 52         - 4K@30fps
         */
        mpp_enc_cfg_set_s32(cfg, "h264:level", 40);
        mpp_enc_cfg_set_s32(cfg, "h264:cabac_en", 1);
        mpp_enc_cfg_set_s32(cfg, "h264:cabac_idc", 0);
        mpp_enc_cfg_set_s32(cfg, "h264:trans8x8", 1);

       //  mpp_env_get_u32("constraint_set", &constraint_set, 0);

        if (constraint_set & 0x3f0000)
            mpp_enc_cfg_set_s32(cfg, "h264:constraint_set", constraint_set);
    } break;
    case MPP_VIDEO_CodingHEVC :
    case MPP_VIDEO_CodingMJPEG :
    case MPP_VIDEO_CodingVP8 : {
    } break;
    default : {
        LOG_ERROR("unsupport encoder coding type %d\n", enc_params.type);
    } break;
    }
    //enc_params.split_mode = 0;
    //enc_params.split_arg = 0;
    //enc_params.split_out = 0;

    //mpp_env_get_u32("split_mode", &enc_params.split_mode, MPP_ENC_SPLIT_NONE);
    //mpp_env_get_u32("split_arg", &enc_params.split_arg, 0);
    //mpp_env_get_u32("split_out", &enc_params.split_out, 0);


    if (enc_params.split_mode) {
        LOG_DEBUG("%p split mode %d arg %d out %d\n", mpp_ctx,enc_params.split_mode, enc_params.split_arg, enc_params.split_out);
        mpp_enc_cfg_set_s32(cfg, "split:mode", enc_params.split_mode);
        mpp_enc_cfg_set_s32(cfg, "split:arg", enc_params.split_arg);
        mpp_enc_cfg_set_s32(cfg, "split:out", enc_params.split_out);
    }

    mpp_enc_cfg_set_s32(cfg, "prep:mirroring", enc_params.mirroring);
    mpp_enc_cfg_set_s32(cfg, "prep:rotation", enc_params.rotation);
    mpp_enc_cfg_set_s32(cfg, "prep:flip", enc_params.flip);

    // config gop_len and ref cfg
    mpp_enc_cfg_set_s32(cfg, "rc:gop", enc_params.gop_len ? enc_params.gop_len : enc_params.fps_out_num * 2);
#if 0
    RK_U32 gop_mode = enc_params.gop_mode;
    MppEncRefCfg ref = nullptr;
    if (gop_mode) {
     

        mpp_enc_ref_cfg_init(&ref);

        if (enc_params.gop_mode < 4)
            mpi_enc_gen_ref_cfg(ref, gop_mode);
        else
            mpi_enc_gen_smart_gop_ref_cfg(ref, enc_params.gop_len, enc_params.vi_len);

        mpp_enc_cfg_set_ptr(cfg, "rc:ref_cfg", ref);

       // ret = mpp_mpi->control(mpp_ctx, MPP_ENC_SET_REF_CFG, ref);
       // if (ret) {
       //     LOG_ERROR("mpi control enc set ref cfg failed ret %d\n", ret);
       //     goto RET;
      //  }
      //  mpp_enc_ref_cfg_deinit(&ref);
    }
#endif
    ret = mpp_mpi->control(mpp_ctx, MPP_ENC_SET_CFG, cfg);
    if (ret) {
        LOG_ERROR("mpi control enc set cfg failed ret %d\n", ret);
        goto RET;
    }
#if 0
    if (enc_params.type == MPP_VIDEO_CodingAVC || enc_params.type == MPP_VIDEO_CodingHEVC) {
        RcApiBrief rc_api_brief;
        rc_api_brief.type = enc_params.type;
        rc_api_brief.name = (enc_params.rc_mode == MPP_ENC_RC_MODE_SMTRC) ?
                            "smart" : "default";

        ret = mpi->control(mpp_ctx, MPP_ENC_SET_RC_API_CURRENT, &rc_api_brief);
        if (ret) {
            LOG_ERROR("mpi control enc set rc api failed ret %d\n", ret);
            goto RET;
        }
    }

    if (ref)
        mpp_enc_ref_cfg_deinit(&ref);
#endif


#if 0
    /* optional */
    {
        ret = mpp_mpi->control(mpp_ctx, MPP_ENC_SET_SEI_CFG, &enc_params.sei_mode);
        if (ret) {
            LOG_ERROR("mpi control enc set sei cfg failed ret %d\n", ret);
            goto RET;
        }
    }
#endif

    if (enc_params.type == MPP_VIDEO_CodingAVC || enc_params.type == MPP_VIDEO_CodingHEVC) {
        enc_params.header_mode = MPP_ENC_HEADER_MODE_EACH_IDR;
        ret = mpp_mpi->control(mpp_ctx, MPP_ENC_SET_HEADER_MODE, &enc_params.header_mode);
        if (ret) {
            LOG_ERROR("mpi control enc set header mode failed ret %d\n", ret);
            goto RET;
        }
    }

#if 0
    /* setup test mode by env */
    mpp_env_get_u32("osd_enable", &enc_params.osd_enable, 0);
    mpp_env_get_u32("osd_mode", &enc_params.osd_mode, MPP_ENC_OSD_PLT_TYPE_DEFAULT);
    mpp_env_get_u32("roi_enable", &enc_params.roi_enable, 0);
    mpp_env_get_u32("user_data_enable", &enc_params.user_data_enable, 0);
    if (enc_params.roi_enable) {
        mpp_enc_roi_init(&enc_params.roi_mpp_ctx, enc_params.width, enc_params.height, enc_params.type, 4);
        mpp_assert(enc_params.roi_mpp_ctx);
    }
#endif

RET:
    return ret;
}

MppEncoder::MppEncoder() {
    memset(&enc_params, 0, sizeof(MppEncoderParams));
    this->mpp_ctx = NULL;
    this->mpp_mpi = NULL;
    memset(&osd_data, 0, sizeof(MppEncOSDData));
#if FILEOUT
    fp_mpp = fopen("/home/rkmpp.h264", "w+b");
    if (nullptr == fp_mpp) {
            LOG_DEBUG("failed to open output file\n");
    }
#endif
}

MppEncoder::~MppEncoder() {
    if (this->mpp_ctx) {
        mpp_destroy(this->mpp_ctx);
        this->mpp_ctx = NULL;
    }

    if (this->cfg) {
        mpp_enc_cfg_deinit(this->cfg);
        this->cfg = NULL;
    }

    if (this->frm_buf) {
        mpp_buffer_put(this->frm_buf);
        this->frm_buf = NULL;
    }

    if (this->pkt_buf) {
        mpp_buffer_put(this->pkt_buf);
        this->pkt_buf = NULL;
    }

    if (this->md_info) {
        mpp_buffer_put(this->md_info);
        this->md_info = NULL;
    }

    if (this->osd_data.buf) {
        mpp_buffer_put(this->osd_data.buf);
        this->osd_data.buf = NULL;
    }

    if (this->buf_grp) {
        mpp_buffer_group_put(this->buf_grp);
        this->buf_grp = NULL;
    }
}

int MppEncoder::Init(MppEncoderParams& params, void* userdata) {
    int ret;
    MppPollType timeout = MPP_POLL_BLOCK;

    this->userdata = userdata;

    this->InitParams(params);

    ret = mpp_buffer_group_get_internal(&this->buf_grp, MPP_BUFFER_TYPE_DRM | MPP_BUFFER_FLAGS_CACHABLE);
    if (ret) {
        LOG_DEBUG("failed to get mpp buffer group ret %d\n", ret);
        goto MPP_TEST_OUT;
    }
    ret = mpp_buffer_get(this->buf_grp, &this->frm_buf, this->frame_size + this->header_size);
    if (ret) {
        LOG_DEBUG("failed to get buffer for input frame ret %d\n", ret);
        goto MPP_TEST_OUT;
    }

    ret = mpp_buffer_get(this->buf_grp, &this->pkt_buf, this->frame_size);
    if (ret) {
        LOG_DEBUG("failed to get buffer for output packet ret %d\n", ret);
        goto MPP_TEST_OUT;
    }

    ret = mpp_buffer_get(this->buf_grp, &this->md_info, this->mdinfo_size);
    if (ret) {
        LOG_DEBUG("failed to get buffer for motion info output packet ret %d\n", ret);
        goto MPP_TEST_OUT;
    }

    // encoder demo
    ret = mpp_create(&this->mpp_ctx, &this->mpp_mpi);
    if (ret) {
        LOG_ERROR("mpp_create failed ret %d\n", ret);
        goto MPP_TEST_OUT;
    }

    LOG_DEBUG("%p encoder test start w %d h %d type %d\n",
              this->mpp_ctx, enc_params.width, enc_params.height, enc_params.type);

    ret = mpp_mpi->control(mpp_ctx, MPP_SET_OUTPUT_TIMEOUT, &timeout);
    if (MPP_OK != ret) {
        LOG_ERROR("mpi control set output timeout %d ret %d\n", timeout, ret);
        goto MPP_TEST_OUT;
    }

    ret = mpp_init(mpp_ctx, MPP_CTX_ENC, enc_params.type);
    if (ret) {
        LOG_ERROR("mpp_init failed ret %d\n", ret);
        goto MPP_TEST_OUT;
    }

    this->SetupEncCfg();

MPP_TEST_OUT:
    if (ret) {
        if (this->mpp_ctx) {
            mpp_destroy(this->mpp_ctx);
            this->mpp_ctx = NULL;
        }

        if (this->cfg) {
            mpp_enc_cfg_deinit(this->cfg);
            this->cfg = NULL;
        }

        if (this->frm_buf) {
            mpp_buffer_put(this->frm_buf);
            this->frm_buf = NULL;
        }

        if (this->pkt_buf) {
            mpp_buffer_put(this->pkt_buf);
            this->pkt_buf = NULL;
        }

        if (this->md_info) {
            mpp_buffer_put(this->md_info);
            this->md_info = NULL;
        }

        if (this->osd_data.buf) {
            mpp_buffer_put(this->osd_data.buf);
            this->osd_data.buf = NULL;
        }

        if (this->buf_grp) {
            mpp_buffer_group_put(this->buf_grp);
            this->buf_grp = NULL;
        }

        // if (this->roi_ctx) {
        //     mpp_enc_roi_deinit(this->roi_ctx);
        //     this->roi_ctx = NULL;
        // }
    }

    return ret;
}

int MppEncoder::SetCallback(MppEncoderFrameCallback callback) {
    this->callback = callback;
    return 0;
}

int MppEncoder::GetHeader(char* enc_buf, int max_size) {
    int ret;
    void* out_ptr = enc_buf;
    size_t out_len = 0;

    if (enc_params.type == MPP_VIDEO_CodingAVC || enc_params.type == MPP_VIDEO_CodingHEVC) {
        MppPacket packet = NULL;

        /*
         * Can use packet with normal malloc buffer as input not pkt_buf.
         * Please refer to vpu_api_legacy.cpp for normal buffer case.
         * Using pkt_buf buffer here is just for simplifing demo.
         */
        mpp_packet_init_with_buffer(&packet, this->pkt_buf);
        /* NOTE: It is important to clear output packet length!! */
        mpp_packet_set_length(packet, 0);

        ret = mpp_mpi->control(mpp_ctx, MPP_ENC_GET_HDR_SYNC, packet);
        if (ret) {
            LOG_DEBUG("mpi control enc get extra info failed\n");
            return -1;
        } else {
            /* get and write sps/pps for H.264 */

            void *ptr   = mpp_packet_get_pos(packet);
            size_t len  = mpp_packet_get_length(packet);

            memcpy(out_ptr, ptr, len);
            out_ptr = (char*)(out_ptr) + len;
            out_len += len;
        }

        mpp_packet_deinit(&packet);
    }
    return out_len;
}

int MppEncoder::Encode(void* mpp_buf, char* enc_buf, int max_size) {
    MPP_RET ret;
    void* out_ptr = enc_buf;
    size_t out_len = 0;

    MppMeta meta = NULL;
    MppFrame frame = NULL;
    MppPacket packet = NULL;
    RK_U32 eoi = 1;
    RK_U32 frm_eos = 0;
  

    ret = mpp_frame_init(&frame);
    if (ret) {
        LOG_DEBUG("mpp_frame_init failed\n");
        return -1;
    }

    mpp_frame_set_width(frame, enc_params.width);
    mpp_frame_set_height(frame, enc_params.height);
    mpp_frame_set_hor_stride(frame, enc_params.hor_stride);
    mpp_frame_set_ver_stride(frame, enc_params.ver_stride);
    mpp_frame_set_fmt(frame, enc_params.fmt);
    mpp_frame_set_eos(frame, frm_eos);
    mpp_frame_set_buffer(frame, mpp_buf);

    meta = mpp_frame_get_meta(frame);
    mpp_packet_init_with_buffer(&packet, pkt_buf);
    /* NOTE: It is important to clear output packet length!! */
    mpp_packet_set_length(packet, 0);
    mpp_meta_set_packet(meta, KEY_OUTPUT_PACKET, packet);
    mpp_meta_set_buffer(meta, KEY_MOTION_INFO, this->md_info);

#if 0
    if (enc_params.osd_enable || enc_params.user_data_enable || enc_params.roi_enable) {
        if (enc_params.user_data_enable) {
            MppEncUserData user_data;
            char *str = "this is user data\n";

            if ((enc_params.frame_count & 10) == 0) {
                user_data.pdata = str;
                user_data.len = strlen(str) + 1;
                mpp_meta_set_ptr(meta, KEY_USER_DATA, &user_data);
            }
            static RK_U8 uuid_debug_info[16] = {
                0x57, 0x68, 0x97, 0x80, 0xe7, 0x0c, 0x4b, 0x65,
                0xa9, 0x06, 0xae, 0x29, 0x94, 0x11, 0xcd, 0x9a
            };

            MppEncUserDataSet data_group;
            MppEncUserDataFull datas[2];
            char *str1 = "this is user data 1\n";
            char *str2 = "this is user data 2\n";
            data_group.count = 2;
            datas[0].len = strlen(str1) + 1;
            datas[0].pdata = str1;
            datas[0].uuid = uuid_debug_info;

            datas[1].len = strlen(str2) + 1;
            datas[1].pdata = str2;
            datas[1].uuid = uuid_debug_info;

            data_group.datas = datas;

            mpp_meta_set_ptr(meta, KEY_USER_DATAS, &data_group);
        }

        if (enc_params.osd_enable) {
            /* gen and cfg osd plt */
            mpi_enc_gen_osd_plt(&enc_params.osd_plt, enc_params.frame_count);

            enc_params.osd_plt_cfg.change = MPP_ENC_OSD_PLT_CFG_CHANGE_ALL;
            enc_params.osd_plt_cfg.type = MPP_ENC_OSD_PLT_TYPE_USERDEF;
            enc_params.osd_plt_cfg.plt = &enc_params.osd_plt;

            ret = mpp_mpi->control(mpp_ctx, MPP_ENC_SET_OSD_PLT_CFG, &enc_params.osd_plt_cfg);
            if (ret) {
                LOG_DEBUG("mpi control enc set osd plt failed ret %d\n", ret);
                goto RET;
            }

            /* gen and cfg osd plt */
            mpi_enc_gen_osd_data(&enc_params.osd_data, enc_params.buf_grp, enc_params.width,
                                 enc_params.height, enc_params.frame_count);
            mpp_meta_set_ptr(meta, KEY_OSD_DATA, (void*)&enc_params.osd_data);
        }

        if (enc_params.roi_enable) {
            RoiRegionCfg *region = &enc_params.roi_region;

            /* calculated in pixels */
            region->x = MPP_ALIGN(enc_params.width / 8, 16);
            region->y = MPP_ALIGN(enc_params.height / 8, 16);
            region->w = 128;
            region->h = 256;
            region->force_intra = 0;
            region->qp_mode = 1;
            region->qp_val = 24;

            mpp_enc_roi_add_region(enc_params.roi_mpp_ctx, region);

            region->x = MPP_ALIGN(enc_params.width / 2, 16);
            region->y = MPP_ALIGN(enc_params.height / 4, 16);
            region->w = 256;
            region->h = 128;
            region->force_intra = 1;
            region->qp_mode = 1;
            region->qp_val = 10;

            mpp_enc_roi_add_region(enc_params.roi_mpp_ctx, region);

            /* send roi info by metadata */
            mpp_enc_roi_setup_meta(enc_params.roi_mpp_ctx, meta);
        }
    }
#endif

    /*
        * NOTE: in non-block mode the frame can be resent.
        * The default input timeout mode is block.
        *
        * User should release the input frame to meet the requirements of
        * resource creator must be the resource destroyer.
        */
    ret = mpp_mpi->encode_put_frame(mpp_ctx, frame);

    if (ret) {
        LOG_DEBUG("chn %d encode put frame failed\n", chn);
         mpp_frame_deinit(&frame);
        return -1;
    }
     mpp_frame_deinit(&frame);
    do {
        ret = mpp_mpi->encode_get_packet(mpp_ctx, &packet);
        if (ret) {
            LOG_DEBUG("chn %d encode get packet failed\n", chn);
            return -1;
        }

        // mpp_assert(packet);

        if (packet) {
            // write packet to file here
            void *ptr   = mpp_packet_get_pos(packet);
            size_t len  = mpp_packet_get_length(packet);
            char log_buf[256];
            RK_S32 log_size = sizeof(log_buf) - 1;
            RK_S32 log_len = 0;

            // if (!enc_params.first_pkt)
            //     enc_params.first_pkt = mpp_time();

           // RK_U32 pkt_eos = mpp_packet_get_eos(packet);

            /* set encode result */
            if (this->callback != nullptr) {
                this->callback(this->userdata, (const char*)ptr, len);
            }
#if FILEOUT
            if(fp_mpp != nullptr) {
             fwrite(ptr, 1,len,fp_mpp);
           }
#endif
            if (enc_buf != nullptr && max_size > 0) {
                if ((int)(out_len + log_len) < max_size) {
                    memcpy(out_ptr, ptr, len);
                    out_len += len;
                    out_ptr = (char*)out_ptr + len;
                } else {
                    LOG_ERROR("error enc_buf no enought");
                }
            }

            // log_len += snprintf(log_buf + log_len, log_size - log_len,
            //                     "encoded frame %-4d", enc_params.frame_count);

            /* for low delay partition encoding */
            if (mpp_packet_is_partition(packet)) {
                eoi = mpp_packet_is_eoi(packet);

                // log_len += snprintf(log_buf + log_len, log_size - log_len,
                //                     " pkt %d", enc_params.frm_pkt_cnt);
                // enc_params.frm_pkt_cnt = (eoi) ? (0) : (enc_params.frm_pkt_cnt + 1);
            }

            log_len += snprintf(log_buf + log_len, log_size - log_len,
                                " size %-7zu", len);

            if (mpp_packet_has_meta(packet)) {
                meta = mpp_packet_get_meta(packet);
                RK_S32 temporal_id = 0;
                RK_S32 lt_idx = -1;
                RK_S32 avg_qp = -1;
                RK_S32  bps_rt = -1;
                RK_S32 use_lt_idx = -1;
              //  RK_S64 sse = 0;
              //  RK_FLOAT psnr = 0;

                if (MPP_OK == mpp_meta_get_s32(meta, KEY_TEMPORAL_ID, &temporal_id))
                    log_len += snprintf(log_buf + log_len, log_size - log_len,
                                        " tid %d", temporal_id);

                if (MPP_OK == mpp_meta_get_s32(meta, KEY_LONG_REF_IDX, &lt_idx))
                    log_len += snprintf(log_buf + log_len, log_size - log_len,
                                        " lt %d", lt_idx);

                if (MPP_OK == mpp_meta_get_s32(meta, KEY_ENC_AVERAGE_QP, &avg_qp))
                    log_len += snprintf(log_buf + log_len, log_size - log_len,
                                        " qp %d", avg_qp);

                if (MPP_OK == mpp_meta_get_s32(meta, KEY_ENC_BPS_RT, &bps_rt))
                        log_len += snprintf(log_buf + log_len, log_size - log_len,
                                            " bps_rt %d", bps_rt);

                if (MPP_OK == mpp_meta_get_s32(meta, KEY_ENC_USE_LTR, &use_lt_idx))
                        log_len += snprintf(log_buf + log_len, log_size - log_len, " vi");

              //  if (MPP_OK == mpp_meta_get_s64(meta, KEY_ENC_SSE, &sse)) {
              //          psnr = 3.01029996 * (psnr_const - log2(sse));
              //          log_len += snprintf(log_buf + log_len, log_size - log_len,
             //                               " psnr %.4f", psnr);
            }

            LOG_DEBUG("chn %d %s\n", chn, log_buf);

            mpp_packet_deinit(&packet);

            // enc_params.stream_size += len;
            // enc_params.frame_count += eoi;

             //if (enc_params.pkt_eos) {
              //  LOG_DEBUG("chn %d found last packet\n", chn);
                 //mpp_assert(enc_params.frm_eos);
            // }
        }
    } while (!eoi);

    // if (enc_params.frm_eos && enc_params.pkt_eos)
    //     break;
    return out_len;
}

int MppEncoder::Reset() {
    if (mpp_mpi != NULL) {
        mpp_mpi->reset(mpp_ctx);
    }
    return 0;
}

size_t MppEncoder::GetFrameSize() {
    return this->frame_size;
}

void* MppEncoder::ImportBuffer(int index, size_t size, int fd, int type) {
    MppBuffer buf;
    MppBufferInfo info;
    memset(&info, 0, sizeof(MppBufferInfo));
    info.type = (MppBufferType)type; // MPP_BUFFER_TYPE_EXT_DMA
    info.fd =  fd;
    info.size = size;
    info.index = index;
    mpp_buffer_import(&buf, &info);
    return buf;
}

void* MppEncoder::GetInputFrameBuffer() {
    int ret;
    if (this->frm_buf == nullptr) {
        ret = mpp_buffer_get(this->buf_grp, &this->frm_buf, this->frame_size);
        if (ret) {
            LOG_DEBUG("failed to get buffer for input frame ret %d\n", ret);
            return NULL;
        }
    }
    return this->frm_buf;
}

int MppEncoder::GetInputFrameBufferFd(void* mpp_buffer) {
    return mpp_buffer_get_fd(mpp_buffer);
}

void* MppEncoder::GetInputFrameBufferAddr(void* mpp_buffer) {
    return mpp_buffer_get_ptr(mpp_buffer);
}
