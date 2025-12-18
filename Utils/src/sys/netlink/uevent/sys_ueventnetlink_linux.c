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


INT32 sys_uevent_netlink_create(UINT32 uPid)
{
    INT32 iSockfd, iRetval;
    struct sockaddr_nl stSrcaddr;

    /* Create a socket */
    iSockfd = socket(PF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);
    if(iSockfd == -1) {
        PR_ERR("error getting socket:errno[%d:%s]\n", errno, strerror(errno));
        return -1;
    }

    /* To prepare binding */
    memset(&stSrcaddr, 0, sizeof(stSrcaddr));
    stSrcaddr.nl_family = AF_NETLINK;       /**< 地址族固定为AF_NETLINK */
    stSrcaddr.nl_pid = uPid;                /**< 本机pid，可以根据getpid()获取，但是同一个pid不能重复bind绑定，因此根据情况用户自己传入 */
    /* 绑定情况下nl_pid为0时则说明让内核自动分配一个唯一的pid，但是通常情况下由用户自己指定 */
    stSrcaddr.nl_groups = 1;                /**< 订阅组播组1(uevent时间固定组号) */

    /* Bind */
    iRetval = bind(iSockfd, (struct sockaddr *)&stSrcaddr, sizeof(stSrcaddr));
    if(iRetval < 0){
        PR_ERR("bind failed:errno[%d:%s]\n", errno, strerror(errno));
        close(iSockfd);
        return -1;
    }

    return iSockfd;
}

static UINT32 uevent_action_parse(CHAR *pBuf)
{
    if (0 == strcmp(pBuf, "add")) {
        return UEVENT_NETLINK_ACTION_ADD;
    } else if (0 == strcmp(pBuf, "remove")) {
        return UEVENT_NETLINK_ACTION_REMOVE;
    } else if (0 == strcmp(pBuf, "bind")) {
        return UEVENT_NETLINK_ACTION_BIND;
    } else if (0 == strcmp(pBuf, "unbind")) {
        return UEVENT_NETLINK_ACTION_UNBIND;
    } else {
        PR_WRN("unknow action type(%s)\n", pBuf);
        return UEVENT_NETLINK_ACTION_UNKNOW;
    }
}

static UINT32 uevent_subsystem_parse(CHAR *pBuf)
{
    if (0 == strcmp(pBuf, "mmc")) {
        return UEVENT_NETLINK_SUBSYSTEM_MMC;
    } else if (0 == strcmp(pBuf, "bdi")) {
        return UEVENT_NETLINK_SUBSYSTEM_BDI;
    } else if (0 == strcmp(pBuf, "block")) {
        return UEVENT_NETLINK_SUBSYSTEM_BLOCK;
    } else if (0 == strcmp(pBuf, "usb")) {
        return UEVENT_NETLINK_SUBSYSTEM_USB;
    } else if (0 == strcmp(pBuf, "net")) {
        return UEVENT_NETLINK_SUBSYSTEM_NET;
    } else if (0 == strcmp(pBuf, "queues")) {
        return UEVENT_NETLINK_SUBSYSTEM_QUEUES;
    } else {
        PR_WRN("unknow subsystem type(%s)\n", pBuf);
        return UEVENT_NETLINK_SUBSYSTEM_UNKNOW;
    }
}

static UINT32 uevent_mmc_type_parse(CHAR *pBuf)
{
    if (0 == strcmp(pBuf, "SD")) {
        return UEVENT_MMC_TYPE_SD;
    } else {
        PR_WRN("unknow mmc type(%s)\n", pBuf);
        return UEVENT_MMC_TYPE_UNKNOW;
    }
}

static VOID uevent_mmc_private_data_parse(SYS_UEVENT_NETLINK_MMC_T *pMmc, CHAR *pBuf)
{
    if (0 == strncmp(pBuf, "MMC_TYPE", 8)) {
        pMmc->uMmcType = uevent_mmc_type_parse(pBuf + 9);
    } else if (0 == strncmp(pBuf, "MMC_NAME", 8)) {
        strncpy(pMmc->strMmcName, pBuf + 9, sizeof(pMmc->strMmcName) - 1);
        pMmc->strMmcName[sizeof(pMmc->strMmcName) - 1] = '\0';
    } else if (0 == strncmp(pBuf, "MODALIAS", 8)) {
        strncpy(pMmc->strModAlias, pBuf + 9, sizeof(pMmc->strModAlias) - 1);
        pMmc->strModAlias[sizeof(pMmc->strModAlias) - 1] = '\0';
    } else if (0 == strncmp(pBuf, "DRIVER", 6)) {
        strncpy(pMmc->strDriver, pBuf + 7, sizeof(pMmc->strDriver) - 1);
        pMmc->strDriver[sizeof(pMmc->strDriver) - 1] = '\0';
    }
}

