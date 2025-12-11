#include "app.h"
#include "lvgl/lvgl.h"
#include "Factory.h"
#include "PageManager.h"
#include "Base/Thread.h"
#include "LvThread.h"
#include "ResourcePool.h"
#include "Logger.h"
#include "StatusBar/StatusBar.h"
#include "Logger.h"


void lv_linux_indev_init(void) {
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

lv_color32_t* plv_disp_buf = NULL;

static int LvLinuxPort(void)
{
   lv_init();
   print_lvgl_version();
   /*Linux display device init*/
   lv_linux_disp_init();
   return 0;
}




int LvMain(void)
{
	static Factory factory;
    static PageManager manager(&factory);
	LvLinuxPort(); 
    
    Resource.Init();
      /*----------------------- Pages Init -----------------------*/
    StatusBar::Init(lv_layer_top());
    manager.Install("Startup", "Pages/Startup");
	manager.Install("Template", "Pages/Template");
	manager.Install("VideoStream", "Pages/VideoStream");
    manager.SetGlobalLoadAnimType(PageManager::LOAD_ANIM_OVER_TOP, 500);
    manager.Push("Pages/Startup");
  
     /*Handle LVGL tasks*/
    while(1) {
        lv_timer_handler();
        usleep(5000);
    }



}
