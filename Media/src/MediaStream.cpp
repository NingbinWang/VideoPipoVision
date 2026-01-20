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

void MediaStream::MakeEsStreamElement(MEDIA_STREAM_ELEMENT*pStream,UINT uChan, UINT uLen, UINT uType,UINT uStandedTime, DATE_TIME_T stNow)
{
    pStream->u32Magic   = STREAM_ELEMENT_MAGIC;
    pStream->eType = uType;
    pStream->u32dataLen = uLen;
    pStream->stAbsTime = stNow;
    pStream->u32TimeStamp = uStandedTime;
    pStream->u8Id = 0;
    pStream->u8Chan = (UINT8)uChan;
}

void MediaStream::MakeEsStreamVideoElement(MEDIA_STREAM_ELEMENT*pStream,UINT uChan, UINT uLen,UINT uType,DATE_TIME_T stNow,MEDIA_PACK_VIDEO_CHN_ATTR_T *pParam)
{
    pStream->u32Magic   = STREAM_ELEMENT_MAGIC;
    pStream->eType = uType;
    pStream->u32dataLen = uLen;
    pStream->stAbsTime = stNow;
    pStream->streamType= pParam->encCfg.streamType;
    pStream->timeStamp = pParam->timeStamp;
    pStream->id = 0;
    pStream->chan = (UINT8)chan;

	pStream->videoInfo.videoStreamType = getVideoStreamType(pParam->encCfg.encType.videoType);
	pStream->videoInfo.width = pParam->width;
	pStream->videoInfo.height = pParam->height;
	pStream->videoInfo.fps = pParam->encCfg.fps & 0xFF;
	pStream->videoInfo.bps = pParam->encCfg.bps;
	pStream->videoInfo.IFrameInterval = pParam->encCfg.IFrameInterval;
    pStream->videoInfo.bFristFrm = pParam->bFirstFrm;

	pStream->audioInfo.audioStreamType = getAudioStreamType(pParam->encCfg.encType.audioType);
	pStream->audioInfo.audio_num	 = 0;

	switch(pStream->audioInfo.audioStreamType)
	{
		case STREAM_TYPE_AUDIO_MPEG2:
			pStream->audioInfo.frame_len	 = 592;
			pStream->audioInfo.sample_rate	 = 16000;
			pStream->audioInfo.bit_rate      = 64000;
			break;
		case STREAM_TYPE_AUDIO_MPEG1:
			pStream->audioInfo.frame_len	 = 592;
			pStream->audioInfo.sample_rate	 = 16000;
			pStream->audioInfo.bit_rate      = 64000;
			break;
        case STREAM_TYPE_AUDIO_AAC:
            pStream->audioInfo.frame_len     = 160;
            pStream->audioInfo.sample_rate   = 16000;
            pStream->audioInfo.bit_rate      = 64000;
			break;
		case STREAM_TYPE_AUDIO_G722_1:
			pStream->audioInfo.frame_len	 = 80;
			pStream->audioInfo.sample_rate	 = 16000;
			pStream->audioInfo.bit_rate 	 = 25*80*8;
			break;
		case STREAM_TYPE_AUDIO_G711A:
			pStream->audioInfo.frame_len	 = 320;
			pStream->audioInfo.sample_rate	 = 8000;
			pStream->audioInfo.bit_rate 	 = 25*320*8;
			break;
		case STREAM_TYPE_AUDIO_G711U:
			pStream->audioInfo.frame_len	 = 320;
			pStream->audioInfo.sample_rate	 = 8000;
			pStream->audioInfo.bit_rate 	 = 25*320*8;
			break;
		case STREAM_TYPE_AUDIO_G726:
			pStream->audioInfo.frame_len	 = 80;
			pStream->audioInfo.sample_rate	 = 8000;
			pStream->audioInfo.bit_rate  = 25*80*8;
			break;
		case STREAM_TYPE_AUDIO_L16: 
			pStream->audioInfo.frame_len	 = 1280;
			pStream->audioInfo.sample_rate	 = 16000;
			pStream->audioInfo.bit_rate	 = 25*1280*8;
			break;
		default:
			break;
	}	
}

