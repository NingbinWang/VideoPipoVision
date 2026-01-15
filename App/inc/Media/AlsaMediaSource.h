#pragma once 
#include <string>
#include <queue>
#include <stdint.h>
#include "Media.h"
#include "Logger.h"

class AlsaMediaSource 
{
public:
    AlsaMediaSource( const std::string& dev);
    ~AlsaMediaSource();

private:
    char *mOutputbuf=nullptr;
    char *mFramebuf=nullptr;
};

