#include "open.h"

int open_input(MusicHandle* handle, const char* url) {
    if (!handle || !url) return FFMPEG_CORE_ERR_NULLPTR;
    int re = 0;
    if ((re = avformat_open_input(&handle->fmt, url, NULL, NULL)) < 0) {
        return re;
    }
    if ((re = avformat_find_stream_info(handle->fmt, NULL)) < 0) {
        return re;
    }
    return FFMPEG_CORE_ERR_OK;
}

int find_audio_stream(MusicHandle* handle) {
    if (!handle || !handle->fmt) return FFMPEG_CORE_ERR_NULLPTR;
    for (unsigned int i = 0; i < handle->fmt->nb_streams; i++) {
        AVStream* is = handle->fmt->streams[i];
        if (is->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            // 确保有对应的解码器
            if (!avcodec_find_decoder(is->codecpar->codec_id)) {
                continue;
            }
            handle->is = is;
            return FFMPEG_CORE_ERR_OK;
        }
    }
    return FFMPEG_CORE_ERR_NO_AUDIO_OR_DECODER;
}
