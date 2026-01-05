#include <unistd.h>

#include "UdpSocket.h"
#include "SysSocket.h"

UdpSocket::~UdpSocket()
{
    SysSocket_close(mSockfd);
}

int UdpSocket::creat_socket()
{
    mSockfd =  SysSocket_create(SYS_AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_UDP);
    return mSockfd;
}

int UdpSocket::sendto(void *pBuf,int iLen,Ipv4Address& addr)
{
	return SysSocket_send_to(mSockfd,pBuf,iLen, AF_INET, addr.getIp().c_str(),addr.getPort());
}

