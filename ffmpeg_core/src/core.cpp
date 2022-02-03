#include "core.h"

#include <malloc.h>
#include <string.h>
#include "cpp2c.h"
#include "wchar_util.h"
#include "open.h"
#include "output.h"
#include "loop.h"
#include "decode.h"
#include "filter.h"

#define CODEPAGE_SIZE 3

void free_music_handle(MusicHandle* handle) {
    if (!handle) return;
    if (handle->device_id) SDL_CloseAudioDevice(handle->device_id);
    if (handle->thread) {
        DWORD status;
        while (GetExitCodeThread(handle->thread, &status)) {
            if (status == STILL_ACTIVE) {
                status = 0;
                handle->stoping = 1;
                Sleep(10);
            } else {
                break;
            }
        }
    }
    if (handle->graph) {
        avfilter_graph_free(&handle->graph);
    }
    c_linked_list_clear(&handle->filters, nullptr);
    if (handle->buffer) av_audio_fifo_free(handle->buffer);
    if (handle->swrac) swr_free(&handle->swrac);
    if (handle->decoder) avcodec_free_context(&handle->decoder);
    if (handle->fmt) avformat_close_input(&handle->fmt);
    if (handle->sdl_initialized) {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }
    if (handle->s && handle->settings_is_alloc) {
        free_ffmpeg_core_settings(handle->s);
    }
    free(handle);
}

void free_music_info_handle(MusicInfoHandle* handle) {
    if (!handle) return;
    if (handle->fmt) avformat_close_input(&handle->fmt);
    free(handle);
}

void free_ffmpeg_core_settings(FfmpegCoreSettings* s) {
    if (!s) return;
    free(s);
}

int ffmpeg_core_log_format_line(void* ptr, int level, const char* fmt, va_list vl, char* line, int line_size, int* print_prefix) {
    return av_log_format_line2(ptr, level, fmt, vl, line, line_size, print_prefix);
}

void ffmpeg_core_log_set_callback(void(*callback)(void*, int, const char*, va_list)) {
    av_log_set_callback(callback);
}

int ffmpeg_core_open(const wchar_t* url, MusicHandle** handle) {
    return ffmpeg_core_open2(url, handle, nullptr);
}

int ffmpeg_core_open2(const wchar_t* url, MusicHandle** h, FfmpegCoreSettings* s) {
    if (!url || !h) return FFMPEG_CORE_ERR_NULLPTR;
    std::string u;
    // 将文件名转为UTF-8，ffmpeg API处理的都是UTF-8文件名
    if (!wchar_util::wstr_to_str(u, url, CP_UTF8)) {
        return FFMPEG_CORE_ERR_INVAILD_NAME;
    }
#if NDEBUG
    // 设置ffmpeg日志级别为Error
    av_log_set_level(AV_LOG_ERROR);
#else
    av_log_set_level(AV_LOG_VERBOSE);
#endif
    MusicHandle* handle = (MusicHandle*)malloc(sizeof(MusicHandle));
    int re = FFMPEG_CORE_ERR_OK;
    if (!handle) {
        return FFMPEG_CORE_ERR_OOM;
    }
    memset(handle, 0, sizeof(MusicHandle));
    if (s) {
        handle->s = s;
    } else {
        handle->settings_is_alloc = 1;
        handle->s = ffmpeg_core_init_settings();
        if (!handle->s) {
            re = FFMPEG_CORE_ERR_OOM;
            goto end;
        }
    }
    handle->first_pts = INT64_MIN;
    if ((re = open_input(handle, u.c_str()))) {
        goto end;
    }
    if ((re = find_audio_stream(handle))) {
        goto end;
    }
    if ((re = open_decoder(handle))) {
        goto end;
    }
    if ((re = init_output(handle))) {
        goto end;
    }
    if ((re = init_filters(handle))) {
        goto end;
    }
    handle->mutex = CreateMutexW(nullptr, FALSE, nullptr);
    if (!handle->mutex) {
        re = FFMPEG_CORE_ERR_FAILED_CREATE_MUTEX;
        goto end;
    }
    handle->thread = CreateThread(nullptr, 0, event_loop, handle, 0, &handle->thread_id);
    if (!handle->thread) {
        re = FFMPEG_CORE_ERR_FAILED_CREATE_THREAD;
        goto end;
    }
    *h = handle;
    return re;
end:
    free_music_handle(handle);
    return re;
}

