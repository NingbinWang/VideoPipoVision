#include "MediaVi.h"
#include "V4l2Device.h"
#include "V4l2Capture.h"
#include "Logger.h"
#include <time.h>
#include <sys/time.h>
 V4l2Capture* videoCapture = NULL;
MediaVi::MediaVi(const VI_CFG_PARAM_T&  params) : m_params(params)
{
}

MediaVi::~MediaVi() 
{
	
}

bool MediaVi::initdev(const char *in_devname)
{
	V4l2IoType ioTypeIn  = IOTYPE_MMAP;
	int format = 0;
	int width = 0;
	int height = 0;
	int fps = 0;
     LOG_DEBUG("V4L2 initdev:%s\n",in_devname);
    if(m_params.vSensorType == CMOS_OV_5969)
    {
        format = V4l2Device::fourcc("NV12");
        ioTypeIn =  IOTYPE_MMAP;
        width = m_params.image_viW;
        height = m_params.image_viH;
        fps =m_params.frame_rate;
   }else if(m_params.vSensorType == CMOS_OV_8858)
   {
        format = V4l2Device::fourcc("NV12");
        ioTypeIn =  IOTYPE_MMAP;
        width = m_params.image_viW;
        height = m_params.image_viH;
        fps =m_params.frame_rate;
   }
   if(m_params.eType == VI_V4L2){
        V4L2DeviceParameters param(in_devname, format, width, height, fps, ioTypeIn);
        videoCapture = V4l2Capture::create(param,V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE);
        if (videoCapture == NULL)
        {
             LOG_WARNING("Cannot reading from V4L2 capture interface for device:%s\n",in_devname);
             return false;
        }
        LOG_DEBUG("V4L2 capture OK! device:%s\n",in_devname);
   }
   
    return true;
}

int MediaVi::readFramebuf(char* buffer, int bufferSize)
{
    int ret = -1 ;
    LOG_DEBUG("readFramebuf \n");
    ret = videoCapture->read(buffer,bufferSize);
    LOG_DEBUG("readFramebuf OK\n");
    return ret;
}
#ifdef MEDIARKMPP
void * MediaVi::readtomppbuf(int* index)
{
    return  videoCapture->readtobuf(index);
}

 bool MediaVi::readputmppbuf(int index)
{
   return videoCapture->readputbuf(index);
}
#endif
bool MediaVi::poll()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return videoCapture->isReadable(&tv);
}