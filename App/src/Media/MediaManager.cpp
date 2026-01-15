
#include "MediaManager.h"
#include "SysMemory.h"
#include "autoconf.h"
#include "Logger.h"
#include <pthread.h>
#include "SysTime.h"


MEDIA_PARAM_T *pManagerParm = NULL;


int MediaManagerInit()
{
	UINT32 shareTotalSize = 0;
	UINT32 uIndex = 0;
	pManagerParm = (MEDIA_PARAM_T*)SysMemory_malloc(sizeof(MEDIA_PARAM_T));
	//VI
	pManagerParm->u32ViChanCnt = 1;
	pManagerParm->astViCfgParam[0].u32Enable = TRUE;
	pManagerParm->astViCfgParam[0].u32Chn = 0;
	pManagerParm->astViCfgParam[0].eViType = VI_V4L2;
	pManagerParm->astViCfgParam[0].u32Image_viW = 1920;
	pManagerParm->astViCfgParam[0].u32Image_viH = 1080;
	pManagerParm->astViCfgParam[0].u32Frame_rate = 30;
	pManagerParm->astViCfgParam[0].eSensorType = CMOS_OV_5969;
	pManagerParm->astViCfgParam[0].u32Frame_rate = 30;

	strncpy(pManagerParm->astViCfgParam[0].strDevname,MAINDEVNAME,VI_DEVNAME_STR_LEN-1);
	pManagerParm->astViCfgParam[0].strDevname[VI_DEVNAME_STR_LEN-1]='/0';
    if(pManagerParm->astViCfgParam[0].eViType == VI_V4L2)
    {
    	if(strlen(MAINFORMAT) != 4){
			LOG_ERROR("set in formate %s is not right length!\n",MAINFORMAT);
    	}
    }
	strncpy(pManagerParm->astViCfgParam[0].strFormat,MAINFORMAT,sizeof(pManagerParm->astViCfgParam[0].strFormat)-1);
	pManagerParm->astViCfgParam[0].strFormat[sizeof(pManagerParm->astViCfgParam[0].strFormat)-1]='/0';
	for(uIndex = 0;uIndex < pManagerParm->u32ViChanCnt;uIndex++){
			pManagerParm->astViCfgParam[uIndex].eViewMirror = VIEW_NATURAL;
			pManagerParm->astViCfgParam[uIndex].stDayNightInfo.u32DayNightMode = 0;
			pManagerParm->astViCfgParam[uIndex].u8EnWdr = 1;
			pManagerParm->astViCfgParam[uIndex].u8WdrLevel = 50;
				//裸的数据流Pool池
			pManagerParm->aRawStreamPool[uIndex].stVideoHeader.u32ImageHeight = pManagerParm->astViCfgParam[uIndex].u32Image_viH;
			pManagerParm->aRawStreamPool[uIndex].stVideoHeader.u32ImageWidth = pManagerParm->astViCfgParam[uIndex].u32Image_viW;
			pManagerParm->aRawStreamPool[uIndex].rIdx = 0;
			pManagerParm->aRawStreamPool[uIndex].wIdx = 0;
			pManagerParm->aRawStreamPool[uIndex].u32len = 15*1024*1024;
			pManagerParm->aRawStreamPool[uIndex].addr[0] = SysMemory_malloc(pManagerParm->aRawStreamPool[uIndex].u32len);
	}
	//enc
	pManagerParm->u32EncChanCnt = 1;
	pManagerParm->astEncCfgParam[0].uEncW = 1920;
	pManagerParm->astEncCfgParam[0].uEncH = 1080;
	pManagerParm->astEncCfgParam[0].eEncoderType = MEDIA_ENC_CODETYPE_AVC;
	//encoder status
	pManagerParm->stEncoderStatus.stEncBasicParam.bEnable = true;
	pManagerParm->stEncoderStatus.stEncBasicParam.encH = 1080;
	pManagerParm->stEncoderStatus.stEncBasicParam.encW = 1920;
	//音频
	pManagerParm->stAudioStreamPool.u32len = 256*1024;
	pManagerParm->stAudioStreamPool.rIdx = 0;
	pManagerParm->stAudioStreamPool.wIdx = 0;
	//音频的内存池
	pManagerParm->stAudioStreamPool.addr[0] = SysMemory_malloc(pManagerParm->stAudioStreamPool.u32len);
    //RecPool池
	pManagerParm->astRecPool[0].totalLen = 12*1024*1024;
	pManagerParm->astRecPool[1].totalLen = 4*1024*1024;//不使用
	pManagerParm->astRecPool[2].totalLen = 4*1024*1024;//不使用
	pManagerParm->astRecPool[0].addr[0] = SysMemory_malloc(pManagerParm->astRecPool[0].totalLen);
	MediaInit(pManagerParm);
	return 0;
}

MEDIA_PARAM_T * MediaManagerGet()
{
	return pManagerParm;
}

void  MediaManagerGetStream(UINT uChan,void* pUserdata)
{
   
    UINT uLen1 = 0;
	UINT uLen2 = 0;
	UINT uCurLen=0;
    UINT32 * pData = NULL;
    UINT uR = 0;
	UINT uW = 0;
    REC_POOL_INFO_T *pPool = NULL;
    bool bHaveData = true;
	if(pManagerParm == NULL)
	{
		return;
	}
    while (1)
    {
        
        if (false == bHaveData)
        {
            SysTime_sleep_ms(10);
        }
        bHaveData = false;
        pPool=&(pManagerParm->astRecPool[uChan]);

        uR=pPool->rIdx;
        uW=pPool->wIdx;
            
        // get data length in share memory
        if (uW >= uR)
        {
                uLen1 = uW - uR;
                uLen2 = 0;
         }
         else
         {
                uLen1 = pPool->totalLen - uR;
                uLen2 = uW;
         }
         uCurLen = uLen1+uLen2;
         if(uCurLen < (UINT)pManagerParm->astRecPool[uChan].stVideoHeader.sSize)
         {
                continue;
         }

         if(uCurLen >= (UINT)pManagerParm->astRecPool[uChan].stVideoHeader.sSize)
         {          
                if((void*)pPool->addr[0] == NULL)
                {
                    LOG_ERROR("[chan%d] invalid addr[0] !\n",uChan);
                    continue;
                }                
                pData = (UINT32 *)((PUINT8)pPool->addr[0] + uR);
				//取流回调
        }
    }
}


