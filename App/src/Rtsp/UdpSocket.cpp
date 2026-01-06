#include <unistd.h>

#include "UdpSocket.h"
#include "SysSocket.h"

UdpSocket::~UdpSocket()
{
    SysSocket_close(mSockfd);
}

int UdpSocket::creat_socket()
{
    mSockfd =  SysSocket_create(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    return mSockfd;
}

int UdpSocket::sendto(void *pBuf,int iLen,Ipv4Address& addr)
{
	return SysSocket_send_to(mSockfd,pBuf,iLen, AF_INET, addr.getIp().c_str(),addr.getPort());
}

int UdpSocket::bind(Ipv4Address& addr)
{
    return SysSocket_bind(mSockfd,AF_INET,addr.getIp().c_str(), addr.getPort());
}


