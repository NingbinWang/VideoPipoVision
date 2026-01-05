#ifndef _SYS_FILE_H_
#define _SYS_FILE_H_

#include "Common.h"

#ifndef FILE_WINDOWS
/** FILE_SEARCH_SEPARATOR */
#define FILE_SEARCH_SEPARATOR ('/')
#else
/** FILE_SEARCH_SEPARATOR */
#define FILE_SEARCH_SEPARATOR ('\\')
#endif

/**@enum   SYS_LOCK_TYPE_E 
 * @brief  文件锁类型枚举
 */
typedef enum 
{
    SYS_LOCK_SHARE = 1,     /*共享锁*/
    SYS_LOCK_EXCLUSIVE = 2, /*独占锁*/
    SYS_LOCK_UNLOCK = 8,    /*解锁*/
}SYS_LOCK_TYPE_E;

/**
 * @enum SYS_FS_ATTR_E 
 * @brief  定义文件属性枚举
 */
typedef enum {
    /** Standard file attribute */
    SYS_FS_ATTR_RDONLY         =   0x01,
    SYS_FS_ATTR_HIDDEN         =   0x02,
    SYS_FS_ATTR_SYSTEM         =   0x04,
    SYS_FS_ATTR_VOLUME         =   0x08,
    SYS_FS_ATTR_DIR            =   0x10,
    SYS_FS_ATTR_ARCH           =   0x20,

    /** Other file attribute */
    SYS_FS_ATTR_NONE           =   0x40,
    SYS_FS_ATTR_ALL            =   0x7f,
    SYS_FS_ATTR_FILE_ONLY      =   0x100,
} SYS_FS_ATTR_E;

/**@struct SYS_FS_STAT_T 
 * @brief  定义文件属性结构
 */
typedef struct
{
    UINT64 uSize;               /**< 文件实际大小,单位Byte*/
    UINT32 uAttr;               /**< 位图，参考SYS_FS_ATTR_T*/
    UINT32 uClusterSize;        /**< 文件簇大小,单位Byte*/
    UINT64 uSpaceSize;          /**< 文件实际占用空间大小,单位 Byte,大小为文件大小的整数倍*/
    UINT32 uStartCluser;        /**< 文件起始簇号 */
    UINT32 uRes[3];             /**< 预留字节*/
}SYS_FS_STAT_T;

/** 不透明文件ID */
typedef LONG  FILE_ID;
/** 不透明目录ID */
typedef LONG  DIR_ID;
/** 不透明目录FD */
typedef INT32  FILE_FD;

/** 文件位置偏移current */
#ifndef SEEK_CUR
#define SEEK_CUR             (1)
#endif

/**文件位置偏移到头部 */
#ifndef SEEK_SET
#define SEEK_SET             (0)
#endif

/**文件位置偏移到尾部 */
#ifndef SEEK_END
#define SEEK_END             (2)
#endif

/**只读模式 */
#ifndef O_RDONLY
#define O_RDONLY             (0)
#endif

/**只写模式 */
#ifndef O_WRONLY
#define O_WRONLY             (1)
#endif

/**读写模式 */
#ifndef O_RDWR
#define O_RDWR               (2)
#endif

/** 创建 */
#ifndef O_CREAT
#define O_CREAT              (0x00000040)
#endif

/** 追加模式 */
#ifndef O_APPEND
#define O_APPEND             (0x00000400)
#endif

/** 数据同步，直到数据被写入实际flash才返回  */
#ifndef O_DSYNC
#define O_DSYNC              (0x00001000)
#endif

/** 数据同步，直到数据被写入实际flash，并且更新文件属性信息才返回 */
#ifndef O_SYNC
#define O_SYNC               (0x00101000)
#endif

/** 绕过告诉缓冲区，直接操作IO */
#ifndef O_DIRECT
#define O_DIRECT             (0x00010000)
#endif


/**@fn         ReadDirCallBack      
 * @brief      读目录文件
 * @param[in]  strDir          查看目录
 * @param[in]  pUserParam      自定义参数
 * @return     成功 0  失败 -1 
 */
