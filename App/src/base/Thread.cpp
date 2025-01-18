#include "Thread.h"

Thread::Thread() :
    mArg(nullptr),
    mIsStart(false),
    mIsDetach(false)
{

}

Thread::~Thread()
{
    if(mIsStart == true && mIsDetach == false)
        detach();
}
    
bool Thread::start(void *arg)
{
    mArg = arg;

    if(pthread_create(&mThreadId, nullptr, threadRun, this))
        return false;

    mIsStart = true;
    return true;
}

bool Thread::detach()
{
    if(mIsStart != true)
        return false;

    if(mIsDetach == true)
        return true;

    if(pthread_detach(mThreadId))
        return false;

    mIsDetach = true;

    return true;
}

bool Thread::join()
{
    if(mIsStart != true || mIsDetach == true)
        return false;

    if(pthread_join(mThreadId, nullptr))
        return false;
    
    return true;
}

bool Thread::cancel()
{
    if(mIsStart != true)
        return false;

    if(pthread_cancel(mThreadId))
        return false;
    
    mIsStart = false;

    return true;
}

pthread_t Thread::getThreadId() const
{
    return mThreadId;
}

void *Thread::threadRun(void *arg)
{
    Thread* thread = (Thread*)arg;
    thread->run(thread->mArg);
    return nullptr;
}
