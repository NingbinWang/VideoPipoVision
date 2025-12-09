
#pragma once 
#include <string>
#include <queue>
#include <stdint.h>
#include "Media.h"
#include "Logger.h"
#include "Net/MediaSource.h"
#include "Media/FFMpegVISource.h"
#include "Media/Encoder.h"



class FFMpegVISource : public MediaSource
{
public:
    static FFMpegVISource* createNew(UsageEnvironment* env, std::string dev);
    
    FFMpegVISource(UsageEnvironment* env, const std::string& dev);
    virtual ~FFMpegVISource();

protected:
    bool videoInit();
    virtual void readFrame();

private:
    struct Nalu
    {
        Nalu(uint8_t* data, int size) : mData(data), mSize(size)
        { }

        uint8_t* mData;
        int mSize;
    };



private:
    UsageEnvironment* mEnv;
    std::string mDev;
    std::queue<Nalu> mNaluQueue;
    AVFormatContext* mFmtCtx;
};