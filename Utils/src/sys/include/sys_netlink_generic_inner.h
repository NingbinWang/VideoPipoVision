#ifndef SYS_NETLINK_GENERIC_INNER_H
#define SYS_NETLINK_GENERIC_INNER_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "sys_common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * @brief       通用netlink消息socket句柄创建接口
 * @param[in]   uPid     本机pid，可以根据getpid()获取，但是同一个pid不能重复bind绑定，因此根据情况用户自己传入
 * @return      成功返回套接字描述符, 负值表示失败
 * @note        绑定情况下pid为0时则说明让内核自动分配一个唯一的pid，但是通常情况下由用户自己指定
 */
INT32 sys_genlnetlink_create_nl_socket(UINT32 uPid);

/**
 * @brief           通用netlink消息id获取(familyid 和 groupid)
 * @param[in]       iSockfd     套接字描述符
 * @param[in/out]   pGenlId     传入家族名和组名返回家族id和组id，参考@SYS_GENLNETLINK_ID_T
 * @return          成功返回0, 其他值表示失败
 */
INT32 sys_genlnetlink_get_id(INT32 iSockfd, SYS_GENLNETLINK_ID_T * pGenlId);

/**
 * @brief       通用netlink消息数据发送
 * @param[in]   iSockfd     套接字描述符
 * @param[in]   pBuffer     数据发送缓冲区地址
 * @param[in]   uBufferLen  发送数据大小
 * @return      成功返回0, 其他值表示失败
 */
INT32 sys_genlnetlink_send_msg(INT32 iSockfd, VOID *pBuffer, UINT32 uBufferLen);

/**
 * @brief       通用netlink消息数据接收
 * @param[in]   iSockfd     套接字描述符
 * @param[in]   pBuffer     数据接收缓冲区地址
 * @param[in]   uBufferLen  数据接收缓冲区大小
 * @return      成功返回0, 其他值表示失败
 */
INT32 sys_genlnetlink_recv_msg(INT32 iSockfd, VOID *pBuffer, UINT32 uBufferLen);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_NETLINK_GENERIC_INNER_H */

/**@}*/
/**@}*/


