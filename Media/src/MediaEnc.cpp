#include "MediaEnc.h"
#include "MppEncoder.h"
#include "Logger.h"
MppEncoder * encoder = nullptr;
MediaEnc::MediaEnc(const ENC_PARAM_T&  params) : m_params(params)
{
   // char userdata[64] = {0};
    MppEncoderParams encoderparam = {0};
    encoderparam.width = m_params.viW;

    encoder = new MppEncoder();
    encoder->Init(encoderparam,nullptr);

}

MediaEnc::~MediaEnc() 
{
	
}
