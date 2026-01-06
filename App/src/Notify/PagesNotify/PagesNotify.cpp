#include "PagesNotify.h"
#include "PagesNotify_Def.h"

void PagesNotify_Init(void){
   NotifyBroker* center = NotifyCenter::Broker();
   Notification* SYS_Storage = new Notification("Storage",center, 0);
   Notification* SYS_StatusBar = new Notification("StatusBar", center, 0);
   extern void SYS_Storage_Init(Notification* account);
   SYS_Storage_Init(SYS_Storage);
   extern void SYS_StatusBar_Init(Notification* account);
   SYS_StatusBar_Init(SYS_StatusBar);
}