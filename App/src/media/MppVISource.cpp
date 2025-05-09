#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "media/MppVISource.h"
#include "base/New.h"
#include "Logger.h"

#define MPPFILEOUT 1 //the fileout use for debug 
#if MPPFILEOUT
#include <fstream>
#include <iostream>
FILE* fp_output = nullptr;
#endif

MppVISource* MppVISource::createNew(UsageEnvironment* env, std::string dev)
{
    //return new V4l2MediaSource(env, dev);
    return New<MppVISource>::allocate(env, dev);
}

MppVISource::MppVISource(UsageEnvironment* env, const std::string& dev) :
    MediaSource(env),
    mEnv(env),
    mDev(dev)
{
    bool ret;
    VI_CFG_PARAM_T param;
    ENC_STATUS_T enc_status;
    const char* in_devname = mDev.c_str();
    param.vSensorType = CMOS_OV_8858;
    param.image_viH = 1080;
    param.image_viW = 1920;
    param.frame_rate = 15;
    param.eType = VI_V4L2;
    setFps(15);
//MediaVi
    mVi = new MediaVi(param);
    ret = mVi->initdev(in_devname);
    assert(ret == true);
//MediaEnc
    enc_status.viW = 1920;
    enc_status.viH = 1080;
    MediaEncInit(&enc_status);
    this->mOutputbuf = (char *)malloc(FRAME_MAX_SIZE);
#if MPPFILEOUT
    fp_output = fopen("/home/mytest.h264", "w+b");
    if (nullptr == fp_output) {
            LOG_DEBUG("failed to open output file\n");
    }
#endif
    for(int i = 0; i < DEFAULT_FRAME_NUM; ++i)
       mEnv->threadPool()->addTask(mTask);
    LOG_DEBUG("MppVISource OK\n");
}

MppVISource::~MppVISource()
{
    if(this->mOutputbuf != nullptr)
        free(this->mOutputbuf);

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

void MppVISource::readFrame()
{
    MutexLockGuard mutexLockGuard(mMutex);
    char * framebuf = nullptr;
    size_t size = 0;
    if(mAvFrameInputQueue.empty())
        return;
    AvFrame* frame = mAvFrameInputQueue.front();
   
    if(mNaluQueue.empty())
    {
        bool ret;
        while(1)
        {
            void * vibuf = nullptr;
            int index = -1;
            ret = mVi->poll();
            if(ret == false)
                continue;
#ifdef MEDIARKMPP
            vibuf =  mVi->readtomppbuf(&index);
            if(vibuf == nullptr)
            {
                LOG_WARNING("don't have framebuf\n");
                return;
            }
#endif
#ifdef MEDIARKAI
            IMAGE_FRAME_T srcimg = {0};
            size_t mpp_frame_size =MediaEncGetFrameSize();
            void* mpp_frame_addr = MediaEncGetInputFrameBufferAddr(vibuf);
            int mpp_frame_fd = MediaEncGetInputFrameBufferFd(vibuf);
            //LOG_DEBUG("MediaEncGetInputFrame mpp_frame_size: %d mpp_frame_fd:%d mpp_frame_addr:%p\n",mpp_frame_size,mpp_frame_fd,mpp_frame_addr);
            srcimg.width =  1920;
            srcimg.height = 1080;
            srcimg.width_stride = 1920;
            srcimg.height_stride =  1080;
            srcimg.virt_addr = (char *)mpp_frame_addr;
            srcimg.fd = mpp_frame_fd;
            DETECT_RESULT_GROUP_T result = {0};
            MediaAi_VideoReport(&srcimg,&result);
            //void* mppbuffer = MediaEncGetInputFrame();
            MediaAi_VideoDrawRect(&srcimg,&result);
            //MediaAi_VideoDrawobj(&srcimg,&result,mppbuffer);
#endif
           size =  MediaEncEncode(vibuf,this->mOutputbuf,MPPENCOERSIZE);
#if MPPFILEOUT
           if(fp_output != nullptr) {
             fwrite(this->mOutputbuf, 1, size,fp_output);
           }
#endif
#ifdef MEDIARKMPP
           mVi->readputmppbuf(index);
#endif
           framebuf = (char *)malloc(size);
           memcpy(framebuf, this->mOutputbuf, size);
           memset(this->mOutputbuf,0,size);
           mNaluQueue.push(Nalu((uint8_t*)framebuf, size));
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
    free(nalu.mData);
    mAvFrameInputQueue.pop();
    mAvFrameOutputQueue.push(frame);
}

