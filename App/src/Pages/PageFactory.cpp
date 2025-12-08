#include "Factory.h"
#define PAGE_CLASS_MATCH(className) \
do{ \
    if (strcmp(name, #className) == 0) \
    { \
        return new Page::className; \
    } \
}while(0)

PageBase* Factory::CreatePage(const char* name)
{
    return nullptr;
}
