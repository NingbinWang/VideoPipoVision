#ifndef _PAGESNOTIFY_DEF_H_
#define _PAGESNOTIFY_DEF_H_
#include <stdint.h>

namespace PagesNotifyDef
{

// StatusBar 
typedef struct
{
    bool bShowLabelRec;
    const char* strLabelRec;
} STATUSBAR_INFO_T;

// Storage
typedef enum
{
    STORAGE_CMD_LOAD,
    STORAGE_CMD_SAVE,
    STORAGE_CMD_ADD,
    STORAGE_CMD_REMOVE
} STORAGE_CMD_E;

typedef enum
{
    STORAGE_TYPE_UNKNOW,
    STORAGE_TYPE_INT,
    STORAGE_TYPE_FLOAT,
    STORAGE_TYPE_DOUBLE,
    STORAGE_TYPE_STRING
} STORAGE_TYPE_E;

typedef struct
{
    STORAGE_CMD_E eCmd;
    const char* strKey;
    void* pValue;
    uint16_t size;
    STORAGE_TYPE_E type;
} STORAGE_INFO_T;



#define STORAGE_VALUE_REG(act, data, dataType)\
do{\
    PagesNotifyDef::STORAGE_INFO_T info; \
    info.eCmd = PagesNotifyDef::STORAGE_CMD_ADD; \
    info.strKey = #data; \
    info.pValue = &data; \
    info.size = sizeof(data); \
    info.type = dataType; \
    act->Notify("Storage", &info, sizeof(info)); \
}while(0)

typedef struct
{
    bool  bIsDetect;
    float fTotalSizeMB;
    float fFreeSizeMB;
} STORAGE_BASIC_INFO_T;






}


#endif