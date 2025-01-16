
#pragma once 
#include <string>
#include <queue>
#include <stdint.h>
#include "Media.h"
#include "Logger.h"
#include "net/MediaSource.h"



class MediaVISource : public MediaSource
{
public:
    static MediaVISource* createNew(UsageEnvironment* env, std::string dev);
    
    MediaVISource(UsageEnvironment* env, const std::string& dev);
    virtual ~MediaVISource();

protected:
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
    MediaVi *mVi;
    std::queue<Nalu> mNaluQueue;
};