#ifndef SYS_SHM_INTERFACE_H
#define SYS_SHM_INTERFACE_H

#include "sys_common.h"

#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/

/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */

#ifndef PROT_READ
/** 共享内存映射区操作权限 (读) */
#define PROT_READ             (1u)
#endif

#ifndef PROT_WRITE
/** 共享内存映射区操作权限 (写) */
#define PROT_WRITE            (2u)
#endif

#ifndef PROT_EXEC
/** 共享内存映射区操作权限 (执行) */
#define PROT_EXEC             (4u)
#endif

#ifndef MAP_SHARED
/** 共享内存映射区操作属性 (共享) */
#define MAP_SHARED            (1u)
#endif

#ifndef MAP_PRIVATE
/** 共享内存映射区操作属性 (私有) */
#define MAP_PRIVATE           (2u)
#endif

#ifndef MAP_FAILED
/** 共享内存映射区结果, 表示映射失败 */
#define MAP_FAILED            ((VOID *) -1)
#endif

/* ========================================================================== */
/*                             Type Declaration                               */
/* ========================================================================== */

/**@enum   SYS_SHM_MODE_BIT_E 
 * @brief  模式位枚举定义
 */
typedef enum
{
    SYS_SHM_MODE_BIT_CREATE = 01000,   /*创建如果不存在*/
    SYS_SHM_MODE_BIT_EXCL   = 02000,   /*如果key存在则返回失败*/
    SYS_SHM_MODE_BIT_NOWAIT = 04000,   /*等待时返回错误*/
}SYS_SHM_MODE_BIT_E;

/**@enum   SYS_SHM_CMD_IPC_CMD_E 
 * @brief  控制命令枚举定义
 */
typedef enum
{
    SYS_SHM_CMD_IPC_CMD_RMID = 0, /*删除空闲内存描述符*/
    SYS_SHM_CMD_IPC_CMD_SET  = 1, /*设置参数*/
    SYS_SHM_CMD_IPC_CMD_STAT = 2, /*获取参数*/
    SYS_SHM_CMD_IPC_CMD_INFO = 3, /*查看IPCS信息*/
}SYS_SHM_CMD_IPC_CMD_E;

/**@enum   SYS_SHM_ATTACH_FLAG_E 
 * @brief  控制命令枚举定义
 */
typedef enum
{
    SYS_SHM_ATTACH_FLAG_RW     = 0,      /*默认读写模式*/
    SYS_SHM_ATTACH_FLAG_RDONLY = 010000, /*只读*/
    SYS_SHM_ATTACH_FLAG_ROUND  = 020000, /*设置参数*/
    SYS_SHM_ATTACH_FLAG_REMAP  = 040000, /*获取参数*/
    SYS_SHM_ATTACH_FLAG_EXEC   = 0100000, /*查看IPCS信息*/
}SYS_SHM_ATTACH_FLAG_E;

/**@struct   SYS_SHM_INFO_T 
 * @brief    共享内存空间信息
 */
typedef struct
{
    UINT8 uRes[256];    /**< 保留字段 */
}SYS_SHM_INFO_T;

/**
 * @brief      键值转换函数，用于获取唯一标识符
 * @param[in]  strPath    文件路径，文件必须存在
 * @param[in]  iProjectId 序号值，取值范围1-255
 * @return     成功返回key键值 失败返回ERROR
 */
INT32 sys_share_memeory_ftok(const CHAR *strPath, INT32 iProjectId);

/**
 * @brief      system V 用于创建或者获取共享内存
 * @param[in]  iKey 标识符
 * @param[in]  uSize 共享内存大小
 * @param[in]  eIpcType 共享内存FLAG
 * @return     成功返回共享内存句柄ID 失败返回ERROR
 */
INT32 sys_share_memeory_get(INT32 iKey, UINT64 uSize, SYS_SHM_MODE_BIT_E eIpcType);

/**
 * @brief      system V 将共享内存附加到进程的虚拟地址空间
 * @param[in]  iShmID 共享内存操作句柄
 * @param[in]  pShareMemoryAddr 指定共享内存地址，设置NULL由系统自动分配一个合适地址
 * @param[in]  eAttachFlag 共享内存FLAG
 * @return     成功返回共享内存地址 失败返回NULL
 */
VOID *sys_share_memeory_attach(INT32 iShmID, const VOID *pShareMemoryAddr, SYS_SHM_ATTACH_FLAG_E eAttachFlag);

/**
 * @brief      system V 共享内存分离接口
 * @param[in]  pShareMemoryAddr 共享内存地址
 * @return     成功返回 OK 错误返回 ERROR
 */
INT32 sys_share_memeory_detach(const VOID *pShareMemoryAddr);

/**
 * @brief      system V 控制共享内存
 * @param[in]  iShmID 共享内存操作句柄
 * @param[in]  eCmd   指令
 * @param[in]  pStData 共享内存信息，删除可以传入NULL
 * @return     成功返回 OK 错误返回 ERROR
 */
INT32 sys_share_memeory_control(INT32 iShmID, SYS_SHM_CMD_IPC_CMD_E eCmd, SYS_SHM_INFO_T *pStData);

/**
 * @brief      posix共享内存创建接口
 * @param[in]  strName 共享内存名称
 * @param[in]  iFlag   Flag类型，参考man 3 shm_open
 * @param[in]  iMode   类型，参考man 3 shm_open
 * @return     成功返回文件描述符，错误返回 ERROR
 */
INT32 sys_share_memeory_open(const CHAR *strName, INT32 iFlag, INT32 iMode);

/**
 * @brief      posix共享内存删除接口
 * @param[in]  strName 共享内存名称
 * @return     成功返回 OK 错误返回 ERROR
 */
INT32 sys_share_memeory_unlink(const CHAR *strName);

/**
 * @brief      将文件映射到共享内存
 * @param[in]  iFd               文件描述符
 * @param[in]  pShareMemoryAddr  共享内存地址，指定为NULL则由系统自动分配
 * @param[in]  iSize             共享内存大小
 * @param[in]  iProt             共享内存映射区操作权限，参考man 2 mmap，如PORT_READ|PORT_WRITE
 * @param[in]  iFlag             标志位参数，参考man 2 mmap，如MAP_SHARED
 * @param[in]  ioffset           偏移值
 * @return     成功返回贡献内存地址 失败返回NULL
 */
VOID *sys_share_memeory_mmap(INT32 iFd, VOID *pShareMemoryAddr, INT32 iSize, INT32 iProt, INT32 iFlag, INT32 ioffset);

/**
 * @brief      取消共享内存映射
 * @param[in]  pShareMemoryAddr 共享内存地址，由mmap创建
 * @param[in]  iSize   共享内存大小
 * @return     成功返回OK 失败返回ERROR
 */
INT32 sys_share_memeory_munmap(VOID *pShareMemoryAddr, INT32 iSize);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* SYS_SHM_INTERFACE_H */

