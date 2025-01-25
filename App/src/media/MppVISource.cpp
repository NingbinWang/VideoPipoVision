#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "media/MppVISource.h"
#include "base/New.h"
#include "Logger.h"

#include <fstream>
#include <iostream>

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
    param.vSensorType = CMOS_OV_5969;
    param.image_viH = 1080;
    param.image_viW = 1920;
    param.frame_rate = 30;
    param.eType = VI_V4L2;
    setFps(30);
//MediaVi
    mVi = new MediaVi(param);
    ret = mVi->initdev(in_devname);
    assert(ret == true);
//MediaEnc
    enc_status.viW = 1920;
    enc_status.viH = 1080;

    mEncoder = new MediaEnc(enc_status);

   this->mOutputbuf = (char *)malloc(FRAME_MAX_SIZE);


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
    FILE* fp_output = NULL;
     LOG_DEBUG("MppVISource readFrame\n");
    if(mAvFrameInputQueue.empty())
        return;
    AvFrame* frame = mAvFrameInputQueue.front();
    fp_output = fopen("/home/test.h264", "w+b");
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
            vibuf =  mVi->readtomppbuf(&index);
            if(vibuf == nullptr)
            {
                LOG_WARNING("don't have framebuf\n");
                return;
            }
             //this->mOutputbuf = (char *)malloc(1024*1024*5);
           LOG_DEBUG("readtomppbuf index=%d\n",index);
           size_t size =  mEncoder->Encode(vibuf,this->mOutputbuf,1024*1024*1);
           LOG_DEBUG("readputmppbuf %d\n",index);
           mVi->readputmppbuf(index);
           framebuf = (char *)malloc(size);
           memcpy(framebuf, this->mOutputbuf, size);
           fwrite(framebuf, 1, size, fp_output);
           memset(this->mOutputbuf,0,size);
           //LOG_DEBUG("h264 output %10x size= %d\n",this->mOutputbuf,size);
           // mEncoder->GetHeader(this->mOutputbuf,FRAME_MAX_SIZE);
            mNaluQueue.push(Nalu((uint8_t*)framebuf, size));
            LOG_DEBUG("MppVISource readFrame end .....\n");
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
    LOG_DEBUG("readFrame OK\n");
}

