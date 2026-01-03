#ifndef _NOTIFIYBROKER_H_
#define _NOTIFIYBROKER_H_
#include "Notification.h"

class NotifyBroker
{
public:
    /* The name of the data center will be used as the ID of the main account */
    const char* Name;

    /* Main account, will automatically follow all accounts */
    Notification AccountMaster;

public:
    NotifyBroker(const char* name);
    ~NotifyBroker();
    bool AddAccount(Notification* account);
    bool RemoveAccount(Notification* account);
    bool Remove(std::vector<Notification*>* vec, Notification* account);
    Notification* SearchAccount(const char*id);
    Notification* Find(std::vector<Notification*>* vec,const char* id);
    uint32_t GetAccountLen();

private:
    std::vector<Notification*> AccountPool;

};

#endif
