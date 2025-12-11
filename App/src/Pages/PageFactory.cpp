#include "Factory.h"
#include "StartUp/StartUp.h"
#include "Template/Template.h"
#include "VideoStream/VideoStream.h"

#define PAGE_CLASS_MATCH(className) \
do{ \
    if (strcmp(name, #className) == 0) \
    { \
        return new Page::className; \
    } \
}while(0)

PageBase* Factory::CreatePage(const char* name)
{
	PAGE_CLASS_MATCH(Startup);
	PAGE_CLASS_MATCH(VideoStream);
	PAGE_CLASS_MATCH(Template);
    return nullptr;
}
