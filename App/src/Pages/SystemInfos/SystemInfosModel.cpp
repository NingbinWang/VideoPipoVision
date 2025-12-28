#include "SystemInfosModel.h"
#include <stdio.h>
#include "HAL/HAL.h"

using namespace Page;

void SystemInfosModel::Init()
{
    account->Subscribe("Power");
    account->Subscribe("Storage");
}

void SystemInfosModel::Deinit()
{
    if (account)
    {
        delete account;
        account = nullptr;
    }
}



void SystemInfosModel::GetBatteryInfo(
    int* usage,
    float* voltage,
    char* state, uint32_t len
)
{
    HAL::Power_Info_t power;
    account->Pull("Power", &power, sizeof(power));
    *usage = power.usage;
    *voltage = power.voltage / 1000.0f;
    strncpy(state, power.isCharging ? "CHARGE" : "DISCHARGE", len);
}

void SystemInfosModel::GetStorageInfo(
    bool* detect,
    char* usage, uint32_t len
)
{
    SystemInfoDef::Storage_Basic_Info_t info;
    account->Pull("Storage", &info, sizeof(info));
    *detect = info.isDetect;
    snprintf(
        usage, len,
        "%0.1f GB",
        info.totalSizeMB / 1024.0f
    );
}

