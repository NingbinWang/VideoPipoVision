#ifndef SYS_MEDIUM_INTERFACE_H
#define SYS_MEDIUM_INTERFACE_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include"sys_common.h"

#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/

/* ========================================================================== */
/*                             Type Declaration                               */
/* ========================================================================== */
/**
 * @enum  SYS_MEDIUM_FS_TYPE_E 
 * @brief 文件系统类型枚举值定义，支持ext, ext2, ext3, ext4, hpfs, iso9660, JFS, minix, msdos, ncpfs nfs, ntfs, proc, Reiserfs, smb, sysv, umsdos, vfat, XFS, xiafs
 */
typedef enum
{
    SYS_MEDIUM_FS_EXT = 0,
    SYS_MEDIUM_FS_EXT2,
    SYS_MEDIUM_FS_EXT3,
    SYS_MEDIUM_FS_EXT4,
    SYS_MEDIUM_FS_HPFS,
    SYS_MEDIUM_FS_IOS9660,
    SYS_MEDIUM_FS_JFS,
    SYS_MEDIUM_FS_MINIX,
    SYS_MEDIUM_FS_MSDOS,
    SYS_MEDIUM_FS_NCPFS,
    SYS_MEDIUM_FS_NFS,
    SYS_MEDIUM_FS_NTFS,
    SYS_MEDIUM_FS_PROC,
    SYS_MEDIUM_FS_REISERFS,
    SYS_MEDIUM_FS_SMB,
    SYS_MEDIUM_FS_SYSV,
    SYS_MEDIUM_FS_UMSDOS,
    SYS_MEDIUM_FS_VFAT,
    SYS_MEDIUM_FS_XFS,
    SYS_MEDIUM_FS_XIAFS,
    SYS_MEDIUM_FS_EXFAT,
    SYS_MEDIUM_FS_YAFFS2,
    SYS_MEDIUM_FS_TYPE_UNKNOWN
}SYS_MEDIUM_FS_TYPE_E;

/**
 *  @enum  SYS_MEDIUM_BOOT_REPORT_E
 *  @brief BOOT 状态错误枚举
 *
 *  @note 
 */

typedef enum {
    SYS_FSDIAG_BOOT_BLOCKERROR,             /**< 读写block有异常 该问题可能是介质本身不能被读写导致的-->检查介质是否正常 */
    SYS_FSDIAG_BOOT_SECTORERR,              /**<逻辑扇区错误-->格式化 扇区都异常，没有方案可以恢复，查询数据的时候必然都是乱的，如果要找数据，那就要拿到卡 */
    SYS_FSDIAG_BOOT_NOMATCHSECTOR,          /**<逻辑扇区大小不是512字节-->格式化 */
    SYS_FSDIAG_BOOT_ZEROCLUSTERSIZE,        /**<空簇-->格式化 */
    SYS_FSDIAG_BOOT_CLUSTERERR,             /**<簇异常-->尝试恢复 */
    SYS_FSDIAG_BOOT_NOFAT,                  /**<没有fat表 -->格式化 */
    SYS_FSDIAG_BOOT_FATERR,                 /**<2个FAT表都有问题-->格式化 */
    SYS_FSDIAG_BOOT_NOLASTSECTOR,           /**<没有簇链结束标志-->尝试恢复 */
    SYS_FSDIAG_BOOT_ZEROFATSIZE,            /**<FAT表的size是0-->格式化 */
    SYS_FSDIAG_BOOT_NODATASPACE,            /**<没有数据空间的簇-->尝试恢复 */
    SYS_FSDIAG_BOOT_NOROOTDIR,              /**<没有根目录簇号-->尝试恢复 */
    SYS_FSDIAG_BOOT_NOENOUGHDATACLUSTERS,   /**<没有足够的簇写数据-->尝试恢复 */
    SYS_FSDIAG_BOOT_NOTFAT32FS,             /**<这个不是FAT32文件系统-->尝试恢复 */
    SYS_FSDIAG_BOOT_NOBACKUPBOOTSECTOR,     /**<没有找到BPB的备份数据-->尝试恢复 */
    SYS_FSDIAG_BOOT_NOFSINFO,               /**<没有FSINFO数据-->格式化 */
    SYS_FSDIAG_BOOT_NOENOUGHCLUSTERSFORFAT, /**<没有足够的用户空间-->格式化 */
    SYS_FSDIAG_BOOT_ROOTDIRZEROSIZE,        /**<根目录是空簇-->格式化 */
    SYS_FSDIAG_BOOT_OUTOFCLUSTERSRANG,      /**<超过簇号-->格式化 */
    SYS_FSDIAG_BOOT_BADCLUSTERSTOOLAGE,     /**<坏簇超过可容纳的范围-->尝试恢复 */
    SYS_FSDIAG_BOOT_BADCLUSTERUNREADABLE,   /**<坏簇-->尝试恢复 */
    SYS_FSDIAG_BOOT_NOMEM,                  /**<没有内存--> */
    SYS_FSDIAG_BOOT_VOLERR,                 /**<文件类型标志不对-->尝试恢复 */
    SYS_FSDIAG_BOOT_OK,
} SYS_MEDIUM_BOOT_REPORT_E;

