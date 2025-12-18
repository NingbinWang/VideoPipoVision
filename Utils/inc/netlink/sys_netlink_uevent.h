#ifndef SYS_NETLINK_UEVENT_H
#define SYS_NETLINK_UEVENT_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "sys_common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ========================================================================== */
/*                             Type Declaration                               */
/* ========================================================================== */

/**
 *  @enum SYS_UEVENT_NETLINK_ACTION_E
 *  @brief 内核对象事件类型
 */
typedef enum {
    UEVENT_NETLINK_ACTION_ADD,
    UEVENT_NETLINK_ACTION_REMOVE,
    UEVENT_NETLINK_ACTION_BIND,
    UEVENT_NETLINK_ACTION_UNBIND,

    UEVENT_NETLINK_ACTION_UNKNOW,
} SYS_UEVENT_NETLINK_ACTION_E;

/**
 *  @enum SYS_UEVENT_NETLINK_SUBSYSTEM_E
 *  @brief 内核对象事件所属子系统
 */
typedef enum {
    UEVENT_NETLINK_SUBSYSTEM_MMC,
    UEVENT_NETLINK_SUBSYSTEM_BDI,   /**< block device inode */
    UEVENT_NETLINK_SUBSYSTEM_BLOCK,
    UEVENT_NETLINK_SUBSYSTEM_USB,
    UEVENT_NETLINK_SUBSYSTEM_NET,
    UEVENT_NETLINK_SUBSYSTEM_QUEUES,

    UEVENT_NETLINK_SUBSYSTEM_UNKNOW,
} SYS_UEVENT_NETLINK_SUBSYSTEM_E;

/**
 *  @enum    SYS_UEVENT_MMC_TYPE_E
 *  @brief   uevent事件mmc子系统私有数据mmc类型枚举
 */
typedef enum
{
    UEVENT_MMC_TYPE_SD,

    UEVENT_MMC_TYPE_UNKNOW,
}  SYS_UEVENT_MMC_TYPE_E;

/**
 *  @enum    SYS_UEVENT_BLOCK_DEVTYPE_E
 *  @brief   uevent事件block子系统私有数据设备类型枚举
 */
typedef enum
{
    UEVENT_BLOCK_DEVTYPE_DISK,
    UEVENT_BLOCK_DEVTYPE_PARTITION,

    UEVENT_BLOCK_DEVTYPE_UNKNOW,
}  SYS_UEVENT_BLOCK_DEVTYPE_E;

/**
 *  @enum    SYS_UEVENT_USB_DEVTYPE_E
 *  @brief   uevent事件block子系统私有数据设备类型枚举
 */
typedef enum
{
    UEVENT_USB_DEVTYPE_USBDEVICE,
    UEVENT_USB_DEVTYPE_USBINTERFACE,

    UEVENT_USB_DEVTYPE_UNKNOW,
}  SYS_UEVENT_USB_DEVTYPE_E;

/**
 *  @struct  SYS_UEVENT_NETLINK_MMC_T
 *  @brief   MMC子系统私有信息描述结构体
 */
typedef struct {
    UINT32 uMmcType;        /**< mmc具体类型 参考@SYS_UEVENT_MMC_TYPE_E */
    CHAR strMmcName[16];    /**< mmc名称 */
    CHAR strModAlias[128];  /**< 设备模块别名 (当事件类型为bind/remove时使用) */
    CHAR strDriver[32];     /**< 设备驱动名称 (当事件类型为bind时使用) */
} SYS_UEVENT_NETLINK_MMC_T;

/**
 *  @struct  SYS_UEVENT_NETLINK_BLOCK_T
 *  @brief   BLOCK子系统私有信息描述结构体
 */
typedef struct {
    UINT32 uDevType;        /**< 块设备类型 参考@SYS_UEVENT_BLOCK_DEVTYPE_E */
    UINT32 uMajor;          /**< 主设备号 */
    UINT32 uMinor;          /**< 次设备号 */
    UINT32 uPartn;          /**< 分区编号 (当块设备类型为partition时使用) */
    CHAR strDevName[32];    /**< 块设备名称 */
} SYS_UEVENT_NETLINK_BLOCK_T;

/**
 *  @struct  SYS_UEVENT_NETLINK_USB_T
 *  @brief   USB子系统私有信息描述结构体
 */
typedef struct {
    UINT32 uDevType;        /**< usb设备类型 参考@SYS_UEVENT_USB_DEVTYPE_E */
    UINT32 uMajor;          /**< 主设备号 (当usb设备类型为usb_deviec时使用) */
    UINT32 uMinor;          /**< 次设备号 (usb_deviec) */
    UINT32 uBusNum;         /**< usb总线号 (usb_deviec) */
    UINT32 uDevNum;         /**< usb总线上的设备编号 (usb_deviec) */
    CHAR strDevName[32];    /**< usb设备节点名称 (usb_deviec) */
    CHAR strProduct[32];    /**< usb设备产品信息 (<vendor_id>/<product_id>/<bcdDevice>) */
    CHAR strType[16];       /**< usb设备类 */
    CHAR strDriver[32];     /**< 设备驱动名称 (当事件类型为bind时使用) */
    CHAR strInterface[16];  /**< usb接口类别 (类/子类/协议)(当usb设备类型为usb_interface时使用) */
    CHAR strModAlias[128];  /**< 设备模块别名 (usb_interface(unbind时没有该信息)) */
} SYS_UEVENT_NETLINK_USB_T;

/**
 *  @struct  SYS_UEVENT_NETLINK_NET_T
 *  @brief   NET子系统私有信息描述结构体
 */
typedef struct {
    UINT32 uIfIndex;        /**< 内核内部索引号 */
    CHAR strInterface[16];  /**< 网络接口名称 */
} SYS_UEVENT_NETLINK_NET_T;

/**
 *  @struct  SYS_UEVENT_NETLINK_RECV_T
 *  @brief   uevent事件数据接收结构体
 */
typedef struct {
    UINT32 uAction;         /**< 事件类型 参考@SYS_UEVENT_NETLINK_ACTION_E */
    UINT32 uSubSystem;      /**< 所属子系统 参考@SYS_UEVENT_NETLINK_SUBSYSTEM_E */
    CHAR strDevPath[128];   /**< 设备在sysfs中的路径 */
    UINT8 aPrivateData[0];  /**< 事件私有数据 */
} SYS_UEVENT_NETLINK_RECV_T;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_NETLINK_UEVENT_H */

