#ifndef _MEDIAENCINNER_H_
#define _MEDIAENCINNER_H_
#include "Media.h"
#include "MediaFrame.h"
#include "Logger.h"
#include "Common.h"
#include <thread>
#include "MppEncoder.h"


#define MEDIA_ENC_MAX_NALU_NUM    (20)  //PAE_MAX_NALU_CNT
#define MPPENCOERSIZE             (5*1024*1024)


typedef enum
{
    ENC_FRM_I   = 0,
    ENC_FRM_P   = 1,
    ENC_FRM_B   = 2,
    ENC_FRM_MAX,
}MEDIA_ENC_STREAM_TYPE_E;



typedef struct
{
    VOID    *pVirAddr;    //虚拟地址
    VOID    *pPhyAddr;    //物理地址
    UINT32   u32Stride;  //stride[0]Y分量 lineoffset stride[1]UV分量   lineoffset
    INT32    iSize;
}MEDIA_ENC_FRAME_T;


typedef struct
{
    UINT32                      u32TimeStamp;          /* 时间戳 */
    MEDIA_ENC_STREAM_TYPE_E     stFrame_type;         /* 帧类型 I_FRAME_MODE/P_FRAME_MODE/BP_FRAME_MODE */
    UINT32                      u32NaluNum;            /* nalu个数 */
	MEDIA_YUV_FRAME_T           stEncFrame;
    UINT32                      astNalu[MEDIA_ENC_MAX_NALU_NUM];  /* nalu长度 */
	UINT8                       u8Nalutype[MEDIA_ENC_MAX_NALU_NUM];
}MEDIA_ENC_STREAM_T;

typedef void (*MediaEncCallback)(void* userdata, const char* data, int size);





class MediaEnc
{
public:
	static MediaEnc* createNew(MEDIA_ENC_PARAM_T&Params);
    MediaEnc(MEDIA_ENC_PARAM_T* pParams);
	~MediaEnc();
	int MediaEncEncode(void* mpp_buf, char* enc_buf, int max_size);
	int MediaEncGetHeader(char* enc_buf, int max_size);
	int MediaEncSetCallback(MediaEncCallback callback );
	size_t MediaEncGetFrameSize();
	void* MediaEncGetInputFrame();
	int MediaEncGetInputFrameBufferFd(void * source);
	void* MediaEncGetInputFrameBufferAddr(void * source);
	MppFrameFormat MediaEncFmtTranslation(MEDIA_FORMAT_TYPE_E eType);
	MppCodingType MediaEncTypeTranslation(MEDIA_ENC_CODETYPE_E eType);
	MppEncRcMode MediaEncTypeTranslation(MEDIA_ENC_RCMODE_E eType);
	VOID MediaEncStopThread();
	VOID MediaEncGetStreamFunc(VOID* pUserdata);
	VOID MediaEncStartThread();

private:
	UINT32       muChan;
	MppEncoder * mpEncoder;
	std::thread  mThread;
	bool bRunning{false};
};


#endif

