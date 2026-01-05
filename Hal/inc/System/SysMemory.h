#ifndef _SYS_MEMORY_H_
#define _SYS_MEMORY_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include"Common.h"

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * @brief      申请一块固定大小的内存，内部初始化为0
 * @param[in]  uSize 需要申请的内存大小
 * @return     成功返回内存指针，失败返回NULL
 */
VOID *SysMemory_malloc(UINT32 uSize);

/**
 * @brief      申请一块大小为nmemb * uSize 的内存，内部初始化为0
 * @param[in]  uMemBlock  申请多少个块
 * @param[in]  uSize 每个单元的大小
 * @return     成功返回内存指针，失败返回NULL
 */
VOID *SysMemory_calloc(UINT32 uMemBlock, UINT32 uSize);

/**
 * @brief      改变指针pPtr所指内存块大小
 * @param[in]  pPtr 内存指针
 * @param[in]  uSize 调整后的大小，语义同man realloc
 * @return     成功返回内存指针，失败返回NULL
 * @note       如果返回失败，原指针还是有效的
 */
VOID *SysMemory_realloc(VOID *pPtr, UINT32 uSize);

/**
 * @brief      申请一块带对齐的内存块，内部初始化为0
 * @param[in]  uAlign 需要按多少字节对齐
 * @param[in]  uSize 需要申请的内存大小
 * @return     成功返回内存指针，失败返回NULL
 */
VOID *SysMemory_align(UINT32 uAlign, UINT32 uSize);

/**
 * @brief      释放内存
 * @param[in]  pPtr 需要释放的内存指针
 * @return     无
 */
VOID SysMemory_free(VOID *pPtr);

/**
 * @brief       内存拷贝
 * @param[in]   pDst     目标地址
 * @param[in]   pSrc     源地址
 * @param[in]   uLength  拷贝长度
 * @return      无
 */
VOID *SysMemory_copy(VOID *pDst, VOID *pSrc, UINT32 uLength);



#endif/* SYS_MEM_INTERFACE_H */

