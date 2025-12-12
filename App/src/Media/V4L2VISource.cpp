#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "Media/V4L2VISource.h"
#include "Base/New.h"
#include "Logger.h"

V4L2VISource::V4L2VISource( const std::string& dev) :
    mDev(dev)
{
    bool ret;
    VI_CFG_PARAM_T param;
    ENC_STATUS_T enc_status;
    const char* in_devname = mDev.c_str();
    param.vSensorType = CMOS_OV_5969;
    param.image_viH = 1080;
    param.image_viW = 1920;
    param.frame_rate = 30;
    param.eType = VI_V4L2;
//MediaVi
    mVi = new MediaVi(param);
    ret = mVi->initdev(in_devname);
    assert(ret == true);
    LOG_DEBUG("V4L2VISource OK\n");
}

V4L2VISource::~V4L2VISource()
{
  
}

size_t V4L2VISource::readFrame(char *outputbuf)
{
    bool ret;
    while(1)
    {
        size_t size = 0;
        void * vibuf = nullptr;
        ret = mVi->poll();
        if(ret == false)
                continue;
		LOG_INFO("read a Frame  outputbuf = %p\n",outputbuf);
        size = mVi->readFramebuf(outputbuf,V4L2_MAX_SIZE);
        if(size < 0)
        {
                LOG_WARNING("don't have framebuf\n");
                return size;
        }
        return size;
    }
  
}

