#include "MediaPriv.h"
#include "V4l2Device.h"
#include "V4l2Capture.h"
#include "New.h"
#include "SysMemory.h"
#include "Thread.h"
#define MEDIA_SHARM_MEM_MAX (1024*1024*32)



MediaStream* MediaStream::createNew(MEDIA_SHAREDATA_T &ShareData)
{
    //return new RtspServer(env, addr);
    return New<MediaStream>::allocate(&ShareData);
}
MediaStream::MediaStream(MEDIA_SHAREDATA_T *pShareData):
mpShareData(pShareData)
{
	mpShareData.addr = (CPU_BITS)SysMemory_malloc(MEDIA_SHARM_MEM_MAX);
	mpShareData.u32len = MEDIA_SHARM_MEM_MAX;
	mpShareData.rIdx = 0;
	mpShareData.wIdx = 0;



	
}

MediaStream::MediaStream() 
{
	
}




