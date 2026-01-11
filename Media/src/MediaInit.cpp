#include "MediaPriv.h"
#include "New.h"
#include "SysMemory.h"
MEDIA_PARAM_T* pMediaInitParam = NULL;
MEDIA_INNER_PARAM_T* pInnerParam = NULL;

VI_CFG_PARAM_T *Media_Get_ViParam(void)
{
	if(pMediaInitParam == NULL){
		printf("[%s:%d]Media_Get_ViParam error YOU MSUT init media param\n", __FUNCTION__, __LINE__);
		return NULL;
	}
    return pMediaInitParam->stViCfgParam;
}

MEDIA_INNER_PARAM_T* Media_Get_InnerParam(void)
{
	return pInnerParam;
}


#ifdef MEDIARKMPP
RKrga *rkrga = nullptr;
RKnpu * rknpu = nullptr;
RKrga * Media_GetRkrga(void)
{
	return rkrga;
}
RKnpu * Media_GetRknpu(void)
{
	return rknpu;
}
#endif

INT32 MediaInit(MEDIA_PARAM_T* pParam)
{
	
	UINT32 u32ViChan = 0;
	if(pParam == NULL)
	{
		return ERROR;
	}
	pMediaInitParam = pParam;
	pInnerParam = (MEDIA_INNER_PARAM_T*)SysMemory_malloc(sizeof(MEDIA_INNER_PARAM_T));
	if(!pInnerParam){
		LOG_ERROR("Malloc MEDIA_INNER_PARAM_T fail \n");
	}
	for(u32ViChan = 0;u32ViChan < pParam->u32ViChanCnt;u32ViChan++){
		pInnerParam->pVi[u32ViChan] = MediaVi::createNew(pParam->astViCfgParam[u32ViChan]);
		pParam->astStreamShareBuf[u32ViChan].stHeader.u32ImgHeight = pParam->astViCfgParam[u32ViChan].u32Image_viH;
		pParam->astStreamShareBuf[u32ViChan].stHeader.u32ImgWidth = pParam->astViCfgParam[u32ViChan].u32Image_viW;
		pInnerParam->pStream[u32ViChan] = MediaStream::createNew(pParam->astStreamShareBuf[u32ViChan]);
	}



	

#ifdef MEDIARKMPP
	rkrga = new RKrga();
	rknpu = new RKnpu();	
#endif
    return 0;
}