typedef INT32 (*ReadDirCallBack)(const CHAR *strDirPath, VOID *pUserParam);

/**@fn         ReadFileCallBack      
 * @brief      遍历目录文件信息
 * @param[in]  strFilePath     文件全路径
 * @param[in]  pStFileState    文件状态信息指针
 * @param[in]  pUserParam      自定义数据
 * @return       成功 0  失败 -1 
 */
typedef INT32 (*ReadFileCallBack)(const CHAR *strFilePath, SYS_FS_STAT_T *pStFileState, VOID *pUserParam);

/**
 * @brief     打开文件      
 * @param[in] strPath 文件路径
 * @param[in] strMode 打开文件的方式, 参考man 3 fopen,如wr+
 * @return    成功返回 文件句柄 错误返回 NULL, 支持通过get_last_errno获取错误码
 */
FILE_ID* SysFile_fopen(const CHAR *strPath, const CHAR *strMode);

/**
 * @brief      关闭文件      
 * @param[in]  pFileID     文件句柄，通过SysFile_fopen获得
 * @return     成功返回0  错误返回 其他, 参考ERROR_CODE_E, 支持通过get_last_errno获取错误码
 */
INT32 SysFile_fclose(FILE_ID *pFileID);

/**
 * @brief      读文件数据      
 * @param[in]  pFileID 文件句柄，通过SysFile_fopen获得
 * @param[in]  pBuffer 目的地址
 * @param[in]  uSize  每块字节数
 * @param[in]  uCount  要读取的块数
 * @return     成功返回 实际读取的块数 错误返回 ERROR, 支持通过get_last_errno获取错误码
 * @note       到达文件末尾的时候，返回值可能小于uCount
 */
INT32 SysFile_fread(FILE_ID *pFileID, VOID *pBuffer, UINT32 uSize, UINT32 uCount);

/**
 * @brief      写文件     
 * @param[in]  pFileID 文件句柄 
 * @param[in]  pBuffer 要写入文件的内容存放的地址
 * @param[in]  uSize   每块数据的大小
 * @param[in]  uCount  要写入的块数
 * @return     成功返回 实际写入的块数 错误返回 ERROR ,  支持通过get_last_errno获取错误码
 */
INT32 SysFile_fwrite(FILE_ID *pFileID, const VOID *pBuffer, UINT32 uSize, UINT32 uCount);

/**
 * @brief      强制写文件到存储介质      
 * @param[in]  pFileID  文件句柄
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E, 支持通过get_last_errno获取错误码
 */
INT32 SysFile_fsync(FILE_ID *pFileID);

/**
 * @brief      重新定位文件读写位置
 * @param[in]  pFileID  文件句柄
 * @param[in]  lOffset 为偏移量，正数表示正向偏移，负数表示负向偏移
 * @param[in]  iStartPos 设定从文件的哪里开始偏移,可能取值为：SEEK_CUR、 SEEK_END 或 SEEK_SET
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E, 支持通过get_last_errno获取错误码
 */
INT32 SysFile_fseek(FILE_ID *pFileID, LONG lOffset, INT32 iStartPos);

/**
 * @brief      文件指针所指位置相对于开头的偏移，单位字节byte          
 * @param[in]  pFileID  文件句柄
 * @return     成功返回 偏移值 错误返回 ERROR, 支持通过get_last_errno获取错误码
 */
LONG SysFile_ftell(FILE_ID *pFileID);

/**
 * @brief      查看文件信息
 * @param[in]  strPath 路径
 * @param[in]  pStSate 文件信息
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E, 支持通过get_last_errno获取错误码
 */
INT32 SysFile_fstat(const CHAR *strPath, SYS_FS_STAT_T *pStSate);

/**
 * @brief      判断是否已经到达文件末尾
 * @param[in]  pFileID  文件句柄
 * @return     > 0 ,已经到达文件末尾;= 0 , 未到达文件末尾;< 0 , 文件句柄错误
 */
INT32 SysFile_feof(FILE_ID *pFileID);

