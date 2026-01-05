#ifndef _SYS_NETLINK_UEVENT_INNER_H_
#define _SYS_NETLINK_UEVENT_INNER_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Common.h"
#include "SysNetlink.h"



/* ========================================================================== */
/*                             Type Declaration                               */
/* ========================================================================== */


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */


/**
 * @brief       内核对象事件netlink消息socket句柄创建接口
 * @param[in]   uPid     本机pid，可以根据getpid()获取，但是同一个pid不能重复bind绑定，因此根据情况用户自己传入
 * @return      成功返回套接字描述符, 负值表示失败
 * @note        绑定情况下pid为0时则说明让内核自动分配一个唯一的pid，但是通常情况下由用户自己指定
 */
INT32 SysUeventNetlink_create(UINT32 uPid);


/**
 * @brief       内核对象事件netlink消息id获取
 * @param[in]   iSockfd     套接字描述符
 * @param[in]   pBuffer     数据接收缓冲区地址
 * @param[in]   uBufferLen  数据接收缓冲区的大小
 * @return      成功返回0, 其他值表示失败
 */
INT32 SysUeventNetlink_recv(INT32 iSockfd,  VOID *pBuffer, UINT32 uBufferLen);

#endif /* SYS_NETLINK_UEVENT_INNER_H */
