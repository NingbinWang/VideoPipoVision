#include "MediaEncInner.h"
#include "SysMemory.h"
#include "MediaPriv.h"
int  MediaEnc::MediaEncEncode(void* mpp_buf, char* enc_buf, int max_size)
{
   return mpEncoder->Encode(mpp_buf,enc_buf,max_size);
}


int  MediaEnc::MediaEncGetHeader(char* enc_buf, int max_size)
{
   return mpEncoder->GetHeader(enc_buf,max_size);
}

int  MediaEnc::MediaEncSetCallback(MediaEncCallback callback )
{
   return mpEncoder->SetCallback((MppEncoderFrameCallback)callback);
}

size_t MediaEnc::MediaEncGetFrameSize()
{
   return mpEncoder->GetFrameSize();
}

void* MediaEnc::MediaEncGetInputFrame()
{
   return mpEncoder->GetInputFrameBuffer();
}

int MediaEnc::MediaEncGetInputFrameBufferFd(void * source)
{
   return mpEncoder->GetInputFrameBufferFd(source);
}

void* MediaEnc::MediaEncGetInputFrameBufferAddr(void * source)
{
   return mpEncoder->GetInputFrameBufferAddr(source);
}


MppFrameFormat MediaEnc::MediaEncFmtTranslation(MEDIA_FORMAT_TYPE_E eType)
{
    switch(eType){
			case MEDIA_FORMAT_YUV420SP:
				return MPP_FMT_YUV420SP;
			default:
				return MPP_FMT_BUTT;
    }
	return MPP_FMT_BUTT;

}

MppCodingType MediaEnc::MediaEncTypeTranslation(MEDIA_ENC_CODETYPE_E eType)
{
	
	switch(eType){
		case MEDIA_ENC_CODETYPE_AVC:
			return MPP_VIDEO_CodingAVC;
		default:
			return MPP_VIDEO_CodingMax;
	}
	return MPP_VIDEO_CodingMax;
}

MppEncRcMode MediaEnc::MediaEncTypeTranslation(MEDIA_ENC_RCMODE_E eType)
{
	switch(eType){
			case MEDIA_ENC_RCMODE_VBR:
				return MPP_ENC_RC_MODE_VBR;
			default:
				return MPP_ENC_RC_MODE_BUTT;
		}
	return MPP_ENC_RC_MODE_BUTT;
}

MediaEnc* MediaEnc::createNew(MEDIA_ENC_PARAM_T&pParams)
{
    return New<MediaEnc>::allocate(&pParams);
}
MediaEnc::MediaEnc(MEDIA_ENC_PARAM_T *pParams)
{
	 MEDIA_FORMAT_TYPE_E eType ;
	 MppEncoderParams stEncoderParam;
	 eType = MediaForccFrame(pParams->strStreamType);
	 stEncoderParam.width = (RK_U32)pParams->uEncW;
     stEncoderParam.height = (RK_U32)pParams->uEncH;
     stEncoderParam.fmt =MediaEncFmtTranslation(eType);
     stEncoderParam.type = MediaEncTypeTranslation(pParams->eEncoderType);
     stEncoderParam.rc_mode =  MediaEncTypeTranslation(pParams->eRcMode);
	 mpEncoder = Media_Getmppencoder();
	 mpEncoder->Init(stEncoderParam,nullptr);//Encoder目前只有一个所以这是要修正的
	 muChan = pParams->uChan;
}

MediaEnc::~MediaEnc() 
{
	
}


VOID MediaEnc::MediaEncGetStreamFunc(void* pUserdata)
{
	MEDIA_INNER_PARAM_T* pInnerParam =  Media_Get_InnerParam();
	MEDIA_VIDEO_FRAME_T stFrameInfo={0};
	MEDIA_ENC_FRAME_T   stEncFrame = {0};
	stEncFrame.pVirAddr = SysMemory_malloc(MPPENCOERSIZE);
	while(bRunning){
		pInnerParam->apVi[muChan]->GetFrameFromQueue(&stFrameInfo, 1000);
        //SendStreamToHost 裸流
	    pInnerParam->pStream->SendStreamToRawPool(muChan,(PUINT8)stFrameInfo.stYuvframe.pVirAddr,(UINT32)stFrameInfo.stVideoHeader.sSize);
	    //设置OSD回调
	    //设置AI回调
        //送入编码
		stEncFrame.iSize =  MediaEnc::MediaEncEncode(stFrameInfo.stYuvframe.pPhyAddr,(char *)stEncFrame.pVirAddr,MPPENCOERSIZE);
		//stEncFrame.pVirAddr为H264的编码数据
	    pInnerParam->pStream->SendStreamToRecPool(muChan,(PUINT8)stEncFrame.pVirAddr,stEncFrame.iSize,true, false);
		pInnerParam->apVi[muChan]->PutFrameInQueue(&stFrameInfo);
	}

}



VOID MediaEnc::MediaEncStopThread() {
    	bRunning = false;
        if (mThread.joinable()) {
            mThread.join();
        }
    }


VOID MediaEnc::MediaEncStartThread() 
{
	bRunning = true;
	mThread = std::thread([this]() {
		MediaEncGetStreamFunc(NULL);
	});

}


