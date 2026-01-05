#ifndef SYS_MQUEUE_H
#define SYS_MQUEUE_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Common.h"


/* ========================================================================== */
/*                             Type Declaration                               */
/* ========================================================================== */

/** MSG_Q_ID 仅仅一个指针值，具体指向什么各平台内部处理*/
typedef LONG  MSG_Q_ID;

/**
 *  @struct   SYS_MQUEUE_STATE_T
 *  @brief    mqueue state for debug
 */
typedef struct {
    CHAR    strName[32];            /**< 消息队列的名字 */
    UINT32  uFlags;                 /**< 阻塞标志 */
    UINT32  uMsgMax;                /**< 消息队列最大消息数 */
    UINT32  uMsgSize;               /**< 消息长度 */
    UINT32  uCurMsg;                /**< 队列中的消息数量 */
} SYS_MQUEUE_STATE_T;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * @brief      创建一个消息队列
 * @param[in]  pStHandle 消息队列对象，入参，需非空
 * @param[in]  strName   消息队列名字
 * @param[in]  iMaxMsg   一个消息队列最大消息个数
 * @param[in]  iMsgLen   消息长度
 * @return     成功返回 0, 并且handle被修改;其他失败，参考ERROR_CODE_E
 */
INT32 SysMqueue_create(MSG_Q_ID *pStHandle,const CHAR *strName, INT32 iMaxMsg, INT32 iMsgLen);

/**
 * @brief      发送消息
 * @param[in]  pStHandle  消息队列句柄
 * @param[in]  strMsg     发送的消息
 * @param[in]  uLength    消息长度
 * @param[in]  iWaitMs    阻塞时长
 * @return     成功返回 0  错误返回 其他, 参考ERROR_CODE_E
 */
INT32 SysMqueue_send(MSG_Q_ID *pStHandle, const CHAR *strMsg, UINT32 uLength, INT32 iWaitMs);

/**
 * @brief      接受消息
 * @param[in]  pStHandle  消息队列句柄
 * @param[in]  strBuff    接受消息缓存
 * @param[in]  iBuffLen   缓存大小
 * @param[in]  iWaitMs    阻塞时长
 * @return     成功返回 0  错误返回 其他, 参考ERROR_CODE_E
 */
INT32 SysMqueue_recv(MSG_Q_ID *pStHandle, CHAR *strBuff, INT32 iBuffLen, INT32 iWaitMs);

/**
 * @brief      关闭消息队列
 * @param[in]  pStHandle 消息队列句柄
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E
 */
INT32 SysMqueue_close(MSG_Q_ID *pStHandle);

/**
 * @brief      flush消息队列
 * @param[in]  pStHandle 消息队列句柄
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E
 */
INT32 SysMqueue_flush(MSG_Q_ID *pStHandle);

/**
 * @brief      查询消息队列
 * @param[in]  pStHandle       消息队列句柄
 * @param[out] puRemainCount   剩余消息个数
 * @param[out] puRemainBuff    剩余缓存大小
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E
 */
INT32 SysMqueue_query(MSG_Q_ID *pStHandle, UINT32 *puRemainCount, UINT32 *puRemainBuff);

/**
 * @brief       获取所有消息队列的状态
 * @param[out]  pStatus      指向消息队列状态结构体数组的指针, @ref SYS_MQUEUE_STATE_T 
 * @return      当成功时返回消息队列的数目,负值表示失败
 */
INT32 SysMqueue_status(SYS_MQUEUE_STATE_T* pStatus);

#endif /* SysMqueue_H */


