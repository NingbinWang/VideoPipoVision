
#pragma once 
#include <string>
#include <queue>
#include <stdint.h>
#include "Media.h"
#include "Logger.h"
#include "Net/MediaSource.h"


#define MPPENCOERSIZE 5*1024*1024
class MppVISource : public MediaSource
{
public:
    static MppVISource* createNew(UsageEnvironment* env, std::string dev);
    
    MppVISource(UsageEnvironment* env, const std::string& dev);
    virtual ~MppVISource();

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
    char *mOutputbuf=nullptr;
    std::string mDev;
    MediaVi *mVi;
    int mframe_size;
    std::queue<Nalu> mNaluQueue;
};