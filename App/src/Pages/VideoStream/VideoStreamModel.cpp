#include "VideoStreamModel.h"

using namespace Page;

uint32_t VideoStreamModel::GetData()
{
    return lv_tick_get();
}
