#ifndef SYS_NETLINK_H
#define SYS_NETLINK_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Common.h"
#include "SysGenlNetlink.h"
#include "SysUeventNetlink.h"

/* ========================================================================== */
/*                             Type Declaration                               */
/* ========================================================================== */

typedef enum {
    SYS_NETLINK_GENERIC,
    SYS_NETLINK_UEVENT,
    SYS_NETLINK_MAX,
} SYS_NETLINK_PROTOCOL_TYPE_E;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */


/**
 * @brief       netlink消息socket句柄创建接口
 * @param[in]   uProtocol     netlink协议类型 参考@SYS_NETLINK_PROTOCOL_TYPE_E
 * @param[in]   pArg     netlink协议类型 参考@SYS_NETLINK_PROTOCOL_TYPE_E
 * @return      成功返回套接字描述符, 负值表示失败
 * @note        该接口创建的socket套接字请使用sys_netlink_close进行关闭，否则会导致内存泄漏问题
 */
INT32 SysNetlink_create(UINT32 uProtocol, VOID *pArg);

/**
 * @brief       netlink消息发送
 * @param[in]   iSockfd     套接字描述符
 * @param[in]   pBuffer     netlink数据
 * @param[in]   uBufferLen  需要发送的netlink数据长度
 * @return      成功返回0, 其他值表示失败
 */
INT32 SysNetlink_send(INT32 iSockfd, VOID *pBuffer, UINT32 uBufferLen);

/**
 * @brief       netlink消息接收
 * @param[in]   iSockfd     套接字描述符
 * @param[in]   pBuffer     netlink数据
 * @param[in]   uBufferLen  预期需要接收的数据长度
 * @return      成功返回0, 其他值表示失败
 */
INT32 SysNetlink_recv(INT32 iSockfd, VOID *pBuffer, UINT32 uBufferLen);

/**
 * @brief       netlink消息套接字关闭
 * @param[in]   iSockfd     套接字描述符
 * @return      成功返回0, 其他值表示失败
 */
INT32 SysNetlink_close(INT32 iSockfd);

#endif /* SYS_GENLNETLINK_H */


