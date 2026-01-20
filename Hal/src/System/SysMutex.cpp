/**
********************************************************************************
* @file         sys_mutex_freertos.c
* @brief        operation system mutex abstraction interface implement for posix
*
* @author       guoyongxu
* @version      1.0.0
* @since        2022-08-29
* @copyright    Hangzhou HikAuto Technology Co., Ltd.
* @note         
* @par changelog:
* <table>
* <tr><th>Date        <th>Version  <th>Author     <th>Description
* <tr><td>2022/08/29  <td>1.0.0    <td>guoyongxu  <td>Initial release
* </table>
********************************************************************************
*/

#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include "SysObject.h"
#include "Logger.h"
#include "SysMutex.h"
#include "SysErrno.h"


static pthread_mutex_t stObjPoolMutex = PTHREAD_MUTEX_INITIALIZER;

void* _sys_mutex_create_with_name (const char* name, INT32 iType)
{
    INT32 ret;
    const char* mutex_name; 
    SysMutex_t* mutex;
    pthread_mutexattr_t attr;

    mutex_name = name ? name : "anonymous_mutex";

    pthread_mutex_lock (&stObjPoolMutex);

    mutex = (SysMutex_t*)SysObject_allocate (SYSOBJECT_CLASS_MUTEX, mutex_name);
    if (!mutex) {
        pthread_mutex_unlock (&stObjPoolMutex);
        LOG_ERROR ("Failed to allocate mutex object\n");
        return NULL;            
    }

    pthread_mutex_unlock (&stObjPoolMutex);
    
    pthread_mutexattr_init(&attr);

    ret = pthread_mutexattr_settype(&attr, iType);
    if (ret) {
        LOG_ERROR ("Mutex set type error:%s ret %d\n", 
                strerror(errno), ret);
        SysObject_delete((SYSOBJECT_T*)mutex);
        return NULL;
    }

    ret = pthread_mutex_init(&mutex->mutex, &attr);
    if (ret) {
        LOG_ERROR ("Failed to init mutex, ret %d - errno[%d:%s]\n", ret, errno, strerror(errno));
        SysObject_delete((SYSOBJECT_T*)mutex);
        return NULL;
    }

    return mutex;   
}

INT32 _sys_mutex_lock (void* pMutex, INT32 iWaitMs)
{
    INT32 ret;
    UINT64 sec, msec, nsec;
    struct timespec ts;
    SYSOBJECT_T* object = (SYSOBJECT_T*)pMutex;
    SysMutex_t* mutex = (SysMutex_t*)pMutex;

    if (!mutex || object->type != SYSOBJECT_CLASS_MUTEX)  {
        LOG_ERROR ("Invaild params for mutex %p\n",
            mutex);
        return -EINVAL;
    }

    if (iWaitMs == NO_WAIT)
        return pthread_mutex_trylock(&mutex->mutex);
    else if (iWaitMs == WAIT_FOREVER)
        return pthread_mutex_lock(&mutex->mutex);
    else {
#if __GLIBC_MINOR__ >= 30
        clock_gettime(CLOCK_MONOTONIC, &ts);
#else
        clock_gettime(CLOCK_REALTIME, &ts);
#endif

        sec  = iWaitMs / 1000;
        msec = iWaitMs % 1000;
        nsec = msec * 1000 * 1000 + ts.tv_nsec;
        
        ts.tv_sec  += (sec + nsec / (1000 * 1000 * 1000));
        ts.tv_nsec = nsec % (1000 * 1000 * 1000);

#if __GLIBC_MINOR__ >= 30
        ret = pthread_mutex_clocklock(&mutex->mutex, CLOCK_MONOTONIC, &ts);
#else
        ret = pthread_mutex_timedlock(&mutex->mutex, &ts);
#endif
        if (ret) {
            LOG_ERROR ("Pthread mutex lock timeout, ret %d - errno[%d:%s]\n", 
                ret, errno, strerror(errno));
            return -ETIMEDOUT;
        }
    }

    SysPthread_get_name(mutex->strHolder, sizeof(mutex->strHolder)); 

    return 0;
}

INT32 _sys_mutex_unlock (void* pMutex)
{
    SYSOBJECT_T* object = (SYSOBJECT_T*)pMutex;
    SysMutex_t* mutex = (SysMutex_t*)pMutex;

    if (!mutex || object->type != SYSOBJECT_CLASS_MUTEX)  {
        LOG_ERROR ("Invaild params for mutex %p\n",
            mutex);
        return -EINVAL;
    }
    strcpy(mutex->strHolder, "none");

    return pthread_mutex_unlock(&mutex->mutex);
}

INT32 _sys_mutex_destroy (void* pMutex)
{
    SYSOBJECT_T* object = (SYSOBJECT_T*)pMutex;
    SysMutex_t* mutex = (SysMutex_t*)pMutex;

    if (!mutex || object->type != SYSOBJECT_CLASS_MUTEX)  {
        LOG_ERROR ("Invaild params for mutex %p\n",
            mutex);
        return -EINVAL;
    }

    pthread_mutex_lock (&stObjPoolMutex);

    SysObject_delete(object);

    pthread_mutex_unlock (&stObjPoolMutex);
    
    return 0;
}

INT32 SysMutex_create_with_name(MUTEX_ID *pStMutex, 
                    const char* name, INT32 iType)
{
    SysMutex_t* mutex;

    if (!pStMutex) {
        LOG_ERROR (" Invaild params of pStMutex %p\n", pStMutex);
        return -EINVAL;
    }

    mutex = (SysMutex_t*)_sys_mutex_create_with_name(name, iType);
    if (!mutex)
        return -ENODEV;
    
    *pStMutex = (MUTEX_ID)mutex;

    return 0;   
}

INT32 SysMutex_create(MUTEX_ID *pStMutex, INT32 iType)
{   
    return SysMutex_create_with_name(pStMutex, NULL, iType);
}

INT32 SysMutex_destroy(MUTEX_ID *pStMutex)
{
    if (!pStMutex) {
        LOG_ERROR("invalid pStMutex(%p)\n", pStMutex);
        return -EINVAL;
    }

    return _sys_mutex_destroy((void *)*pStMutex);
}

INT32 SysMutex_lock(MUTEX_ID *pStMutex, INT32 iWaitMs)
{
    if (!pStMutex) {
        LOG_ERROR("invalid pStMutex(%p)\n", pStMutex);
        return -EINVAL;
    }

    return _sys_mutex_lock ((void *)*pStMutex, iWaitMs);
}

INT32 SysMutex_unlock(MUTEX_ID *pStMutex)
{
    if (!pStMutex) {
        LOG_ERROR("invalid pStMutex(%p)\n", pStMutex);
        return -EINVAL;
    }

    return _sys_mutex_unlock ((void *)*pStMutex);
}

INT32 sSysMutex_number (void)
{
    return SysObject_number (SYSOBJECT_CLASS_MUTEX);
}

INT32 SysMutex_status(SYS_MUTEX_STATE_T* pStatus)
{
    UINT32 uLoopCnt = 0u;
    SysMutex_t* pMutex = NULL;

    if (!pStatus) 
        return -EINVAL;

    pthread_mutex_lock (&stObjPoolMutex);

    for_each_object (pMutex, SYSOBJECT_CLASS_MUTEX) {
        strcpy(pStatus[uLoopCnt].strName, pMutex->object.name);
        strcpy(pStatus[uLoopCnt].strHolder, pMutex->strHolder);
        uLoopCnt++;
    }

    pthread_mutex_unlock (&stObjPoolMutex);

    return uLoopCnt;
}


