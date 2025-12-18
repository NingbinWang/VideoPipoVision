#ifndef _APP_H_
#define _APP_H_


#include <string.h>
#include "rockchip/rk_mpi.h"
#include "rockchip/mpp_frame.h"
#include <string.h>
#include <pthread.h>
#include <memory>
#include "sys_common.h"
#include "Media.h"
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include "opencv2/opencv.hpp"
#include <iostream>
#include "Logger.h"


/**********************
 *      TYPEDEFS
 **********************/

 typedef enum {
    DISP_SMALL,
    DISP_MEDIUM,
    DISP_LARGE,
} disp_size_t;




int app_main(void);





#endif
