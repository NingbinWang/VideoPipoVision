#include "media_priv.h"
#include "V4l2Device.h"
#include "V4l2Capture.h"
#include "logger.h"



// -----------------------------------------
//    V4L2Device
// -----------------------------------------
Media_vi::Media_vi(const VI_CFG_PARAM_T&  params) : m_params(params)
{
}

Media_vi::~Media_vi() 
{
	
}

bool Media_vi::init(){
    const char *in_devname = "/dev/video0";	
	V4l2IoType ioTypeIn  = IOTYPE_MMAP;
	int format = 0;
	int width = 0;
	int height = 0;
	int fps = 0;
    if(m_params.vSensorType == CMOS_OV_5969)
    {
        format = V4l2Device::fourcc("NV12");
        width = m_params.image_viW;
        height = m_params.image_viH;
        fps =m_params.frame_rate;
    }
    V4L2DeviceParameters param(in_devname, format, width, height, fps, ioTypeIn);
	V4l2Capture* videoCapture = V4l2Capture::create(param,V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE);
    if (videoCapture == NULL)
	{	
		LOG(WARN) << "Cannot reading from V4L2 capture interface for device:" << in_devname; 
	}
    return true;
}
