#include "sys_common.h"
#include "sys_mem.h"
#include "sys_mutex.h"
#include "sys_log.h"
#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>


/**
 * @fn         sys_timespec_get
 * @brief      Get the time spec struct.
 * @param[in]  iWaitMs   wait_ms - NO_WAIT, or max wait time(ms)
 * @param[out] tp        time spec struct
 * @return     无
 */
static inline VOID sys_timespec_get(INT iWaitMs, struct timespec *tp)
{
    time_t        sec;
    long long int nsec;
    if (iWaitMs == NO_WAIT)
    {
        nsec = 0;
    }
    else
    {
        nsec = iWaitMs;
        nsec *= 1000000LL;
    }

#ifdef USE_NPTL
    tp->tv_sec = 0;
#else
    if (clock_gettime(CLOCK_REALTIME, tp) == -1)
    {
        // PR_ERR("getTimespec: clock_gettime call fail, error %d(%s)\n", errno, strerror(errno));
        tp->tv_sec  = time(NULL) + 1;
        tp->tv_nsec = 0;
    }
    else
    {
        sec = time(NULL) + 1;
        if (labs(tp->tv_sec - sec) > 30)
        {
            tp->tv_sec  = sec;
            tp->tv_nsec = 0;
        }
    }

    nsec += tp->tv_nsec;
    // SYS_COMMON_INFO("getTimespec: current time sec = %ld, time = %ld, nsec = %ld, total nsec = %lld\n",
    //         tp->tv_sec, time(NULL)+1, tp->tv_nsec, nsec);
#endif

    sec = 0;
    if (nsec >= NSEC2SEC)
    {
        sec  = nsec / NSEC2SEC;
        nsec = nsec % NSEC2SEC;
    }
    tp->tv_sec += sec;
    tp->tv_nsec = nsec;
    // SYS_COMMON_INFO("getTimespec: after time sec = %ld, time = %ld, nsec = %ld\n",
    //         tp->tv_sec, time(NULL)+1, tp->tv_nsec);

    return;
}

/**
 * @fn         sys_mutex_create
 * @brief      创建互斥锁
 * @param[in]  pStMutex  互斥锁句柄指针
 * @param[in]  iType     互斥锁类型  MUTEX_TYPE_E
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E
 */
INT sys_mutex_create(MUTEX_ID *pStMutex, INT iType)
{
    INT               iRet  = 0;
    pthread_mutexattr_t mAttr = {0};

    if (pStMutex == NULL)
    {
        PR_ERR("Mutex init invalid ptr");
        return ERROR;
    }

    *pStMutex = (MUTEX_ID)sys_mem_malloc(sizeof(pthread_mutex_t));
    if ((pthread_mutex_t *)*pStMutex == NULL)
    {
        return ERROR;
    }

    pthread_mutexattr_init(&mAttr);
    // pthread_mutexattr_settype(&mAttr,(type==0)?PTHREAD_MUTEX_ADAPTIVE_NP:PTHREAD_MUTEX_RECURSIVE_NP);
    iRet = pthread_mutexattr_settype(&mAttr, iType);
    if (iRet != 0)
    {
        PR_ERR("Mutex set type error:%s ret:%d-%s \n", strerror(errno), iRet, strerror(iRet));
        return iRet;
    }

    iRet = pthread_mutex_init((pthread_mutex_t *)*pStMutex, (const pthread_mutexattr_t *)&mAttr);
    if (iRet != 0)
    {
        PR_ERR("Mutex init error:%s ret:%d-%s \n", strerror(errno), iRet, strerror(iRet));
    }

    return iRet;
}

/**
 * @fn         sys_mutex_lock
 * @brief      加锁
 * @param[in]  pStMutex   互斥锁句柄
 * @param[in]  uWaitTime  等待时间, 单位ms
 * @return     成功返回 0  错误返回 其他, 参考ERROR_CODE_E
 */
INT sys_mutex_lock(MUTEX_ID *pStMutex, INT iWaitMs)
{
    INT           iRet      = 0;
    struct timespec stAbsTime = {0};

    if (pStMutex == NULL || (pthread_mutex_t *)*pStMutex == NULL)
    {
        PR_ERR("Mutex lock invalid ptr. \n");
        return ERROR;
    }

    if (iWaitMs == NO_WAIT)
    {
        iRet = pthread_mutex_trylock((pthread_mutex_t *)*pStMutex);
    }
    else if (iWaitMs == WAIT_FOREVER)
    {
        iRet = pthread_mutex_lock((pthread_mutex_t *)*pStMutex);
    }
    else
    {
        /*TODO J6平台设置超时时间在校时时会导致等待时间和预计的不一致*/
        sys_timespec_get(iWaitMs, &stAbsTime);
        iRet = pthread_mutex_timedlock((pthread_mutex_t *)*pStMutex, &stAbsTime);
    }

    return iRet;
}

/**
 * @fn         sys_mutex_unlock
 * @brief      解锁
 * @param[in]  pStMutex   互斥锁句柄
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E
 */
INT sys_mutex_unlock(MUTEX_ID *pStMutex)
{
    if (pStMutex == NULL || (pthread_mutex_t *)*pStMutex == NULL)
    {
        PR_ERR("Mutex unlock invalid ptr. \n");
        return ERROR;
    }

    return pthread_mutex_unlock((pthread_mutex_t *)*pStMutex);
}

/**
 * @fn         sys_mutex_destroy
 * @brief      销毁锁句柄
 * @param[in]  pStMutex     互斥锁句柄
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E
 */
INT sys_mutex_destroy(MUTEX_ID *pStMutex)
{
    INT iRet = 0;

    if (pStMutex == NULL || (pthread_mutex_t *)*pStMutex == NULL)
    {
        PR_ERR("Mutex destroy invalid ptr. \n");
        return ERROR;
    }

    if ((iRet = pthread_mutex_destroy((pthread_mutex_t *)*pStMutex)) != 0)
    {
        PR_ERR("Mutex destroy error:%s ret:%d-%s \n", strerror(errno), iRet, strerror(iRet));
        return iRet;
    }

    sys_mem_free((pthread_mutex_t *)*pStMutex);
    *pStMutex = (MUTEX_ID)NULL;

    return OK;
}