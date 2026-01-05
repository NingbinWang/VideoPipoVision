#include "SysShareMemory.h"
#include "Logger.h"
#include "SysPosix.h"
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <errno.h>

/**
 * @brief      键值转换函数，用于获取唯一标识符
 * @param[in]  strPath    文件路径，文件必须存在
 * @param[in]  iProjectId 序号值，取值范围1-255
 * @return     成功返回key键值 失败返回ERROR
 */
INT32 SysShareMemory_ftok(const CHAR *strPath, INT32 iProjectId)
{
    INT32 iRet = ERROR;
    INT32 iKey = 0;

    if((NULL == strPath) || (iProjectId < 1 || iProjectId > 255))
    {
        LOG_ERROR("invalid param input\n");
        return iRet;
    }

    iRet = SysPosix_access(strPath);
    if(OK != iRet)
    {
        LOG_ERROR("strPath:%s not exist\n",strPath);
        return ERROR;
    }

    iKey = ftok(strPath, iProjectId);
    if(ERROR == iKey)
    {
        LOG_ERROR("ftok error,ERROR:%s\n",strerror(errno));
        return ERROR;
    }

    return iKey;
}

/**
 * @brief      用于创建或者获取共享内存
 * @param[in]  uKey 标识符
 * @param[in]  uSize 共享内存大小
 * @param[in]  eIpcType 共享内存FLAG
 * @return     成功返回共享内存的ID ， 失败返回 - 1
 */
INT32 SysShareMemory_get(INT32 iKey, UINT64 uSize, SYS_SHM_MODE_BIT_E eIpcType)
{
    INT32 iRet = ERROR;
    INT32 iShmID = 0;
    
    if(SYS_SHM_MODE_BIT_CREATE != eIpcType && SYS_SHM_MODE_BIT_EXCL != eIpcType &&
        SYS_SHM_MODE_BIT_NOWAIT != eIpcType)
    {
        LOG_ERROR("invalid param input\n");
        return iRet;
    }
    
    iShmID = shmget(iKey, uSize, eIpcType | 0666);
    if(iShmID < 0)
    {
        LOG_ERROR("iKey:%u shmget error,ERROR:%s\n",iKey,strerror(errno));
        return iRet;
    }
    return iShmID;
}

/**
 * @brief      将共享内存附加到进程的虚拟地址空间
 * @param[in]  iShmID 共享内存操作句柄
 * @param[in]  pShareMemoryAddr 共享内存地址
 * @param[in]  eAttachFlag 共享内存FLAG
 * @return     成功返回共享内存 失败返回NULL
 */
VOID *SysShareMemory_attach(INT32 iShmID, const VOID *pShareMemoryAddr, SYS_SHM_ATTACH_FLAG_E eAttachFlag)
{
    VOID *pBuf = NULL;
    
    if(SYS_SHM_ATTACH_FLAG_RW != eAttachFlag && SYS_SHM_ATTACH_FLAG_RDONLY != eAttachFlag &&
        SYS_SHM_ATTACH_FLAG_ROUND != eAttachFlag && SYS_SHM_ATTACH_FLAG_REMAP != eAttachFlag &&
        SYS_SHM_ATTACH_FLAG_EXEC != eAttachFlag)
    {
        LOG_ERROR("invalid param input\n");
        return NULL;
    }
    pBuf = shmat(iShmID, pShareMemoryAddr, eAttachFlag);
    if(MAP_FAILED == pBuf)
    {
        LOG_ERROR("iShmID:%d shmat error,ERROR:%s\n",iShmID,strerror(errno));
        return NULL;
    }
    return pBuf;
}

/**
 * @brief      共享内存分离接口
 * @param[in]  pShareMemoryAddr 共享内存地址
 * @return     成功返回 OK 错误返回 ERROR
 */
INT32 SysShareMemory_detach(const VOID *pShareMemoryAddr)
{
    INT32 iRet = ERROR;
    
    if(NULL == pShareMemoryAddr)
    {
        LOG_ERROR("invalid param input\n");
        return iRet;
    }
    iRet = shmdt(pShareMemoryAddr);
    if(iRet < 0)
    {
        LOG_ERROR("shmdt error,ERROR:%s\n",strerror(errno));
        return ERROR;
    }
    return OK;
}

