#ifndef _SYSMUTEX_H_
#define _SYSMUTEX_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Common.h"

#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/

/* ========================================================================== */
/*                             Type Declaration                               */
/* ========================================================================== */

/**
 *  @enum   MUTEX_TYPE_E
 *  @brief  mutex type
 */
typedef enum
{
    MUTEX_NORMAL = 0,
    MUTEX_RELOCK = 1
} MUTEX_TYPE_E;

/**
 *  @struct   SYS_MUTEX_STATE_T
 *  @brief    mutex state
 */
typedef struct {
    CHAR    strName[32];           /**< 锁的名字 */
    CHAR    strHolder[32];         /**< 锁的占有者 */
    UINT32  uTakeCount;            /**< 获取锁的次数 */
    UINT32  uGiveCount;            /**< 给与锁的次数 */
    UINT32  uTakeFailedCount;      /**< 获取锁失败次数 */
    UINT32  uTakeTimeoutCount;     /**< 获取锁超时次数 */
} SYS_MUTEX_STATE_T;

/** MUTEX_ID 仅仅一个指针值，具体指向什么各平台内部处理*/
typedef LONG  MUTEX_ID;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * @brief      创建互斥锁
 * @param[in]  pStMutex  互斥锁句柄指针
 * @param[in]  iType     互斥锁类型  MUTEX_TYPE_E
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E
 */
INT32 SysMutex_create(MUTEX_ID *pStMutex, INT32 iType);

/**
 * @brief      加锁
 * @param[in]  pStMutex   互斥锁句柄
 * @param[in]  iWaitMs    等待时间, 单位ms
 * @return     成功返回 0  错误返回 其他, 参考ERROR_CODE_E
 */
INT32 SysMutex_lock(MUTEX_ID *pStMutex, INT32 iWaitMs);

/**
 * @brief      解锁
 * @param[in]  pStMutex   互斥锁句柄
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E
 */
INT32 SysMutex_unlock(MUTEX_ID *pStMutex);

/**
 * @brief      销毁锁句柄
 * @param[in]  pStMutex     互斥锁句柄
 * @return     成功返回 0 @return 错误返回 其他, 参考ERROR_CODE_E
 */
INT32 SysMutex_destroy(MUTEX_ID *pStMutex);

/**
 * @brief      创建互斥锁 (指令互斥锁名称)
 * @param[in]  pStMutex  互斥锁句柄指针
 * @param[in]  strName   互斥锁名称
 * @param[in]  iType     互斥锁类型  MUTEX_TYPE_E
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E
 */
INT32 SysMutex_create_with_name(MUTEX_ID *pStMutex, 
                        const CHAR *strName, INT32 iType);

/**
 * @brief       获取创建的互斥对象的个数
 * @return      创建的互斥对象的数目
 */
INT32 SysMutex_number (VOID);

/**
 * @brief       获取指定互斥锁的状态
 * @param[in]   pHandle      先前创建的互斥量实例对象的句柄
 * @param[out]  pState       指向互斥量状态结构体的指针, @ref SYS_MUTEX_STATE_T 
 * @return      返回 0 表示成功，负值表示失败
 */
INT32 SysMutex_state(MUTEX_ID *pHandle, SYS_MUTEX_STATE_T *pState);

/**
 * @brief       获取所有互斥量的状态
 * @param[out]  pStatus      指向互斥量状态结构体数组的指针, @ref SYS_MUTEX_STATE_T 
 * @return      当成功时返回互斥对象的数目,负值表示失败
 */
INT32 SysMutex_status(SYS_MUTEX_STATE_T *pStatus);

#ifdef __cplusplus
}
#endif/*__cplusplus*/
#endif/* SYS_MUTEX_H */