int ffmpeg_core_info_open(const wchar_t* url, MusicInfoHandle** handle) {
    if (!url || !handle) return FFMPEG_CORE_ERR_NULLPTR;
    std::string u;
    // 将文件名转为UTF-8，ffmpeg API处理的都是UTF-8文件名
    if (!wchar_util::wstr_to_str(u, url, CP_UTF8)) {
        return FFMPEG_CORE_ERR_INVAILD_NAME;
    }
#if NDEBUG
    // 设置ffmpeg日志级别为Error
    av_log_set_level(AV_LOG_ERROR);
#else
    av_log_set_level(AV_LOG_VERBOSE);
#endif
    MusicInfoHandle* h = (MusicInfoHandle*)malloc(sizeof(MusicInfoHandle));
    int re = FFMPEG_CORE_ERR_OK;
    if (!h) {
        return FFMPEG_CORE_ERR_OOM;
    }
    memset(h, 0, sizeof(MusicInfoHandle));
    if ((re = open_input2(h, u.c_str()))) {
        goto end;
    }
    if ((re = find_audio_stream2(h))) {
        goto end;
    }
    *handle = h;
    return FFMPEG_CORE_ERR_OK;
end:
    free_music_info_handle(h);
    return re;
}

int ffmpeg_core_play(MusicHandle* handle) {
    if (!handle) return FFMPEG_CORE_ERR_NULLPTR;
    SDL_PauseAudioDevice(handle->device_id, 0);
    handle->is_playing = 1;
    return FFMPEG_CORE_ERR_OK;
}

int ffmpeg_core_pause(MusicHandle* handle) {
    if (!handle) return FFMPEG_CORE_ERR_NULLPTR;
    SDL_PauseAudioDevice(handle->device_id, 1);
    handle->is_playing = 0;
    return FFMPEG_CORE_ERR_OK;
}

int64_t ffmpeg_core_get_cur_position(MusicHandle* handle) {
    if (!handle) return -1;
    // 忽略 SDL 可能长达 0.01s 的 buffer
    return handle->pts;
}

int ffmpeg_core_song_is_over(MusicHandle* handle) {
    if (!handle || !handle->buffer) return 0;
    if (handle->is_eof && av_audio_fifo_size(handle->buffer) == 0) return 1;
    else return 0;
}

int64_t ffmpeg_core_get_song_length(MusicHandle* handle) {
    if (!handle || !handle->fmt) return -1;
    return handle->fmt->duration;
}

int64_t ffmpeg_core_info_get_song_length(MusicInfoHandle* handle) {
    if (!handle || !handle->fmt) return -1;
    return handle->fmt->duration;
}

int ffmpeg_core_get_channels(MusicHandle* handle) {
    if (!handle || !handle->decoder) return -1;
    return handle->decoder->channels;
}

int ffmpeg_core_info_get_channels(MusicInfoHandle* handle) {
    if (!handle || !handle->is) return -1;
    return handle->is->codecpar->channels;
}

int ffmpeg_core_get_freq(MusicHandle* handle) {
    if (!handle || !handle->decoder) return -1;
    return handle->decoder->sample_rate;
}

int ffmpeg_core_info_get_freq(MusicInfoHandle* handle) {
    if (!handle || !handle->is) return -1;
    return handle->is->codecpar->sample_rate;
}

int ffmpeg_core_seek(MusicHandle* handle, int64_t time) {
    if (!handle) return FFMPEG_CORE_ERR_NULLPTR;
    DWORD re = WaitForSingleObject(handle->mutex, INFINITE);
    if (re == WAIT_OBJECT_0) {
        handle->is_seek = 1;
        handle->seek_pos = time;
    } else {
        return FFMPEG_CORE_ERR_WAIT_MUTEX_FAILED;
    }
    handle->have_err = 0;
    ReleaseMutex(handle->mutex);
    while (1) {
        if (!handle->is_seek) break;
        Sleep(10);
    }
    return handle->have_err ? handle->err : FFMPEG_CORE_ERR_OK;
}

int ffmpeg_core_is_playing(MusicHandle* handle) {
    if (!handle) return -1;
    return handle->is_playing;
}

