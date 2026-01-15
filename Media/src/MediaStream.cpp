#include "MediaStream.h"
#include "New.h"
#include "SysMemory.h"
#include "Thread.h"
#include "SysTime.h"
#include "MediaStream.h"

MediaStream* MediaStream::createNew(MEDIA_PARAM_T* pParam)
{
    return New<MediaStream>::allocate(pParam);
}


MediaStream::MediaStream(MEDIA_PARAM_T *pParam)
{
	mpParam = pParam;
	mpSendStreamToRawPoolMutex = Mutex::createNew();
	mpSendStreamToRecPoolMutex = Mutex::createNew();
}

MediaStream::~MediaStream() 
{
	
}

INT32  MediaStream::SendStreamToRawPool(UINT32 uChan,PUINT8 pStreamSrc,UINT32 uLength)
{
	UINT32 u32W = 0;
	UINT32 u32R = 0;
	UINT32 u32SpareLen = 0;
	UINT32 u32Part1 = 0;
	UINT32 u32Part2;
	UINT32 u32TotalLen;
	PUINT8 ptr = NULL;
    
	if(pStreamSrc == NULL)
	{
		LOG_ERROR("pStreamSrc is NULL\n");
		return ERROR;
	}
	uLength = (uLength + 3) & ~3; // 将 uLength 向上舍入到最小的、不小于它的 4 的倍数，常用于4 字节对齐
	MutexLockGuard mutexLockGuard(mpSendStreamToRawPoolMutex);
	u32W = mpParam->aRawStreamPool[uChan].wIdx;	//读写指针获得锁之后再赋值，避免不同线程的读写指针可能相同
	u32R = mpParam->aRawStreamPool[uChan].rIdx;
	u32TotalLen = mpParam->aRawStreamPool[uChan].u32len;
	ptr = (PUINT8)mpParam->aRawStreamPool[uChan].addr[0];
	u32SpareLen = (u32R + u32TotalLen - u32W - 1) % u32TotalLen;	
	if(uLength > u32SpareLen)
	{
		LOG_ERROR("Lost one frame to raw pool,len=%d,spareLen=%d\n",uLength,u32SpareLen);
		return OK;
	}
	u32Part1 = u32TotalLen - u32W;
	if(uLength > u32Part1)
	{
		u32Part2 = uLength - u32Part1;
		SysMemory_copy((void *)(ptr + u32W),(void *)pStreamSrc, u32Part1);
		SysMemory_copy((void *)ptr,(void *)(pStreamSrc+u32Part1), u32Part2);
	}
	else
	{
		SysMemory_copy((void *)(ptr+u32W),(void *)pStreamSrc,uLength);
	}	
	mpParam->aRawStreamPool[uChan].wIdx = (mpParam->aRawStreamPool[uChan].wIdx + uLength) % (UINT)u32TotalLen;
	return OK;
}



INT32  MediaStream::SendStreamToRecPool(UINT32 uChan,PUINT8 pStreamSrc,UINT32 uLength, BOOL bVideo, BOOL dropMode)
{
	UINT32 u32W = 0;
	UINT32 u32R = 0;
	UINT32 u32SpareLen = 0;
	UINT32 u32Part1 = 0;
	UINT32 u32Part2;
	UINT32 u32TotalLen;
	PUINT8 ptr;
    REC_POOL_INFO_T *pRecPool;
    SYS_DATE_TIME_T  stNowTime = {0};
    pRecPool=&mpParam->astRecPool[uChan];
    
	if((NULL == pStreamSrc) || (0 == uLength) || (0 == pRecPool->totalLen))
    {
        LOG_ERROR("pStreamSrc= %p len=0x%x  pRecPool->totalLen=0x%x!\n",pStreamSrc,uLength,pRecPool->totalLen);
        return ERROR;
    }
    
    if((void*)pRecPool->addr[0] == NULL)
    {
        LOG_ERROR("pAddr is NULL \n");
        return ERROR;
    }
    
	uLength = (uLength + 3) & ~3;//4字节对齐
    MutexLockGuard mutexLockGuard(mpSendStreamToRecPoolMutex);
    if (pRecPool->totalLen)
    {
        u32W = pRecPool->wIdx;
        if(!dropMode)
        {
            u32R = pRecPool->rIdx; //判断读指针，不循环覆盖
        }
        else
        {
            u32R = pRecPool->wIdx; //不判断读指针，循环覆盖
        }
        u32TotalLen = pRecPool->totalLen;
        ptr = (PUINT8)pRecPool->addr[0];
        u32SpareLen = (u32R + u32TotalLen - u32W - 1) % u32TotalLen;

        //统计上传到录像缓冲区次数
        if(bVideo)
        {
            pRecPool->vFrmCounter++;
        }
        
        if(uLength > u32SpareLen)
        {
             //mpParam->encStatus[chan].RecPoolFrmLost++;
			 
			 LOG_WARNING("chan[%d] RecPool is overflow w=%d r=%d len=%d totalLen=%d!\n",uChan,u32W,u32R,uLength,u32TotalLen);
             return ERROR;
        }
        
        u32Part1 = u32TotalLen - u32W;

        if(uLength > u32Part1)
        {
            memcpy((PUINT8)(ptr + u32W),(PUINT8)pStreamSrc,  u32Part1);
            u32Part2 = uLength - u32Part1;
            memcpy((PUINT8)ptr, (PUINT8)(pStreamSrc+u32Part1), u32Part2);
        }
        else
        {
            memcpy((PUINT8)(ptr+u32W),(PUINT8)pStreamSrc,  uLength);
        }
        SysTime_get_msec(&pRecPool->lastFrameStdTime);
        SysTime_get_in_struct(&stNowTime);
        memcpy((void *)&pRecPool->lastFrameAbsTime,(void *)&stNowTime,sizeof(DATE_TIME_T));
        pRecPool->wIdx = (pRecPool->wIdx + uLength) % u32TotalLen;
        LOG_DEBUG("[DSP] chan[%d]SendStreamToRecPool w=%d, r=%d len=%d\n",uChan,pRecPool->wIdx,pRecPool->rIdx,uLength);
    }
        
    return OK;
}










