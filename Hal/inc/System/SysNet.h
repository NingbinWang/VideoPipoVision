#ifndef SYS_NET_H
#define SYS_NET_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "Common.h"


/* ========================================================================== */
/*                             Type Declaration                               */
/* ========================================================================== */
#define SYS_ARP_ENTRY_MAX_COUNT (128)
/**
 *  @struct   SYS_ARP_ATF_E
 *  @brief    ARP的条目的状态信息
 */

typedef enum {
	SYS_ATF_COM = 0x02,	/*条目已完成（Complete），表示 MAC 地址已解析成功（常见于缓存条目）*/
	SYS_ATF_PERM = 0x04, 	/*永久条目（Permanent），静态配置的 ARP 记录，不会被自动刷新或删除*/
	SYS_ATF_PUBL = 0x08,	/*发布模式（Publish），响应其他主机对该 IP 的 ARP 请求（代理 ARP）*/
	SYS_ATF_DONTPUB= 0x40,	/*不发布（Don't Publish），禁止响应代理 ARP 请求。*/
	SYS_ATF_USETRAILERS = 0x10, /*已废弃，原用于支持尾部封装（Trailer Encapsulation）*/
	SYS_ATF_NETMASK = 0x20, /*使用网络掩码（Netmask），表示 arp_netmask 字段有效*/
	SYS_ATF_UNKNOW = 0x0
}SYSNET_ARP_ATF_E;

/**
 *  @struct   SYS_ARP_ENTRY_T
 *  @brief    ARP的条目
 */

typedef struct {
	CHAR 				strDevName[16];   /**<网络接口 */
    CHAR 				strIP[64];    /**<IP 地址 */
    CHAR 				strMac[64];   /**<MAC 地址 */
	INT32               iFlags;		   /**<ARP的flags */
}  SYSNET_ARP_ENTRY_T;


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



/*
 * @brief           查询ARP条目是否存在
 * @param[in]       strDevName  网卡名称
 * @param[in]   	strIp       ARP条目ip地址
 * @return 存在返回 TRUE 不存在发挥 FALSE
*/
BOOL SysNet_arp_entry_exist(const CHAR* strDevName,const CHAR* strIp);


/*
 * @brief           添加ARP条目
 * @param[in]       stEntry  ARP条目
 * @return 成功返回0;错误，返回 -1
 */
INT32 SysNet_arp_entry_add(SYSNET_ARP_ENTRY_T* pstEntry);


/*
 * @brief           删除ARP条目
 * @param[in]       strDevName  网卡名称
 * @param[in]   	strIp       ARP条目ip地址
 * @return 成功返回0;错误，返回 -1
 */
INT32 SysNet_arp_entry_delete(const CHAR* strDevName,const CHAR* strIp);



#endif/* SysNet_INTERFACE_H */

