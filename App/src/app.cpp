#include "app.h"
#include  "Media_vi.h"
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include "logger.h"

#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int opencv_demo()
{
    // 创建VideoCapture对象，参数为0表示打开默认摄像头
    VideoCapture cap(1);

    // 检查摄像头是否成功打开
    if (!cap.isOpened())
    {
        cout << "Failed to open camera!" << endl;
        return -1;
    }

    // 循环读取摄像头捕捉到的帧并显示
    while (true)
    {
        // 读取一帧图像
        Mat frame;
        cap.read(frame);

        // 检查是否成功读取到一帧图像
        if (frame.empty())
        {
            cout << "Failed to read frame from camera!" << endl;
            break;
        }
         cout << "read frame from camera!" << endl;
    }
    // 释放摄像头资源和所有窗口
    cap.release();
    return 0;
}
int media_demo()
{
    VI_CFG_PARAM_T param;
    param.vSensorType = CMOS_OV_5969;
    param.image_viH = 1944;
    param.image_viW = 2592;
    param.frame_rate = 30;
    initLogger(2);
    Media_vi *vi = new Media_vi(param);
    vi->init();
    return 0;
}



int app_main(void)
{
  //  opencv_demo();
  media_demo();
    return 0;
}