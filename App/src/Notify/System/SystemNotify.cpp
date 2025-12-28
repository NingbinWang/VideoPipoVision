#include "SystemNotify.h"
#include "Systeminfo_Def.h"

void SystemNotify_Init(void){
   NotifyBroker* center = NotifyCenter::Broker();
   Notification* SYS_Storage = new Notification("Storage",center, 0);
   Notification* SYS_Power = new Notification("Power", center, 0);
   Notification* SYS_StatusBar = new Notification("StatusBar", center, 0);
   extern void SYS_Storage_Init(Notification* account);
   SYS_Storage_Init(SYS_Storage);
   extern void SYS_Power_Init(Notification* account);
   SYS_Power_Init(SYS_Power);
   extern void SYS_StatusBar_Init(Notification* account);
   SYS_StatusBar_Init(SYS_StatusBar);
}