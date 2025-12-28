#ifndef SYS_NETLINK_GENERIC_H
#define SYS_NETLINK_GENERIC_H

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

typedef struct {
    UINT32 uFamilyId;   /**< 家族id */
    UINT32 uGroupId;    /**< 组id */
    CHAR *pFamily;      /**< 家族名称，获取id时根据名称获取对应家族id */
    CHAR *pGroup;       /**< 组名称，获取id时根据名称获取对应组id */
} SYS_GENLNETLINK_ID_T;

typedef struct {
    UINT32 uNlaType;    /**< netlink属性类型，表示发送或接收数据的类型 */
    UINT32 uDataLen;    /**< 发送或接收数据长度 */
    UINT8  aData[0];    /**< 发送或接收数据, 注意数据需要按照4字节对齐来存放 */
} SYS_GENLNETLINK_ATTR_T;

typedef struct {
    INT32  iFamilyId;   /**< 家族id，同时也是nlmsg中的消息类型 */
    UINT32 uGenlCmd;    /**< 操作命令,结合familyid对指定家族发送控制命令 */
    UINT32 uGenlVer;    /**< 家族内部版本，用于版本控制，兼容性考虑 */
    UINT32 uAttrNr;     /**< 需要发送或接收的属性数据数量 */
    UINT8  aAttrMsg[0];
} SYS_GENLNETLINK_MSG_CONFIG_T;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_NETLINK_GENERIC_H */