/**
 * @brief      控制共享内存
 * @param[in]  iShmID 共享内存操作句柄
 * @param[in]  eCmd   指令
 * @param[in]  pStData 共享内存信息，删除可以传入NULL
 * @return     成功返回 OK 错误返回 ERROR
 */
INT32 SysShareMemory_control(INT32 iShmID, SYS_SHM_CMD_IPC_CMD_E eCmd, SYS_SHM_INFO_T *pStData)
{
    INT32 iRet = ERROR;
    //struct shmid_ds stBuf;
    
    iRet = shmctl(iShmID, eCmd, NULL);
    if(iRet < 0)
    {
        LOG_ERROR("shmctl error,ERROR:%s\n",strerror(errno));
        return ERROR;
    }
    return OK;
}

/**@fn         SysShareMemory_open      
 * @brief      posix共享内存创建接口
 * @param[in]  strName 共享内存名称
 * @param[in]  iFlag   Flag类型
 * @param[in]  iMode   类型
 * @return     成功返回文件描述符，错误返回 ERROR
 */
INT32 SysShareMemory_open(const CHAR *strName, INT32 iFlag, INT32 iMode)
{
    INT32 iRet = ERROR;

    if(NULL == strName)
    {
        LOG_ERROR("invalid param input\n");
        return iRet;
    }

    iRet = shm_open(strName, iFlag, iMode);
    if(iRet < 0)
    {
        LOG_ERROR("shm_open error,ERROR:%s\n",strerror(errno));
        return ERROR;
    }
    
    return iRet;
}

/**@fn         SysShareMemory_unlink      
 * @brief      posix共享内存删除接口
 * @param[in]  strName 共享内存名称
 * @return     成功返回 OK 错误返回 ERROR
 */
INT32 SysShareMemory_unlink(const CHAR *strName)
{
    INT32 iRet = ERROR;

    if(NULL == strName)
    {
        LOG_ERROR("invalid param input\n");
        return iRet;
    }

    iRet = shm_unlink(strName);
    if(iRet < 0)
    {
        LOG_ERROR("shm_unlink error,ERROR:%s\n",strerror(errno));
        return ERROR;
    }
    
    return iRet;
}

/**@fn         SysShareMemory_mmap
 * @brief      将文件映射到共享内存
 * @param[in]  iFd               文件描述符
 * @param[in]  pShareMemoryAddr  共享内存地址，指定为NULL则由系统自动分配
 * @param[in]  iSize             共享内存大小
 * @param[in]  iProt             共享内存映射区操作权限，参考man 3 mmap，如PORT_READ|PORT_WRITE
 * @param[in]  iFlag             标志位参数，参考man 3 mmap，如MAP_SHARED
 * @param[in]  ioffset           偏移值
 * @return     成功返回贡献内存地址 失败返回NULL
 */
VOID *SysShareMemory_mmap(INT32 iFd, VOID *pShareMemoryAddr, INT32 iSize, INT32 iProt, INT32 iFlag, INT32 ioffset)
{
    VOID *pAddr = NULL;

    if (iFd < 0) {
        LOG_ERROR("Invalid params fd(%d)\n", iFd);
        return NULL;
    }

    pAddr = mmap(pShareMemoryAddr, iSize, iProt, iFlag, iFd, ioffset);
    if(MAP_FAILED == pAddr)
    {
        LOG_ERROR("mmap error,ERROR:%s\n",strerror(errno));
        return NULL;
    }
    
    return pAddr;
}

/**@fn         SysShareMemory_munmap
 * @brief      取消共享内存映射
 * @param[in]  pShareMemoryAddr 共享内存地址，由mmap创建
 * @param[in]  iSize   共享内存大小
 * @return     成功返回OK 失败返回ERROR
 */
INT32 SysShareMemory_munmap(VOID *pShareMemoryAddr, INT32 iSize)
{
    INT32 iRet = ERROR;
    
    if(NULL == pShareMemoryAddr || iSize <= 0)
    {
        LOG_ERROR("invalid param input\n");
        return iRet;
    }

    iRet = munmap(pShareMemoryAddr, iSize);
    if(OK != iRet)
    {
        LOG_ERROR("munmap error,ERROR:%s\n",strerror(errno));
        return ERROR;
    }
    return OK;
}


