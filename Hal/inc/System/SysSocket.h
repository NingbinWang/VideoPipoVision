#ifndef SYS_SOCKET_H
#define SYS_SOCKET_H
/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Common.h"
#include "SysSocketDefine.h"

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * @brief      创建socket
 * @param[in]  iDomain 协议族, 参考man 2 socket
 * @param[in]  iType 套接字类型, 参考man 2 socket
 * @param[in]  iProtocol 协议类型, 参考man 2 socket
 * @return     成功返回 套接字句柄 错误返回 ERROR, 支持通过get_last_errno获取错误码
 */
INT32 SysSocket_create(INT32 iDomain, INT32 iType, INT32 iProtocol);

/**
 * @brief      绑定socket
 * @param[in]  iSockFd  创建的套接字
 * @param[in]  iDomain 协议族, 参考man 2 socket
 * @param[in]  strIPAddr 要绑定的地址, 为NULL则代表监听所有地址
 * @param[in]  uPort 要绑定的端口
 * @return     成功返回 套接字句柄 错误返回 ERROR, 支持通过get_last_errno获取错误码
 */
INT32 SysSocket_bind(INT32 iSockFd, INT32 iDomain, const CHAR *strIPAddr, UINT16 uPort);

/**
 * @brief     在socket上建立监听      
 * @param[in] iSockFd 创建的套接字描述符
 * @param[in] iMaxConNum 在套接字上排队的最大连接数
 * @return    成功返回 套接字句柄  错误返回 ERROR, 支持通过get_last_errno获取错误码
 */
INT32 SysSocket_listen(INT32 iSockFd, INT32 iMaxConNum);

/**
 * @brief       从已完成的连接队列中获取一个已完成的队列      
 * @param[in]   iSockFd   监听套接字
 * @param[out]  strClientIP 接收到的对端地址，可为空
 * @param[out]  uAddrLen, strClientIP 的长度，若strClientIP为空则填0
 * @param[out]  puClientPort 接收到的对端端口，可为空
 * @return      成功返回 套接字句柄 错误返回 ERROR, 支持通过get_last_errno获取错误码
 * @note        puClientPort的存储是本机序，调用者无需再做ntoh转换
 */
INT32 SysSocket_accept(INT32 iSockFd, CHAR *strClientIP, UINT16 uAddrLen, UINT16 *puClientPort);

/**
 * @brief      连接服务器
 * @param[in]  iSockFd  创建的套接字
 * @param[in]  iDomain 协议族, 参考man 2 socket
 * @param[in]  strIPAddr 要连接的地址
 * @param[in]  uPort 要连接的端口
 * @return      OK/ERROR, 支持通过get_last_errno获取错误码
 */
INT32 SysSocket_connect(INT32 iSockFd, INT32 iDomain, const CHAR *strIPAddr, UINT16 uPort);

/**
 * @brief      将数据发送到指定地址上 
 * @param[in]  iSockFd 建立的socket
 * @param[in]  pBuf  要发送的数据
 * @param[in]  iLen 数据长度
 * @param[in]  iDomain 协议族, 参考man 2 socket
 * @param[in]  strIPAddr  目的ip地址
 * @param[in]  uPort 目的端口
 * @return     成功发送的字节数 错误返回 ERROR, 支持通过get_last_errno获取错误码
 */
INT32 SysSocket_send_to(INT32 iSockFd, VOID *pBuf, INT32 iLen, INT32 iDomain, const CHAR *strIPAddr, UINT16 uPort);

/**
 * @brief      从对端接收数据      
 * @param[in]  iSockFd  套接字句柄
 * @param[in]  pBuf 接收缓冲区
 * @param[in]  iBufLen 接受缓冲区的长度
 * @param[out] strClientIP 接收到的对端地址
 * @param[out] uAddrLen, strClientIP 的长度
 * @param[out] puClientPort 接收到的对端端口
 * @return     成功接收的字节数 错误返回 ERROR, 支持通过get_last_errno获取错误码
 */
INT32 SysSocket_recvfrom(INT32 iSockFd, VOID *pBuf, INT32 iBufLen, CHAR *strClientIP, UINT16 uAddrLen, UINT16 *puClientPort);