/**
 * @struct SYS_MEDIUM_INFO_T 
 * @brief  介质信息结构体定义，主要包括文件系统类型、总大小、剩余大小、簇大小
 */
typedef struct
{   
    SYS_MEDIUM_FS_TYPE_E eType; /**< 文件系统类型*/
    UINT32 uTotalSize;          /**< 单位MB*/
    UINT32 uRemainSize;         /**< 单位MB*/
    UINT32 uClusterSize;        /**< 簇大小，单位B*/
}SYS_MEDIUM_INFO_T;

/**@struct    SYS_MEDIUM_PARTITION_INFO_T
 * @brief     介质分区信息结构体 
 */
typedef struct
{
    UINT32 uCapacityRatio;    /**< 容量占比，单分区则占比为100，多分区容量占比总和100，否则异常 */
    UINT32 uType;             /**< 分区类型，fdisk -l 所显示的type类型，不指定内部默认为使用 WIN95_FAT32_LBA */
    UINT8  aRes[24];
}SYS_MEDIUM_PARTITION_INFO_T;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
/**
 * @brief      将存储设备的分区挂载到目录中，目前只支持一个介质一个分区
 * @param[in]  strPath      节点路径
 * @param[in]  eFsType      文件系统类型
 * @param[in]  strMountPath 挂载路径
 * @return     成果返回OK，失败返回ERROR,
 */
INT32 sys_medium_mount(const CHAR *strPath, SYS_MEDIUM_FS_TYPE_E eFsType, const CHAR *strMountPath);

/**
 * @brief      卸载被挂载的存储介质，目前只支持一个介质一个分区
 * @param[in]  strMountPath 挂载路径
 * @return     成果返回OK，失败返回ERROR
 */
INT32 sys_medium_umount(const CHAR *strMountPath);

/**
 * @brief      制作介质分区
 * @param[in]  strDevPath 介质节点  
 * @param[in]  iPartNum  分区个数  
 * @return     成功OK, 其他参考ERROR_CODE_E, 支持get_last_errno获取错误码
 */
INT32 sys_medium_make_part(const CHAR *strDevPath, INT32 iPartNum);

/**@fn         sys_medium_make_part    
 * @brief      制作介质分区，支持多分区制作
 * @param[in]  strRootDevPath   介质根节点  
 * @param[in]  uPartNum         分区数  
 * @param[in]  pStPartInfo      分区信息，外部需要传入uPartNum对应个数
 * @return     成功OK, 其他参考ERROR_CODE_E, 支持get_last_errno获取错误码
 */
INT32 sys_medium_make_mutil_part(const CHAR *strRootDevPath, UINT32 uPartNum, SYS_MEDIUM_PARTITION_INFO_T *pStPartInfo);

/**
 * @brief      格式化介质
 * @param[in]  strPath   节点路径
 * @param[in]  eFsType   文件系统类型 
 * @param[in]  uUintSize 分配单元大小
 * @return     成功OK, 其他参考ERROR_CODE_E, 支持get_last_errno获取错误码
 */
INT32 sys_medium_format(const CHAR *strPath, SYS_MEDIUM_FS_TYPE_E eFsType, UINT32 uUintSize);

/**
 * @brief      判断设备节点是否存在
 * @param[in]  strPath   节点路径  
 * @return     成果返回OK，失败返回ERROR,
 */
INT32 sys_medium_check_dev_node(const CHAR *strPath);

/**
 * @brief      medium获取介质信息
 * @param[in]  strPath   节点路径
 * @param[out] pMediumInfo 介质信息指针  
 * @return     成功 ok 失败 ERROR
 */
INT32 sys_medium_get_info(const CHAR *strPath, SYS_MEDIUM_INFO_T *pMediumInfo);

/**
 * @brief      强制写文件到存储介质      
 * @param[in]  strPath  盘符路径
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E, 支持通过get_last_errno获取错误码
 */
INT32 sys_medium_sync(const CHAR *strPath);

/**
 * @brief       boot检查初始化 用于做boot检查的初始化
 * @param[in]   strPath   文件系统对应的介质路径
 * @param[in]   eFsType   文件系统的类型
*  @return      成功时为0，错误时为负数
 */
INT32 sys_medium_boot_check(const CHAR *strPath, SYS_MEDIUM_FS_TYPE_E eFsType);

/**
 * @brief      medium获取介质信息
 * @param[in]  strNodePath       设备节点，例/dev/mmcblk0 /dev/mmcblk0p1等
 * @param[out] puSize            设备节点大小 MB
 * @return     成功 ok 失败 ERROR
 */
INT32 sys_medium_get_node_size(const CHAR *strNodePath, UINT32 *puSize);

/**
 * @brief      获取根节点分区数量
 * @param[in]  strRootNode       设备根节点，不支持分区节点，例只支持/dev/mmcblk0
 * @param[out] puNum             分区数量
 * @return     成功 ok 失败 ERROR
 */
INT32 sys_medium_get_node_partition(const CHAR *strRootNode, UINT32 *puNum);

#ifdef __cplusplus
}
#endif/*__cplusplus*/

#endif/* SYS_MEDIUM_INTERFACE_H */