/*******************************************
* 功  能：PS视频流封装
*
* 参  数：chan             -I  通道号
*         pEncAttr         -I  编码参数
*
* 返回值：返回状态码
********************************************/
static INT MediaStream:PackPSStream(MEDIA_STREAM_PACK_CTRL_T  *pCtrl, MEDIA_STREAM_PACK_PATAM_T *pParam)
{
    INT32 i = 0;
    INT32 nRet = HIKDSP_OK;
    UINT clock;    
    PUINT8 pOutBuf = pParam->out_buf;
    UINT32 outBufSize = pParam->out_buf_length;
    PSMUX_PROCESS_PARAM muxPrcParam;
    clock = pParam->timeStamp * 45;
    
    if(pCtrl->psHandle == NULL)
    {
   
         LOG_ERROR("psHandle is null !!!\n");
        return HIKDSP_ERR_INVALID_PARAM;
    }
    
    if((pParam->naluNum == 0) || (pParam->out_buf == NULL) || (pParam->out_buf_length == 0))
    {
        PACK_PRT_ERROR("invalid param !!!\n");
        return HIKDSP_ERR_INVALID_PARAM;
    }
    
    OsalMem_set(&muxPrcParam, 0, sizeof(muxPrcParam), GET_LINE);
    
    muxPrcParam.company_mark = CO_TYPE_HK;
    muxPrcParam.camera_mark = CAMERA_TYPE_UNDEF;
    
    if(pParam->picture_mode == I_FRAME_MODE)
    {
        muxPrcParam.global_time.year=(UINT)pParam->now.year;
        muxPrcParam.global_time.month=(UINT)pParam->now.month;
        muxPrcParam.global_time.date=(UINT)pParam->now.day;
        muxPrcParam.global_time.hour=(UINT)pParam->now.hour;
        muxPrcParam.global_time.minute=(UINT)pParam->now.minute;
        muxPrcParam.global_time.second=(UINT)pParam->now.second;
        muxPrcParam.global_time.msecond=(UINT)pParam->now.milliSecond;
        muxPrcParam.is_key_frame=TRUE;
        muxPrcParam.frame_type=FRAME_TYPE_VIDEO_IFRAME;
    }
    else if (pParam->picture_mode == P_FRAME_MODE)
    {
        muxPrcParam.is_key_frame=FALSE;
        muxPrcParam.frame_type=FRAME_TYPE_VIDEO_PFRAME;
    }
    else if ((pParam->picture_mode == AUDIO_I_FRAME_MODE) || (pParam->picture_mode == AUDIO_P_FRAME_MODE))
    {
        muxPrcParam.is_key_frame=FALSE;
        muxPrcParam.frame_type=FRAME_TYPE_AUDIO_FRAME;
    }
    else if (pParam->picture_mode == PRIVT_FRAME_MODE)
    {
        muxPrcParam.is_key_frame=FALSE;
        muxPrcParam.frame_type=FRAME_TYPE_PRIVT_FRAME;
    }
    else
    {
        PACK_PRT_ERROR("invalid picture_mode = %d \n",pParam->picture_mode);
        return HIKDSP_ERR_INVALID_PARAM;
    }
    
    for(i = 0; i < pParam->naluNum; i++)
    {
        muxPrcParam.sys_clk_ref=clock;
        muxPrcParam.ptime_stamp=clock;
        muxPrcParam.frame_num=pParam->frameNum;
        muxPrcParam.unit_in_buf = pParam->nalu[i].nalu_ptr;
        muxPrcParam.unit_in_len =pParam->nalu[i].nalu_len;
        muxPrcParam.out_buf= pOutBuf;
        muxPrcParam.out_buf_len = 0;
        muxPrcParam.out_buf_size = outBufSize;
        muxPrcParam.is_unit_start = 1;
        muxPrcParam.is_unit_end   = 1;
        muxPrcParam.is_first_unit = (i==0);
        muxPrcParam.is_last_unit  = (i == (pParam->naluNum-1));
		muxPrcParam.encrypt_round = pParam->encrypt_round[i];
        
        nRet = PSMUX_Process(pCtrl->psHandle,&muxPrcParam);
        if (nRet == HIK_PSMUX_LIB_S_OK)
        {
            pOutBuf += muxPrcParam.out_buf_len;
            outBufSize -= muxPrcParam.out_buf_len;
            pParam->out_length += muxPrcParam.out_buf_len;
        }
        else
        {
            PACK_PRT_ERROR("PSMux process error!0x%x\n",nRet);
            return HIKDSP_ERR_UNDEFINED;
        }
    }
        
    return HIKDSP_OK;
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