int ffmpeg_core_get_bits(MusicHandle* handle) {
    if (!handle || !handle->decoder) return -1;
    return handle->decoder->bits_per_coded_sample;
}

int ffmpeg_core_info_get_bits(MusicInfoHandle* handle) {
    if (!handle || !handle->is) return -1;
    return handle->is->codecpar->bits_per_coded_sample;
}

int64_t ffmpeg_core_get_bitrate(MusicHandle* handle) {
    if (!handle || !handle->decoder) return -1;
    return handle->decoder->bit_rate;
}

int64_t ffmpeg_core_info_get_bitrate(MusicInfoHandle* handle) {
    if (!handle || !handle->is) return -1;
    return handle->is->codecpar->bit_rate;
}

std::wstring get_metadata_str(AVDictionary* dict, const char* key, int flags) {
    auto re = av_dict_get(dict, key, nullptr, flags);
    if (!re || !re->value) return L"";
    std::string value(re->value);
    std::wstring result;
    unsigned int cps[CODEPAGE_SIZE] = { CP_UTF8, CP_ACP, CP_OEMCP };
    for (size_t i = 0; i < CODEPAGE_SIZE; i++) {
        if (wchar_util::str_to_wstr(result, value, cps[i])) {
            return result;
        }
    }
    return L"";
}

wchar_t* ffmpeg_core_get_metadata(MusicHandle* handle, const char* key) {
    if (!handle | !key) return nullptr;
    if (handle->fmt->metadata) {
        auto re = get_metadata_str(handle->fmt->metadata, key, 0);
        if (!re.empty()) {
            wchar_t* r = nullptr;
            if (cpp2c::string2char(re, r)) {
                return r;
            }
        }
    }
    if (handle->is->metadata) {
        auto re = get_metadata_str(handle->is->metadata, key, 0);
        if (!re.empty()) {
            wchar_t* r = nullptr;
            if (cpp2c::string2char(re, r)) {
                return r;
            }
        }
    }
    return nullptr;
}

wchar_t* ffmpeg_core_info_get_metadata(MusicInfoHandle* handle, const char* key) {
    if (!handle || !key) return nullptr;
    if (handle->fmt->metadata) {
        auto re = get_metadata_str(handle->fmt->metadata, key, 0);
        if (!re.empty()) {
            wchar_t* r = nullptr;
            if (cpp2c::string2char(re, r)) {
                return r;
            }
        }
    }
    if (handle->is->metadata) {
        auto re = get_metadata_str(handle->is->metadata, key, 0);
        if (!re.empty()) {
            wchar_t* r = nullptr;
            if (cpp2c::string2char(re, r)) {
                return r;
            }
        }
    }
    return nullptr;
}

FfmpegCoreSettings* ffmpeg_core_init_settings() {
    FfmpegCoreSettings* s = (FfmpegCoreSettings*)malloc(sizeof(FfmpegCoreSettings));
    if (!s) return nullptr;
    memset(s, 0, sizeof(FfmpegCoreSettings));
    s->speed = 1.0;
    s->volume = 100;
    return s;
}

int ffmpeg_core_settings_set_volume(FfmpegCoreSettings* s, int volume) {
    if (!s) return 0;
    if (volume >= 0 && volume <= 100) {
        s->volume = volume;
        return 1;
    }
    return 0;
}

int ffmpeg_core_set_volume(MusicHandle* handle, int volume) {
    if (!handle || !handle->s) return FFMPEG_CORE_ERR_NULLPTR;
    int r = ffmpeg_core_settings_set_volume(handle->s, volume);
    if (!r) return FFMPEG_CORE_ERR_FAILED_SET_VOLUME;
    DWORD re = WaitForSingleObject(handle->mutex, INFINITE);
    if (re == WAIT_OBJECT_0) {
        handle->need_reinit_filters = 1;
    } else {
        return FFMPEG_CORE_ERR_WAIT_MUTEX_FAILED;
    }
    handle->have_err = 0;
    ReleaseMutex(handle->mutex);
    while (1) {
        if (!handle->is_seek) break;
        Sleep(10);
    }
    return handle->have_err ? handle->err : FFMPEG_CORE_ERR_OK;
}
