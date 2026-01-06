#include "app.h"
#include "Common.h"
#include "Media.h"
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <iostream>
#include "Logger.h"
#include "UsageEnvironment.h"
#include "ThreadPool.h"
#include "EventScheduler.h"
#include "Event.h"
#include "RtspServer.h"
#include "MediaSession.h"
#include "InetAddress.h"
#include "H264RtpSink.h"
#include "Common.h"
#include "autoconf.h"
#ifdef MEDIARKMPP
#include "Media/MppVISource.h"
#else
#include "Media/MediaVISource.h"
#endif
#ifdef USE_LVGL
#include "LvThread.h"
#endif
#ifdef USE_AI
#include "AiModel.h"
#endif

INT32 AppRtspServer(const char* strIp)
{
    EventScheduler* scheduler = EventScheduler::createNew(EventScheduler::POLLER_SELECT);//创建调度器
    ThreadPool* threadPool = ThreadPool::createNew(2);//创建线程池
    UsageEnvironment* env = UsageEnvironment::createNew(scheduler, threadPool);//创建环境变量

    Ipv4Address ipAddr(strIp, 5001);//创建IP
    RtspServer* server = RtspServer::createNew(env, ipAddr);//创建对应的RTSPServer
    MediaSession* session = MediaSession::createNew("live");//创建一个session
#ifdef MEDIARKMPP
    MediaSource* videoSource = MppVISource::createNew(env, V4L2DEVNAME);
#endif  
    RtpSink* rtpSink = H264RtpSink::createNew(env, videoSource);
	//MediaSource* audioSource = AlsaMediaSource::createNew(env);
    //RtpSink* audioRtpSink = AACRtpSink::createNew(env, audioSource);
    
    session->addRtpSink(MediaSession::TrackId0, rtpSink);
   // session->addRtpSink(MediaSession::TrackId1, audioRtpSink);
    server->addMeidaSession(session);
    server->start();
    std::cout<<"Play the media using the URL \""<<server->getUrl(session)<<"\""<<std::endl;
    env->scheduler()->loop();
    return 0;
}

int app_main(void)
{
  
   //Logger::setLogFile("xxx.log");
   Logger::setLogLevel(Logger::LogDebug);
   Media_Init();
#ifdef USE_AI
   AiModelInit();
#endif
#ifdef USE_LVGL
   //LvThreadInit();
#endif
	AppRtspServer("192.168.0.14");
   return 0;
}