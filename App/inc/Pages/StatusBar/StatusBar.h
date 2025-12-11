#ifndef _STATUSBAR_H_
#define _STATUSBAR_H_

#include "Page.h"

struct
{
    lv_obj_t* cont;

    struct
    {
        lv_obj_t* img;
        lv_obj_t* label;
    } satellite;

    lv_obj_t* imgSD;
    lv_obj_t* imgBT;
    lv_obj_t* labelClock;
    lv_obj_t* labelRec;

    struct
    {
        lv_obj_t* img;
        lv_obj_t* objUsage;
        lv_obj_t* label;
    } battery;
} ui;

namespace StatusBar
{

typedef enum
{
    STYLE_TRANSP,
    STYLE_BLACK,
    STYLE_MAX
} Style_t;

void Init(lv_obj_t* par);
void SetStyle(Style_t style);
void Appear(bool en);

}


#endif

