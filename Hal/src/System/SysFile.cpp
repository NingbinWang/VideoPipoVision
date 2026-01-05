#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/ioctl.h>
#include "SysFile.h"
#include "SysMedium.h"
#include <sys/file.h>
#include "Logger.h"
/**@fn        SysFile_open    
 * @brief     打开文件      
 * @param[in] strPath   文件路径
 * @param[in] iFlag     打开文件的方式, 参考man 2 open,如 O_RDWR | O_CREAT | O_DIRECT | O_DSYNC
 * @return    成功返回文件句柄 错误返回ERROR，并输出strerror
 */
FILE_FD SysFile_open(const CHAR * strPath, INT32 iFlag)
{
    INT32 iRet = ERROR;
    INT32 iFileId = -1;
    
    if(NULL == strPath)
    {
        LOG_ERROR("invaild param input\n");
        return iRet;
    }

    iFileId = open(strPath, iFlag);
    if(iFileId < 0)
    {
        LOG_ERROR("open file:%s error iFlag:%d errno:%s \n",strPath,iFlag,strerror(errno));
        return iRet;
    }
	return iFileId;
}

/**@fn         SysFile_close
 * @brief      关闭文件      
 * @param[in]  iFileFd   文件句柄，通过SysFile_open获得
 * @return     成功返回文件句柄 错误返回ERROR，并输出strerror
 */
INT32 SysFile_close(FILE_FD iFileFd)
{
    INT32 iRet = ERROR;
    
    if(iFileFd < 0)
    {
        LOG_ERROR("invaild param input\n");
        return iRet;
    }
    iRet = close(iFileFd);
    if (OK != iRet)
    {
        LOG_ERROR("close file id:%d error,errno:%s\n",iFileFd,strerror(errno));
        return iRet;
    }
	return iRet;
}

/**@fn         SysFile_read
 * @brief      读文件数据      
 * @param[in]  iFileFd 文件句柄，通过SysFile_open获得
 * @param[in]  pBuffer 目的地址
 * @param[in]  uSize   要读取的字节数
 * @return     成功返回 实际读取的块数 错误返回 ERROR, 并输出strerror
 */
INT32 SysFile_read(FILE_FD iFileFd, VOID *pBuffer, UINT32 uSize)
{
    INT32 iRet = ERROR;
    INT32 iReadCount= 0;

    if(iFileFd < 0 || NULL == pBuffer || 0 == uSize)
    {
        LOG_ERROR("invalid param inptu\n");
        return iRet;
    }

    iReadCount = read(iFileFd, pBuffer, uSize);
    if(iReadCount < 0)
    {
        LOG_ERROR("read error pBuffer:%p uSize:%u,errno:%s\n",pBuffer,uSize,strerror(errno));
        return iReadCount;
    }

    return iReadCount;
}

/**@fn         SysFile_write
 * @brief      写文件     
 * @param[in]  iFileFd  文件句柄，通过SysFile_open获得 
 * @param[in]  pBuffer    要写入文件的内容存放的地址
 * @param[in]  uSize      要写入的字节数
 * @return     成功返回 实际写入的块数 错误返回 ERROR, 并输出strerror
 */
INT32 SysFile_write(FILE_FD iFileFd, const VOID * pBuffer, UINT32 uSize)
{
    INT32 iRet = ERROR;
    INT32 iWriteCount= 0;

    if(iFileFd < 0 || NULL == pBuffer || 0 == uSize)
    {
        LOG_ERROR("invalid param input\n");
        return iRet;
    }

    iWriteCount = write(iFileFd, pBuffer, uSize);
    if(iWriteCount < 0)
    {
        LOG_ERROR("write error,errno:%s\n",strerror(errno));
        return iWriteCount;
    }

    return iWriteCount;    
}

/**@fn         SysFile_seek
 * @brief      重新定位文件读写位置
 * @param[in]  pFileID  文件句柄，通过SysFile_open获得 
 * @param[in]  lOffset为偏移量，正数表示正向偏移，负数表示负向偏移
 * @param[in]  iStartPos 设定从文件的哪里开始偏移,可能取值为：SEEK_CUR、 SEEK_END 或 SEEK_SET
 * @return     成功返回 OK 错误返回ERROR，并输出strerror
 */
INT32 SysFile_seek(FILE_FD iFileFd, LONG lOffset, INT32 iStartPos)
{
    INT32 iRet = ERROR;

    if(iFileFd < 0)
    {
        LOG_ERROR("invalid param input\n");
        return iRet;
    }
    
    iRet = lseek(iFileFd, lOffset, iStartPos);
    if (ERROR == iRet)
    {
        LOG_ERROR("lseek lOffset %ld iStartPos %d error,errno:%s\n",lOffset,iStartPos,strerror(errno));
        return iRet;
    }
    return iRet;
}



