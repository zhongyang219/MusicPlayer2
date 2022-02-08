#include "open.h"

int open_input(MusicHandle* handle, const char* url) {
    if (!handle || !url) return FFMPEG_CORE_ERR_NULLPTR;
    int re = 0;
    if ((re = avformat_open_input(&handle->fmt, url, NULL, NULL)) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Failed to open \"%s\": %s (%i)\n", url, av_err2str(re), re);
        return re;
    }
    if ((re = avformat_find_stream_info(handle->fmt, NULL)) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Failed to find streams in \"%s\": %s (%i)\n", url, av_err2str(re), re);
        return re;
    }
    // handle->fmt->flags |= AVFMT_FLAG_FAST_SEEK;  // 允许快速定位
    return FFMPEG_CORE_ERR_OK;
}

int open_input2(MusicInfoHandle* handle, const char* url) {
    if (!handle || !url) return FFMPEG_CORE_ERR_NULLPTR;
    int re = 0;
    if ((re = avformat_open_input(&handle->fmt, url, NULL, NULL)) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Failed to open \"%s\": %s (%i)\n", url, av_err2str(re), re);
        return re;
    }
    if ((re = avformat_find_stream_info(handle->fmt, NULL)) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Failed to find streams in \"%s\": %s (%i)\n", url, av_err2str(re), re);
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

int find_audio_stream2(MusicInfoHandle* handle) {
    if (!handle || !handle->fmt) return FFMPEG_CORE_ERR_NULLPTR;
    for (unsigned int i = 0; i < handle->fmt->nb_streams; i++) {
        AVStream* is = handle->fmt->streams[i];
        if (is->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            handle->is = is;
            return FFMPEG_CORE_ERR_OK;
        }
    }
    return FFMPEG_CORE_ERR_NO_AUDIO;
}
