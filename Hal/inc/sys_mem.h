#ifndef SYS_MEM_INTERFACE_H
#define SYS_MEM_INTERFACE_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include"sys_common.h"

#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * @brief      申请一块固定大小的内存，内部初始化为0
 * @param[in]  uSize 需要申请的内存大小
 * @return     成功返回内存指针，失败返回NULL
 */
VOID *sys_mem_malloc(UINT32 uSize);

/**
 * @brief      申请一块大小为nmemb * uSize 的内存，内部初始化为0
 * @param[in]  uMemBlock  申请多少个块
 * @param[in]  uSize 每个单元的大小
 * @return     成功返回内存指针，失败返回NULL
 */
VOID *sys_mem_calloc(UINT32 uMemBlock, UINT32 uSize);

/**
 * @brief      改变指针pPtr所指内存块大小
 * @param[in]  pPtr 内存指针
 * @param[in]  uSize 调整后的大小，语义同man realloc
 * @return     成功返回内存指针，失败返回NULL
 * @note       如果返回失败，原指针还是有效的
 */
VOID *sys_mem_realloc(VOID *pPtr, UINT32 uSize);

/**
 * @brief      申请一块带对齐的内存块，内部初始化为0
 * @param[in]  uAlign 需要按多少字节对齐
 * @param[in]  uSize 需要申请的内存大小
 * @return     成功返回内存指针，失败返回NULL
 */
VOID *sys_mem_align(UINT32 uAlign, UINT32 uSize);

/**
 * @brief      释放内存
 * @param[in]  pPtr 需要释放的内存指针
 * @return     无
 */
VOID sys_mem_free(VOID *pPtr);

/**
 * @brief       内存拷贝
 * @param[in]   pDst     目标地址
 * @param[in]   pSrc     源地址
 * @param[in]   uLength  拷贝长度
 * @return      无
 */
VOID *sys_mem_copy(VOID *pDst, VOID *pSrc, UINT32 uLength);

#ifdef __cplusplus
}
#endif/*__cplusplus*/

#endif/* SYS_MEM_INTERFACE_H */

