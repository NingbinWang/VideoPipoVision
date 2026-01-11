#include "Media.h"
#include "MediaManager.h"
#include "SysMemory.h"
#include "autoconf.h"
MEDIA_PARAM_T *pManagerParm = NULL;


void MediaManagerInit()
{
	pManagerParm = (MEDIA_PARAM_T*)SysMemory_malloc(sizeof(MEDIA_PARAM_T));
	pManagerParm->u32ViChanCnt = 1;
	pManagerParm->astViCfgParam[0].u32Enable = TRUE;
	pManagerParm->astViCfgParam[0].eViType = VI_V4L2;
	pManagerParm->astViCfgParam[0].u32Image_viW = 1920;
	pManagerParm->astViCfgParam[0].u32Image_viH = 1080
	pManagerParm->astViCfgParam[0].u32Frame_rate = 30;
	pManagerParm->astViCfgParam[0].eSensorType = CMOS_OV_5969;
	pManagerParm->astViCfgParam[0].u32Frame_rate = 30;
	pManagerParm->astViCfgParam[0].eViewMirror = VIEW_NATURAL;
	pManagerParm->astViCfgParam[0].stDayNightInfo.u32DayNightMode = 0;
	pManagerParm->astViCfgParam[0].u8EnWdr = 1;
	pManagerParm->astViCfgParam[0].u8WdrLevel = 50;
	strncpy(pManagerParm->astViCfgParam[0].strDevname,MAINDEVNAME,VI_DEVNAME_STR_LEN-1);
	pManagerParm->astViCfgParam[0].strDevname[VI_DEVNAME_STR_LEN-1]='/0';



	
	Media_Init(pManagerParm);
}

