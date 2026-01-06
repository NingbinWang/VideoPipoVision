#include "VideoStream/VideoStreamView.h"
#include "Logger.h"
#include "ResourcePool.h"

using namespace Page;



void VideoStreamView::Create(lv_obj_t* root)
{
	lv_obj_t* label = lv_label_create(root);
	lv_obj_set_style_text_font(label, Resource.GetFont("customize_24_aligned"), 0);
	lv_obj_set_style_text_color(label, lv_color_black(), 0);
	lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 30);
	lv_label_set_text(label, "LVGL");
	ui.labelTitle = label;

	label = lv_label_create(root);
	lv_obj_set_style_text_font(label,Resource.GetFont("customize_26_aligned"), 0);
	lv_obj_set_style_text_color(label, lv_color_black(), 0);
	lv_label_set_text(label, "LVGL");
	lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 50);
	ui.labelTick = label;


	lv_obj_t* img = lv_img_create(root);

	lv_img_set_src(img,Resource.GetImage("logoargb"));
	lv_obj_center(img);
	
	ui.canvas = img;
	ui.group = lv_group_create();
	lv_indev_set_group(lv_get_indev(LV_INDEV_TYPE_ENCODER), ui.group);

	lv_group_add_obj(ui.group, ui.canvas);
	lv_group_add_obj(ui.group, ui.labelTitle);
	lv_group_focus_obj(ui.canvas);



}
