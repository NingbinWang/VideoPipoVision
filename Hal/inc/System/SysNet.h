#ifndef SYS_NET_H
#define SYS_NET_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "Common.h"


/* ========================================================================== */
/*                             Type Declaration                               */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * @brief     配置IP
 * @param[in] strDevName 设备网卡名
 * @param[in] iDomain    协议族
 * @param[in] strIP      IP地址
 * @return    成功返回0;错误，返回 -1
 */
INT32 SysNet_set_ip(const CHAR *strDevName, INT32 iDomain, const CHAR *strIP);

/**
 * @brief      获取设备IP地址
 * @param[in]  strEthName  网卡名
 * @param[in]  iDomain     协议族
 * @param[in]  strIpAddr   缓冲区地址指针
 * @param[in]  uLength     缓冲区大小长度
 * @return     成功返回0;错误，返回 -1
 */
INT32 SysNet_get_ip(const CHAR *strEthName, INT32 iDomain, CHAR *strIpAddr, UINT16 uLength);

/**
 * @brief      设置设备mac地址
 * @param[in]  strEthName  网卡名
 * @param[in]  pStMacAddr  缓冲区地址指针
 * @param[in]  uLength     缓冲区大小长度
 * @return     成功返回0;错误，返回 -1
 */
INT32 SysNet_set_mac_addr(const CHAR *strEthName,UINT8 *pStMacAddr,UINT16 uLength);

/**
 * @brief      获取设备mac地址
 * @param[in]  strEthName  网卡名
 * @param[in]  pStMacAddr  缓冲区地址指针
 * @param[in]  uLength     缓冲区大小长度
 * @return     成功返回0;错误，返回 -1
 */
INT32 SysNet_get_mac_addr(const CHAR *strEthName,UINT8 *pStMacAddr,UINT16 uLength);

/**
 * @brief      设置掩码
 * @param[in]  strEthName      网卡名
 * @param[in]  pStAddr         缓冲区地址指针
 * @param[in]  uLength         缓冲区大小长度
 * @return     成功返回0;错误，返回 -1
 */
INT32 SysNet_set_netmask(const CHAR *strEthName,UINT8 *pStAddr,UINT16 uLength);

/**
 * @brief      获取掩码
 * @param[in]  strEthName      网卡名
 * @param[in]  pStAddr         缓冲区地址指针
 * @param[in]  uLength         缓冲区大小长度
 * @return     成功返回0;错误，返回 -1
 */
INT32 SysNet_get_netmask(const CHAR *strEthName,UINT8 *pStAddr,UINT16 uLength);

/**
 * @brief      设置网关
 * @param[in]  strEthName      网卡名
 * @param[in]  pStAddr         缓冲区地址指针
 * @param[in]  uLength         缓冲区大小长度
 * @return     成功返回0;错误，返回 -1
 */
INT32 SysNet_set_gateway(const CHAR *strEthName,UINT8 *pStAddr,UINT16 uLength);

/**
 * @brief      获取网关
 * @param[in]  strEthName      网卡名
 * @param[in]  pStAddr         缓冲区地址指针
 * @param[in]  uLength         缓冲区大小长度
 * @return     成功返回0;错误，返回 -1
 */
INT32 SysNet_get_gateway(const CHAR *strEthName,UINT8 *pStAddr,UINT16 uLength);

/**
 * @brief      获取每个IP的mac地址
 * @param[in]  strIpAddr       IP地址
 * @param[in]  strNetCartName  网卡名
 * @param[out] strMac          mac地址
 * @param[in]  uLength         缓冲区长度
 * @return     成功返回0;错误，返回 -1
 */
INT32 SysNet_get_ip_mac(CHAR *strIpAddr ,const CHAR *strNetCartName ,UINT8 *strMac ,UINT8 uLength);

/**
 * @brief     检测网络设备接口结点是否存在
 * @param[in] strDevName 设备名称
 * @return    成功返回 OK, 错误返回 ERROR, 不支持通过get_last_errno获取错误码
 */
INT32 SysNet_dev_node_check(const CHAR *strDevName);

/**
 * @brief     启动网卡
 * @param[in] strName 网卡名
 * @return    成功返回0;错误，返回 -1
 */
INT32 SysNet_ifconfig_up(const CHAR *strName);

/**
 * @brief     关闭网卡
 * @param[in] strName 网卡名
 * @return    成功返回0;错误，返回 -1
 */
INT32 SysNet_ifconfig_down(const CHAR *strName);

/**
 * @brief     配置网卡IP、网关、掩码
 * @param[in] strDevName 设备网卡名
 * @param[in] strIP      IP地址
 * @param[in] strNetMask 掩码
 * @param[in] strGateWay  网关
 * @return    成功返回0;错误，返回 -1
 */
INT32 SysNet_ifconfig(const CHAR *strDevName, const CHAR *strIP,const CHAR *strNetMask, const CHAR *strGateWay);

/* @fn              SysNet_ping
 * @brief           ping对端地址是否连通
 * @param[in]       pDstIpAddr 对端ip地址
 * @param[in/out]   pTimeout   超时时间(unit: ms)/成功返回数据往返响应时间(unit: us)
 * @param[out]      pTtl       协议包生存时间
 * @return 成功返回0;错误，返回 -1
 */
INT32 SysNet_ping(CHAR *pDstIpAddr, UINT32 *pTimeout, UINT32 *pTtl);

#endif/* SysNet_INTERFACE_H */

