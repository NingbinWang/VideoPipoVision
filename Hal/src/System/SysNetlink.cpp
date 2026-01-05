#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <linux/genetlink.h>
#include <linux/netlink.h>
#include <pthread.h>

#include "Common.h"
#include "Syserrno.h"
#include "Logger.h"
#include "SysNetlink.h"
#include "SysNetlinkUeventInner.h"
#include "SysNetlinkGenericInner.h"
#include "SysList.h"


typedef struct {
    INT32 iFd;
    UINT32 uProtocol;
    struct list_head stlsNode;
} NETLINK_FD_DESC_T;


static pthread_mutex_t mtxsNetlinkList = PTHREAD_MUTEX_INITIALIZER;

static LIST_HEAD(lsNetlinkFdList);

INT32 SysNetlink_create(UINT32 uProtocol, VOID *pArg)
{
    INT32 iSocketFd = -1;
    INT32 iRet = 0;
    SYS_GENLNETLINK_ID_T *pGenl = NULL;
    NETLINK_FD_DESC_T *pDesc = NULL;

    if (uProtocol >= SYS_NETLINK_MAX) {
        LOG_ERROR("Invalid params protocol(%u)\n", uProtocol);
        return -EINVAL;
    }

    switch (uProtocol) {
        case SYS_NETLINK_GENERIC:
            if (NULL == pArg) {
                LOG_ERROR("Invalid params protocol(%u)\n", uProtocol);
                return -EINVAL;
            }

            pGenl = (SYS_GENLNETLINK_ID_T *)pArg;

            iSocketFd = SysGenlNetlink_create_nl_socket(0);
            if (iSocketFd < 0) {
                LOG_ERROR("SysGenlNetlink_create_nl_socket failed\n");
                break;
            }

            iRet = SysGenlNetlink_get_id(iSocketFd, pGenl);
            if (iRet < 0) {
                LOG_ERROR("SysGenlNetlink_get_id failed\n");
                close(iSocketFd);
                iSocketFd = iRet;
                break;
            }

            if (pGenl->pGroup) {
                /* 将获取到的组id加入到多组播组中 */
                //if (setsockopt(iSocketFd, SOL_NETLINK, NETLINK_ADD_MEMBERSHIP, &pGenl->uGroupId, sizeof(pGenl->uGroupId)) < 0) {
                //    LOG_ERROR("setsockopt failed errno - [%d:%s]\n", errno, strerror(errno));
                //    close(iSocketFd);
                //    iSocketFd = -1;
                //    break;
                //}

                LOG_DEBUG("Listening for multicast notifications (group=%d)...\n", pGenl->uGroupId);
            }
            break;

        case SYS_NETLINK_UEVENT:
            iSocketFd = SysUeventNetlink_create(0);
            if (iSocketFd < 0) {
                LOG_ERROR("sys_uevent_netlink_create failed\n");
                break;
            }
            break;
    }

    if (iSocketFd > 0) {
        pDesc = (NETLINK_FD_DESC_T *)malloc(sizeof(NETLINK_FD_DESC_T));
        if (NULL == pDesc) {
            LOG_ERROR("netlink fd description malloc failed errno[%d:%s]\n", errno, strerror(errno));
            close(iSocketFd);
            iSocketFd = -1;
        } else {
            pDesc->iFd = iSocketFd;
            pDesc->uProtocol = uProtocol;
            pthread_mutex_lock(&mtxsNetlinkList);
            list_add_tail(&pDesc->stlsNode, &lsNetlinkFdList);
            pthread_mutex_unlock(&mtxsNetlinkList);
        }
    }

    return iSocketFd;
}

static NETLINK_FD_DESC_T *netlink_fd2desc(INT32 iSockFd)
{
    NETLINK_FD_DESC_T* pDesc = NULL;
    NETLINK_FD_DESC_T* pDescTmp = NULL;

    pthread_mutex_lock(&mtxsNetlinkList);
    list_for_each_entry(pDesc, &lsNetlinkFdList, stlsNode) {
        if (pDesc->iFd == iSockFd ) {
            pDescTmp = pDesc;
            break;
        }
    }
    pthread_mutex_unlock(&mtxsNetlinkList);
    return pDescTmp;
}

INT32 SysNetlink_recv(INT32 iSockfd, VOID *pBuffer, UINT32 uBufferLen)
{
    INT32 iRet = -1;
    NETLINK_FD_DESC_T* pDesc = NULL;

    if (iSockfd <= 0 || NULL == pBuffer || 0 == uBufferLen) {
        LOG_ERROR("Invalid params socket fd(%u) buffer(%p) len(%u)\n", iSockfd, pBuffer, uBufferLen);
        return -EINVAL;
    }

    pDesc = netlink_fd2desc(iSockfd);
    if (NULL == pDesc) {
        LOG_ERROR("socket fd(%d) is abnormal\n", iSockfd);
        return -EINVAL;
    }

    switch (pDesc->uProtocol) {

        case SYS_NETLINK_GENERIC:
            //iRet = SysGenlNetlink_recv_msg(iSockfd, pBuffer, uBufferLen);
            if (iRet < 0) {
                LOG_ERROR("SysGenlNetlink_recv_msg failed\n");
            }
            break;

        case SYS_NETLINK_UEVENT:
            iRet = SysUeventNetlink_recv(iSockfd, pBuffer, uBufferLen);
            if (iRet < 0) {
                LOG_ERROR("SysUeventNetlink_recv failed\n");
            }
            break;

        default:
            LOG_ERROR("socket fd(%d) protocol(%u) not support\n", iSockfd, pDesc->uProtocol);
            break;
    }

    return iRet;
}

INT32 SysNetlink_send(INT32 iSockfd, VOID *pBuffer, UINT32 uBufferLen)
{
    INT32 iRet = -1;
    NETLINK_FD_DESC_T* pDesc = NULL;

    if (iSockfd <= 0 || NULL == pBuffer || 0 == uBufferLen) {
        LOG_ERROR("Invalid params socket fd(%u) buffer(%p) len(%u)\n", iSockfd, pBuffer, uBufferLen);
        return -EINVAL;
    }

    pDesc = netlink_fd2desc(iSockfd);
    if (NULL == pDesc) {
        LOG_ERROR("socket fd(%d) is abnormal\n", iSockfd);
        return -EINVAL;
    }

    switch (pDesc->uProtocol) {

        case SYS_NETLINK_GENERIC:
            iRet = SysGenlNetlink_send_msg(iSockfd, pBuffer, uBufferLen);
            if (iRet < 0) {
                LOG_ERROR("SysGenlNetlink_send_msg failed\n");
            }
            break;

        default:
            LOG_ERROR("socket fd(%d) protocol(%u) not support send\n", iSockfd, pDesc->uProtocol);
            break;
    }

    return 0;
}

INT32 SysNetlink_close(INT32 iSockfd)
{
    NETLINK_FD_DESC_T* pDesc = NULL;

    pDesc = netlink_fd2desc(iSockfd);
    if (NULL == pDesc) {
        LOG_ERROR("socket fd(%d) is abnormal\n", iSockfd);
        return -EINVAL;
    }

    if (close(iSockfd) < 0) {
        LOG_ERROR("clsoe socket fd failed errno[%d:%s]\n", errno, strerror(errno));
        return -1;
    }

    pthread_mutex_lock(&mtxsNetlinkList);
    list_del(&pDesc->stlsNode);
    pthread_mutex_unlock(&mtxsNetlinkList);

    free(pDesc);
    return 0;
}
