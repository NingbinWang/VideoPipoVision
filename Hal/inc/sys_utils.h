#ifndef SYS_UTILS_INTERFACE_H
#define SYS_UTILS_INTERFACE_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "sys_common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */
/** ROL:value循环左移bits位*/
#define SYS_ROL(value,bits) ((value << bits) | (value >> (sizeof(value)*8 -bits)))

/** ROR:value循环右移bits位*/
#define SYS_ROR(value,bits) ((value >> bits) | (value << (sizeof(value)*8 -bits)))

#ifndef min
/** 取a、b最小值 */
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif /* min */
#ifndef max
/** 取a、b最大值 */
#define max(a, b) ((a) < (b) ? (b) : (a))
#endif /* max */
#ifndef CLAMP
/** 若X在[low,high]范围内,则等于X;如果X小于low,则返回low;如果X大于high,则返回high */
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#endif /* CLAMP */

/** 指针对齐 */
#define PTR_ALIGN(p, a)         ((typeof(p))ALIGN((ULONG)(p), (a)))
/** 判断是否对齐 */
#define IS_ALIGNED(x, a)        (((x) & ((typeof(x))(a) - 1)) == 0)
/** 判断是否为2的次幂 */
#define IS_POWEROF2(v) (((v != 0UL) && ((v & (v - 1UL)) == 0UL))? (1) : (0))

#ifndef SYS_CHECK_PTR_BREAK
/** 判断指针是否为空，若为空，输出一段打印后break */
#define SYS_CHECK_PTR_BREAK(p, PRINTF){ \
    if (NULL == (p)) \
    { \
        PRINTF("func %s, line %d Ptr is NULL\n", __FUNCTION__, __LINE__);\
        break;\
    } \
}
#endif /* SYS_CHECK_PTR_BREAK */

#ifndef SYS_CHECK_RET_BREAK
/** 判断返回值，若为ERROR，输出一段打印后break */
#define SYS_CHECK_RET_BREAK(iRet, PRINTF){ \
    if (ERROR == iRet) \
    { \
        PRINTF("func %s, line %d iRet is ERROR\n", __FUNCTION__, __LINE__);\
        break;\
    } \
}
#endif /* SYS_CHECK_RET_BREAK */

#ifndef SYS_CHECK_PTR_RET
/** 判断指针是否为空，若为空，输出一段打印后返回 */
#define SYS_CHECK_PTR_RET(p, PRINTF, RET) do { \
    if (NULL == (p)) \
    { \
        PRINTF("func %s, line %d Ptr is NULL\n", __FUNCTION__, __LINE__);\
        return RET;\
    } \
}while (0)
#endif /* SYS_CHECK_PTR_RET */

#ifndef SYS_CHECK_PTR_VOID
/** 判断指针是否为空，若为空，输出一段打印后返回 */
#define SYS_CHECK_PTR_VOID(p, PRINTF) do { \
    if (NULL == (p)) \
    { \
        PRINTF("%s[%d] Ptr is NULL\n", __FUNCTION__, __LINE__);\
        return ;\
    } \
}while (0)
#endif /* SYS_CHECK_PTR_VOID */

/** 向上对齐指定字节数 */
#define    BYTES_ALIGN(p, alignbyte)    (((ULONG)(p) + (ULONG)((alignbyte) - 1)) &~ ((alignbyte) - 1))


#ifndef ARRAY_SIZE
/** 计算数组大小 */
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))
#endif /* ARRAY_SIZE */

#ifndef ROUND_UP
/** 向上对齐 */
#define ROUND_UP(x, align)       (((int)(x) + (align - 1)) & ~(align - 1))
#endif /* ROUND_UP */

#ifndef ROUND_DOWN
/** 向下对齐 */
#define ROUND_DOWN(x, align)    ((int)(x) & ~(align - 1))
#endif /* ROUND_DOWN */

#ifndef SYS_BUF_LEN_UPDATE
/** 计算循环读缓冲区中的len1和len2的宏，降低代码量 */
#define SYS_BUF_LEN_UPDATE(uLen1, uLen2, uRIdx, uWIdx, uBufSize) \
do\
{\
    if(uWIdx >= uRIdx)\
    {\
        uLen1 = uWIdx - uRIdx;\
        uLen2 = 0;\
    }\
    else\
    {\
        uLen1 = uBufSize - uRIdx;\
        uLen2 = uWIdx;\
    }\
}while(0)
#endif /* SYS_BUF_LEN_UPDATE */