static UINT32 uevent_block_devtype_parse(CHAR *pBuf)
{
    if (0 == strcmp(pBuf, "disk")) {
        return UEVENT_BLOCK_DEVTYPE_DISK;
    } else if (0 == strcmp(pBuf, "partition")) {
        return UEVENT_BLOCK_DEVTYPE_PARTITION;
    } else {
        PR_WRN("unknow block devtype(%s)\n", pBuf);
        return UEVENT_BLOCK_DEVTYPE_UNKNOW;
    }
}

static VOID uevent_block_private_data_parse(SYS_UEVENT_NETLINK_BLOCK_T *pBlock, CHAR *pBuf)
{
    if (0 == strncmp(pBuf, "MAJOR", 5)) {
        pBlock->uMajor = atoi(pBuf + 6);
    } else if (0 == strncmp(pBuf, "MINOR", 5)) {
        pBlock->uMinor = atoi(pBuf + 6);
    } else if (0 == strncmp(pBuf, "DEVNAME", 7)) {
        strncpy(pBlock->strDevName, pBuf + 8, sizeof(pBlock->strDevName) - 1);
        pBlock->strDevName[sizeof(pBlock->strDevName) - 1] = '\0';
    } else if (0 == strncmp(pBuf, "DEVTYPE", 7)) {
        pBlock->uDevType = uevent_block_devtype_parse(pBuf + 8);
    } else if (0 == strncmp(pBuf, "PARTN", 5)) {
        pBlock->uPartn = atoi(pBuf + 6);
    }
}

static UINT32 uevent_usb_devtype_parse(CHAR *pBuf)
{
    if (0 == strcmp(pBuf, "usb_device")) {
        return UEVENT_USB_DEVTYPE_USBDEVICE;
    } else if (0 == strcmp(pBuf, "usb_interface")) {
        return UEVENT_USB_DEVTYPE_USBINTERFACE;
    } else {
        PR_WRN("unknow usb devtype(%s)\n", pBuf);
        return UEVENT_USB_DEVTYPE_UNKNOW;
    }
}

static VOID uevent_usb_private_data_parse(SYS_UEVENT_NETLINK_USB_T *pUsb, CHAR *pBuf)
{
    if (0 == strncmp(pBuf, "MAJOR", 5)) {
        pUsb->uMajor = atoi(pBuf + 6);
    } else if (0 == strncmp(pBuf, "MINOR", 5)) {
        pUsb->uMinor = atoi(pBuf + 6);
    } else if (0 == strncmp(pBuf, "DEVNAME", 7)) {
        strncpy(pUsb->strDevName, pBuf + 8, sizeof(pUsb->strDevName) - 1);
        pUsb->strDevName[sizeof(pUsb->strDevName) - 1] = '\0';
    } else if (0 == strncmp(pBuf, "DEVTYPE", 7)) {
        pUsb->uDevType = uevent_usb_devtype_parse(pBuf + 8);
    } else if (0 == strncmp(pBuf, "PRODUCT", 7)) {
        strncpy(pUsb->strProduct, pBuf + 8, sizeof(pUsb->strProduct) - 1);
        pUsb->strProduct[sizeof(pUsb->strProduct) - 1] = '\0';
    } else if (0 == strncmp(pBuf, "TYPE", 4)) {
        strncpy(pUsb->strType, pBuf + 5, sizeof(pUsb->strType) - 1);
        pUsb->strType[sizeof(pUsb->strType) - 1] = '\0';
    } else if (0 == strncmp(pBuf, "BUSNUM", 6)) {
        pUsb->uBusNum = atoi(pBuf + 7);
    } else if (0 == strncmp(pBuf, "DEVNUM", 6)) {
        pUsb->uDevNum = atoi(pBuf + 7);
    } else if (0 == strncmp(pBuf, "INTERFACE", 9)) {
        strncpy(pUsb->strInterface, pBuf + 10, sizeof(pUsb->strInterface) - 1);
        pUsb->strInterface[sizeof(pUsb->strInterface) - 1] = '\0';
    } else if (0 == strncmp(pBuf, "MODALIAS", 8)) {
        strncpy(pUsb->strModAlias, pBuf + 9, sizeof(pUsb->strModAlias) - 1);
        pUsb->strModAlias[sizeof(pUsb->strModAlias) - 1] = '\0';
    } else if (0 == strncmp(pBuf, "DRIVER", 6)) {
        strncpy(pUsb->strDriver, pBuf + 7, sizeof(pUsb->strDriver) - 1);
        pUsb->strDriver[sizeof(pUsb->strDriver) - 1] = '\0';
    }
}

static VOID uevent_net_private_data_parse(SYS_UEVENT_NETLINK_NET_T *pNet, CHAR *pBuf)
{
    if (0 == strncmp(pBuf, "IFINDEX", 7)) {
        pNet->uIfIndex = atoi(pBuf + 8);
    } else if (0 == strncmp(pBuf, "INTERFACE", 5)) {
        strncpy(pNet->strInterface, pBuf, sizeof(pNet->strInterface) - 1);
        pNet->strInterface[sizeof(pNet->strInterface) - 1] = '\0';
    }
}

