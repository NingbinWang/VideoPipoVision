#include "app.h"
#include "Net/UsageEnvironment.h"
#include "Base/ThreadPool.h"
#include "Net/EventScheduler.h"
#include "Net/Event.h"
#include "Net/RtspServer.h"
#include "Net/MediaSession.h"
#include "Net/InetAddress.h"
#include "Net/H264FileMediaSource.h"
#include "Net/H264RtpSink.h"
#include "autoconf.h"
#ifdef MEDIARKMPP
#include "Media/MppVISource.h"
#else
#include "Media/MediaVISource.h"
#endif
#ifdef LVGL
#include "LvThread.h"
#endif



int v4l2rtsp()
{
    EventScheduler* scheduler = EventScheduler::createNew(EventScheduler::POLLER_SELECT);//创建调度器
    ThreadPool* threadPool = ThreadPool::createNew(2);//创建线程池
    UsageEnvironment* env = UsageEnvironment::createNew(scheduler, threadPool);//创建环境变量

    Ipv4Address ipAddr("192.168.31.146", 5001);
    RtspServer* server = RtspServer::createNew(env, ipAddr);
    MediaSession* session = MediaSession::createNew("live");
#ifdef MEDIARKMPP
     MediaSource* videoSource = MppVISource::createNew(env, V4L2DEVNAME);
#else
    MediaSource* videoSource = MediaVISource::createNew(env, V4L2DEVNAME);
#endif  
    RtpSink* rtpSink = H264RtpSink::createNew(env, videoSource);

    session->addRtpSink(MediaSession::TrackId0, rtpSink);
    //session->startMulticast(); //多播

    server->addMeidaSession(session);
    server->start();

    std::cout<<"Play the media using the URL \""<<server->getUrl(session)<<"\""<<std::endl;

    env->scheduler()->loop();

    return 0;
}



int app_main(void)
{
  
    //Logger::setLogFile("xxx.log");
    Logger::setLogLevel(Logger::LogInfo);
#ifdef MEDIARKAI
   unsigned char *model;
   int model_size = 0;
   model = read_file_data(MODEL_PATH,&model_size);
   MediaAi_Init(model,model_size,LABEL_NALE_TXT_PATH);
#endif
   Media_Init();
#ifdef LVGL
   LvMain();
#endif

   return 0;
}