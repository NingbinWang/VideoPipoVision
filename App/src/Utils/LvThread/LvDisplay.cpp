
#include "Common.h"
#include "lvgl/lvgl.h"
#include "Logger.h"


static const CHAR *getenv_default(const CHAR *name, const CHAR *dflt)
{
    return getenv(name) ? : dflt;
}

#if LV_USE_LINUX_FBDEV
void LvLinuxDispInit(void)
{
    const CHAR *device = getenv_default("LV_LINUX_FBDEV_DEVICE", "/dev/fb0");
    lv_display_t * disp = lv_linux_fbdev_create();
    lv_linux_fbdev_set_file(disp, device);
}
#elif LV_USE_LINUX_DRM
void LvLinuxDispInit(void)
{
    const CHAR *device = getenv_default("LV_LINUX_DRM_CARD", "/dev/dri/card0");
    lv_display_t * disp = lv_linux_drm_create();
    lv_linux_drm_set_file(disp, device, -1);
}
#else
#error Unsupported configuration
#endif