/**
 * @brief      获取文件当前定位
 * @param[in]  pFileID  文件句柄
 * @param[out] plOffset 文件输出偏移
 * @return     成功返回0,错误返回其他
 */
INT32 SysFile_fgetpos(FILE_ID *pFileID, LONG *plOffset);

/**
 * @brief      读目录文件夹
 * @param[in]  strDir          查看目录
 * @param[in]  pUserParam      自定义参数
 * @param[in]  pCallBack       读数据回调接口
 * @return     成功 0  失败 -1 
 */
INT32 SysFile_read_dir(const CHAR *strDir, VOID *pUserParam, ReadDirCallBack pCallBack);

/**
 * @brief      读目录文件
 * @param[in]  strDir          查看目录
 * @param[in]  pUserParam      自定义参数
 * @param[in]  pCallBack       读数据回调接口
 * @return     成功 0  失败 -1 
 */
INT32 SysFile_read_dir_file(const CHAR *strDir, VOID *pUserParam, ReadFileCallBack pCallBack);

/**
 * @brief     打开文件      
 * @param[in] strPath   文件路径
 * @param[in] iFlag     打开文件的方式, 参考man 2 open,如 O_RDWR | O_CREAT | O_DIRECT | O_DSYNC
 * @return    成功返回文件句柄 错误返回ERROR，并输出strerror
 */
FILE_FD SysFile_open(const CHAR *strPath, INT32 iFlag);

/**
 * @brief      关闭文件      
 * @param[in]  iFileID   文件句柄，通过SysFile_open获得
 * @return     成功返回文件句柄 错误返回ERROR，并输出strerror
 */
INT32 SysFile_close(FILE_FD iFileID);

/**
 * @brief      读文件数据      
 * @param[in]  iFileID 文件句柄，通过SysFile_open获得
 * @param[in]  pBuffer 目的地址
 * @param[in]  uSize   要读取的字节数
 * @return     成功返回 实际读取的块数 错误返回 ERROR, 并输出strerror
 */
INT32 SysFile_read(FILE_FD iFileID, VOID *pBuffer, UINT32 uSize);

/**
 * @brief      写文件     
 * @param[in]  iFileID  文件句柄，通过SysFile_open获得 
 * @param[in]  pBuffer    要写入文件的内容存放的地址
 * @param[in]  uSize      要写入的字节数
 * @return     成功返回 实际写入的块数 错误返回 ERROR, 并输出strerror
 */
INT32 SysFile_write(FILE_FD iFileID, const VOID *pBuffer, UINT32 uSize);

/**
 * @brief      重新定位文件读写位置
 * @param[in]  iFileID  文件句柄，通过SysFile_open获得 
 * @param[in]  lOffset 为偏移量，正数表示正向偏移，负数表示负向偏移
 * @param[in]  iStartPos 设定从文件的哪里开始偏移,可能取值为：SEEK_CUR、 SEEK_END 或 SEEK_SET
 * @return     成功返回 OK 错误返回ERROR，并输出strerror
 */
INT32 SysFile_seek(FILE_FD iFileID, LONG lOffset, INT32 iStartPos);

/**
 * @brief      文件锁
 * @param[in]  iFd   文件句柄
 * @param[in]  eType 文件锁类型
 * @return     成功返回OK 失败返回ERROR
 */    
INT32 SysFile_flock(INT32 iFd, SYS_LOCK_TYPE_E eType);

/**
 * @brief      修改文件权限
 * @param[in]  iFd   文件句柄
 * @param[in]  iMode 权限模式，用八进制表示，如0666
 * @return     成功返回OK 失败返回ERROR
 */    
INT32 SysFile_fchmod(INT32 iFd, INT32 iMode);

/**
 * @brief      调整文件大小，将文件截断到指定大小
 * @param[in]  iFd   文件句柄
 * @param[in]  uSize 需要调整的文件大小
 * @return     成功返回OK 失败返回ERROR
 */    
INT32 SysFile_ftruncate(INT32 iFd, UINT32 uSize);

#endif/*SYS_FILE_H*/