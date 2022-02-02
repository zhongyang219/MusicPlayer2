#include "core.h"

#include <malloc.h>
#include <string.h>
#include "wchar_util.h"
#include "open.h"
#include "output.h"
#include "loop.h"
#include "decode.h"

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
    if (handle->buffer) av_audio_fifo_free(handle->buffer);
    if (handle->swrac) swr_free(&handle->swrac);
    if (handle->decoder) avcodec_free_context(&handle->decoder);
    if (handle->fmt) avformat_close_input(&handle->fmt);
    if (handle->sdl_initialized) {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }
    free(handle);
}

int ffmpeg_core_open(const wchar_t* url, MusicHandle** h) {
    if (!url || !h) return FFMPEG_CORE_ERR_NULLPTR;
    std::string u;
    // 将文件名转为UTF-8，ffmpeg API处理的都是UTF-8文件名
    if (!wchar_util::wstr_to_str(u, url, CP_UTF8)) {
        return FFMPEG_CORE_ERR_INVAILD_NAME;
    }
    // 设置ffmpeg日志级别为Error
    av_log_set_level(AV_LOG_ERROR);
    MusicHandle* handle = (MusicHandle*)malloc(sizeof(MusicHandle));
    int re = FFMPEG_CORE_ERR_OK;
    if (!handle) {
        return FFMPEG_CORE_ERR_OOM;
    }
    memset(handle, 0, sizeof(MusicHandle));
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

int ffmpeg_core_get_channels(MusicHandle* handle) {
    if (!handle || !handle->decoder) return -1;
    return handle->decoder->channels;
}

int ffmpeg_core_get_freq(MusicHandle* handle) {
    if (!handle || !handle->decoder) return -1;
    return handle->decoder->sample_rate;
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

int64_t ffmpeg_core_get_bitrate(MusicHandle* handle) {
    if (!handle || !handle->decoder) return -1;
    return handle->decoder->bit_rate;
}
