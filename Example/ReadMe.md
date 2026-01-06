# 学习过程

## 参考
https://blog.csdn.net/2401_84815887/article/details/152910053

步骤 2：从 MppPacket 中提取 SPS/PPS
MPP 通常会在第一帧前或 IDR 帧前附带 SPS/PPS。你需要解析 NAL 类型：

c

编辑



// 判断 NAL 类型（H.264）
#define NAL_TYPE(ptr) ((ptr)[0] & 0x1F)

void parse_sps_pps_from_mpp_packet(MppPacket packet, uint8_t **sps, int *sps_len,
                                   uint8_t **pps, int *pps_len) {
    void *data = mpp_packet_get_data(packet);
    size_t size = mpp_packet_get_size(packet);

    uint8_t *p = (uint8_t *)data;
    while (p < (uint8_t*)data + size) {
        // 查找 0x00000001 或 0x000001 起始码
        if (p[0] == 0 && p[1] == 0 && p[2] == 0 && p[3] == 1) {
            p += 4;
        } else if (p[0] == 0 && p[1] == 0 && p[2] == 1) {
            p += 3;
        } else {
            p++;
            continue;
        }

        int nal_type = NAL_TYPE(p);
        uint8_t *nal_start = p;
        // 找下一个起始码或结尾
        uint8_t *next = find_next_start_code(nal_start, (uint8_t*)data + size);
        int nal_size = next ? (next - nal_start) : ((uint8_t*)data + size - nal_start);

        if (nal_type == 7 && sps && !*sps) { // SPS
            *sps = malloc(nal_size);
            memcpy(*sps, nal_start, nal_size);
            *sps_len = nal_size;
        } else if (nal_type == 8 && pps && !*pps) { // PPS
            *pps = malloc(nal_size);
            memcpy(*pps, nal_start, nal_size);
            *pps_len = nal_size;
        }

        p = next ? next : (uint8_t*)data + size;
    }
}
💡 注意：RK3566 的 MPP 默认输出 Annex-B 格式（带起始码 0x00000001），而 MP4 要求 AVCC 格式（长度前缀）。写入 MP4 时需转换。

步骤 3：初始化 MP4 文件（使用 mp4v2）
c

编辑



#include <mp4v2/mp4v2.h>

MP4FileHandle mp4_file;
MP4TrackId video_track;

// 假设已获取 sps, pps
MP4FileHandle create_mp4(const char* filename, uint8_t* sps, int sps_len,
                         uint8_t* pps, int pps_len, int width, int height, int fps) {
    MP4FileHandle hFile = MP4Create(filename, 0);
    if (hFile == MP4_INVALID_FILE_HANDLE) return NULL;

    video_track = MP4AddH264VideoTrack(hFile,
        MP4_MSECS_TIME_SCALE / fps,          // timeScale
        MP4_MSECS_TIME_SCALE / fps,          // sampleDuration
        width, height,
        sps[1],  // profile
        sps[2],  // compat
        sps[3],  // level
        3,       // sampleLenFieldSizeMinusOne (表示长度用 4 字节)
        sps, sps_len,
        pps, pps_len);

    MP4SetVideoProfileLevel(hFile, 0x01); // baseline
    return hFile;
}
✅ MP4AddH264VideoTrack 会自动构建 avcC box。

步骤 4：将 MppPacket 转换为 AVCC 格式并写入
MP4 要求每个 NAL 前加 4 字节大端长度，而不是起始码。

c

编辑



int write_mpp_packet_to_mp4(MP4FileHandle hFile, MP4TrackId track,
                            MppPacket packet, uint64_t pts_ms) {
    uint8_t *input = (uint8_t *)mpp_packet_get_data(packet);
    size_t input_size = mpp_packet_get_size(packet);

    // 输出 buffer（AVCC 格式）
    uint8_t *output = malloc(input_size + 1024); // 略大一点
    uint8_t *out_ptr = output;

    uint8_t *p = input;
    while (p < input + input_size) {
        // 跳过起始码
        if (p[0] == 0 && p[1] == 0 && p[2] == 0 && p[3] == 1) {
            p += 4;
        } else if (p[0] == 0 && p[1] == 0 && p[2] == 1) {
            p += 3;
        } else {
            p++;
            continue;
        }

        uint8_t *nal_start = p;
        uint8_t *next = find_next_start_code(nal_start, input + input_size);
        int nal_len = next ? (next - nal_start) : (input + input_size - nal_start);

        // 写入 4 字节长度（大端）
        uint32_t len_be = __builtin_bswap32(nal_len); // 或手动转
        memcpy(out_ptr, &len_be, 4);
        out_ptr += 4;
        memcpy(out_ptr, nal_start, nal_len);
        out_ptr += nal_len;

        p = next ? next : (input + input_size);
    }

    // 写入 MP4
    MP4WriteSample(hFile, track, output, out_ptr - output,
                   MP4_INVALID_DURATION, // duration
                   pts_ms * 1000,        // PTS in microseconds
                   0);                   // isSyncSample (IDR 帧应为 true)

    free(output);
    return 0;
}
🔔 关键：IDR 帧需设置 isSyncSample = 1，否则播放器可能无法 seek。

步骤 5：主循环示例
c

编辑



bool first_frame = true;
uint8_t *sps = NULL, *pps = NULL;
int sps_len = 0, pps_len = 0;
MP4FileHandle mp4 = NULL;

while (get_encoded_packet(&packet)) {
    if (first_frame) {
        parse_sps_pps_from_mpp_packet(packet, &sps, &sps_len, &pps, &pps_len);
        mp4 = create_mp4("output.mp4", sps, sps_len, pps, pps_len, 1920, 1080, 30);
        first_frame = false;
    }

    uint64_t pts = get_current_pts_ms(); // 你的时间戳逻辑
    write_mpp_packet_to_mp4(mp4, video_track, packet, pts);

    mpp_packet_deinit(&packet);
}

// 清理
MP4Close(mp4);
free(sps); free(pps);
五、注意事项
时间戳单位：mp4v2 使用 微秒（μs） 作为 PTS/DTS 单位。
帧类型判断：可通过 NAL 类型（如 5 = IDR）设置 isSyncSample。
内存对齐：确保 MppPacket 数据可读（某些平台需 mmap 或 cache flush）。
H.265 支持：mp4v2 也支持 HEVC，使用 MP4AddH265VideoTrack，但需提取 VPS/SPS/PPS。