#ifndef _TCPSOCKET_H_
#define _TCPSOCKET_H_
#include <string>
#include <stdint.h>

#include "InetAddress.h"

class  TcpSocket
{
public:
    explicit TcpSocket(int sockfd) :
        mSockfd(sockfd) { }

    ~TcpSocket();

    int fd() const { return mSockfd; }
    int creat_socket();
    int close();
    int bind(Ipv4Address& addr);
	int listen(int iMaxConNum);
	int send(void *pBuf,int iLen);
	void setReuseAddr(int on);
	int accept();
private:
    int mSockfd;
};

#endif //_SOCKET_H_