/**
 * @brief      从对端接收数据      
 * @param[in]  iSockFd  套接字句柄
 * @param[in]  pBuf 接收缓冲区
 * @param[in]  iLen 接受缓冲区的长度
 * @return     成功接收的字节数 错误返回 ERROR; 返回0代表对端关闭, 支持通过get_last_errno获取错误码
 */
INT32 SysSocket_recv(INT32 iSockFd, VOID *pBuf, INT32 iLen);

/**
 * @brief      向对端发送数据      
 * @param[in]  iSockFd  套接字句柄
 * @param[in]  pBuf 发送缓冲区
 * @param[in]  iLen 发送缓冲区的长度
 * @return     成功发送的字节数 错误返回 ERROR, 支持通过get_last_errno获取错误码
 */
INT32 SysSocket_send(INT32 iSockFd, VOID *pBuf, INT32 iLen);

/**
 * @brief      关闭socket      
 * @param[in]  iSockFd  套接字句柄
 * @return     成功返回 0 错误返回 ERROR, 支持通过get_last_errno获取错误码
 */
INT32 SysSocket_close(INT32 iSockFd);

/**
 * @brief      设置套接字属性
 * @param[in]  iSockFd  套接字句柄
 * @param[in]  iOptLevel  操作层面，比如是套接字层面(SOL_SOCKET)还是协议层面
 * @param[in]  iOptName 操作名称
 * @param[in]  pOptValue  option_name对应的取值
 * @param[in]  iOptLength option_value的长度
 * @return     成功，返回OK；失败参考ERROR_ID_E, 支持通过get_last_errno获取错误码
 */
INT32 SysSocket_set_sock_opt(INT32 iSockFd, INT32 iOptLevel, INT32 iOptName, const VOID *pOptValue, INT32 iOptLength);

/**
 * @brief      获取套接字属性
 * @param[in]  iSockFd    套接字句柄
 * @param[in]  iOptLevel  操作层面，比如是套接字层面(SOL_SOCKET)还是协议层面
 * @param[in]  iOptName   操作名称
 * @param[in]  pOptValue  option_name对应的取值
 * @param[in]  iOptLength 调用者填写option_value的长度, 返回时代表实际长度
 * @return     成功,返回OK；失败参考ERROR_ID_E, 支持通过get_last_errno获取错误码
 */
INT32 SysSocket_get_sock_opt(INT32 iSockFd, INT32 iOptLevel, INT32 iOptName, VOID *pOptValue, INT32 *iOptLength);

/**
 * @brief      shutdown socket      
 * @param[in]  iSockFd  套接字句柄
 * @param[in]  iHow 选项
 * @return     成功返回 0错误返回 ERROR, 支持通过get_last_errno获取错误码
 */
INT32 SysSocket_shutdown(INT32 iSockFd, INT32 iHow);

/**     
 * @brief      获取套接字本地地址
 * @param[in]  iSockFd 套接字
 * @param[out] pStLocalAddr 本地套接字结构体指针
 * @param[out] pUAddrLength 结构体大小
 * @return     成功返回 套接字句柄 错误返回 ERROR, 支持通过get_last_errno获取错误码
 */
INT32 SysSocket_get_local_addr(INT32 iSockFd, struct sys_sockaddr *pStLocalAddr, socklen_t *pUAddrLength);

/**
 * @brief      获取套接字对端地址
 * @param[in]  iSockFd 套接字
 * @param[out] pStRemoteAddr 远程套接字结构体指针
 * @param[out] pUAddrLength  结构体大小
 * @return     成功返回 套接字句柄 错误返回 ERROR, 支持通过get_last_errno获取错误码
 */
INT32 SysSocket_get_peer_addr(INT32 iSockFd, struct sys_sockaddr *pStRemoteAddr, socklen_t *pUAddrLength);

/**
 * @brief      转换网络二进制结构到ASCII类型的地址
 * @param[in]  iAf  地址协议族 Af_INET、Af_INET6
 * @param[in]  pSrcAddr    地址结构体指针,iAf_INET时地址结构是in_addr、iAf_INET6时指向in6_addr结构体指针
 * @param[out] strDstAddr  网络序表示的32位IP地址
 * @param[out] uSize       网络序表示的32位IP地址
 * @return     成功,返回非NULL字符串；失败返回NULL,错误见ERRNO
 */
