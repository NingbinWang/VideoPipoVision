#pragma once 
#include <string>
#include <queue>
#include <stdint.h>
#include "Media.h"
#include "Logger.h"

#define V4L2_MAX_SIZE (1024*1024*50)
class V4L2VISource 
{
public:
    V4L2VISource( const std::string& dev);
    ~V4L2VISource();
    size_t readFrame(char *outputbuf);

private:
    char *mOutputbuf=nullptr;
    std::string mDev;
    MediaVi *mVi;
    int mWidth;
    int mHeight;
    char *mFramebuf=nullptr;
};

