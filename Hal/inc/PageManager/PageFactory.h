#ifndef _PAGEFACTORY_H_
#define _PAGEFACTORY_H_
#include "PageBase.h"

class PageFactory
{
public:

    virtual PageBase* CreatePage(const char* name)
    {
        return nullptr;
    };
};

#endif

