#ifndef SYS_COMMON_H
#define SYS_COMMON_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/

/** 不等待 */
#define NO_WAIT             (0)
/** 永远等待 */
#define WAIT_FOREVER        ((UINT32)(-1))

/** 结构参数偏移 */
#define STRUCT_OFF_SET(type, member) (((size_t) &((type *)0)->member))
/** 结构体成员大小 */
#define STRUCT_OFF_SIZE(type, member) ((sizeof((type *)0)->member))
/** 根据结构体子成员找到父地址 */
#define STRUCT_CONTAINER_OF(ptr, type, member) ((type *)((char *)ptr - STRUCT_OFF_SET(type, member)))

/* ========================================================================== */
/*                             Type Declaration                               */
/* ========================================================================== */

/** unsigned char 重定义 */
#ifndef HAL_TYPE_UINT8
#define HAL_TYPE_UINT8
/** unsigned char 重定义 */
typedef unsigned char     UINT8;
#endif /* HAL_TYPE_UINT8 */


/** unsigned short 重定义 */
#ifndef HAL_TYPE_UINT16
#define HAL_TYPE_UINT16
/** unsigned short 重定义 */
typedef unsigned short    UINT16;
#endif /* HAL_TYPE_UINT16 */

/** unsigned int 重定义 */
#ifndef HAL_TYPE_UINT32
#define HAL_TYPE_UINT32
/** unsigned int 重定义 */
typedef unsigned int     UINT32;
#endif /* HAL_TYPE_UINT32 */

/** unsigned __int64 重定义 */
#ifndef HAL_TYPE_UINT64
#define HAL_TYPE_UINT64
#if defined(_MSC_VER)
/** unsigned __int64 重定义 */
typedef unsigned __int64 UINT64;
#else
/** unsigned long long 重定义 */
typedef unsigned long long UINT64;
#endif /* _MSC_VER */
#endif/* HAL_TYPE_UINT64 */

/** unsigned char 重定义 */
#ifndef HAL_TYPE_CHAR
#define HAL_TYPE_CHAR
/** unsigned char 重定义 */
typedef char     CHAR;
#endif /* HAL_TYPE_CHAR */

/** signed char 重定义 */
#ifndef HAL_TYPE_INT8
#define HAL_TYPE_INT8
/** signed char 重定义 */
typedef signed char     INT8;
#endif /* HAL_TYPE_INT8 */

/** short char 重定义 */
#ifndef HAL_TYPE_INT16
#define HAL_TYPE_INT16
/** short char 重定义 */
typedef short    INT16;
#endif /* HAL_TYPE_INT16 */

/** int 重定义 */
#ifndef HAL_TYPE_INT32
#define HAL_TYPE_INT32
/** int 重定义 */
typedef int    INT32;
#endif /* HAL_TYPE_INT32 */

/** __int64 重定义 */
#ifndef HAL_TYPE_INT64
#define HAL_TYPE_INT64
#if defined(_MSC_VER)
/** __int64 重定义 */
typedef __int64  INT64;
#else
/** long long 重定义 */
typedef long long INT64;
#endif /* _MSC_VER */
#endif /* HAL_TYPE_INT64 */

/** double 重定义 */
#ifndef HAL_TYPE_FLOAT64
#define HAL_TYPE_FLOAT64
/** double 重定义 */
typedef double   FLOAT64;
#endif /* HAL_TYPE_FLOAT64 */

/** float 重定义 */
#ifndef HAL_TYPE_FLOAT32
#define HAL_TYPE_FLOAT32
/** float 重定义 */
typedef float   FLOAT32;
#endif /* HAL_TYPE_FLOAT32 */

/** double 重定义 */
#ifndef HAL_TYPE_DOUBLE
#define HAL_TYPE_DOUBLE
/** double 重定义 */
typedef double   DOUBLE;
#endif /* HAL_TYPE_DOUBLE */

/** float 重定义 */
#ifndef HAL_TYPE_FLOAT
#define HAL_TYPE_FLOAT
/** float 重定义 */
typedef float   FLOAT;
#endif /* HAL_TYPE_FLOAT */

