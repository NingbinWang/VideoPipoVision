#include <alsa/asoundlib.h>
#include <fdk-aac/aacenc_lib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PCM_SAMPLE_RATE   48000
#define PCM_CHANNELS      2
#define PCM_FORMAT        SND_PCM_FORMAT_S16_LE
#define AAC_BITRATE       128000  // 128 kbps

// 将 PCM 帧数转换为字节数
#define PCM_FRAMES 1024
#define PCM_BUFFER_SIZE (PCM_FRAMES * PCM_CHANNELS * sizeof(int16_t))

static HANDLE_AACENCODER aac_encoder = NULL;
static FILE *aac_file = NULL;

// 初始化 FDK-AAC 编码器
int init_aac_encoder() {
    if (aacEncOpen(&aac_encoder, 0, PCM_CHANNELS) != AACENC_OK) {
        fprintf(stderr, "Failed to open AAC encoder\n");
        return -1;
    }

    // 设置编码参数
    aacEncoder_SetParam(aac_encoder, AACENC_AOT, 2); // MPEG-4 AAC-LC
    aacEncoder_SetParam(aac_encoder, AACENC_SAMPLERATE, PCM_SAMPLE_RATE);
    aacEncoder_SetParam(aac_encoder, AACENC_CHANNELMODE, PCM_CHANNELS == 2 ? MODE_2 : MODE_1);
    aacEncoder_SetParam(aac_encoder, AACENC_BITRATE, AAC_BITRATE);
    aacEncoder_SetParam(aac_encoder, AACENC_TRANSMUX, TT_MP4_RAW); // 输出 raw AAC（无 ADTS）

    if (aacEncEncode(aac_encoder, NULL, NULL, NULL, NULL) != AACENC_OK) {
        fprintf(stderr, "Failed to initialize AAC encoder\n");
        return -1;
    }

    // 获取 AudioSpecificConfig（用于 MP4 的 esds box）
    UCHAR asc[64];
    UINT asc_size = sizeof(asc);
    if (aacEncGetAudioSpecificConfig(aac_encoder, asc, &asc_size) == AACENC_OK) {
        printf("AudioSpecificConfig (%d bytes): ", asc_size);
        for (int i = 0; i < asc_size; i++) printf("%02X ", asc[i]);
        printf("\n");
        // 可保存 asc 用于后续 MP4 封装
    }

    return 0;
}

// 编码一帧 PCM 数据
int encode_pcm_to_aac(int16_t *pcm_samples, int num_samples, uint8_t **out_data, int *out_len) {
    INT_PCM input_buffer[num_samples];
    for (int i = 0; i < num_samples; i++) {
        input_buffer[i] = pcm_samples[i];
    }

    AACENC_BufDesc in_buf = {0}, out_buf = {0};
    AACENC_InArgs in_args = {0};
    AACENC_OutArgs out_args = {0};

    // 输入配置
    void *in_ptr = input_buffer;
    INT in_sizes = num_samples * sizeof(INT_PCM);
    INT in_ident = IN_AUDIO_DATA;
    in_buf.numBufs = 1;
    in_buf.bufs = &in_ptr;
    in_buf.bufferIdentifiers = &in_ident;
    in_buf.bufSizes = &in_sizes;
    in_buf.bufElSizes = &in_sizes;

    // 输出缓冲区（需足够大）
    static uint8_t output_buffer[8192];
    void *out_ptr = output_buffer;
    INT out_sizes = sizeof(output_buffer);
    INT out_ident = OUT_BITSTREAM_DATA;
    out_buf.numBufs = 1;
    out_buf.bufs = &out_ptr;
    out_buf.bufferIdentifiers = &out_ident;
    out_buf.bufSizes = &out_sizes;
    out_buf.bufElSizes = &out_sizes;

    in_args.numInSamples = num_samples;

    AACENC_ERROR err = aacEncEncode(aac_encoder, &in_buf, &out_buf, &in_args, &out_args);
    if (err != AACENC_OK && err != AACENC_ENCODE_EOF) {
        fprintf(stderr, "Encoding error: %d\n", err);
        return -1;
    }

    if (out_args.numOutBytes > 0) {
        *out_data = output_buffer;
        *out_len = out_args.numOutBytes;
        return 0;
    }
    return -1;
}

// ALSA 录音主循环
int record_and_encode(const char* alsa_device, const char* aac_filename) {
    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *params;
    snd_pcm_uframes_t frames = PCM_FRAMES;

    // 打开 ALSA 设备
    if (snd_pcm_open(&pcm_handle, alsa_device, SND_PCM_STREAM_CAPTURE, 0) < 0) {
        fprintf(stderr, "Cannot open audio device %s\n", alsa_device);
        return -1;
    }

    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(pcm_handle, params);
    snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle, params, PCM_FORMAT);
    snd_pcm_hw_params_set_channels(pcm_handle, params, PCM_CHANNELS);
    snd_pcm_hw_params_set_rate_near(pcm_handle, params, &PCM_SAMPLE_RATE, 0);
    snd_pcm_hw_params_set_period_size_near(pcm_handle, params, &frames, 0);
    snd_pcm_hw_params(pcm_handle, params);

    snd_pcm_prepare(pcm_handle);

    // 打开输出文件（raw AAC）
    aac_file = fopen(aac_filename, "wb");
    if (!aac_file) {
        perror("fopen");
        return -1;
    }

    // 开始录音和编码
    int16_t *pcm_buffer = malloc(PCM_BUFFER_SIZE);
    uint8_t *aac_data;
    int aac_len;

    for (int i = 0; i < 500; i++) { // 录制约 10 秒（1024/48000 ≈ 21ms/帧）
        if (snd_pcm_readi(pcm_handle, pcm_buffer, frames) != frames) {
            snd_pcm_recover(pcm_handle, -EPIPE, 0);
            continue;
        }

        if (encode_pcm_to_aac(pcm_buffer, frames * PCM_CHANNELS, &aac_data, &aac_len) == 0) {
            fwrite(aac_data, 1, aac_len, aac_file);
        }
    }

    // flush encoder
    encode_pcm_to_aac(NULL, 0, &aac_data, &aac_len); // send EOF
    if (aac_len > 0) fwrite(aac_data, 1, aac_len, aac_file);

    // cleanup
    free(pcm_buffer);
    fclose(aac_file);
    snd_pcm_close(pcm_handle);
    aacEncClose(&aac_encoder);
    return 0;
}

int main() {
    if (init_aac_encoder() < 0) return -1;
    record_and_encode("default", "output.aac");
    return 0;
}

