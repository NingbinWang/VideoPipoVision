#include "sys_common.h"
#include "sys_mem.h"
#include "sys_mqueue.h"
#include "sys_sem.h"
#include <math.h>
#include <semaphore.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include "sys_log.h"
/**
 * @fn         sys_timespec_get
 * @brief      Get the time spec struct.
 * @param[in]  iWaitMs   wait_ms - NO_WAIT, or max wait time(ms)
 * @param[out] tp        time spec struct
 * @return     无
 */
static inline VOID sys_timespec_get(INT32 iWaitMs, struct timespec *tp)
{
    time_t        sec;
    long long nsec;
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
    // PR_INF("getTimespec: current time sec = %ld, time = %ld, nsec = %ld, total nsec = %lld\n",
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
    // PR_INF("getTimespec: after time sec = %ld, time = %ld, nsec = %ld\n",
    //         tp->tv_sec, time(NULL)+1, tp->tv_nsec);

    return;
}

/**
 * @fn         sys_sem_create
 * @brief      创建信号量
 * @param[in]  pStSem        信号量句柄指针
 * @param[in]  strName       信号量名称
 * @param[in]  uInitValue    信号量初始值
 * @return     成功返回 0    错误返回 其他, 参考ERROR_CODE_E
 */
INT32 sys_sem_create(SEM_ID *pStSem, const CHAR *strName, UINT32 uInitValue)
{
    INT32 iRet = 0;

    if (pStSem == NULL)
    {
        PR_ERR("Sem init invalid ptr.\n");
        return ERROR;
    }

    *pStSem = (SEM_ID)sys_mem_malloc(sizeof(sem_t));
    if ((sem_t *)*pStSem == NULL)
    {
        return ERROR;
    }

    iRet = sem_init((sem_t *)*pStSem, 0, uInitValue);
    if (iRet != 0)
    {
        PR_ERR("Sem init error:%s ret:%d-%s \n", strerror(errno), iRet, strerror(iRet));
    }

    return iRet;
}

/**
 * @brief  释放一个信号量资源
 * @param  pStSem   信号量句柄指针
 * @return 成功返回 0
 * @return 错误返回 其他
 */
INT32 sys_sem_post(SEM_ID *pStSem)
{
    if (pStSem == NULL || (sem_t *)*pStSem == NULL)
    {
        PR_ERR("Sem post invalid ptr.\n");
        return ERROR;
    }

    return sem_post((sem_t *)*pStSem);
}

/**
 * @brief  获取一个信号量资源
 * @param  pStSem     信号量句柄指针
 * @param  uWaitTime 阻塞时长
 * @return 成功返回 0
 * @return 错误返回 其他
 */
INT32 sys_sem_wait(SEM_ID *pStSem, UINT32 uWaitTime)
{
    INT32           iRet      = 0;
    struct timespec stAbsTime = {0};

    if (pStSem == NULL || (sem_t *)*pStSem == NULL)
    {
        PR_ERR("Sem wait invalid ptr.\n");
        return ERROR;
    }

    if (uWaitTime == NO_WAIT)
    {
        iRet = sem_trywait((sem_t *)*pStSem);
    }
    else if (uWaitTime == WAIT_FOREVER)
    {
        iRet = sem_wait((sem_t *)*pStSem);
    }
    else
    {
        sys_timespec_get(uWaitTime, &stAbsTime);
        iRet = sem_timedwait((sem_t *)*pStSem, &stAbsTime);
    }

    return iRet;
}

/**
 * @brief  销毁信号量
 * @param  pStSem   信号量句柄指针
 * @return 成功返回 0
 * @return 错误返回 其他
 */
INT32 sys_sem_destroy(SEM_ID *pStSem)
{
    INT32 iRet = 0;

    if (pStSem == NULL || (sem_t *)*pStSem == NULL)
    {
        PR_ERR("Sem destroy invalid ptr.\n");
        return ERROR;
    }

    if ((iRet = sem_destroy((sem_t *)*pStSem)) != 0)
    {
        PR_ERR("Sem destroy error:%s ret:%d-%s \n", strerror(errno), iRet, strerror(iRet));
        return iRet;
    }

    sys_mem_free((sem_t *)*pStSem);
    *pStSem = (SEM_ID)NULL;

    return OK;
}