/** VOID 重定义 */
#ifndef HAL_TYPE_VOID
#define HAL_TYPE_VOID
/** VOID 重定义 */
#define VOID    void
#endif /* HAL_TYPE_VOID */

/** bool 重定义 */
#ifndef HAL_TYPE_BOOL
#define HAL_TYPE_BOOL
/** bool 重定义 */
typedef bool BOOL;
#endif /* HAL_TYPE_BOOL */

/** int 重定义 */
#ifndef HAL_TYPE_INT
#define HAL_TYPE_INT
/** bool 重定义 */
typedef int  INT;
#endif /* HAL_TYPE_INT */

/** unsigned int 重定义 */
#ifndef HAL_TYPE_UINT
#define HAL_TYPE_UINT
/** unsigned int 重定义 */
typedef unsigned int  UINT;
#endif /* HAL_TYPE_UINT */

/** long 重定义 */
#ifndef HAL_TYPE_LONG
#define HAL_TYPE_LONG
/** long 重定义 */
typedef long    LONG;
#endif /* HAL_TYPE_LONG */

/** unsigned long 重定义 */
#ifndef HAL_TYPE_ULONG
#define HAL_TYPE_ULONG
/** unsigned long 重定义 */
typedef unsigned long ULONG;
#endif /* HAL_TYPE_ULONG */

/** int 重定义 */
#ifndef HAL_TYPE_PINT
#define HAL_TYPE_PINT
/** unsigned long 重定义 */
typedef int                *PINT;
#endif /* HAL_TYPE_PINT */

/** unsigned int 重定义 */
#ifndef HAL_TYPE_PUINT
#define HAL_TYPE_PUINT
/** unsigned long 重定义 */
typedef unsigned int       *PUINT;
#endif /* HAL_TYPE_PUINT */

//
#ifndef HAL_TYPE_PVOID
#define HAL_TYPE_PVOID
/** unsigned long 重定义 */
typedef void               *PVOID;
#endif /* HAL_TYPE_PVOID */

//
#ifndef HAL_TYPE_PUINT8
#define HAL_TYPE_PUINT8
//
typedef unsigned char      *PUINT8;
#endif /* HAL_TYPE_PUINT8 */

//
#ifndef HAL_TYPE_PUINT16
#define HAL_TYPE_PUINT16
//
typedef unsigned short     *PUINT16;
#endif /* HAL_TYPE_PUINT16 */

//
#ifndef HAL_TYPE_UINTV
#define HAL_TYPE_UINTV
//
typedef volatile UINT       UINTV;
#endif /* HAL_TYPE_UINTV */

//
#ifndef HAL_TYPE_PUINTV
#define HAL_TYPE_PUINTV
//
typedef volatile UINT *     PUINTV;
#endif /* HAL_TYPE_PUINTV*/

//
#ifndef HAL_TYPE_UINTPTR
#define HAL_TYPE_UINTPTR
//
typedef uintptr_t           UINTPTR;
#endif /* HAL_TYPE_UINTPTR*/

/** _BSP_DATE_TIME_ */
#ifndef _BSP_DATE_TIME_
#define _BSP_DATE_TIME_
/**
 *  @struct _BSP_DATE_TIME_
 *  @brief  date time
 */
typedef struct
{
    INT16 iYear;             /**< 1970 ~ 2038 */
    INT16 iMonth;            /**< 1-12 */
    INT16 iDayOfWeek;        /**< 0-SUNDAY, 1-MONDAY, ..., 6-WEEK_SATURDAY */
    INT16 iDay;              /**< 1-31  */
    INT16 iHour;             /**< 0-23  */
    INT16 iMinute;           /**< 0-59  */
    INT16 iSecond;           /**< 0-59  */
    INT16 iMilliSecond;      /**< 0-999   */
} DATE_TIME_T;
#endif /* _BSP_DATE_TIME_ */

/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */

//
#ifndef TYPE_TRUE
#define TYPE_TRUE
#ifndef TRUE
//
#define TRUE true
#endif /* TRUE */
#endif /* TYPE_TRUE */

//
#ifndef TYPE_FALSE
#define TYPE_FALSE
#ifndef FALSE
//
#define FALSE false
#endif /* FALSE */
#endif /* TYPE_FALSE */

