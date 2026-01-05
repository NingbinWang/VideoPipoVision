#ifndef _RTPINSTANNCE_H_
#define _RTPINSTANNCE_H_
#include <string>
#include <stdint.h>
#include <unistd.h>

#include "InetAddress.h"
#include "SysSocket.h"

#include "Rtp.h"
#include "New.h"

class RtpInstance
{
public:
    enum RtpType
    {
        RTP_OVER_UDP,
        RTP_OVER_TCP
    };

    static RtpInstance* createNewOverUdp(int localSockfd, uint16_t localPort,
                                    std::string destIp, uint16_t destPort)
    {
        //return new RtpInstance(localSockfd, localPort, destIp, destPort);
        return New<RtpInstance>::allocate(localSockfd, localPort, destIp, destPort);
    }

    static RtpInstance* createNewOverTcp(int clientSockfd, uint8_t rtpChannel)
    {
        //return new RtpInstance(clientSockfd, rtpChannel);
        return New<RtpInstance>::allocate(clientSockfd, rtpChannel);
    }

    ~RtpInstance()
    { 
       SysSocket_close(mSockfd);
    }

    uint16_t getLocalPort() const { return mLocalPort; }
    uint16_t getPeerPort() { return mDestAddr.getPort(); }

    int send(RtpPacket* rtpPacket)
    {
        if(mRtpType == RTP_OVER_UDP)
        {
            return sendOverUdp(rtpPacket->mBuffer, rtpPacket->mSize);
        }
        else
        {
            uint8_t* rtpPktPtr = rtpPacket->_mBuffer;
            rtpPktPtr[0] = '$';
            rtpPktPtr[1] = (uint8_t)mRtpChannel;
            rtpPktPtr[2] = (uint8_t)(((rtpPacket->mSize)&0xFF00)>>8);
            rtpPktPtr[3] = (uint8_t)((rtpPacket->mSize)&0xFF);
            return sendOverTcp(rtpPktPtr, rtpPacket->mSize+4);
        }
    }

    bool alive() const { return mIsAlive; }
    int setAlive(bool alive) { mIsAlive = alive; };
    void setSessionId(uint16_t sessionId) { mSessionId = sessionId; }
    uint16_t sessionId() const { return mSessionId; }

private:
    int sendOverUdp(void* pBuf, int iLen)
    {
        return SysSocket_send_to(mSockfd,pBuf,iLen, AF_INET, mDestAddr.getIp().c_str(),mDestAddr.getPort());
    }

    int sendOverTcp(void* pBuf, int iLen)
    {
        return SysSocket_send(mSockfd,pBuf,iLen);
    }

public:
    RtpInstance(int localSockfd, uint16_t localPort, const std::string& destIp, uint16_t destPort) :
        mRtpType(RTP_OVER_UDP), mSockfd(localSockfd), mLocalPort(localPort),
        mDestAddr(destIp, destPort), mIsAlive(false), mSessionId(0)
    {
        
    }

    RtpInstance(int clientSockfd, uint8_t rtpChannel) :
        mRtpType(RTP_OVER_TCP), mSockfd(clientSockfd), 
        mIsAlive(false), mSessionId(0), mRtpChannel(rtpChannel)
    {
        
    }

private:
    RtpType mRtpType;
    int mSockfd;
    uint16_t mLocalPort; //for udp
    Ipv4Address mDestAddr; //for udp
    bool mIsAlive;
    uint16_t mSessionId;
    uint8_t mRtpChannel; //for tcp
};

class RtcpInstance
{
public:
    static RtcpInstance* createNew(int localSockfd, uint16_t localPort,
                                    std::string destIp, uint16_t destPort)
    {
        //return new RtcpInstance(localSockfd, localPort, destIp, destPort);
        return New<RtcpInstance>::allocate(localSockfd, localPort, destIp, destPort);
    }

    ~RtcpInstance()
    {
    	SysSocket_close(mLocalSockfd);
    }

    int send(void* buf, int size)
    {
        return SysSocket_send_to(mLocalSockfd, buf, size,  AF_INET, mDestAddr.getIp().c_str(),mDestAddr.getPort());
    }

    int recv(void* buf, int size, Ipv4Address* addr)
    {
        return 0;
    }

    uint16_t getLocalPort() const { return mLocalPort; }

    int alive() const { return mIsAlive; }
    int setAlive(bool alive) { mIsAlive = alive; };
    void setSessionId(uint16_t sessionId) { mSessionId = sessionId; }
    uint16_t sessionId() const { return mSessionId; }

public:
    RtcpInstance(int localSockfd, uint16_t localPort,
                    std::string destIp, uint16_t destPort) :
        mLocalSockfd(localSockfd), mLocalPort(localPort), mDestAddr(destIp, destPort),
        mIsAlive(false), mSessionId(0)
    {   }

private:
    int mLocalSockfd;
    uint16_t mLocalPort;
    Ipv4Address mDestAddr;
    bool mIsAlive;
    uint16_t mSessionId;
};

#endif //_RTPINSTANNCE_H_