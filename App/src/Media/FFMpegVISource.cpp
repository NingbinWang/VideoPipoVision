#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "Media/FFMpegVISource.h"
#include "Base/New.h"
#include "Logger.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <libavutil/opt.h>
#include <libavutil/avtime.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/avstring.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#ifdef __cplusplus
}
#endif



FFMpegVISource* FFMpegVISource::createNew(UsageEnvironment* env, std::string dev)
{
    return New<FFMpegVISource>::allocate(env, dev);
}

FFMpegVISource::FFMpegVISource(UsageEnvironment* env, const std::string& dev) :
    MediaSource(env),
    mEnv(env),
    mDev(dev)
{
   
    bool ret;
   
    ret = videoInit();
    assert(ret == true);



    for(int i = 0; i < DEFAULT_FRAME_NUM; ++i)
        mEnv->threadPool()->addTask(mTask);
}

FFMpegVISource::~FFMpegVISource()
{

}



void FFMpegVISource::readFrame()
{
    MutexLockGuard mutexLockGuard(mMutex);
    LOG_DEBUG("start readFrame \n");
    if(mAvFrameInputQueue.empty())
        return;

    AvFrame* frame = mAvFrameInputQueue.front();
    if(mNaluQueue.empty())
    {
      //  AVPacket *packet;
        int nalNum = 0;
        
        while(1)
        {
        //    packet = (AVPacket *)av_malloc(sizeof(AVPacket)); 
       //     av_read_frame(mFmtCtx, packet);
        //    LOG_DEBUG("data length:%d\n",packet->size);
        //    bool encode(av_frame frame, packet);
      //      encoder->encode()
           


            if(nalNum > 0)
                break;
        }
    }

    Nalu nalu = mNaluQueue.front();
    mNaluQueue.pop();

    memcpy(frame->mBuffer, nalu.mData, nalu.mSize);
  
    mAvFrameInputQueue.pop();
    mAvFrameOutputQueue.push(frame);
}


bool FFMpegVISource::videoInit()
{
    int ret;
    const char* in_devname = mDev.c_str();
    const AVInputFormat 	*inputFmt;
    AVDictionary *options; //摄像头相关参数
    int videoindex= -1;
   av_log_set_flags(AV_LOG_INFO);
   #if CONFIG_AVDEVICE
    avdevice_register_all();
   #endif
   avformat_network_init();
   mFmtCtx = avformat_alloc_context();
   inputFmt = av_find_input_format("v4l2");
   if (inputFmt == nullptr)    {        
        LOG_DEBUG("can not find_input_format\n");        
        return false;    
    }    
    // 摄像头支持多种参数，因此使用option 指定参数 最大支持到9帧
    av_dict_set(&options, "video_size", "1920*1080", 0);
    av_dict_set(&options, "framerate", "30", 0);
    // av_dict_set(&options, "input_format", "yuyv422", 0);
    ret = avformat_open_input ( &mFmtCtx, in_devname, inputFmt, &options);
    if ( ret < 0){
        LOG_DEBUG("can not open_input_file\n");  
        return false;    
    }
    ret = avformat_find_stream_info(mFmtCtx, nullptr);
    if (ret < 0)
    {
        LOG_DEBUG("findding stream info\n");
        return false;
    }
    // 查找视频流
	for(size_t i=0; i<mFmtCtx->nb_streams; i++){
        if(mFmtCtx->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_VIDEO){
			videoindex=i;
			break;
		}
    }
		
	if(videoindex==-1){
		LOG_DEBUG("Didn't find a video stream.\n");
		return false;
	}
    return true;
}
/*
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