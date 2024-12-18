#include "media_priv.h"
MEDIA_PARAM_T *pMediaInitParam = NULL;

//MEDIA_PARAM_T * halmedia_GetMediaInitParam;

int Media_Ipcinit(void)
{
#if SHMSHARE
     int shmId = 0;
	/* 创建直接共享内存 */
	shmId = shmget((key_t)MEDIA_SHARE_SEGMENT_ID,sizeof(MEDIA_PARAM_T),IPC_CREAT|0666);
	if(-1 == shmId)
	{
		printf("error shmKey = %d\n", shmId);
		return -1;
	}
    
	pMediaInitParam =(MEDIA_PARAM_T *)shmat(shmId, NULL, 0);
	if ((void *)(-1) == (void *)(pMediaInitParam))
	{
		printf("!!! shmat pMediaInitParam SIZE=0x%x failed to exit !!!\n", sizeof(MEDIA_PARAM_T));
		return -1;
	}
#else
    pMediaInitParam =(MEDIA_PARAM_T*) malloc(sizeof(MEDIA_PARAM_T));
	if(!pMediaInitParam)
    {
		printf("[%s:%d]malloc error\n", __FUNCTION__, __LINE__);
        return -1;
    }
    memset(pMediaInitParam,0,sizeof(MEDIA_PARAM_T));
#endif
    return 0;
}

VI_CFG_PARAM_T *Media_Get_ViParam(void)
{
	if(pMediaInitParam == NULL){
		printf("[%s:%d]Media_Get_ViParam error YOU MSUT init media param\n", __FUNCTION__, __LINE__);
		return NULL;
	}
    return pMediaInitParam->viCfgParam;
}


int Media_Init(void)
{
	int ret = -1;
#ifdef HDAL_DEF
    ret = hdal_system_init(HDAL_PROFUCT_ID_A30051);
	if(ret != 0)
	{
		printf("hdal_system_init fail\n");
	}
	ret = hdal_vi_init(HDAL_PROFUCT_ID_A30051);
	if(ret != 0)
	{
		printf("hdal_system_init fail\n");
	}
#endif
    return 0;
}





//int Media_Exit(void)
//{
//    hdal_exit();
//    return 0;
//}

int Media_Memalloc(unsigned int *phy_addr, void **virt_addr,unsigned int size)
{
	int ret = -1;
#ifdef HDAL_DEF
	ret = hdal_Mem_Malloc(phy_addr,virt_addr,size);
#endif
	return ret;
}