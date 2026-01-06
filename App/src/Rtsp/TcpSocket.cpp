#include <unistd.h>

#include "TcpSocket.h"
#include "SysSocket.h"

TcpSocket::~TcpSocket()
{
    SysSocket_close(mSockfd);
}

int TcpSocket::creat_socket()
{
    mSockfd =  SysSocket_create(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    return mSockfd;
}

int TcpSocket::close()
{
    return SysSocket_close(mSockfd);
}

int TcpSocket::bind(Ipv4Address& addr)
{
    return SysSocket_bind(mSockfd,AF_INET,addr.getIp().c_str(), addr.getPort());
}

int TcpSocket::listen(int iMaxConNum)
{
    return  SysSocket_listen(mSockfd,iMaxConNum);
}

int TcpSocket::accept()
{
	CHAR strClientIP[64]= {0};
	UINT16 uClientPort = 0;
    return  SysSocket_accept(mSockfd,strClientIP,64,&uClientPort);
}

void TcpSocket::setReuseAddr(int on)
{
	INT32 optval = on ? 1 : 0;
    SysSocket_set_sock_opt(mSockfd,SYS_SOL_SOCKET,SYS_SO_REUSEADDR,(const VOID*)&optval,sizeof(optval));
}


int TcpSocket::send(void *pBuf,int iLen)
{
	return SysSocket_send(mSockfd,pBuf,iLen);
}

