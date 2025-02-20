# VideoPipoVision
该项目主要是通过C++的代码进行完整的RK的学习，主要是视频的接入，编码，以及yolo的目标检查。

该项目需要的环境：
* 一个泰山派
* 需要一个泰山派支持的摄像头 OV5695
* 一张TF卡
* 一台路由器
* 一台笔记本或者手机

2024年12月28日演示效果为：
目前仅仅只是用软件的libx264进行编码的，出现各种卡顿之类的问题。需要将所有的摄像头的数据转到硬件编码器进行编码，将YUV数据放到RKmpp的硬件驱动中去进行编码H264输出。

![](/docs/images/20241228.png)

2025年1月27日进展：
出现编码出来的数据各种异常，怀疑是自己的RKMPP没有使用正确。

![](/docs/images/20250127.png)

2025年2月3日：已经正常

![](/docs/images/20250203.png)

2025年2月21日 演示效果如下：
![](/docs/images/20250221.jpg)

在使用RKNPU处理后，需要先进行RGA处理，再送到npu出结果，再把结果在用rga进行画框处理，中间已经从多次进行图像处理，改成了仅一次处理，但到PC端还是延迟很大。
后续看看如何进行优化


## 功能介绍

1. 做RTSP的功能，通过RKMPP的H264的编码（完成）
2. 做yolo的识别（完成）
3. 设计时间戳做OSD（未完成）
4. 完成对于libdram的框架与MIPI屏幕的显示输出（未完成）
4. 做WIFI的直接连接与获取（未完成）
5. 手机App的连接用于配置与在线使用（未完成）
6. 编译框架加入Cmake框架

## 1.2 开发环境搭建
下载SDK即可，将VideoPipoVision放到SDK的目录下进行编译。



## 1.3 已知缺陷

在断联的时候会出现内存问题,目前还未解决，应该是开源架构中存在的问题。后续修正一下。

```
2017-08-04 09:52:05 <DEBUG> src/net/TcpConnection.cpp:handleRead:96 client disconnect
malloc(): unsorted double linked list corrupted
[ 3125.985170] rockchip-csi2-dphy csi2-dphy1: csi2_dphy_s_stream_stop stream stop, dphy1
[ 3125.985213] rockchip-csi2-dphy csi2-dphy1: csi2_dphy_s_stream stream on:0, dphy1
Aborted
```


## 参考

可以直接看飞书，是如何制作该项目的。

https://ri02m17fq7.feishu.cn/wiki/XZGgwa2m9i1er2kiidJcC1vunOd

开源项目：
1. https://github.com/ImSjt/RtspServer
2. https://github.com/mpromonet/libv4l2cpp
