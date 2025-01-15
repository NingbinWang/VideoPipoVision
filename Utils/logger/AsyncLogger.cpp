#include <stdio.h>
#include <assert.h>

#include "AsyncLogger.h"
#include "Logger.h"


AsyncLogger* AsyncLogger::mAsyncLogger = NULL;

AsyncLogger::AsyncLogger(std::string file) :
    mFile(file),
    mRun(true)
{
    
    pthread_mutex_init(&mMutex, NULL);
   // assert(mMutex);


    pthread_cond_init(&mCond, NULL);
   // assert(mCond);

    mFp = fopen(mFile.c_str(), "w");
    assert(mFp >= 0);

    for(int i = 0; i < BUFFER_NUM; ++i)
        mFreeBuffer.push(&mBuffer[i]);

    mCurBuffer = mFreeBuffer.front();

   // start(NULL);
}

AsyncLogger::~AsyncLogger()
{
    for(int i = 0; i < mFlushBuffer.size(); ++i)
    {
        LogBuffer* buffer = mFlushBuffer.front();
        fwrite(buffer->data(), 1, buffer->length(), mFp);
        mFlushBuffer.pop();
    }

    fwrite(mCurBuffer->data(), 1, mCurBuffer->length(), mFp);

    fflush(mFp);
    fclose(mFp);

    mRun = false;
    pthread_cond_broadcast(&mCond);

    //delete mMutex;
    //delete mCond;
     pthread_mutex_destroy(&mMutex);
     pthread_cond_destroy(&mCond);
}

AsyncLogger* AsyncLogger::instance()
{
    if(!mAsyncLogger)
    {
        mAsyncLogger = new AsyncLogger(Logger::getLogFile());
    }

    return mAsyncLogger;
}
void AsyncLogger::append(const char* logline, int len)

{
    //MutexLockGuard mutexLockGuard(mMutex);
    pthread_mutex_lock(&mMutex);
    if(mCurBuffer->avail() > len)
    {
        mCurBuffer->append(logline, len);
    }
    else
    {
        mFreeBuffer.pop();
        mFlushBuffer.push(mCurBuffer);

        /* 如果缓存区已经用完，那么就睡眠等待 */
        while(mFreeBuffer.empty())
        {
            //mCond->signal();
            pthread_cond_signal(&mCond);
            //mCond->wait(mMutex);
            pthread_cond_wait(&mCond,&mMutex);
            
        }
        
        mCurBuffer = mFreeBuffer.front();
        mCurBuffer->append(logline, len);
        //mCond->signal();
        pthread_cond_signal(&mCond);
    }
}

void AsyncLogger::run(void *arg)
{
    while(mRun)
    {
        struct timespec abstime;
        struct timespec now;
        bool ret = false;
       // MutexLockGuard mutexLockGuard(mMutex);
        pthread_mutex_lock(&mMutex);
        clock_gettime(CLOCK_REALTIME, &now);
        abstime.tv_sec = now.tv_sec + 3000/1000;
        abstime.tv_nsec = now.tv_nsec + 3000%1000*1000*1000;
        if(pthread_cond_timedwait(&mCond,&mMutex, &abstime) == 0)
            ret = true;
        else
            ret = false;
       // bool ret = mCond->waitTimeout(mMutex, 3000);

        if(mRun == false)
            break;

        if(ret == true) //signal
        {
            bool empty = mFreeBuffer.empty();
            int bufferSize = mFlushBuffer.size();
            for(int i = 0; i < bufferSize; ++i)
            {
                LogBuffer* buffer = mFlushBuffer.front();
                fwrite(buffer->data(), 1, buffer->length(), mFp);
                mFlushBuffer.pop();
                buffer->reset();
                mFreeBuffer.push(buffer);
                fflush(mFp);
            }

            if(empty)
                pthread_cond_signal(&mCond);
                //mCond->signal();
        }
        else //timeout
        {
            if(mCurBuffer->length() == 0)
                continue;
            fwrite(mCurBuffer->data(), 1, mCurBuffer->length(), mFp);
            mCurBuffer->reset();
            fflush(mFp);
        }
    }
}