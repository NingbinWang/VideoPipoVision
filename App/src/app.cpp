#include "app.h"
#include "Media_vi.h"
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

using namespace cv;
using namespace std;

int opencv_demo()
{
    VideoCapture cap(0);
    if (!cap.isOpened())
    {
        cout << "Failed to open camera!" << endl;
        return -1;
    }
    while (true)
    {

        Mat frame;
        cap.read(frame);
        if (frame.empty())
        {
            cout << "Failed to read frame from camera!" << endl;
            break;
        }
         cout << "read frame from camera!" << endl;
    }
    cap.release();
    return 0;
}
int media_demo()
{
    VI_CFG_PARAM_T param;
    param.vSensorType = CMOS_OV_5969;
    param.image_viH = 1944;
    param.image_viW = 2592;
    param.frame_rate = 30;
    Media_vi *vi = new Media_vi(param);
    vi->init();
    return 0;
}


int v4l2rtsp()
{
     //Logger::setLogFile("xxx.log");
    Logger::setLogLevel(Logger::LogWarning);

    EventScheduler* scheduler = EventScheduler::createNew(EventScheduler::POLLER_SELECT);
    ThreadPool* threadPool = ThreadPool::createNew(2);
    UsageEnvironment* env = UsageEnvironment::createNew(scheduler, threadPool);

    Ipv4Address ipAddr("0.0.0.0", 8554);
    RtspServer* server = RtspServer::createNew(env, ipAddr);
    MediaSession* session = MediaSession::createNew("live");
    MediaSource* mediaSource = H264FileMediaSource::createNew(env, "test.h264");
    RtpSink* rtpSink = H264RtpSink::createNew(env, mediaSource);

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
  //  opencv_demo();
  media_demo();
    return 0;
}