#ifndef INT_MINI
//
#define INT_MINI     (-2147483647 - 1) /* minimum (signed) int value */
#endif /* INT_MINI */

#ifndef INT_MAXI
//
#define INT_MAXI      (2147483647)    /* maximum (signed) int value */
#endif /* INT_MAXI */


#ifndef STATUS
//
#define STATUS    int
#endif /* STATUS */

#ifdef _MSC_VER
#undef ERROR
#endif /* _MSC_VER */

#ifndef ERROR
//
#define ERROR               -1
#endif /* ERROR */

#ifndef OK
//
#define OK                  0
#endif /* OK */

//
#ifndef TYPE_ADDR
#define TYPE_ADDR
//
#define UINT_ADDR LONG
//
typedef void * ADDR;
#endif /* TYPE_ADDR */
//
#define SYS_SET_BIT(x,y) ((x)|=(1<<y))  /**< 将X的第Y位置1*/
#define SYS_CLR_BIT(x,y) ((x)&=~(1<<y)) /**< 将X的第Y位清0*/
#define SYS_GET_BIT(x,y) ((x)&(1<<y))   /**< 将X的第Y位清0*/

#ifndef UINT32_MAX
//
#define UINT32_MAX     ((UINT32)(-1))
#endif/* UINT32_MAX*/



#ifndef MIN
//
#define MIN(a, b)    ((a) > (b) ? (b) : (a))
#endif/* MIN*/

#ifndef MAX
//
#define MAX(a,b)  (((a)>(b))?(a):(b))
#endif/* MAX*/


#define SIZE_1KB                           (0x00000400)    /**< 1KB */
#define SIZE_2KB                           (0x00000800)    /**< 2KB */
#define SIZE_4KB                           (0x00001000)    /**< 4KB */
#define SIZE_8KB                           (0x00002000)    /**< 8KB */
#define SIZE_16KB                          (0x00004000)    /**< 16KB */
#define SIZE_32KB                          (0x00008000)    /**< 32KB */
#define SIZE_64KB                          (0x00010000)    /**< 64KB */
#define SIZE_128KB                         (0x00020000)    /**< 128KB */
#define SIZE_256KB                         (0x00040000)    /**< 256KB */
#define SIZE_512KB                         (0x00080000)    /**< 512KB */

#define SIZE_1MB                           (0x00100000)    /**< 1MB */
#define SIZE_2MB                           (0x00200000)    /**< 2MB */
#define SIZE_3MB                           (0x00300000)    /**< 3MB */
#define SIZE_4MB                           (0x00400000)    /**< 4MB */
#define SIZE_5MB                           (0x00500000)    /**< 5MB */
#define SIZE_6MB                           (0x00600000)    /**< 6MB */
#define SIZE_8MB                           (0x00800000)    /**< 8MB */
#define SIZE_10MB                          (0x00A00000)    /**< 10MB */
#define SIZE_12MB                          (0x00C00000)    /**< 12MB */
#define SIZE_16MB                          (0x01000000)    /**< 16MB */
#define SIZE_18MB                          (0x01200000)    /**< 18MB */
#define SIZE_20MB                          (0x01400000)    /**< 20MB */
#define SIZE_22MB                          (0x01600000)    /**< 22MB */
#define SIZE_24MB                          (0x01800000)    /**< 24MB */
#define SIZE_30MB                          (0x01E00000)    /**< 30MB */
#define SIZE_32MB                          (0x02000000)    /**< 32MB */
#define SIZE_64MB                          (0x04000000)    /**< 64MB */
#define SIZE_128MB                         (0x08000000)    /**< 128MB */
#define SIZE_256MB                         (0x10000000)    /**< 256MB */
#define SIZE_512MB                         (0x20000000)    /**< 512MB */

#define SIZE_1GB                           (0x40000000)    /**< 1GB */
#define SIZE_2GB                           (0x80000000)    /**< 2GB */

#ifndef HAL_PATH_MAX
#define HAL_PATH_MAX (256)
#endif/* HAL_PATH_MAX */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_COMMON_H */


