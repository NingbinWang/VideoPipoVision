#ifndef  _MEDIA_STREAM_H_
#define _MEDIA_STREAM_H_
#include "Media.h"
#include "MediaFrame.h"
#include "Logger.h"
#include "Common.h"
#include "Mutex.h"

class MediaStream
{

    /* data */
public:
    static MediaStream* createNew(MEDIA_PARAM_T* pParam);
    MediaStream(MEDIA_PARAM_T* pParam);
    ~MediaStream();
    INT32  SendStreamToRawPool(UINT32 uChan,PUINT8 pStreamSrc,UINT32 uLength);
	INT32  SendStreamToRecPool(UINT32 uChan,PUINT8 pStreamSrc,UINT32 uLength, BOOL bVideo, BOOL dropMode);
   
private:
	MEDIA_PARAM_T* mpParam;
	Mutex* mpSendStreamToRawPoolMutex;
	Mutex* mpSendStreamToRecPoolMutex;
};







#endif
