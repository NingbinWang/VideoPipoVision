#ifndef _SYSTEMINFOS_MODEL_H_
#define _SYSTEMINFOS_MODEL_H_
#include <stdint.h>
#include "Framework/Notification/Notification.h"
#include "App/Notify/System/SystemNotify.h"
namespace Page
{

class SystemInfosModel
{
public:
    void Init();
    void Deinit();


    void GetBatteryInfo(
        int* usage,
        float* voltage,
        char* state, uint32_t len
    );

    void GetStorageInfo(
        bool* detect,
        char* size, uint32_t len
    );

    private:
        Notification* account;

};

}

#endif
