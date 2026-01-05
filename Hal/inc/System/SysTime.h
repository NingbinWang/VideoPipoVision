#ifndef _SYS_TIME_H_
#define _SYS_TIME_H_
/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Common.h"

/* ========================================================================== */
/*                             Type Declaration                               */
/* ========================================================================== */

/**@struct SYS_DATE_TIME_T 
 * @brief  日历时间结构,包括年月日时分秒星期
 */
typedef struct {
    UINT16      uYear;                       /**< 1970 ~ 2038 */
    UINT16      uMonth;                      /**< 1 ~ 12 */
    UINT16      uDay;                        /**< 1 ~ 31 */
    UINT16      uWeek;                       /**< 0-SUNDAY, 1-MONDAY, ..., 6-WEEK_SATURDAY */
    UINT16      uHour;                       /**< 0 ~ 23 */
    UINT16      uMinute;                     /**< 0 ~ 59 */
    UINT16      uSec;                        /**< 0 ~ 59 */
    UINT16      uMilleSecond;                /**< 毫秒 */
}SYS_DATE_TIME_T;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * @brief      获取系统启动后运行的毫秒数
 * @param[out] pMSecond 毫秒数指针
 * @return     成功返回 OK 错误返回 ERROR, 支持通过get_last_errno获取错误码
 */
INT32 SysTime_get_msec(UINT64 *pMSecond);

/**
 * @brief      获取系统启动后运行的秒数
 * @param[out] pSecond 秒数指针
 * @return     成功返回 OK 错误返回 ERROR, 支持通过get_last_errno获取错误码
 */
INT32 SysTime_get_sec(UINT32 *pSecond);

/**
 * @brief      获取系统当前从1970-1-1 00:00:00的绝对时间
 * @param[out] pSecond 秒数指针
 * @return     成功返回 OK, 整数值 错误返回 ERROR, 支持通过get_last_errno获取错误码
 */
INT32 SysTime_get_utc_sec(UINT32 *pSecond);

/**
 * @brief      获取系统UTC时间
 * @param[out] pUtcMSec UTC时间毫秒
 * @return     成功返回 OK  错误返回 ERROR, 支持通过get_last_errno获取错误码
 */
INT32 SysTime_get_utc_msec(UINT64 *pUtcMSec);

/**
 * @brief      时间设置
 * @param[in]  pStDateTime 时间结构体指针
 * @return     成功返回 OK 错误返回 ERROR, 支持通过get_last_errno获取错误码
 */
INT32 SysTime_set_in_struct(const SYS_DATE_TIME_T *pStDateTime);

/**
 * @brief      获取系统日期和时间
 * @param[out] pStDateTime  结构体指针
 * @return     成功返回 OK 错误返回 ERROR, 支持通过get_last_errno获取错误码
 */
INT32 SysTime_get_in_struct(SYS_DATE_TIME_T *pStDateTime);

/**
 * @brief      转换日期时间为从1970年1月1日0点到现在的秒数
 * @param[in]  pStDateTime 时间结构体指针
 * @param[out] pUtcTime     utc时间结构体指针
 * @return     成功返回 OK 错误返回 ERROR, 支持通过get_last_errno获取错误码
 */
INT32 SysTime_convert_struct_to_value(const SYS_DATE_TIME_T *pStDateTime,UINT32 *pUtcTime);


/**
 * @brief      将从1970年1月1日0点到现在的秒转化成datetime结构
 * @param[in]  uUtcTime      1970年1月1日0点到现在的秒数
 * @param[in]  pStDateTime   utc时间结构体指针年月日时分秒
 * @return     成功返回 OK   错误返回 ERROR, 支持通过get_last_errno获取错误码
 */
INT32 SysTime_convert_value_to_struct(const UINT32 uUtcTime, SYS_DATE_TIME_T *pStDateTime);

/**
 * @brief      设置系统当前绝对时间
 * @param[in]  uUtcSec       UTC时间ms
 * @return     成功返回 ok  错误返回 ERROR, 支持通过get_last_errno获取错误码
 */
INT32 SysTime_set_utc_msec(UINT64 uUtcMSec);

/**
 * @brief      将时间值转化为字符串
 * @param[in]  uTime   绝对时间，从1970年开始计算的秒数
 * @param[out] strData 存储时间的字符串
 * @param[in]  uSize   存储时间的字符串长度
 * @return     成功返回OK，失败返回ERROR
 */
INT32 SysTime_convert_value_to_str(UINT32 uTime, CHAR *strData, UINT32 uSize);

/**
 * @brief      将字符串转化为时间值
 * @param[in]  strData 存储时间的字符串
 * @param[out] pTime 绝对时间，从1970年开始计算的秒数
 * @return     成功返回OK，失败返回ERROR
 */
INT32 SysTime_convert_str_to_value(const CHAR *strData, UINT32 *pTime);

/**
 * @brief      将字符串转化为结构体
 * @param[in]  strData 存储时间的字符串
 * @param[out] pStDateTime 结构体指针
 * @return     成功返回OK，失败返回ERROR
 */
INT32 SysTime_convert_str_to_datetime(const CHAR *strData, SYS_DATE_TIME_T *pStDateTime);

/**
 * @brief      将SYS_DATE_TIME_T结构日期转换为字符串(形如2018-08-22 19:28:20)
 * @param[in]  pStDateTime  SYS_DATE_TIME_T结构
 * @param[out] strData      存储时间的字符串
 * @param[in]  uBufLen      字符串长度
 * @return     成功返回OK，失败返回ERROR
 */    
INT32 SysTime_convert_struct_to_str(const SYS_DATE_TIME_T *pStDateTime, CHAR *strData, UINT16 uBufLen);

/**
 * @brief      时间字符串格式化
 * @param[in]  pStDateTime   时间结构体指针
 * @param[in]  strFromat     格式化字符串
 * @param[out] strBuf        输出字符串缓冲区
 * @param[in]  uBufSize      缓冲区大小
 * @return     成功返回OK，失败返回ERROR
 */    
INT32 SysTime_str_format_time(const SYS_DATE_TIME_T *pStDateTime, const CHAR *strFromat, CHAR *strBuf, UINT16 uBufSize);

/**
 * @brief      sleep秒
 * @param[in]  uSecond  SYS_DATE_TIME_T结构
 * @return     成功返回OK，失败返回ERROR
 */    
INT32 SysTime_sleep_second(UINT32 uSecond);

/**
 * @brief      sleep毫秒
 * @param[in]  uMillSecond  SYS_DATE_TIME_T结构
 * @return     成功返回OK，失败返回ERROR
 */    
INT32 SysTime_sleep_ms(UINT64 uMillSecond);

/**
 * @brief      sleep微秒
 * @param[in]  uMicroSecond 微妙
 * @return     成功返回OK，失败返回ERROR
 */    
INT32 SysTime_sleep_us(UINT32 uMicroSecond);

/**
 * @brief      获取系统时间随机粽子信息
 * @return     成功返回 种子值
 */
INT32 SysTime_rand_seed(VOID);



#endif/* SysTime_INTERFACE_H */