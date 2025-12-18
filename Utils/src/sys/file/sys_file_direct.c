#include "sys_log.h"
#include "sys_file.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/**@fn        sys_file_open    
 * @brief     打开文件      
 * @param[in] strPath   文件路径
 * @param[in] iFlag     打开文件的方式, 参考man 2 open,如 O_RDWR | O_CREAT | O_DIRECT | O_DSYNC
 * @return    成功返回文件句柄 错误返回ERROR，并输出strerror
 */
FILE_FD sys_file_open(const CHAR * strPath, INT32 iFlag)
{
    INT32 iRet = ERROR;
    INT32 iFileId = -1;
    
    if(NULL == strPath)
    {
        PR_ERR("invaild param input\n");
        return iRet;
    }

    iFileId = open(strPath, iFlag);
    if(iFileId < 0)
    {
        PR_ERR("open file:%s error iFlag:%d errno:%s \n",strPath,iFlag,strerror(errno));
        return iRet;
    }
	return iFileId;
}

/**@fn         sys_file_close
 * @brief      关闭文件      
 * @param[in]  iFileFd   文件句柄，通过sys_file_open获得
 * @return     成功返回文件句柄 错误返回ERROR，并输出strerror
 */
INT32 sys_file_close(FILE_FD iFileFd)
{
    INT32 iRet = ERROR;
    
    if(iFileFd < 0)
    {
        PR_ERR("invaild param input\n");
        return iRet;
    }
    iRet = close(iFileFd);
    if (OK != iRet)
    {
        PR_ERR("close file id:%d error,errno:%s\n",iFileFd,strerror(errno));
        return iRet;
    }
	return iRet;
}

/**@fn         sys_file_read
 * @brief      读文件数据      
 * @param[in]  iFileFd 文件句柄，通过sys_file_open获得
 * @param[in]  pBuffer 目的地址
 * @param[in]  uSize   要读取的字节数
 * @return     成功返回 实际读取的块数 错误返回 ERROR, 并输出strerror
 */
INT32 sys_file_read(FILE_FD iFileFd, VOID *pBuffer, UINT32 uSize)
{
    INT32 iRet = ERROR;
    INT32 iReadCount= 0;

    if(iFileFd < 0 || NULL == pBuffer || 0 == uSize)
    {
        PR_ERR("invalid param inptu\n");
        return iRet;
    }

    iReadCount = read(iFileFd, pBuffer, uSize);
    if(iReadCount < 0)
    {
        PR_ERR("read error pBuffer:%p uSize:%u,errno:%s\n",pBuffer,uSize,strerror(errno));
        return iReadCount;
    }

    return iReadCount;
}

/**@fn         sys_file_write
 * @brief      写文件     
 * @param[in]  iFileFd  文件句柄，通过sys_file_open获得 
 * @param[in]  pBuffer    要写入文件的内容存放的地址
 * @param[in]  uSize      要写入的字节数
 * @return     成功返回 实际写入的块数 错误返回 ERROR, 并输出strerror
 */
INT32 sys_file_write(FILE_FD iFileFd, const VOID * pBuffer, UINT32 uSize)
{
    INT32 iRet = ERROR;
    INT32 iWriteCount= 0;

    if(iFileFd < 0 || NULL == pBuffer || 0 == uSize)
    {
        PR_ERR("invalid param input\n");
        return iRet;
    }

    iWriteCount = write(iFileFd, pBuffer, uSize);
    if(iWriteCount < 0)
    {
        PR_ERR("write error,errno:%s\n",strerror(errno));
        return iWriteCount;
    }

    return iWriteCount;    
}

/**@fn         sys_file_seek
 * @brief      重新定位文件读写位置
 * @param[in]  pFileID  文件句柄，通过sys_file_open获得 
 * @param[in]  lOffset为偏移量，正数表示正向偏移，负数表示负向偏移
 * @param[in]  iStartPos 设定从文件的哪里开始偏移,可能取值为：SEEK_CUR、 SEEK_END 或 SEEK_SET
 * @return     成功返回 OK 错误返回ERROR，并输出strerror
 */
INT32 sys_file_seek(FILE_FD iFileFd, LONG lOffset, INT32 iStartPos)
{
    INT32 iRet = ERROR;

    if(iFileFd < 0)
    {
        PR_ERR("invalid param input\n");
        return iRet;
    }
    
    iRet = lseek(iFileFd, lOffset, iStartPos);
    if (ERROR == iRet)
    {
        PR_ERR("lseek lOffset %ld iStartPos %d error,errno:%s\n",lOffset,iStartPos,strerror(errno));
        return iRet;
    }
    return iRet;
}
