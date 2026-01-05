#ifdef USE_OPENCV
#include "opencv2/opencv.hpp"
int opencv_demo()
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
        std::cout << "Failed to open camera!" << std::endl;
        return -1;
    }
    while (true)
    {

        cv::Mat frame;
        cap.read(frame);
        if (frame.empty())
        {
            std::cout << "Failed to read frame from camera!" << std::endl;
            break;
        }
         std::cout << "read frame from camera!" << std::endl;
    }
    cap.release();
    return 0;
}
#endif
