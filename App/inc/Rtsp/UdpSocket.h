#ifndef _UDPSOCKET_H_
#define _UDPSOCKET_H_
#include <string>
#include <stdint.h>

#include "InetAddress.h"

class  UdpSocket
{
public:
    explicit UdpSocket(int sockfd) :
        mSockfd(sockfd) { }

    ~UdpSocket();
	int creat_socket();

    int fd() const { return mSockfd; }
    int sendto(void *pBuf,int iLen,Ipv4Address& addr);
   

private:
    int mSockfd;
};

#endif //_SOCKET_H_