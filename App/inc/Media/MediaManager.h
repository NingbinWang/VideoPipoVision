#ifndef _MEDIAMANAGER_H_
#define _MEDIAMANAGER_H_
#include "Media.h"

int MediaManagerInit();
MEDIA_PARAM_T * MediaManagerGet();
void*  MediaManagerGetStream(void* pUserdata);

#endif