/**@fn        SysFile_fopen    
 * @brief     打开文件      
 * @param[in] strPath 文件路径
 * @param[in] mode 打开文件的方式, 参考man 3 fopen,如wr+
 * @return    成功返回 文件句柄 错误返回 NULL, 支持通过get_last_errno获取错误码
 */
FILE_ID* SysFile_fopen(const CHAR *strPath, const CHAR *strMode)
{
    FILE *pHandle = NULL ;
    
    if(NULL == strPath || NULL == strMode)
    {
        LOG_ERROR("file invaild param \n");
        return NULL;
    }

    pHandle = fopen(strPath, strMode);
    if (NULL == pHandle)
    {
        LOG_ERROR("fopen file error :%s strMode:%s errno:%s \n",strPath,strMode,strerror(errno));
        return NULL;
    }
	return (FILE_ID *)pHandle;
}

/**@fn         SysFile_fclose
 * @brief      关闭文件      
 * @param[in]  pFileID     文件句柄，通过SysFile_fopen获得
 * @return     成功返回0  错误返回 其他, 参考ERROR_CODE_E, 支持通过get_last_errno获取错误码
 */
INT32 SysFile_fclose(FILE_ID *pFileID)
{
    INT32 iRet = ERROR;
    if (NULL == pFileID)
    {
        LOG_ERROR("file invaild param \n");
        return ERROR;
    }
    iRet = fclose((FILE *)pFileID);            /*关闭打开的文件*/
    if (OK != iRet)
    {
        LOG_ERROR("fclose file failed - errno[%d:%s]\n", errno, strerror(errno));
        return ERROR;
    }
	return iRet;
}

/**@fn         SysFile_fread
 * @brief      读文件数据      
 * @param[in]  pFileID 文件句柄，通过SysFile_fopen获得
 * @param[in]  pBuffer 目的地址
 * @param[in]  uSize  每块字节数
 * @param[in]  uCount  要读取的块数
 * @return     成功返回 实际读取的块数 错误返回 ERROR, 支持通过get_last_errno获取错误码
 * @note       到达文件末尾的时候，返回值可能小于uCount
 */
INT32 SysFile_fread(FILE_ID *pFileID, VOID *pBuffer, UINT32 uSize, UINT32 uCount)
{
    UINT32  uReadCount= 0;
    if(NULL == pFileID || NULL == pBuffer || 0 == (uSize * uCount))
    {
        LOG_ERROR("file invaild param :%p :%p :%d \n",pFileID,pBuffer,(uSize * uCount));
        return ERROR;
    }

    uReadCount = fread(pBuffer, uSize, uCount, (FILE *)pFileID);
    if (uReadCount == uCount)
    {
        return uReadCount;
    }
    if (SysFile_feof(pFileID) > 0)
    {
        return uReadCount;
    }
    LOG_ERROR("SysFile_feof failed - errno[%d:%s]\n", errno, strerror(errno));
    return ERROR;
}

/**@fn         SysFile_fwrite
 * @brief      写文件     
 * @param[in]  pFileID 文件句柄 
 * @param[in]  pBuffer 要写入文件的内容存放的地址
 * @param[in]  uSize   每块数据的大小
 * @param[in]  uCount  要写入的块数
 * @return     成功返回 实际写入的块数 错误返回 ERROR ,  支持通过get_last_errno获取错误码
 */
INT32 SysFile_fwrite(FILE_ID *pFileID, const VOID *pBuffer, UINT32 uSize, UINT32 uCount)
{
     if(NULL == pFileID || NULL == pBuffer || 0 == (uSize * uCount))
    {
        LOG_ERROR("file invaild param :%p  :%p :%d \n",pFileID ,pBuffer,(uSize * uCount));
        return ERROR;
    }

    if (fwrite(pBuffer, uSize, uCount, (FILE *)pFileID) != uCount)
    {
        LOG_ERROR("fwrite failed - errno[%d:%s]\n", errno, strerror(errno));
        return ERROR;
    }
    return uCount;    
}

/**@fn         SysFile_fsync
 * @brief      强制写文件到存储介质      
 * @param[in]  pFileID  文件句柄
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E, 支持通过get_last_errno获取错误码
 */
INT32 SysFile_fsync(FILE_ID *pFileID)
{
    INT32 iRet = ERROR;
    INT32 iFd = -1 ;
    if (NULL == pFileID)
    {
        return ERROR;
    }
    iRet = fflush((FILE*)pFileID);
    if (OK != iRet)
    {
        LOG_ERROR(" fflush error :%s iRet:%d\n",strerror(errno), iRet);
    }

    iFd = fileno((FILE*)pFileID);
    iRet = fsync(iFd);
    if (OK != iRet)
    {
        LOG_ERROR(" fsync error :%s iRet:%d\n",strerror(errno), iRet);
    }
	return iRet;
}

