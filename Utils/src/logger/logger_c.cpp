#include "Logger.h"      // 原始 C++ Logger 类
#include "logger_c.h"
#include <cstdarg>
#include <cstdio>


extern "C" void _logger_write(LOGLEVEL_E level, const char* file, const char* func, int line, const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[4096];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // 调用原始 C++ Logger（注意：原 Logger() 是临时对象，可能没效果！）
    // 建议：修改 Logger 类，让 write 成为 static 函数
    Logger().write(static_cast<Logger::LogLevel>(level), file, func, line, "%s", buffer);
}

extern "C" LOGLEVEL_E _logger_get_level(void) {
    return static_cast<LOGLEVEL_E>(Logger::getLogLevel());
}



