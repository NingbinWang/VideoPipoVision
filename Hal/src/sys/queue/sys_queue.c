#include <mqueue.h>
#include <unistd.h>
#include <sys/types.h>
#include "sys_log.h"
#include "sys_mqueue.h"
#include <sys/select.h>

#define MSG_Q_MODE          (0666)
#define MSG_Q_PRIO          (0)
#define MSG_Q_OFLAG         (O_CREAT|O_RDWR|O_EXCL)

typedef struct mq_attr  MSG_Q_ATTR;

/**@fn         sys_mqueue_create
 * @brief      创建一个消息队列
 * @param[in]  pStHandle 消息队列对象，入参，需非空
 * @param[in]  strName   消息队列名字
 * @param[in]  iMaxMsg   一个消息队列最大消息个数
 * @param[in]  iMsgLen   消息长度
 * @return     成功返回 0, 并且handle被修改;其他失败，参考ERROR_CODE_E
 */
INT sys_mqueue_create(MSG_Q_ID *pStHandle, const CHAR *strName, INT iMaxMsg, INT iMsgLen)
{
    CHAR sName[64] = {0};
    MSG_Q_ATTR stAttr = {0};
    MSG_Q_ID stMqId;

    if(strName == NULL || pStHandle == NULL)
    {
        PR_ERR("Mqueue open invalid ptr.\n");
        return ERROR;
    }

    #ifdef __x86_64__
    stAttr.mq_maxmsg = iMaxMsg > 8 ? 8 : iMaxMsg;
    snprintf(sName, sizeof(sName), "/%s_%d", strName, getuid());
    #else
    stAttr.mq_maxmsg = iMaxMsg;
    snprintf(sName, sizeof(sName), "/%s", strName);
    #endif

    stAttr.mq_msgsize = iMsgLen;

    mq_unlink(sName);

    stMqId = mq_open(sName, MSG_Q_OFLAG, MSG_Q_MODE, &stAttr);
    if(stMqId == ERROR)
    {
        PR_ERR("Mqueue %s open failed err:%s.\n", sName, strerror(errno));
        return ERROR;
    }

    *pStHandle = stMqId;

    return OK;
}
/**@fn         sys_mqueue_create
 * @brief      发送消息
 * @param[in]  pStHandle  消息队列句柄
 * @param[in]  strMsg     发送的消息
 * @param[in]  uLength    消息长度
 * @param[in]  uWaitTime  阻塞时长
 * @return     成功返回 0  错误返回 其他, 参考ERROR_CODE_E
 */
INT sys_mqueue_send(MSG_Q_ID *pStHandle, const CHAR *strMsg, UINT uLength, INT iWaitMs)
{
    INT iRet = 0;
    
    fd_set stWriteSets;
    fd_set stErrSets;
    
    struct timeval stWaitTime;
    struct timeval *pWaitTime = NULL;

    if(strMsg == NULL || pStHandle == NULL)
    {
        PR_ERR("Mqueue send invalid ptr.\n");
        return ERROR;
    }

    FD_ZERO(&stWriteSets);
    FD_ZERO(&stErrSets);
    FD_SET(*pStHandle, &stWriteSets);
    FD_SET(*pStHandle, &stErrSets);
    if(iWaitMs == WAIT_FOREVER)
    {
        pWaitTime = NULL;
    }
    else
    {
        memset(&stWaitTime, 0, sizeof(stWaitTime));
        stWaitTime.tv_sec = iWaitMs / 1000;
        stWaitTime.tv_usec = (iWaitMs % 1000) * 1000;
        pWaitTime = &stWaitTime;
    }
    
    iRet = select(*pStHandle + 1, NULL, &stWriteSets, &stErrSets, pWaitTime);
    if (iRet > 0 && FD_ISSET(*pStHandle, &stWriteSets))
    {
        iRet = mq_send(*pStHandle, strMsg, uLength, MSG_Q_PRIO);
    }
    else 
    {
        iRet = ERROR;
    }

    return iRet;
}

/**@fn         sys_mqueue_create
 * @brief      接受消息
 * @param[in]  pStHandle  消息队列句柄
 * @param[in]  strBuff    接受消息缓存
 * @param[in]  uBuffLen   缓存大小
 * @param[in]  uWaitTime  阻塞时长
 * @return     成功返回 0  错误返回 其他, 参考ERROR_CODE_E
 */
INT sys_mqueue_recv(MSG_Q_ID *pStHandle, CHAR *strBuff, INT iBuffLen, INT iWaitMs)
{
    INT iRet = 0;
    
    fd_set stReadSets;
    fd_set stErrSets;
    
    struct timeval stWaitTime;
    struct timeval *pWaitTime = NULL;
    
    if(strBuff == NULL || pStHandle == NULL)
    {
        PR_ERR("Mqueue send invalid ptr.\n");
        return ERROR;
    }

    FD_ZERO(&stReadSets);
    FD_ZERO(&stErrSets);
    FD_SET(*pStHandle, &stReadSets);
    FD_SET(*pStHandle, &stErrSets);

    if(WAIT_FOREVER == iWaitMs)
    {
        pWaitTime = NULL;
    }
    else
    {
        memset(&stWaitTime, 0, sizeof(stWaitTime));
        stWaitTime.tv_sec = iWaitMs / 1000;
        stWaitTime.tv_usec  = (iWaitMs % 1000) * 1000;
        pWaitTime = &stWaitTime;
    }
    
    iRet = select(*pStHandle + 1, &stReadSets, NULL, &stErrSets, pWaitTime);
    if (iRet > 0 && FD_ISSET(*pStHandle, &stReadSets))
    {
        iRet = mq_receive(*pStHandle, strBuff, iBuffLen, MSG_Q_PRIO);
    }
    else 
    {
        iRet = ERROR;
    }
    
    return iRet;
}

/**@fn         sys_mqueue_close
 * @brief      关闭消息队列
 * @param[in]  pStHandle 消息队列句柄
 * @return     成功返回 0 错误返回 其他, 参考ERROR_CODE_E
 */
INT sys_mqueue_close(MSG_Q_ID *pStHandle)
{   
    if(NULL == pStHandle)
    {
        return ERROR;
    }
    return mq_close(*pStHandle);
}