static VOID uevent_msg_handle(UINT32 uMsgCnt, SYS_UEVENT_NETLINK_RECV_T *pRecv,CHAR *pBuf, UINT32 uBufLen)
{
    switch (uMsgCnt) {
        case 0:
            return ;
        case 1:
            if (0 == strncmp("ACTION", pBuf, 6)) {
                pRecv->uAction = uevent_action_parse(pBuf + 7);
            } else {
                PR_ERR("msg action error: %s\n", pBuf);
            }
            return ;
        case 2:
            if (0 == strncmp("DEVPATH", pBuf, 7)) {
                strncpy(pRecv->strDevPath, pBuf + 8, sizeof(pRecv->strDevPath) - 1);
                pRecv->strDevPath[sizeof(pRecv->strDevPath) - 1] = '\0';
            } else {
                PR_ERR("msg devpath error: %s\n", pBuf);
            }
            return ;
        case 3:
            if (0 == strncmp("SUBSYSTEM", pBuf, 9)) {
                pRecv->uSubSystem = uevent_subsystem_parse(pBuf + 10);
            } else {
                PR_ERR("msg subsystem error: %s\n", pBuf);
            }
            return ;
        default:
            break;
    }

    switch (pRecv->uSubSystem) {
        case UEVENT_NETLINK_SUBSYSTEM_MMC:
            if (uBufLen < sizeof(SYS_UEVENT_NETLINK_MMC_T)) {
                PR_WRN("buffer len is too small(%lu) - mmc subsystem private data need(%lu)\n",
                    uBufLen + sizeof(SYS_UEVENT_NETLINK_RECV_T), sizeof(SYS_UEVENT_NETLINK_RECV_T) + sizeof(SYS_UEVENT_NETLINK_MMC_T));
                break;
            }
            uevent_mmc_private_data_parse((VOID *)pRecv->aPrivateData, pBuf);
            break;
        case UEVENT_NETLINK_SUBSYSTEM_BLOCK:
            if (uBufLen < sizeof(SYS_UEVENT_NETLINK_BLOCK_T)) {
                PR_WRN("buffer len is too small(%lu) - block subsystem private data need(%lu)\n",
                    uBufLen + sizeof(SYS_UEVENT_NETLINK_RECV_T), sizeof(SYS_UEVENT_NETLINK_RECV_T) + sizeof(SYS_UEVENT_NETLINK_BLOCK_T));
                break;
            }
            uevent_block_private_data_parse((VOID *)pRecv->aPrivateData, pBuf);
            break;
        case UEVENT_NETLINK_SUBSYSTEM_USB:
            if (uBufLen < sizeof(SYS_UEVENT_NETLINK_USB_T)) {
                PR_WRN("buffer len is too small(%lu) - usb subsystem private data need(%lu)\n",
                    uBufLen + sizeof(SYS_UEVENT_NETLINK_RECV_T), sizeof(SYS_UEVENT_NETLINK_RECV_T) + sizeof(SYS_UEVENT_NETLINK_USB_T));
                break;
            }
            uevent_usb_private_data_parse((VOID *)pRecv->aPrivateData, pBuf);
            break;
        case UEVENT_NETLINK_SUBSYSTEM_NET:
            if (uBufLen < sizeof(SYS_UEVENT_NETLINK_NET_T)) {
                PR_WRN("buffer len is too small(%lu) - net subsystem private data need(%lu)\n",
                    uBufLen + sizeof(SYS_UEVENT_NETLINK_RECV_T), sizeof(SYS_UEVENT_NETLINK_RECV_T) + sizeof(SYS_UEVENT_NETLINK_NET_T));
                break;
            }
            uevent_net_private_data_parse((VOID *)pRecv->aPrivateData, pBuf);
            break;
        default:
            break;
    }
}

INT32 sys_uevent_netlink_recv(INT32 iSockfd, VOID *pBuffer, UINT32 uBufferLen)
{
    INT32 iReplen, i = 0, j;
    CHAR aRecvBuf[1024] = {0};

    if (iSockfd <= 0 || NULL == pBuffer || uBufferLen < sizeof(SYS_UEVENT_NETLINK_RECV_T)) {
        PR_ERR("invalid params iSockfd(%d) pBuffer(%p) uBufferLen(%u)\n", iSockfd, pBuffer, uBufferLen);
        return -EINVAL;
    }

    /* 数据接收 */
    iReplen = recv(iSockfd, aRecvBuf, sizeof(aRecvBuf), 0);
    if(iReplen < 0) {
        PR_ERR("recv failed errno - [%d:%s]\n", errno, strerror(errno));
        return -1;
    }
    aRecvBuf[1023] = '\0';  /**< 防止strlen计算数据长度时访问越界 */

    for (j = 0; i < iReplen && aRecvBuf[i]; j++) {
        PR_DBG("recv uevent msg: %s\n", aRecvBuf + i);
        uevent_msg_handle(j, pBuffer, (VOID *)(aRecvBuf + i), uBufferLen - sizeof(SYS_UEVENT_NETLINK_RECV_T));
        i += strlen(aRecvBuf + i) + 1;  /**< 跳过字符串 + '\0' */
    }

    return 0;
}
