#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <linux/genetlink.h>
#include <linux/netlink.h>

#include "sys_common.h"
#include "sys_errno.h"
#include "sys_log.h"
#include "sys_netlink.h"
#include "sys_netlink_uevent_inner.h"
#include "sys_netlink_generic_inner.h"
#include "sys_mutex.h"
#include "sys_list.h"


typedef struct {
    INT32 iFd;
    UINT32 uProtocol;
    struct list_head stlsNode;
} NETLINK_FD_DESC_T;

static MUTEX_ID mtxsNetlinkList = 0;
static LIST_HEAD(lsNetlinkFdList);

INT32 sys_netlink_create(UINT32 uProtocol, VOID *pArg)
{
    INT32 iSocketFd = -1;
    INT32 iRet = 0;
    SYS_GENLNETLINK_ID_T *pGenl = NULL;
    NETLINK_FD_DESC_T *pDesc = NULL;

    if (uProtocol >= SYS_NETLINK_MAX) {
        PR_ERR("Invalid params protocol(%u)\n", uProtocol);
        return -EINVAL;
    }
    if (!mtxsNetlinkList) {
        iRet = sys_mutex_create(&mtxsNetlinkList,MUTEX_NORMAL);
        if (iRet) {
            PR_ERR("mutex(nl-fd-list-mtx) create failed ret(%d)\n", iRet);
            return iRet;
        }
    }

    switch (uProtocol) {
        case SYS_NETLINK_GENERIC:
            if (NULL == pArg) {
                PR_ERR("Invalid params protocol(%u)\n", uProtocol);
                return -EINVAL;
            }

            pGenl = (SYS_GENLNETLINK_ID_T *)pArg;

            iSocketFd = sys_genlnetlink_create_nl_socket(0);
            if (iSocketFd < 0) {
                PR_ERR("sys_genlnetlink_create_nl_socket failed\n");
                break;
            }

            iRet = sys_genlnetlink_get_id(iSocketFd, pGenl);
            if (iRet < 0) {
                PR_ERR("sys_genlnetlink_get_id failed\n");
                close(iSocketFd);
                iSocketFd = iRet;
                break;
            }

            if (pGenl->pGroup) {
                /* 将获取到的组id加入到多组播组中 */
                //if (setsockopt(iSocketFd, SOL_NETLINK, NETLINK_ADD_MEMBERSHIP, &pGenl->uGroupId, sizeof(pGenl->uGroupId)) < 0) {
                //    PR_ERR("setsockopt failed errno - [%d:%s]\n", errno, strerror(errno));
                //    close(iSocketFd);
                //    iSocketFd = -1;
                //    break;
                //}

                PR_DBG("Listening for multicast notifications (group=%d)...\n", pGenl->uGroupId);
            }
            break;

        case SYS_NETLINK_UEVENT:
            iSocketFd = sys_uevent_netlink_create(0);
            if (iSocketFd < 0) {
                PR_ERR("sys_uevent_netlink_create failed\n");
                break;
            }
            break;
    }

    if (iSocketFd > 0) {
        pDesc = malloc(sizeof(NETLINK_FD_DESC_T));
        if (NULL == pDesc) {
            PR_ERR("netlink fd description malloc failed errno[%d:%s]\n", errno, strerror(errno));
            close(iSocketFd);
            iSocketFd = -1;
        } else {
            pDesc->iFd = iSocketFd;
            pDesc->uProtocol = uProtocol;
            sys_mutex_lock(&mtxsNetlinkList, WAIT_FOREVER);
            list_add_tail(&pDesc->stlsNode, &lsNetlinkFdList);
            sys_mutex_unlock(&mtxsNetlinkList);
        }
    }

    return iSocketFd;
}

static NETLINK_FD_DESC_T *netlink_fd2desc(INT32 iSockFd)
{
    NETLINK_FD_DESC_T* pDesc = NULL;
    NETLINK_FD_DESC_T* pDescTmp = NULL;

    sys_mutex_lock(&mtxsNetlinkList, WAIT_FOREVER);

    list_for_each_entry(pDesc, &lsNetlinkFdList, stlsNode) {
        if (pDesc->iFd == iSockFd ) {
            pDescTmp = pDesc;
            break;
        }
    }
    sys_mutex_unlock(&mtxsNetlinkList);

    return pDescTmp;
}

INT32 sys_netlink_recv(INT32 iSockfd, VOID *pBuffer, UINT32 uBufferLen)
{
    INT32 iRet = -1;
    NETLINK_FD_DESC_T* pDesc = NULL;

    if (iSockfd <= 0 || NULL == pBuffer || 0 == uBufferLen) {
        PR_ERR("Invalid params socket fd(%u) buffer(%p) len(%u)\n", iSockfd, pBuffer, uBufferLen);
        return -EINVAL;
    }

    pDesc = netlink_fd2desc(iSockfd);
    if (NULL == pDesc) {
        PR_ERR("socket fd(%d) is abnormal\n", iSockfd);
        return -EINVAL;
    }

    switch (pDesc->uProtocol) {

        case SYS_NETLINK_GENERIC:
            iRet = sys_genlnetlink_recv_msg(iSockfd, pBuffer, uBufferLen);
            if (iRet < 0) {
                PR_ERR("sys_genlnetlink_recv_msg failed\n");
            }
            break;

        case SYS_NETLINK_UEVENT:
            iRet = sys_uevent_netlink_recv(iSockfd, pBuffer, uBufferLen);
            if (iRet < 0) {
                PR_ERR("sys_uevent_netlink_recv failed\n");
            }
            break;

        default:
            PR_ERR("socket fd(%d) protocol(%u) not support\n", iSockfd, pDesc->uProtocol);
            break;
    }

    return iRet;
}

INT32 sys_netlink_send(INT32 iSockfd, VOID *pBuffer, UINT32 uBufferLen)
{
    INT32 iRet = -1;
    NETLINK_FD_DESC_T* pDesc = NULL;

    if (iSockfd <= 0 || NULL == pBuffer || 0 == uBufferLen) {
        PR_ERR("Invalid params socket fd(%u) buffer(%p) len(%u)\n", iSockfd, pBuffer, uBufferLen);
        return -EINVAL;
    }

    pDesc = netlink_fd2desc(iSockfd);
    if (NULL == pDesc) {
        PR_ERR("socket fd(%d) is abnormal\n", iSockfd);
        return -EINVAL;
    }

    switch (pDesc->uProtocol) {

        case SYS_NETLINK_GENERIC:
            iRet = sys_genlnetlink_send_msg(iSockfd, pBuffer, uBufferLen);
            if (iRet < 0) {
                PR_ERR("sys_genlnetlink_send_msg failed\n");
            }
            break;

        default:
            PR_ERR("socket fd(%d) protocol(%u) not support send\n", iSockfd, pDesc->uProtocol);
            break;
    }

    return 0;
}

INT32 sys_netlink_close(INT32 iSockfd)
{
    NETLINK_FD_DESC_T* pDesc = NULL;

    pDesc = netlink_fd2desc(iSockfd);
    if (NULL == pDesc) {
        PR_ERR("socket fd(%d) is abnormal\n", iSockfd);
        return -EINVAL;
    }

    if (close(iSockfd) < 0) {
        PR_ERR("clsoe socket fd failed errno[%d:%s]\n", errno, strerror(errno));
        return -1;
    }

    sys_mutex_lock(&mtxsNetlinkList, WAIT_FOREVER);
    list_del(&pDesc->stlsNode);
    sys_mutex_unlock(&mtxsNetlinkList);

    free(pDesc);

    return 0;
}
