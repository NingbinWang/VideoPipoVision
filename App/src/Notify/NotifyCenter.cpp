#include "NotifyCenter.h"

static NotifyBroker PagesNotifyCenter("PagesNotify");

NotifyBroker* NotifyCenter::Broker()
{
    return &PagesNotifyCenter;
}