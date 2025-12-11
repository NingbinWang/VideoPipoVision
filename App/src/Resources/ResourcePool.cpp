#include "ResourcePool.h"

/* Global resource manager */
ResourcePool Resource;

extern "C" {
#define IMPORT_FONT(name) \
do{\
    LV_FONT_DECLARE(font_##name)\
    Resource.Font_.AddResource(#name, (void*)&font_##name);\
}while(0)

#define IMPORT_IMG(name) \
do{\
    LV_IMG_DECLARE(img_src_##name)\
    Resource.Image_.AddResource(#name, (void*)&img_src_##name);\
}while (0)

    static void Resource_Init()
    {
         /* Import Fonts */
		IMPORT_FONT(customize_14_aligned);
        IMPORT_FONT(customize_16_aligned);
        IMPORT_FONT(customize_18_aligned);
        IMPORT_FONT(customize_20_aligned);
        IMPORT_FONT(customize_24_aligned);
		IMPORT_FONT(customize_26_aligned);

		IMPORT_IMG(logoargb);
    }

} /* extern "C" */

void ResourcePool::Init()
{
    lv_obj_remove_style_all(lv_scr_act());
	lv_display_set_color_format(lv_disp_get_default(),LV_COLOR_FORMAT_XRGB8888);
    //lv_disp_set_bg_color(lv_disp_get_default(), lv_color_black());
    Font_.SetDefault((void*)LV_FONT_DEFAULT);

    Resource_Init();
}