/**@fn         SysFile_fseek
 * @brief      重新定位文件读写位置
 * @param[in]  pFileID  文件句柄
 * @param[in]  lOffset为偏移量，正数表示正向偏移，负数表示负向偏移
 * @param[in]  iStartPos 设定从文件的哪里开始偏移,可能取值为：SEEK_CUR、 SEEK_END 或 SEEK_SET
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E, 支持通过get_last_errno获取错误码
 */
INT32 SysFile_fseek(FILE_ID *pFileID, LONG lOffset, INT32 iStartPos)
{
    INT32 iRet = ERROR;
    if (NULL == pFileID)
    {
        return ERROR;
    }
    iRet = fseek((FILE *)pFileID, lOffset, iStartPos);
    if (OK != iRet)
    {
        LOG_ERROR("fseek failed - errno[%d:%s]\n", errno, strerror(errno));
    }
    return iRet;
}

/**@fn         SysFile_ftell    
 * @brief      文件指针所指位置相对于开头的偏移，单位字节byte          
 * @param[in]  pFileID  文件句柄
 * @return     成功返回 偏移值 错误返回 ERROR, 支持通过get_last_errno获取错误码
 */
LONG SysFile_ftell(FILE_ID *pFileID)
{
    INT32 iRet = ERROR;

    if (NULL == pFileID)
    {
       LOG_ERROR("file invaild param \n");
        return ERROR;
    }

    iRet = (LONG)ftell((FILE *)pFileID);
    if (ERROR == iRet)
    {
        LOG_ERROR("ftell failed - errno[%d:%s]\n", errno, strerror(errno));
    }
	return iRet;
}

/**@fn         SysFile_fstat    
 * @brief      查看文件信息
 * @param[in]  strPath 路径
 * @param[in]  pStSate 文件信息
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E, 支持通过get_last_errno获取错误码
 */
INT32 SysFile_fstat(const CHAR *strPath, SYS_FS_STAT_T *pStSate)
{
    INT32 iRet = ERROR;
    struct stat stState = {0};
    if(NULL == strPath || NULL == pStSate)
    {
        return ERROR;
    }
    iRet = stat(strPath, &stState);
    if(iRet != OK)
    {
        LOG_ERROR("strPath:%s error:%s iRet:%d \n", strPath, strerror(errno), iRet);
        return iRet;
    }
    if (S_ISDIR(stState.st_mode))
    {
        pStSate->uAttr |= SYS_FS_ATTR_DIR;
    }

    pStSate->uSpaceSize = stState.st_blocks * 512;  /* block大小是512 字节，参考man 2 stat */
    pStSate->uSize = stState.st_size;
    pStSate->uClusterSize = stState.st_blksize;
    return iRet;
}

/**@fn         SysFile_feof
 * @brief      判断是否已经到达文件末尾
 * @param[in]  pFileID  文件句柄
 * @return     > 0 ,已经到达文件末尾;= 0 , 未到达文件末尾;< 0 , 文件句柄错误
 */
INT32 SysFile_feof(FILE_ID *pFileID)
{
    if (NULL == pFileID)
    {
        return ERROR;
    }
    
	return feof((FILE *)pFileID);
}

/**@fn         SysFile_read_dir 暂时考虑到拷贝封装的性能消耗，互调反馈      
 * @brief      读目录文件夹
 * @param[in]  strDir          查看目录
 * @param[in]  pUserParam      自定义参数
 * @param[in]  pCallBack       读数据回调接口
 * @return     成功 0  失败 -1 
 */
INT32 SysFile_read_dir(const CHAR *strDir, VOID *pUserParam, ReadDirCallBack pCallBack)
{
    INT32 iRet = ERROR;
    DIR *pDir = NULL ;
    struct dirent *pDirRent = NULL;
    CHAR strDirPath[512] = {0};
    if(!strDir || !pUserParam || !pCallBack)
    {
        LOG_ERROR("invaild param :%p %p %p\n",strDir,pUserParam,pCallBack);
        return iRet;
    }
    pDir = opendir(strDir);
    if(pDir == NULL)
    {
        LOG_ERROR("open dir: %s failed - errno[%d:%s]\n",strDir, errno, strerror(errno));
        return iRet;
    }
    while((pDirRent = readdir(pDir)) != NULL)
    {
        //exfata 读不出文件
        if(strcmp(pDirRent->d_name,".") == 0)
        {
            continue;
        }
        if(strcmp(pDirRent->d_name,"..") == 0)
        {
            continue;
        }
        memset(strDirPath,0,sizeof(strDirPath));
        snprintf(strDirPath,sizeof(strDirPath),"%s/%s",strDir,pDirRent->d_name);
        pCallBack(strDirPath,pUserParam);
    }
    closedir(pDir);
    return OK;
}
/**@fn         SysFile_read_dir_file      
 * @brief      读目录文件
 * @param[in]  strDir          查看目录
 * @param[in]  pUserParam      自定义参数
 * @param[in]  pCallBack       读数据回调接口
 * @return     成功 0  失败 -1 
 */
