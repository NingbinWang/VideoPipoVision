#include "Media.h"
#include "MppEncoder.h"
#include "Logger.h"
MppEncoder * encoder = nullptr;

MediaEnc::MediaEnc(const ENC_STATUS_T&  status) : m_status(status)
{
   // char userdata[64] = {0};
    MppEncoderParams encoderparam = {0};
    encoderparam.width = m_status.viW;
    encoderparam.height = m_status.viH;
    encoderparam.fmt = MPP_FMT_YUV422_UYVY;
    encoderparam.type = MPP_VIDEO_CodingAVC;
    encoderparam.rc_mode =  MPP_ENC_RC_MODE_CBR;
    //encoderparam.bps = 
    encoder = new MppEncoder();
    encoder->Init(encoderparam,nullptr);
}

MediaEnc::~MediaEnc() 
{
	delete encoder;
}

int MediaEnc::Encode(void* mpp_buf, char* enc_buf, int max_size)
{
   return encoder->Encode(mpp_buf,enc_buf,max_size);
}
