#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "media/MediaVISource.h"
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
    param.image_viH = 1080;
    param.image_viW = 1920;
    param.frame_rate = 1;
    mWidth = 1920;
    mHeight = 1080;
    setFps(1);
    MediaVi *vi = new MediaVi(param);
    ret = vi->initdev(in_devname);
    assert(ret == true);
    mVi = vi;
    ret = x264Init();
    assert(ret == true);
    for(int i = 0; i < DEFAULT_FRAME_NUM; ++i)
        mEnv->threadPool()->addTask(mTask);
}

MediaVISource::~MediaVISource()
{
    x264Exit();
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
            ret = mVi->poll();
            if(ret == false)
                return; 

            size =  mVi->readFramebuf(buffer,FRAME_MAX_SIZE);
            if(size < 0)
            {
                LOG_WARNING("don't have framebuf\n");
                return;
            }
            LOG_DEBUG("readFramebuf size= %d\n",size);
            memcpy(mPicIn->img.plane[0], buffer, size);
            mPicIn->i_pts = mPts++;

            ret = x264_encoder_encode(mX264Handle, &mNals, &nalNum, mPicIn, mPicOut);
            if(ret< 0)
            {
                LOG_WARNING("failed to encode data\n");
                return;
            }
            
            for(int i = 0; i < nalNum; ++i)
                mNaluQueue.push(Nalu(mNals[i].p_payload, mNals[i].i_payload));

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

bool MediaVISource::x264Init()
{
	mNals = NULL;
	mX264Handle = NULL;

    mPicIn = New<x264_picture_t>::allocate();
	mPicOut = New<x264_picture_t>::allocate();
	mParam = New<x264_param_t>::allocate();
    
    mCsp = X264_CSP_NV12;

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

bool MediaVISource::x264Exit()
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
