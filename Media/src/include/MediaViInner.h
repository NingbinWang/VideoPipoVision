#ifndef _MEDIAVIINNER_H_
#define _MEDIAVIINNER_H_
#include "Media.h"
#include "MediaFrame.h"
#include "Logger.h"
#include "Common.h"
#include "V4l2Device.h"
#include "V4l2Capture.h"
#include "New.h"
#include "Mutex.h"


class MediaVi
{
public:
    static MediaVi* createNew(VI_CFG_PARAM_T&Params);
    MediaVi(VI_CFG_PARAM_T* pParams);
    ~MediaVi();
    int readFramebuf(char* buffer, int bufferSize);
    bool poll();
	INT32 GetFrameFromQueue(MEDIA_VIDEO_FRAME_T *pstFrameInfo, INT32 timeout);
	BOOL PutFrameInQueue(MEDIA_VIDEO_FRAME_T *pstFrameInfo);

private:
    VI_CFG_PARAM_T* mpParams;
    Mutex* mpMutex;
    V4l2Capture* mpCapture;
};

#endif

