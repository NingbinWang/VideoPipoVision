#ifndef _SYS_MUTEX_H_
#define _SYS_MUTEX_H_
#include "sys_common.h"

#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/
/* timeout options */


typedef enum
{
    MUTEX_NORMAL = 0,
    MUTEX_RELOCK = 1,
}MUTEX_TYPE_E;

/* MUTEX_ID 仅仅一个指针值，具体指向什么各平台内部处理*/
typedef LONG  MUTEX_ID;

/**@fn         sys_mutex_create
 * @brief      创建互斥锁
 * @param[in]  pStMutex  互斥锁句柄指针
 * @param[in]  iType     互斥锁类型  MUTEX_TYPE_E
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E
 */
INT sys_mutex_create(MUTEX_ID *pStMutex, INT iType);

/**@fn         sys_mutex_lock
 * @brief      加锁
 * @param[in]  pStMutex   互斥锁句柄
 * @param[in]  uWaitTime  等待时间, 单位ms
 * @return     成功返回 0  错误返回 其他, 参考ERROR_CODE_E
 */
INT sys_mutex_lock(MUTEX_ID *pStMutex, INT iWaitMs);

/**@fn         sys_mutex_unlock
 * @brief      解锁
 * @param[in]  pStMutex   互斥锁句柄
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E
 */
INT sys_mutex_unlock(MUTEX_ID *pStMutex);

/**@fn         sys_mutex_destroy
 * @brief      销毁锁句柄
 * @param[in]  pStMutex     互斥锁句柄
 * @return     成功返回 0 @return 错误返回 其他, 参考ERROR_CODE_E
 */
INT sys_mutex_destroy(MUTEX_ID *pStMutex);

#ifdef __cplusplus
}
#endif/*__cplusplus*/
#endif/*_SYS_MUTEX_H_*/