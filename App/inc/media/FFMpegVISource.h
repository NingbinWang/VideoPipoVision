
#pragma once 
#include <string>
#include <queue>
#include <stdint.h>
#include "media/Encoder.h"
#include "Logger.h"
#include "net/MediaSource.h"



class FFMpegVISource : public MediaSource
{
public:
    static FFMpegVISource* createNew(UsageEnvironment* env, std::string dev);
    
    FFMpegVISource(UsageEnvironment* env, const std::string& dev);
    virtual ~FFMpegVISource();

protected:
    virtual void readFrame();

private:
    UsageEnvironment* mEnv;
    std::string mDev;
    AVFormatContext *mFmtCtx;
};