#include "app.h"
#include "Media.h"
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
//#include "opencv2/opencv.hpp"
#include <iostream>
#include "Logger.h"
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
#include "lvgl/lvgl.h"
#include "demos/lv_demos.h"

const lv_font_t * font_large;
const lv_font_t * font_normal;
disp_size_t disp_size;
lv_style_t style_title;
lv_style_t style_text_muted;
lv_style_t style_icon;
lv_style_t style_bullet;
lv_obj_t * tv;

void lv_widgets_components_init(void)
{
    if(LV_HOR_RES <= 320) disp_size = DISP_SMALL;
    else if(LV_HOR_RES < 720) disp_size = DISP_MEDIUM;
    else disp_size = DISP_LARGE;

    font_large = LV_FONT_DEFAULT;
    font_normal = LV_FONT_DEFAULT;
/*
    if(disp_size == DISP_LARGE) {
        #ifdef LV_FONT_MONTSERRAT_24
                font_large     = &lv_font_montserrat_24;
        #else
                LV_LOG_WARN("LV_FONT_MONTSERRAT_24 or LV_DEMO_BENCHMARK_ALIGNED_FONTS is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
        #endif
        #ifdef LV_FONT_MONTSERRAT_16
                font_normal    = &lv_font_montserrat_16;
        #else
                LV_LOG_WARN("LV_FONT_MONTSERRAT_16 or LV_DEMO_BENCHMARK_ALIGNED_FONTS is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
        #endif
    }
*/
    #if LV_USE_THEME_DEFAULT
    lv_theme_default_init(NULL, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK,font_normal);
    #endif
    lv_obj_set_style_text_font(lv_screen_active(), font_normal, 0);
    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, font_large);

    lv_style_init(&style_text_muted);
    lv_style_set_text_opa(&style_text_muted, LV_OPA_50);


    lv_style_init(&style_icon);
    lv_style_set_text_color(&style_icon, lv_theme_get_color_primary(NULL));
    lv_style_set_text_font(&style_icon, font_large);

    lv_style_init(&style_bullet);
    lv_style_set_border_width(&style_bullet, 0);
    lv_style_set_radius(&style_bullet, LV_RADIUS_CIRCLE);

}


void init_touchpad(void) {
    lv_indev_t* indev_drv;
	indev_drv = lv_indev_create();
	lv_indev_set_type(indev_drv,LV_INDEV_TYPE_POINTER);
	lv_indev_set_read_cb(indev_drv,NULL);
}



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
// 全局变量
static lv_img_dsc_t g_video_img;
static uint8_t *g_rgb_frame = NULL;  // XRGB8888 缓冲区
void lvgl_video_init(size_t frame_size)
{
    // 配置 LVGL 图像描述符
    g_video_img.data = g_rgb_frame;
    g_video_img.data_size = frame_size;
    g_video_img.header.w = 720;
    g_video_img.header.h = 1080;
    g_video_img.header.cf = LV_COLOR_FORMAT_XRGB8888; // LVGL 9.4 新格式

    // 创建图像控件（全屏）
    lv_obj_t *img = lv_img_create(lv_scr_act());
    lv_img_set_src(img, &g_video_img);
    lv_obj_set_size(img, 1080, 720);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
}
MediaVi *mVi = NULL;
void * vibuf = nullptr;
size_t mpp_frame_size = 3110400;
void VI_INIT(void)
{
    bool ret;
    VI_CFG_PARAM_T param;
    ENC_STATUS_T enc_status;
    std::string mDev = "/dev/video0";
    const char* in_devname = mDev.c_str();
    param.vSensorType = CMOS_OV_5969;
    param.image_viH = 1080;
    param.image_viW = 1920;
    param.frame_rate = 30;
    param.eType = VI_V4L2;
    //setFps(30);
//MediaVi
    mVi = new MediaVi(param);
    ret = mVi->initdev(in_devname);
    assert(ret == true);
    
}
void convertxrgb8888()
{
    char * framebuf = nullptr;
    size_t size = 0;
    bool ret;
    while(1)
    {
            int index = -1;
            ret = mVi->poll();
            if(ret == false)
                continue;

            mVi->readFramebuf((char *)vibuf,3110400);
            memcpy(g_rgb_frame,vibuf,720 * 1080 * 2);
            break;
    }
}
int app_main(void)
{
  
    //Logger::setLogFile("xxx.log");
    Logger::setLogLevel(Logger::LogDebug);
#ifdef MEDIARKAI
   unsigned char *model;
   int model_size = 0;
   model = read_file_data(MODEL_PATH,&model_size);
   MediaAi_Init(model,model_size,LABEL_NALE_TXT_PATH);
#endif
  // v4l2rtsp();
   VI_INIT();
   Media_Init();
   lv_init();
   print_lvgl_version();
   /*Linux display device init*/
   lv_linux_disp_init();
   lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x343247), 0);
    size_t frame_size = 720 * 1080 * 2;
    if (!g_rgb_frame) {
       g_rgb_frame =(uint8_t *) malloc(frame_size);
       memset(g_rgb_frame, 0, frame_size); // 初始黑屏
    }
    /*Create a Demo*/
    //lv_demo_widgets();
    //lv_demo_widgets_start_slideshow();
   // lv_demo_music();
    lv_widgets_components_init();
   
   // convertxrgb8888();
    //lvgl_video_init(frame_size);
     /*Handle LVGL tasks*/
    while(1) {
        lv_timer_handler();
        usleep(5000);
    }

   return 0;
}