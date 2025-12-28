#include <arpa/inet.h>
#include <stdarg.h>
#include "sys_utils.h"

/**@fn         sys_check_sum      
 * @brief      对一块内存空间进行字节和校验
 * @param[in]  pBuf             内存空间指针
 * @param[in]  iLen             长度
 * @return     成功返回校验和  
 */
UINT32 sys_check_sum(VOID *pBuf, INT32 iLen)
{
    UINT32 uSum = 0;
    INT32 iIndex = 0;
    char *pTmp = (char *)pBuf;
    for (iIndex = 0; iIndex < iLen; iIndex++)
    {
        uSum += (int)pTmp[iIndex] & 0xff;
    }
    return uSum;
}

/**@fn         sys_htonl
 * @brief      将本地序转换成网络序
 * @param[in]  x  待转换数值
 * @return     转换后的数值
 * @note 如果平台是大端序，内部不做任何转换
 */
UINT32 sys_htonl(UINT32 x)
{
    return htonl(x);
}

/**@fn         sys_ntohl
 * @brief      将网络序转换成本地序
 * @param[in]  x  待转换数值
 * @return     转换后的数值
 * @note       如果平台是大端序，内部不做任何转换
 */
UINT32 sys_ntohl(UINT32 x)
{
  return ntohl(x);
}

/**@fn         sys_htons
 * @brief      将本地序转换成网络序
 * @param[in]  x  待转换数值
 * @return     转换后的数值
 * @note       如果平台是大端序，内部不做任何转换
 */
UINT16 sys_htons(UINT16 x)
{
    return htons(x);
}

/**@fn        sys_ntohs
 * @brief     将网络序转换成本地序
 * @param[in] x  待转换数值
 * @return    转换后的数值
 * @note      如果平台是大端序，内部不做任何转换
 */
UINT16 sys_ntohs(UINT16 x)
{
    return ntohs(x);
}

/**@fn        sys_ntohll
 * @brief     将网络序转换成本地序
 * @param[in] x  待转换数值
 * @return    转换后的数值
 * @note      如果平台是大端序，内部不做任何转换
 */
UINT64 sys_ntohll(UINT64 x)
{
    return (((UINT64)sys_ntohl((UINT32)((x << 32) >> 32) )) << 32) | (UINT32)sys_ntohl ((UINT32)(x >> 32));
}

/**@fn        sys_htonll
 * @brief     将本地序转换成网络序
 * @param[in] x  待转换数值
 * @return    转换后的数值
 * @note      如果平台是大端序，内部不做任何转换
 */
UINT64 sys_htonll(UINT64 x)
{
    return (((UINT64)sys_htonl((UINT32)((x << 32) >> 32) )) << 32) | (UINT32)sys_htonl ((UINT32)(x >> 32));
}

/**@fn        sys_srand
 * @brief     设置产生一系列伪随机数发生器的起始点，要想把发生器重新初始化
 * @param[in] uSeed  随机数产生器的初始值
 * @return    无
 */
VOID sys_srand(UINT32 uSeed)
{
    srand(uSeed);
}
/**@fn        sys_rand
 * @brief     获取随机数
 * @param[in] 无
 * @return    返回随机数值
 */
INT32 sys_rand(VOID)
{
   return rand();
}
/**@fn        
 * @brief     获取本机字节序
 * @param[in] 无
 * @return    返回字节序枚举
 */
INT32 sys_get_endian_order(VOID)
{
    CHAR *strProbe;
    INT32 iTest;
    iTest = 1;
    strProbe = (char*) &iTest;
    return (*strProbe == 1) ? SYS_ENDIAN_ORDER_LITTLE : SYS_ENDIAN_ORDER_BIG;
}

static char* auto_string(char *str)
{
    static char *saved[4];
    static uint8_t cur_saved; /* = 0 */

    free(saved[cur_saved]);
    saved[cur_saved] = str;
    cur_saved = (cur_saved + 1) & (ARRAY_SIZE(saved)-1);

    return str;
}

static int vasprintf(char **string_ptr, const char *format, va_list p)
{
    int r;
    va_list p2;
    char buf[128];

    va_copy(p2, p);
    r = vsnprintf(buf, 128, format, p);
    va_end(p);

    /* Note: can't use xstrdup/xmalloc, they call vasprintf (us) on failure! */

    if (r < 128) {
        va_end(p2);
        *string_ptr = strdup(buf);
        return (*string_ptr ? r : -1);
    }

    *string_ptr = malloc(r+1);
    r = (*string_ptr ? vsnprintf(*string_ptr, r+1, format, p2) : -1);
    va_end(p2);

    return r;
}

static char* xasprintf(const char *format, ...)
{
    va_list p;
    char *string_ptr;

    va_start(p, format);
    vasprintf(&string_ptr, format, p);
    va_end(p);

    return string_ptr;
}

const char* sys_make_human_readable_str(uintptr_t size)
{   
    uint32_t frac = 0u;
    const char *u;
    const char *fmt;
    static const char unit_chars[] = {
        '\0', 'K', 'M', 'G', 'T',
    };
        
    u = unit_chars;

    fmt = "%llu";
    
    while ((size >= 1024)
     /* && (u < unit_chars + sizeof(unit_chars) - 1) - always true */
    ) {
        fmt = "%llu.%u%c";
        u++;
        frac = (((unsigned)size % 1024) * 10 + 1024/2) / 1024;
        size /= 1024;
    }
    if (frac >= 10) { /* we need to round up here */
        ++size;
        frac = 0;
    }
    
    return auto_string(xasprintf(fmt, size, frac, *u));
}

