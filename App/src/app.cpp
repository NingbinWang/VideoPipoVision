#include "app.h"
#include "Media.h"
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
//#include "opencv2/opencv.hpp"
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
#ifdef MEDIARKMPP
#include "media/MppVISource.h"
#else
#include "media/MediaVISource.h"
#endif

//#ifdef LVGL
#include "lvgl.h"
#include "demos/lv_demos.h"
//#endif
static const char *getenv_default(const char *name, const char *dflt)
{
    return getenv(name) ? : dflt;
}

#if LV_USE_LINUX_FBDEV
static void lv_linux_disp_init(void)
{
    const char *device = getenv_default("LV_LINUX_FBDEV_DEVICE", "/dev/fb0");
    lv_display_t * disp = lv_linux_fbdev_create();

    lv_linux_fbdev_set_file(disp, device);
}
#elif LV_USE_LINUX_DRM
static void lv_linux_disp_init(void)
{
    const char *device = getenv_default("LV_LINUX_DRM_CARD", "/dev/dri/card0");
    lv_display_t * disp = lv_linux_drm_create();

    lv_linux_drm_set_file(disp, device, -1);
}
#else
#error Unsupported configuration
#endif

/**
 * @brief Print LVGL version
 */
static void print_lvgl_version(void)
{
    fprintf(stdout, "%d.%d.%d-%s\n",
            LVGL_VERSION_MAJOR,
            LVGL_VERSION_MINOR,
            LVGL_VERSION_PATCH,
            LVGL_VERSION_INFO);
}

/*
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
    */
static unsigned char *load_data(FILE *fp, size_t ofst, size_t sz)
{
  unsigned char *data;
  int ret;

  data = NULL;

  if (NULL == fp)
  {
    return NULL;
  }

  ret = fseek(fp, ofst, SEEK_SET);
  if (ret != 0)
  {
    printf("blob seek failure.\n");
    return NULL;
  }

  data = (unsigned char *)malloc(sz);
  if (data == NULL)
  {
    printf("buffer malloc failure.\n");
    return NULL;
  }
  ret = fread(data, 1, sz, fp);
  return data;
}
unsigned char *read_file_data(const char *filename, int *model_size)
{
  FILE *fp;
  unsigned char *data;

  fp = fopen(filename, "rb");
  if (NULL == fp)
  {
    printf("Open file %s failed.\n", filename);
    return NULL;
  }

  fseek(fp, 0, SEEK_END);
  int size = ftell(fp);

  data = load_data(fp, 0, size);

  fclose(fp);

  *model_size = size;
  return data;
}

int v4l2rtsp()
{
     //Logger::setLogFile("xxx.log");
    Logger::setLogLevel(Logger::LogDebug);

    EventScheduler* scheduler = EventScheduler::createNew(EventScheduler::POLLER_SELECT);//创建调度器
    ThreadPool* threadPool = ThreadPool::createNew(2);//创建线程池
    UsageEnvironment* env = UsageEnvironment::createNew(scheduler, threadPool);//创建环境变量

    Ipv4Address ipAddr("192.168.31.146", 5001);
    RtspServer* server = RtspServer::createNew(env, ipAddr);
    MediaSession* session = MediaSession::createNew("live");
#ifdef MEDIARKMPP
     MediaSource* videoSource = MppVISource::createNew(env, "/dev/video0");
     //MediaSource* videoSource = MppVISource::createNew(env, "/dev/video11");
#else
    MediaSource* videoSource = MediaVISource::createNew(env, "/dev/video0");
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

#define LABEL_NALE_TXT_PATH "/home/cat/coco_80_labels_list.txt"
#define MODEL_PATH "/home/cat/yolov5s-640-640.rknn"

int app_main(void)
{
#ifdef MEDIARKAI
   unsigned char *model;
   int model_size = 0;
   model = read_file_data(MODEL_PATH,&model_size);
   MediaAi_Init(model,model_size,LABEL_NALE_TXT_PATH);
#endif
   v4l2rtsp();
   lv_init();
   print_lvgl_version();
   /*Linux display device init*/
   lv_linux_disp_init();
   
    /*Create a Demo*/
    lv_demo_widgets();
    lv_demo_widgets_start_slideshow();

     /*Handle LVGL tasks*/
    while(1) {
        lv_timer_handler();
        usleep(5000);
    }

   return 0;
}