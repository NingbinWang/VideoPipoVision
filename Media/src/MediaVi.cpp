#include "MediaPriv.h"
#include "V4l2Device.h"
#include "V4l2Capture.h"
#include "New.h"

V4l2Capture* pCapture = NULL;

MediaVi* MediaVi::createNew(VI_CFG_PARAM_T&pParams)
{
    //return new RtspServer(env, addr);
    return New<MediaVi>::allocate(&pParams);
}
MediaVi::MediaVi(VI_CFG_PARAM_T *pParams) :
mpParams(pParams)
{
	if(mpParams->u32Enable == FALSE){
		return;
	}
	if(mpParams->eViType == VI_V4L2){
		INT32 i32Format =  V4l2Device::fourcc("NV12");
		V4L2DeviceParameters param(mpParams->strDevname,i32Format, mpParams->u32Image_viW, mpParams->u32Image_viH,mpParams->u32Frame_rate, IOTYPE_MMAP);
        pCapture = V4l2Capture::create(param,V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE);
        if (pCapture == NULL)
        {
             LOG_WARNING("Cannot reading from V4L2 capture interface for device:%s\n",mpParams->strDevname);
             return;
        }
        LOG_DEBUG("V4L2 capture OK! device:%s\n",mpParams->strDevname);
	}
}

MediaVi::~MediaVi() 
{
	
}

int MediaVi::readFramebuf(char* buffer, int bufferSize)
{
    int ret = -1 ;
    ret = pCapture->read(buffer,bufferSize);
    return ret;
}
#ifdef MEDIARKMPP
void * MediaVi::readtomppbuf(int* index)
{
    return  pCapture->readtobuf(index);
}

 bool MediaVi::readputmppbuf(int index)
{
   return pCapture->readputbuf(index);
}
#endif
bool MediaVi::poll()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return pCapture->isReadable(&tv);
}