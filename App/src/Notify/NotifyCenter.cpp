#include "NotifyCenter.h"

static NotifyBroker SystemCenter("SystemNotify");

NotifyBroker* NotifyCenter::Broker()
{
    return &SystemCenter;
}