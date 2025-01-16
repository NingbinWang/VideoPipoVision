#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "libavdevice/avdevice.h"

#include "media/FFMpegVISource.h"

#include "base/New.h"
#include "Logger.h"

FFMpegVISource* FFMpegVISource::createNew(UsageEnvironment* env, std::string dev)
{
    return New<FFMpegVISource>::allocate(env, dev);
}

FFMpegVISource::FFMpegVISource(UsageEnvironment* env, const std::string& dev) :
    MediaSource(env),
    mEnv(env),
    mDev(dev)
{
    char* input_name= "video4linux2";
    const char* in_devname = mDev.c_str();
    AVInputFormat *inputFmt;
    avcodec_register_all();    
    avdevice_register_all(); 
    inputFmt = av_find_input_format(input_name); 
    if (inputFmt == NULL)    {        
        LOG_DEBUG("can not find_input_format\n");         
    } 
    if (avformat_open_input ( &mFmtCtx, in_devname, inputFmt, NULL) < 0){
        LOG_DEBUG("can not open_input_file\n");        
    }
	/* print device information*/
	av_dump_format(mFmtCtx, 0, in_devname, 0);

    for(int i = 0; i < DEFAULT_FRAME_NUM; ++i)
        mEnv->threadPool()->addTask(mTask);
}

FFMpegVISource::~FFMpegVISource()
{
   avformat_close_input(&mFmtCtx);
}

void FFMpegVISource::readFrame()
{
    MutexLockGuard mutexLockGuard(mMutex);
    LOG_DEBUG("start readFrame \n");
    if(mAvFrameInputQueue.empty())
        return;
    Encoder *encoder = new Encoder(AV_CODEC_ID_H264,1920,1080,30,12);
    encoder->open(true);
    AvFrame* frame = mAvFrameInputQueue.front();
    if(mNaluQueue.empty())
    {
        AVPacket *packet;
        int nalNum = 0;
        
        while(1)
        {
            packet = (AVPacket *)av_malloc(sizeof(AVPacket)); 
            av_read_frame(mFmtCtx, packet);
            LOG_DEBUG("data length:%d\n",packet->size);
            bool encode(av_frame frame, packet);
            encoder->encode()
           


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