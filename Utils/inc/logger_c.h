/* logger_c.h */
#ifndef LOGGER_C_H
#define LOGGER_C_H

#ifdef __cplusplus
extern "C" {
#endif

/* 日志级别（与 C++ 枚举值一致） */
typedef enum {
    LOG_LEVEL_ERROR   = 0,
    LOG_LEVEL_WARNING = 1,
    LOG_LEVEL_INFO    = 2,
    LOG_LEVEL_DEBUG   = 3
} LOGLEVEL_E;

LOGLEVEL_E _logger_get_level(void);

// 内部函数（带位置信息）
void _logger_write(LOGLEVEL_E level, const char* file, const char* func, int line, const char* format, ...);

// 宏自动注入 __FILE__, __LINE__, __FUNCTION__
#define log_error(format, ...) \
    do { \
        if (LOG_LEVEL_ERROR <= _logger_get_level()) \
            _logger_write(LOG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__); \
    } while(0)

#define log_warning(format, ...) \
    do { \
        if (LOG_LEVEL_WARNING <= _logger_get_level()) \
            _logger_write(LOG_LEVEL_WARNING, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__); \
    } while(0)

#define log_info(format, ...) \
    do { \
        if (LOG_LEVEL_INFO <= _logger_get_level()) \
            _logger_write(LOG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__); \
    } while(0)

#define log_debug(format, ...) \
    do { \
        if (LOG_LEVEL_DEBUG <= _logger_get_level()) \
            _logger_write(LOG_LEVEL_DEBUG, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__); \
    } while(0)


#ifdef __cplusplus
}
#endif

#endif // LOGGER_C_H

