#ifndef SYS_LOG_H
#define SYS_LOG_H
#include <string.h>
#include <errno.h>
#include "sys_time.h"
#include "sys_common.h"
#define NSEC2SEC (1000000 * 1000)
//#include "Logger.h"

//#define PR_ERR LOG_ERROR
//#define PR_WRN LOG_WARNING
//#define PR_DBG LOG_DEBUG
//#define PR_INF LOG_INFO


#define PR_ERR printf
#define PR_WRN printf
#define PR_DBG printf
#define PR_INF printf

#endif