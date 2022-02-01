#include "output.h"

int init_output(MusicHandle* handle) {
    if (!handle) return FFMPEG_CORE_ERR_NULLPTR;
    if (!handle->sdl_initialized) {
        SDL_InitSubSystem(SDL_INIT_AUDIO);
        handle->sdl_initialized = 1;
    }
    SDL_AudioSpec sdl_spec;
    sdl_spec.freq = handle->decoder->sample_rate;
    sdl_spec.format = convert_to_sdl_format(handle->decoder->sample_fmt);
    if (!sdl_spec.format) {
        return FFMPEG_CORE_ERR_UNKNOWN_SAMPLE_FMT;
    }
    sdl_spec.channels = handle->decoder->channels;
    sdl_spec.samples = handle->decoder->sample_rate / 100;
    sdl_spec.callback = SDL_callback;
    sdl_spec.userdata = handle;
    memcpy(&handle->sdl_spec, &sdl_spec, sizeof(SDL_AudioSpec));
    handle->device_id = SDL_OpenAudioDevice(NULL, 0, &sdl_spec, &handle->sdl_spec, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
    if (!handle->device_id) {
        return FFMPEG_CORE_ERR_SDL;
    }
    enum AVSampleFormat target_format = convert_to_sdl_supported_format(handle->decoder->sample_fmt);
    handle->swrac = swr_alloc_set_opts(NULL, handle->decoder->channel_layout, target_format, handle->sdl_spec.freq, handle->decoder->channel_layout, handle->decoder->sample_fmt, handle->decoder->sample_rate, 0, NULL);
    if (!handle->swrac) {
        return FFMPEG_CORE_ERR_OOM;
    }
    int re = 0;
    if ((re = swr_init(handle->swrac)) < 0) {
        return re;
    }
    if (!(handle->buffer = av_audio_fifo_alloc(target_format, handle->decoder->channels, 1))) {
        return FFMPEG_CORE_ERR_OOM;
    }
    handle->target_format = target_format;
    handle->target_format_pbytes = av_get_bytes_per_sample(target_format);
    return FFMPEG_CORE_ERR_OK;
}

enum AVSampleFormat convert_to_sdl_supported_format(enum AVSampleFormat fmt) {
    switch (fmt) {
        case AV_SAMPLE_FMT_DBL:
        case AV_SAMPLE_FMT_FLTP:
        case AV_SAMPLE_FMT_DBLP:
            return AV_SAMPLE_FMT_FLT;
        case AV_SAMPLE_FMT_U8P:
            return AV_SAMPLE_FMT_U8;
        case AV_SAMPLE_FMT_S16P:
            return AV_SAMPLE_FMT_S16;
        case AV_SAMPLE_FMT_S32P:
        case AV_SAMPLE_FMT_S64:
        case AV_SAMPLE_FMT_S64P:
            return AV_SAMPLE_FMT_S32;
        default:
            return fmt;
    }
}

SDL_AudioFormat convert_to_sdl_format(enum AVSampleFormat fmt) {
    fmt = convert_to_sdl_supported_format(fmt);
    switch (fmt) {
        case AV_SAMPLE_FMT_U8:
            return AUDIO_U8;
        case AV_SAMPLE_FMT_S16:
            return AUDIO_S16SYS;
        case AV_SAMPLE_FMT_S32:
            return AUDIO_S32SYS;
        case AV_SAMPLE_FMT_FLT:
            return AUDIO_F32SYS;
        default:
            return 0;
    }
}

void SDL_callback(void* userdata, uint8_t* stream, int len) {
    MusicHandle* handle = (MusicHandle*)userdata;
    DWORD re = WaitForSingleObject(handle->mutex, 10);
    if (re != WAIT_OBJECT_0) {
        // 无法获取Mutex所有权，填充空白数据
        memset(stream, 0, sizeof(len));
        return;
    }
    int samples_need = len / handle->target_format_pbytes / handle->sdl_spec.channels;
    if (av_audio_fifo_size(handle->buffer) == 0) {
        // 缓冲区没有数据，填充空白数据
        memset(stream, 0, sizeof(len));
    } else {
        int writed = av_audio_fifo_read(handle->buffer, (void**)&stream, samples_need);
        if (writed > 0) {
            // 增大缓冲区开始时间
            AVRational base = { 1, handle->sdl_spec.freq };
            handle->pts += av_rescale_q_rnd(writed, base, AV_TIME_BASE_Q, AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
        }
        if (writed < 0) {
            // 读取发生错误，填充空白数据
            memset(stream, 0, sizeof(len));
        } else if (writed < samples_need) {
            // 不足的区域用空白数据填充
            memset(stream + (size_t)writed * handle->target_format_pbytes, 0, (((size_t)samples_need - writed) * handle->target_format_pbytes));
        }
    }
    ReleaseMutex(handle->mutex);
}
