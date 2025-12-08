#include "PageManager.h"
#include "Logger.h"

 /**
   * @brief  Get page loading animation properties
   * @param  anim: Animation type
   * @param  attr: Pointer to attribute
   * @retval Whether the acquisition is successful
   */
bool PageManager::GetLoadAnimAttr(uint8_t anim, LoadAnimAttr_t* attr)
{
    lv_coord_t hor = LV_HOR_RES;
    lv_coord_t ver = LV_VER_RES;

    switch (anim)
    {
    case LOAD_ANIM_OVER_LEFT:
        attr->dragDir = ROOT_DRAG_DIR_HOR;

        attr->push.enter.start = hor;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = 0;

        attr->pop.enter.start = 0;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = hor;
        break;

    case LOAD_ANIM_OVER_RIGHT:
        attr->dragDir = ROOT_DRAG_DIR_HOR;

        attr->push.enter.start = -hor;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = 0;

        attr->pop.enter.start = 0;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = -hor;
        break;

    case LOAD_ANIM_OVER_TOP:
        attr->dragDir = ROOT_DRAG_DIR_VER;

        attr->push.enter.start = ver;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = 0;

        attr->pop.enter.start = 0;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = ver;
        break;

    case LOAD_ANIM_OVER_BOTTOM:
        attr->dragDir = ROOT_DRAG_DIR_VER;

        attr->push.enter.start = -ver;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = 0;

        attr->pop.enter.start = 0;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = -ver;
        break;

    case LOAD_ANIM_MOVE_LEFT:
        attr->dragDir = ROOT_DRAG_DIR_HOR;

        attr->push.enter.start = hor;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = -hor;

        attr->pop.enter.start = -hor;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = hor;
        break;

    case LOAD_ANIM_MOVE_RIGHT:
        attr->dragDir = ROOT_DRAG_DIR_HOR;

        attr->push.enter.start = -hor;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = hor;

        attr->pop.enter.start = hor;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = -hor;
        break;

    case LOAD_ANIM_MOVE_TOP:
        attr->dragDir = ROOT_DRAG_DIR_VER;

        attr->push.enter.start = ver;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = -ver;

        attr->pop.enter.start = -ver;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = ver;
        break;

    case LOAD_ANIM_MOVE_BOTTOM:
        attr->dragDir = ROOT_DRAG_DIR_VER;

        attr->push.enter.start = -ver;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = ver;

        attr->pop.enter.start = ver;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = -ver;
        break;

    case LOAD_ANIM_FADE_ON:
        attr->dragDir = ROOT_DRAG_DIR_NONE;

        attr->push.enter.start = LV_OPA_TRANSP;
        attr->push.enter.end = LV_OPA_COVER;
        attr->push.exit.start = LV_OPA_COVER;
        attr->push.exit.end = LV_OPA_COVER;

        attr->pop.enter.start = LV_OPA_COVER;
        attr->pop.enter.end = LV_OPA_COVER;
        attr->pop.exit.start = LV_OPA_COVER;
        attr->pop.exit.end = LV_OPA_TRANSP;
        break;

    case LOAD_ANIM_NONE:
        memset(attr, 0, sizeof(LoadAnimAttr_t));
        return true;

    default:
        LOG_ERROR("Load anim type error: %d", anim);
        return false;
    }

    /* Determine the setter and getter of the animation */
    if (attr->dragDir == ROOT_DRAG_DIR_HOR)
    {
        attr->setter = [](void* obj, int32_t v)
        {
            lv_obj_set_x((lv_obj_t*)obj, v);
        };
        attr->getter = [](void* obj)
        {
            return (int32_t)lv_obj_get_x((lv_obj_t*)obj);
        };
    }
    else if (attr->dragDir == ROOT_DRAG_DIR_VER)
    {
        attr->setter = [](void* obj, int32_t v)
        {
            lv_obj_set_y((lv_obj_t*)obj, v);
        };
        attr->getter = [](void* obj)
        {
            return (int32_t)lv_obj_get_y((lv_obj_t*)obj);
        };
    }
    else
    {
        attr->setter = [](void* obj, int32_t v)
        {
            lv_obj_set_style_bg_opa((lv_obj_t*)obj, (lv_opa_t)v, LV_PART_MAIN);
        };
        attr->getter = [](void* obj)
        {
            return (int32_t)lv_obj_get_style_bg_opa((lv_obj_t*)obj, LV_PART_MAIN);
        };
    }

    return true;
}