#ifndef SYS_BUF_WRITE_LEN_UPDATE
/** 计算循环写缓冲区中的len1和len2的宏，降低代码量 */
#define SYS_BUF_WRITE_LEN_UPDATE(uLen1, uLen2, uRIdx, uWIdx, uBufSize) \
do\
{\
    if(uWIdx >= uRIdx)\
    {\
        uLen1 = uBufSize - uWIdx;\
        uLen2 = uRIdx;\
    }\
    else\
    {\
        uLen1 = uRIdx - uWIdx;\
        uLen2 = 0;\
    }\
}while(0)
#endif /* SYS_BUF_WRITE_LEN_UPDATE */

/**
 * is_power_of_2() - check if a value is a power of two
 * @n: the value to check
 *
 * Determine whether some value is a power of two, where zero is
 * *not* considered a power of two.
 * Return: true if @n is a power of 2, otherwise false.
 */
static inline __attribute__((const))
bool is_power_of_2(ULONG n)
{
    return (n != 0 && ((n & (n - 1)) == 0));
}

/** 向上对齐 */
#define roundup(x, y) (                 \
{                           \
    const typeof(y) __y = y;            \
    (((x) + (__y - 1)) / __y) * __y;        \
}                           \
)
/** 向下对齐 */
#define rounddown(x, y) (               \
{                           \
    typeof(x) __x = (x);                \
    __x - (__x % (y));              \
}                           \
)

/**
 * container_of - return the start address of struct type, while ptr is the
 * member of struct type.
 */
#ifndef container_of
#define container_of(ptr, type, member) \
    ((type *)((CHAR *)(ptr) - (ULONG)(&((type *)0)->member)))
#endif /* container_of */

/** gcc: https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-alias-function-attribute */
#ifndef __alias
#define __alias(symbol)                 __attribute__((__alias__(#symbol)))
#endif /* __alias */

/**
 *   gcc: https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-aligned-function-attribute
 *   gcc: https://gcc.gnu.org/onlinedocs/gcc/Common-Type-Attributes.html#index-aligned-type-attribute
 *   gcc: https://gcc.gnu.org/onlinedocs/gcc/Common-Variable-Attributes.html#index-aligned-variable-attribute
 */
#ifndef __aligned
#define __aligned(x)                    __attribute__((__aligned__(x)))
#endif /* __aligned */

/**
 * Note: users of __always_inline currently do not write "inline" themselves,
 * which seems to be required by gcc to apply the attribute according
 * to its docs (and also "warning: always_inline function might not be
 * inlinable [-Wattributes]" is emitted).
 *
 *   gcc: https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-always_005finline-function-attribute
 * clang: mentioned
 */
#ifndef __always_inline
#define __always_inline                 inline __attribute__((__always_inline__))
#endif /* __always_inline */

/**
 *   gcc: https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-gnu_005finline-function-attribute
 * clang: https://clang.llvm.org/docs/AttributeReference.html#gnu-inline
 */
#ifndef __gnu_inline
#define __gnu_inline                    __attribute__((__gnu_inline__))
#endif /* __gnu_inline */

/**
 *   gcc: https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-noreturn-function-attribute
 * clang: https://clang.llvm.org/docs/AttributeReference.html#noreturn
 * clang: https://clang.llvm.org/docs/AttributeReference.html#id1
 */
#ifndef __noreturn
#define __noreturn                      __attribute__((__noreturn__))
#endif /* __noreturn */

/**
 *   gcc: https://gcc.gnu.org/onlinedocs/gcc/Common-Type-Attributes.html#index-packed-type-attribute
 * clang: https://gcc.gnu.org/onlinedocs/gcc/Common-Variable-Attributes.html#index-packed-variable-attribute
 */
#ifndef __packed
#define __packed                        __attribute__((__packed__))
#endif /* __packed */

/**
 *   gcc: https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-section-function-attribute
 *   gcc: https://gcc.gnu.org/onlinedocs/gcc/Common-Variable-Attributes.html#index-section-variable-attribute
 * clang: https://clang.llvm.org/docs/AttributeReference.html#section-declspec-allocate
 */
#ifndef __section
#define __section(section)              __attribute__((__section__(section)))
#endif /* __section */