const CHAR *SysSocket_inet_ntop(INT32 iAf, const VOID *pSrcAddr, CHAR *strDstAddr, socklen_t uSize);

/**
 * @brief      转换ASCII类型的地址到网络十进制结构
 * @param[in]  iAf  地址协议族 iAf_INET、iAf_INET6
 * @param[in]  pSrcAddr    地址结构体指针,iAf_INET时地址结构是in_addr、iAf_INET6时指向in6_addr结构体指针
 * @param[out] strDstAddr  网络序表示的32位IPV4地址或者128位的IPV6地址
 * @return     成功返回OK 失败返回ERROR
 */
INT32 SysSocket_inet_pton(INT32 iAf, const VOID *pSrcAddr, VOID *strDstAddr);

/**
 * @brief      将一个点分十进制的IP转换成一个长整数型数（u_long类型）
 * @param[in]  strIP  地址字符串 
 * @param[out] pUAdddr 
 * @return     成功则将字符串转换为32位二进制网络字节序的IPV4地址，否则为INADDR_NONE
 */
INT32 SysSocket_inet_addr(const CHAR *strIP,ULONG *pUAdddr);

/**
 * @brief      将一个十进制网络字节序转换为点分十进制IP格式的字符串
 * @param[in]  stIn  32位的IPv4地址
 * @return     成功返回一个字符指针，指向一块存储着点分格式IP地址的静态缓冲区（同一线程内共享此内存);错误，返回NULL。
 */
CHAR *SysSocket_inet_ntoa(struct sys_in_addr stIn);

/**
 * @brief      将一个点分十进制IP格式的字符串转换为十进制网络字节序
 * @param[in]  strIp  十进制IP格式的字符串
 * @param[out] pStInp 网络字节序IP地址
 * @return     成功返回0;错误，返回 -1。
 */
INT32 SysSocket_inet_aton(const CHAR *strIp, struct sys_in_addr *pStInp);

/**
 * @brief      获取tcp状态
 * @param[in]  iSockFd 套接字
 * @return     成功返回 tcp链接状态 错误返回 ERROR, 支持通过get_last_errno获取错误码
 */
INT32 SysSocket_get_tcp_state(INT32 iSockFd);

/**
 * @brief      获取套接字错误码
 * @return     详见errno 
 */
INT32 SysSocket_get_last_errno(VOID);

/**
 * @brief      向对端发送消息（支持多缓冲区和控制信息）
 * @param[in]  iSockFd    套接字句柄
 * @param[in]  pStMsg    自定义的消息头结构体（参考SYS_IMSG_HEADER_T）
 * @param[in]  iFlags     发送标志（参考SYS_MSG_FLAGS_E）
 * @return     成功发送的字节数，错误返回 ERROR
 */
INT32 SysSocket_sendmsg(INT32 iSockFd, SYS_IMSG_HEADER_T *pStMsg, INT32 iFlags);

/**
 * @brief      从对端接收消息（支持多缓冲区和控制信息）
 * @param[in]  iSockFd    套接字句柄
 * @param[out] pStMsg    自定义的消息头结构体（参考SYS_IMSG_HEADER_T）
 * @param[in]  iFlags     接收标志（参考SYS_MSG_FLAGS_E）
 * @return     成功接收的字节数，错误返回 ERROR
 */
INT32 SysSocket_recvmsg(INT32 iSockFd, SYS_IMSG_HEADER_T *pStMsg, INT32 iFlags);

/**
 * @brief      获取下一个控制头
 * @param[in]  pStMsgHeader   消息头
 * @param[in]  pStCMsgHeader  控制消息头
 * @return     成功下一个控制头指针，失败返回NULL
 */
SYS_CMSG_HEADER_T *SysSocket_cmsg_nxthdr(SYS_IMSG_HEADER_T *pStMsgHeader, SYS_CMSG_HEADER_T *pStCMsgHeader);

#endif/* SysSocket_INTERFACE_H */

/**@}*/
/**@}*/