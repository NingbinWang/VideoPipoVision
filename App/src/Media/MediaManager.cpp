#include "Media.h"
#include "MediaManager.h"
#include "SysMemory.h"
MEDIA_PARAM_T *pManagerParm = NULL;



void MediaManagerInit()
{
	pManagerParm = (MEDIA_PARAM_T*)SysMemory_malloc(sizeof(MEDIA_PARAM_T));
	
	Media_Init(pManagerParm);
}