/**
 *   gcc: https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-unused-function-attribute
 *   gcc: https://gcc.gnu.org/onlinedocs/gcc/Common-Type-Attributes.html#index-unused-type-attribute
 *   gcc: https://gcc.gnu.org/onlinedocs/gcc/Common-Variable-Attributes.html#index-unused-variable-attribute
 *   gcc: https://gcc.gnu.org/onlinedocs/gcc/Label-Attributes.html#index-unused-label-attribute
 * clang: https://clang.llvm.org/docs/AttributeReference.html#maybe-unused-unused
 */
#ifndef __always_unused 
#define __always_unused                 __attribute__((__unused__))
#endif /* __always_unused */

/**
 *   gcc: https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-used-function-attribute
 *   gcc: https://gcc.gnu.org/onlinedocs/gcc/Common-Variable-Attributes.html#index-used-variable-attribute
 */
#ifndef __maybe_unused
#define __maybe_unused                  __attribute__((__unused__))
#endif /* __maybe_unused */

/**
 *   gcc: https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-used-function-attribute
 *   gcc: https://gcc.gnu.org/onlinedocs/gcc/Common-Variable-Attributes.html#index-used-variable-attribute
 */
#ifndef __used 
#define __used                          __attribute__((__used__))
#endif /* __used */

/**
 *   gcc: https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-weak-function-attribute
 *   gcc: https://gcc.gnu.org/onlinedocs/gcc/Common-Variable-Attributes.html#index-weak-variable-attribute
 */
#ifndef __weak
#define __weak                          __attribute__((__weak__))
#endif /* __weak */

/**@enum   SYS_ENDIAN_ORDER_E 
 * @brief  字节序枚举定义
 */
typedef enum
{
    SYS_ENDIAN_ORDER_LITTLE,
    SYS_ENDIAN_ORDER_BIG,
} SYS_ENDIAN_ORDER_E;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * @brief      对一块内存空间进行字节和校验
 * @param[in]  pBuf             内存空间指针
 * @param[in]  iLen             长度
 * @return     成功返回校验值
 */
UINT32 sys_check_sum(VOID *pBuf, INT32 iLen);

/**
 * @brief      将本地序转换成网络序
 * @param[in]  uX  待转换数值
 * @return     转换后的数值
 * @note 如果平台是大端序，内部不做任何转换
 */
UINT32 sys_htonl(UINT32 uX);

/**
 * @brief      将网络序转换成本地序
 * @param[in]  uX  待转换数值
 * @return     转换后的数值
 * @note       如果平台是大端序，内部不做任何转换
 */
UINT32 sys_ntohl(UINT32 uX);

/**
 * @brief      将本地序转换成网络序
 * @param[in]  uX  待转换数值
 * @return     转换后的数值
 * @note       如果平台是大端序，内部不做任何转换
 */
UINT16 sys_htons(UINT16 uX);

/**
 * @brief     将网络序转换成本地序
 * @param[in] uX  待转换数值
 * @return    转换后的数值
 * @note      如果平台是大端序，内部不做任何转换
 */
UINT16 sys_ntohs(UINT16 uX);

/**
 * @brief     将网络序转换成本地序
 * @param[in] uX  待转换数值
 * @return    转换后的数值
 * @note      如果平台是大端序，内部不做任何转换
 */
UINT64 sys_ntohll(UINT64 uX);

/**
 * @brief     将本地序转换成网络序
 * @param[in] uX  待转换数值
 * @return    转换后的数值
 * @note      如果平台是大端序，内部不做任何转换
 */
UINT64 sys_htonll(UINT64 uX);

/**
 * @brief     设置产生一系列伪随机数发生器的起始点，产生随机数
 * @param[in] uSeed  随机数产生器的初始值
 * @return    无
 */
VOID sys_srand(UINT32 uSeed);

/**
 * @brief     获取随机数
 * @return    返回随机数
 */
INT32 sys_rand(VOID);

/**@fn        sys_get_endian_order
 * @brief     获取大小端
 * @return    返回大小端枚举定义SYS_ENDIAN_ORDER_E
 */
INT32 sys_get_endian_order(VOID);

/**
 * @brief     返回间隔约为1.5-2.0的质数，用于调整偏好质数大小的数据结构。
 *            间隔最近的质数函数返回下一个最大的质数，或者它所知道的关于MAXINT/4的最大值
 * @param     uNumber   最近间隔数
 * @return    下一个最大素数
 */
UINT32 sys_spaced_primes_closest(UINT32 uNumber);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_UTILS_INTERFACE_H */

/**@}*/
/**@}*/

