#ifndef SYS_POSIX_H
#define SYS_POSIX_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "sys_common.h"
#include "sys_mutex.h"
#include "sys_sem.h"
#include "sys_mqueue.h"
#include "sys_pthread.h"

#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/

/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */
/** FD SET大小定义 */
#define SYS_FD_SETSIZE    (1024)

/* ========================================================================== */
/*                             Type Declaration                               */
/* ========================================================================== */
/**
 *  @struct   SYS_FD_SET_T
 *  @brief    文件描述符集合
 */
typedef struct 
{
    UINT8 aFdBits [(SYS_FD_SETSIZE+7)/8];  /**< arrays of File descriptors */
}SYS_FD_SET_T;

/**
 *  @struct   SYS_TIMEVAL_T
 *  @brief    表示时间的结构体/A struct representing time
 */
typedef struct {
    LONG tvSec;        /**< seconds */
    LONG tvUsec;       /**< microseconds */
}SYS_TIMEVAL_T;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
/**
 * @brief      调用系统命令,如果命令的前缀为$，则进程在后台执行
 * @param[in]  iPid 进程ID
 * @return     无
 */
VOID sys_posix_kill(INT32 iPid);

/**
 * @brief      判断进程是否存在
 * @param[in]  iPid 进程ID
 * @return     存在返回OK，不存在返回ERROR
 */
INT32 sys_posix_verify(INT32 iPid);

/**
 * @brief      底层数据交互接口,IO控制接口
 * @param[in]  iFd      设备句柄
 * @param[in]  uCmd     命令
 * @param[out] pData    数据缓存
 * @return     存在返回OK，不存在返回ERROR
 */
INT32 sys_posix_ioctl(INT32 iFd, UINT32 uCmd,VOID *pData);

/**
 * @brief      调用系统命令,如果命令的前缀为$，则进程在后台执行
 * @param[in]  strCmd 系统命令字符串
 * @return     调用失败，返回-1；调用成功，若后台执行返回大于0的子进程号，前台执行则返回system接口返回值
 */
INT32 sys_posix_cmd_call(const CHAR *strCmd);

/**@fn         sys_posix_cmd_call_safety
 * @brief      安全方式调用系统命令
 * @param[in]  strCmd 系统命令字符串
 * @return     调用失败，返回-1；调用成功，返回0
 * @note       注意默认要执行的工具在/bin/路径下, 不在的要指定绝对路径
 */
INT32 sys_posix_cmd_call_safety(const CHAR *strCmd);

/**
 * @brief      拷贝文件 
 * @param[in]  strSrcPath  待移动文件全路径
 * @param[in]  strDstPath  目的路径,全路径
 * @return     成功返回 0   错误返回 其他, 参考ERROR_CODE_E, 支持通过get_last_errno获取错误码
 */
INT32 sys_posix_cp(const CHAR *strSrcPath, const CHAR *strDstPath);

/**
 * @brief      移动文件 
 * @param[in]  strSrcPath  待移动文件全路径
 * @param[in]  strDstPath  目的路径,全路径
 * @return     成功返回 0   错误返回 其他, 参考ERROR_CODE_E, 支持通过get_last_errno获取错误码
 */
INT32 sys_posix_move(const CHAR *strSrcPath, const CHAR *strDstPath);

/**
 * @brief      创建目录
 * @param[in]  strPath 路径
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E, 支持通过get_last_errno获取错误码
 */
INT32 sys_posix_mkdir(const CHAR *strPath);

/**
 * @brief      删除目录
 * @param[in]  strPath 路径
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E, 支持通过get_last_errno获取错误码
 */
INT32 sys_posix_rmdir(const CHAR *strPath);

/**
 * @brief      删除
 * @param[in]  strPath 文件路径
 * @return     成功返回 0 错误返回 其他, 参考ERROR_ID_E
 */
INT32 sys_posix_rm(const CHAR *strPath);

/**
 * @brief       强制写所有文件到存储介质
 * @return      成功 0  失败 -1 
 */
VOID sys_posix_sync(VOID);

/**
 * @brief      重命名文件
 * @param[in]  strOldPath 旧文件路径
 * @param[in]  strNewPath 新文件路径
 * @return     成功返回 0  错误返回 其他, 参考ERROR_ID_E
 */
INT32 sys_posix_rename(const CHAR *strOldPath, const CHAR *strNewPath);

/**
 * @brief      判断文件是否存在
 * @param[in]  strPath 路径
 * @return     存在返回 0 失败返回 其他
 */    
INT32 sys_posix_access(const CHAR *strPath);

/**
 * @brief      设置套接字属性，参考man fcntl
 * @param[in]  iFd      套接字句柄
 * @param[in]  iType    操作层面，比如是套接字层面(SOL_SOCKET)还是协议层面
 * @param[in]  iOptions 操作名称
 * @return     成功返回对应的值；失败参考ERROR_ID_E
 */
INT32 sys_posix_fcntl(INT32 iFd, UINT32 iType, UINT32 iOptions);

/**
 * @brief      select封装。
 * @param[in]  iFds  最大iFd + 1
 * @param[in]  pStReadFdSet 可读集合
 * @param[in]  pStWriteFdSet 可写集合
 * @param[in]  pStExceptionFdSet 异常集合
 * @param[in]  pStTimeOut  超时时间
 * @return     > 0 有事件(不代表有事件的iFd个数)； = 失败返回ERROR
 * @return     = 0 无事件 @return  ERROR , 失败，通过get_last_errno获取错误码
 * @note       并非所有的系统都支持读、写、异常集合,
 *             比如安霸就只支持读集合
 */
INT32 sys_posix_select(INT32 iFds, SYS_FD_SET_T *pStReadFdSet, SYS_FD_SET_T *pStWriteFdSet,
                    SYS_FD_SET_T *pStExceptionFdSet, SYS_TIMEVAL_T *pStTimeOut);

/**
 *  @brief      展开堆栈和转储调用跟踪
 *  @return     无
 */
VOID sys_posix_dump_stack (VOID);

/**
 * @brief      获取当前进程的进程/CPU名称
 * @param[out] strPname     进程/CPU名称
 * @param[in]  uNameLen     进程/CPU名称长度, 最大可以支持32字节
 * @return     0表示成功, 其他值表示失败
 */
INT32 sys_posix_processname_get (CHAR *strPname, UINT32 uNameLen);

/**
 * @brief      设置当前进程的进程/CPU名称
 * @param[in]  strPname     进程/CPU名称, 最大支持设置32字节长度
 * @return     0表示成功, 其他值表示失败
 */
INT32 sys_posix_processname_set (const CHAR *strPname);

/**
 * @brief      获取错误码
 * @return     详见errno 
 */
INT32 sys_posix_get_last_errno(VOID);

#ifdef __cplusplus
}
#endif/*__cplusplus*/

#endif/* SYS_POSIX_H */

