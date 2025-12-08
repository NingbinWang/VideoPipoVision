#ifndef _MEDIA_PRIV_H_
#define _MEDIA_PRIV_H_
#include "Media.h"
#include "MediaFrame.h"
#include "RKnpu.h"
#include "Logger.h"
#include "RKrga.h"
// Media内部参数
typedef struct
{
    //BOOL                    bDspStarted[MAX_CORE_NUM];/**< 启动完成后置True */
    //UINT                    bootInfo[MAX_CORE_NUM];
    /*DSP 内部使用参数*/
    //CMD_SHARE_BUF           cmdShareBuf;        /*DSP->DSP 命令缓冲，DSP内部使用*/
   //DSP_MEM_SEC             dspMemSec[MAX_DSP_MEM_SEC_NUM];/*DSP共享缓存，DSP内部使用*/
    UINT                    dspMemSecCnt;
    UINT                    dspProcessId[2];
    UINT                    debugPin[27];


    BOOL                    algLoad;
    /*linux启动完成*/
    UINT32                  SysStart;	
}MEDIA_INNER_PARAM_T;


RKrga * Media_GetRkrga(void);
RKnpu * Media_GetRknpu(void);


#endif