INT32 SysFile_read_dir_file(const CHAR *strDir, VOID *pUserParam, ReadFileCallBack pCallBack)
{
    INT32 iRet = ERROR;
    DIR *pDir = NULL ;
    struct dirent *pDirRent = NULL;
    CHAR strFilePath[512] = {0};
    SYS_FS_STAT_T stStatBuf = {0};
    if(!strDir || !pUserParam || !pCallBack)
    {
        return iRet;
    }

    pDir = opendir(strDir);
    if(pDir == NULL)
    {
        LOG_ERROR("open dir: %s failed - errno[%d:%s]\n",strDir, errno, strerror(errno));
        return iRet;
    }
    while((pDirRent = readdir(pDir)) != NULL)
    {
		if(strcmp(pDirRent->d_name,".") == 0)
        {
            continue;
        }
        if(strcmp(pDirRent->d_name,"..") == 0)
        {
            continue;
        }
        snprintf(strFilePath,sizeof(strFilePath),"%s/%s",strDir,pDirRent->d_name);
        iRet = SysFile_fstat(strFilePath, &stStatBuf);
        if(iRet < 0)
        {
            LOG_ERROR("fstat %s failed- errno[%d:%s]\n", strFilePath, errno, strerror(errno));
            break;
        }
        pCallBack(strFilePath,&stStatBuf,pUserParam);
    }

    closedir(pDir);
    return OK;
}

/**@brief	  获取文件当前定位
 * @param[in] pFileID  文件句柄
 * @param[in] plOffset为偏移量指针
 * @return 成功返回 0
 * @return 错误返回 其他, 参考ERROR_CODE_E, 支持通过get_last_errno获取错误码
 */
INT32 SysFile_fgetpos(FILE_ID *pFileID, LONG *plOffset)
{
    INT32 iRet = ERROR;
#ifndef SYS_ANDRIOD
    if (NULL == pFileID || NULL == plOffset)
    {
        return ERROR;
    }
    
    iRet = fgetpos((FILE *)pFileID, (fpos_t *)plOffset);
    if (OK != iRet)
    {
        LOG_ERROR("fgetpos failed - errno[%d:%s]\n", errno, strerror(errno));
    }
#endif
    return iRet;
}

/**@fn         SysFile_flock
 * @brief      文件锁
 * @param[in]  iFd   文件句柄
 * @param[in]  eType 文件锁类型
 * @return     成功返回OK 失败返回ERROR
 */    
INT32 SysFile_flock(INT32 iFd, SYS_LOCK_TYPE_E eType)
{
    INT32 iRet = ERROR;

    iRet = flock(iFd, eType);
    if(OK != iRet)
    {
        LOG_ERROR("flock error,ERROR:%s\n",strerror(errno));
        return iRet;
    }

    return OK;
}

/**@fn         SysFile_fchmod
 * @brief      修改文件权限
 * @param[in]  iFd   文件句柄
 * @param[in]  iMode 权限模式，用八进制表示，如0666
 * @return     成功返回OK 失败返回ERROR
 */    
INT32 SysFile_fchmod(INT32 iFd, INT32 iMode)
{
    INT32 iRet = ERROR;

    iRet = fchmod(iFd, iMode);
    if(OK != iRet)
    {
        LOG_ERROR("fchmod error,ERROR:%s\n",strerror(errno));
        return iRet;
    }

    return OK;
}

/**@fn         SysFile_ftruncate
 * @brief      调整文件大小，将文件截断到指定大小
 * @param[in]  iFd   文件句柄
 * @param[in]  uSize 需要调整的文件大小
 * @return     成功返回OK 失败返回ERROR
 */    
INT32 SysFile_ftruncate(INT32 iFd, UINT32 uSize)
{
    INT32 iRet = ERROR;

    iRet = ftruncate(iFd, uSize);
    if(OK != iRet)
    {
        LOG_ERROR("ftruncate error,ERROR:%s\n",strerror(errno));
        return iRet;
    }

    return OK;
}

