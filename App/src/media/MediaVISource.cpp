#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "media/MediaVISource.h"
#include "media/Encoder.h"
#include "base/New.h"
#include "Logger.h"

MediaVISource* MediaVISource::createNew(UsageEnvironment* env, std::string dev)
{
    //return new V4l2MediaSource(env, dev);
    return New<MediaVISource>::allocate(env, dev);
}

MediaVISource::MediaVISource(UsageEnvironment* env, const std::string& dev) :
    MediaSource(env),
    mEnv(env),
    mDev(dev)
{
    bool ret;
    VI_CFG_PARAM_T param;
    const char* in_devname = mDev.c_str();
    param.vSensorType = CMOS_OV_5969;
    param.image_viH = 1944;
    param.image_viW = 2592;
    param.frame_rate = 30;
    
    MediaVi *vi = new MediaVi(param);
    LOG_DEBUG("init media dev %s\n",mDev);
    ret = vi->initdev(in_devname);
    assert(ret == true);

    mVi = vi;
  
    for(int i = 0; i < DEFAULT_FRAME_NUM; ++i)
        mEnv->threadPool()->addTask(mTask);
}

MediaVISource::~MediaVISource()
{
     delete mVi;
}

static inline int startCode3(uint8_t* buf)
{
    if(buf[0] == 0 && buf[1] == 0 && buf[2] == 1)
        return 1;
    else
        return 0;
}

static inline int startCode4(uint8_t* buf)
{
    if(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 1)
        return 1;
    else
        return 0;
}

void MediaVISource::readFrame()
{
    MutexLockGuard mutexLockGuard(mMutex);
    LOG_DEBUG("start readFrame \n");
    if(mAvFrameInputQueue.empty())
        return;

    AvFrame* frame = mAvFrameInputQueue.front();
    if(mNaluQueue.empty())
    {
        bool ret;
        int nalNum = 0;
        
        while(1)
        {
            char buffer[FRAME_MAX_SIZE] = {0};
            size_t size = 0; 
            size =  mVi->readFramebuf(buffer,FRAME_MAX_SIZE);


            if(nalNum > 0)
                break;
        }
    }

    Nalu nalu = mNaluQueue.front();
    mNaluQueue.pop();

    memcpy(frame->mBuffer, nalu.mData, nalu.mSize);
    if(startCode3(nalu.mData))
    {
        frame->mFrame = frame->mBuffer+3;
        frame->mFrameSize = nalu.mSize-3;
    }
    else
    {
        frame->mFrame = frame->mBuffer+4;
        frame->mFrameSize = nalu.mSize-4;
    }

    mAvFrameInputQueue.pop();
    mAvFrameOutputQueue.push(frame);
}

/*
bool V4l2MediaSource::videoInit()
{
    int ret;
    char devName[100];
    struct v4l2_capability cap;

    mFd = v4l2_open(mDev.c_str(), O_RDWR);
    if(mFd < 0)
        return false;

    ret = v4l2_querycap(mFd, &cap);
    if(ret < 0)
        return false;

    if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
        return false;
    
    ret = v4l2_enuminput(mFd, 0, devName);
    if(ret < 0)
        return false;

    ret = v4l2_s_input(mFd, 0);
    if(ret < 0)
        return false;
    
    ret = v4l2_enum_fmt(mFd, V4L2_PIX_FMT_YUYV, V4L2_BUF_TYPE_VIDEO_CAPTURE);
    if(ret < 0)
        return false;
    
    ret = v4l2_s_fmt(mFd, &mWidth, &mHeight, V4L2_PIX_FMT_YUYV, V4L2_BUF_TYPE_VIDEO_CAPTURE);
    if(ret < 0)
        return false;
    
    mV4l2Buf = v4l2_reqbufs(mFd, V4L2_BUF_TYPE_VIDEO_CAPTURE, 4);
    if(!mV4l2Buf)
        return false;
    
    ret = v4l2_querybuf(mFd, mV4l2Buf);
    if(ret < 0)
        return false;
    
    ret = v4l2_mmap(mFd, mV4l2Buf);
    if(ret < 0)
        return false;
    
    ret = v4l2_qbuf_all(mFd, mV4l2Buf);
    if(ret < 0)
        return false;

    ret = v4l2_streamon(mFd);
    if(ret < 0)
        return false;
    
    ret = v4l2_poll(mFd);
    if(ret < 0)
        return false;
    
    return true;
}

bool V4l2MediaSource::videoExit()
{
    int ret;

    ret = v4l2_streamoff(mFd);
    if(ret < 0)
        return false;

    ret = v4l2_munmap(mFd, mV4l2Buf);
    if(ret < 0)
        return false;

    ret = v4l2_relbufs(mV4l2Buf);
    if(ret < 0)
        return false;

    v4l2_close(mFd);

    return true;
}

bool V4l2MediaSource::x264Init()
{
	mNals = NULL;
	mX264Handle = NULL;
	//mPicIn = new x264_picture_t;
	//mPicOut = new x264_picture_t;
	//mParam = new x264_param_t;
    
    mPicIn = New<x264_picture_t>::allocate();
	mPicOut = New<x264_picture_t>::allocate();
	mParam = New<x264_param_t>::allocate();
    
    mCsp = X264_CSP_YUYV;

    x264_param_default(mParam);
	mParam->i_width   = mWidth;
    mParam->i_height  = mHeight;
	mParam->i_keyint_max = mFps;
	mParam->i_fps_num  = mFps;
	mParam->i_csp=mCsp;

	mX264Handle = x264_encoder_open(mParam);
	if(!mX264Handle)
	{
		LOG_ERROR("failed to open x264 encoder\n");
		return false;
	}

	x264_picture_init(mPicOut);
    x264_picture_alloc(mPicIn, mCsp, mWidth, mHeight);

    return true;
}

bool V4l2MediaSource::x264Exit()
{
    x264_picture_clean(mPicIn);
    x264_encoder_close(mX264Handle);

    //delete mPicIn;
    //delete mPicOut;
    //delete mParam;
    Delete::release(mPicIn);
    Delete::release(mPicOut);
    Delete::release(mParam);

    return true;
}
*/