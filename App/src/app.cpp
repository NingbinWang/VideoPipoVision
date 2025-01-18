#include "app.h"
#include "Media.h"
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include "opencv2/opencv.hpp"
#include <iostream>

#include "Logger.h"
#include "net/UsageEnvironment.h"
#include "base/ThreadPool.h"
#include "net/EventScheduler.h"
#include "net/Event.h"
#include "net/RtspServer.h"
#include "net/MediaSession.h"
#include "net/InetAddress.h"
#include "net/H264FileMediaSource.h"
#include "net/H264RtpSink.h"
#include "media/FFMpegVISource.h"



int opencv_demo()
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
        std::cout << "Failed to open camera!" << std::endl;
        return -1;
    }
    while (true)
    {

        cv::Mat frame;
        cap.read(frame);
        if (frame.empty())
        {
            std::cout << "Failed to read frame from camera!" << std::endl;
            break;
        }
         std::cout << "read frame from camera!" << std::endl;
    }
    cap.release();
    return 0;
}


int v4l2rtsp()
{
     //Logger::setLogFile("xxx.log");
    Logger::setLogLevel(Logger::LogDebug);

    EventScheduler* scheduler = EventScheduler::createNew(EventScheduler::POLLER_SELECT);//创建调度器
    ThreadPool* threadPool = ThreadPool::createNew(2);//创建线程池
    UsageEnvironment* env = UsageEnvironment::createNew(scheduler, threadPool);//创建环境变量

    Ipv4Address ipAddr("192.168.0.126", 8554);
    RtspServer* server = RtspServer::createNew(env, ipAddr);
    MediaSession* session = MediaSession::createNew("live");
    MediaSource* videoSource = FFMpegVISource::createNew(env, "/dev/video0");
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
   v4l2rtsp();
   return 0;
}