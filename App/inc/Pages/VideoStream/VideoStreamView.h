#ifndef _VIDEOSTREAM_VIEW_H_
#define _VIDEOSTREAM_VIEW_H_

#include "Page.h"

namespace Page
{

class VideoStreamView
{
public:
    void Create(lv_obj_t* root);

public:
    struct
    {
        lv_obj_t* labelTitle;
        lv_obj_t* labelTick;
        lv_obj_t* canvas;
        lv_group_t* group;
    } ui;

private:

};

}

#endif
