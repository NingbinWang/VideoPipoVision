#include "SystemNotify.h"
#include "Systeminfo_Def.h"
#include "Filters.h"

static void onTimer(Notification* account)
{
    static bool lastStatus = false;

    HAL::Power_Info_t power;
    HAL::Power_GetInfo(&power);
    if (power.isCharging != lastStatus)
    {
        //AccountSystem::MusicPlayer_Info_t info;
        //info.music = power.isCharging ? "BattChargeStart" : "BattChargeEnd";
       // account->Notify("MusicPlayer", &info, sizeof(info));

        lastStatus = power.isCharging;
    }
}

static int onEvent(Notification* account, Notification::EventParam_t* param)
{
    static Filter::Hysteresis<int16_t> battUsageHysFilter(2);
    static Filter::MedianQueue<int16_t, 10> battUsageMqFilter;

    if (param->event == Notification::EVENT_TIMER)
    {
        onTimer(account);
        return 0;
    }

    if (param->event != Notification::EVENT_SUB_PULL)
    {
        return Notification::ERROR_UNSUPPORTED_REQUEST;
    }

    if (param->size != sizeof(HAL::Power_Info_t))
    {
        return Notification::ERROR_SIZE_MISMATCH;
    }

    HAL::Power_Info_t powerInfo;
    HAL::Power_GetInfo(&powerInfo);

    int16_t usage = powerInfo.usage;
    usage = battUsageHysFilter.GetNext(usage);
    usage = battUsageMqFilter.GetNext(usage);
    powerInfo.usage = (uint8_t) usage;

    memcpy(param->data_p, &powerInfo, param->size);

    return 0;
}

void SYS_Power_Init(Notification* account){
    account->SetEventCallback(onEvent);
    account->SetTimerPeriod(500);
}