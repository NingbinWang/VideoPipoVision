#include "Record.h"
#include "Media.h"
#include "MediaFrame.h"
#include "Thread.h"
#include "MediaManager.h"
#include "Logger.h"
#include <mp4v2/mp4v2.h>
/*
//   年月日     时分秒 自动计数值
文件格式:ch0-20260114-165306-0000001.mp4

*/

#include <iostream>

int RecordH264Mp4(const CHAR* strFilePath,PUINT8 pData,UINT32 u32Length)
{
 
    // 1. 创建 MP4 文件
    MP4FileHandle mp4File = MP4Create(strFilePath);
    if (mp4File == MP4_INVALID_FILE_HANDLE) {
        LOG_ERROR("Failed to create MP4 file.");
        return ERROR;
    }

    // 2. 设置时间尺度（单位：毫秒）
    MP4SetTimeScale(mp4File, 90000);

    // 3. 创建视频轨道（假设为 H.264 码流）
    const char* videoType = "avc1"; // H.264 类型
    MP4TrackId videoTrack = MP4AddH264VideoTrack(
        mp4File,
        90000,  // 时间尺度
        0,      // 默认样本时长（0 表示由写入数据决定）
        1920,   // 视频宽度
        1080,   // 视频高度
        nullptr, // AVCC 配置数据（需根据实际编码器设置）
        0       // AVCC 数据长度
    );

    if (videoTrack == MP4_INVALID_TRACK_ID) {
        LOG_ERROR("Failed to create video track.");
        MP4Close(mp4File);
        return ERROR;
    }

    // 4. 写入样本数据（示例：假设已有编码好的 H.264 数据）
    MP4Duration sampleDuration = 3000; // 样本时长（根据帧率计算，如 30fps 则约为 3000/90000 秒）
    MP4WriteSample(mp4File, videoTrack, pData, u32Length, sampleDuration);

    // 5. 关闭文件
    MP4Close(mp4File);
    LOG_INFO("MP4 file created successfully: %s\n",strFilePath);
    return 0;
}


