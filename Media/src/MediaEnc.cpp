#include "MediaPriv.h"
#include "MppEncoder.h"


MppEncoder * encoder = nullptr;

int MediaEncEncode(void* mpp_buf, char* enc_buf, int max_size)
{
   return encoder->Encode(mpp_buf,enc_buf,max_size);
}


int MediaEncGetHeader(char* enc_buf, int max_size)
{
   return encoder->GetHeader(enc_buf,max_size);
}

int MediaEncSetCallback(MediaEncCallback callback )
{
   return encoder->SetCallback((MppEncoderFrameCallback)callback);
}

int MediaEncInit(const ENC_STATUS_T* status)
{
   // char userdata[64] = {0};
    MppEncoderParams encoderparam;
    memset(&encoderparam,0,sizeof(MppEncoderParams));
    encoderparam.width = (RK_U32)status->viW;
    encoderparam.height = (RK_U32)status->viH;
    encoderparam.fmt = MPP_FMT_YUV420SP;
    //encoderparam.fmt = MPP_FMT_BGRA8888;
    encoderparam.type = MPP_VIDEO_CodingAVC;
    encoderparam.rc_mode =  MPP_ENC_RC_MODE_BUTT;
    encoder = new MppEncoder();
    encoder->Init(encoderparam,nullptr);
    return 0;
}

void MediaEncDeInit()
{
   delete encoder;
}

size_t MediaEncGetFrameSize()
{
   return encoder->GetFrameSize();
}

void* MediaEncGetInputFrame()
{
   return encoder->GetInputFrameBuffer();
}

int MediaEncGetInputFrameBufferFd(void * source)
{
   return encoder->GetInputFrameBufferFd(source);
}

void* MediaEncGetInputFrameBufferAddr(void * source)
{
   return encoder->GetInputFrameBufferAddr(source);
}