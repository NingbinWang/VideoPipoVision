#pragma once
#include <string>
#include <queue>
#include <string.h>
#include <pthread.h>

class LogBuffer
{
public:
    LogBuffer() :
        mCurPtr(mData)
    { }

    ~LogBuffer()
    { }

    void append(const char* buf, size_t len)
    {
        if (avail() > len)
        {
            memcpy(mCurPtr, buf, len);
            mCurPtr += len;
        }
    }

    const char* data() const { return mData; }
    int length() const { return (int)(mCurPtr - mData); }

    char* current() { return mCurPtr; }
    int avail() const { return (int)(end() - mCurPtr); }
    void add(int len) { mCurPtr += len; }

    void reset() { mCurPtr = mData; }
    void bzero() { memset(mData, 0, BUFFER_SIZE); }

private:
    enum
    {
        BUFFER_SIZE = 1024*1024,
    };

    const char* end() const { return mData + BUFFER_SIZE; }

private:
    char mData[BUFFER_SIZE];
    char* mCurPtr;
};

class AsyncLogger
{
public:
    virtual ~AsyncLogger();

    static AsyncLogger* instance();

    void append(const char* logline, int len);
 
protected:
    AsyncLogger(std::string file);
    virtual void run(void *arg);

private:
    enum
    {
        BUFFER_NUM = 4,
    };

    pthread_mutex_t mMutex;
    pthread_cond_t mCond;
    std::string mFile;
    FILE* mFp;
    bool mRun;

    LogBuffer mBuffer[BUFFER_NUM];
    LogBuffer* mCurBuffer;
    std::queue<LogBuffer*> mFreeBuffer;
    std::queue<LogBuffer*> mFlushBuffer;

    static AsyncLogger* mAsyncLogger;
};

