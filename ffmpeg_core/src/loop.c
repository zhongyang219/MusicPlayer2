#include "loop.h"

#include <inttypes.h>
#include "libavutil/timestamp.h"
#include "decode.h"
#include "filter.h"
#include "file.h"
#include "cda.h"
#include "open.h"

#define ft2ts(t) (((size_t)t.dwHighDateTime << 32) | (size_t)t.dwLowDateTime)

int seek_to_pos(MusicHandle* handle) {
    if (!handle) return FFMPEG_CORE_ERR_NULLPTR;
    int re = FFMPEG_CORE_ERR_OK;
    DWORD r = WaitForSingleObject(handle->mutex, INFINITE);
    AVRational base = { 1, handle->sdl_spec.freq };
    if (r != WAIT_OBJECT_0) {
        re = FFMPEG_CORE_ERR_WAIT_MUTEX_FAILED;
        goto end;
    }
    if (handle->seek_pos >= handle->pts && handle->seek_pos <= handle->end_pts) {
        // 已经在缓冲区，直接从缓冲区移除不需要的数据
        int64_t samples = min(av_rescale_q_rnd(handle->seek_pos - handle->pts, AV_TIME_BASE_Q, base, AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX), av_audio_fifo_size(handle->buffer));
        if ((re = av_audio_fifo_drain(handle->buffer, samples)) < 0) {
            av_log(NULL, AV_LOG_FATAL, "Failed to drain %" PRIi64 " samples in buffer: %s (%i)\n", samples, av_err2str(re), re);
            ReleaseMutex(handle->mutex);
            goto end;
        }
        // 增大当前时间
        handle->pts += av_rescale_q_rnd(samples, base, AV_TIME_BASE_Q, AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
    } else {
        // 不在缓冲区，调用av_seek_frame并清空缓冲区
        int flags = 0;
        // 修复flac文件解码完之后，继续调用解码器会导致报错的BUG
        // 同时解决decoder内多余buffer的问题
        av_log(NULL, AV_LOG_VERBOSE, "Try to reopen decoder \"%s\".\n", handle->codec->name ? handle->codec->name : "(null)");
        if ((re = reopen_decoder(handle))) {
            ReleaseMutex(handle->mutex);
            goto end;
        }
        // 指的是定位到指定位置之前的关键帧，而不是从后往前定位
        flags |= AVSEEK_FLAG_BACKWARD;
        if ((re = av_seek_frame(handle->fmt, -1, handle->seek_pos + handle->first_pts, flags)) < 0) {
            av_log(NULL, AV_LOG_FATAL, "Failed to seek frame %" PRIi64 ": %s (%i)\n", handle->seek_pos + handle->first_pts, av_err2str(re), re);
            ReleaseMutex(handle->mutex);
            goto end;
        }
        re = 0;
        av_audio_fifo_reset(handle->buffer);
        handle->set_new_pts = 1;
        handle->is_eof = 0;
    }
    reset_filters_buffer(handle);
    ReleaseMutex(handle->mutex);
end:
    handle->is_seek = 0;
    return re;
}

int reopen_file(MusicHandle* handle) {
    if (!handle) return FFMPEG_CORE_ERR_NULLPTR;
    int re = FFMPEG_CORE_ERR_OK;
    if (handle->is_file) {
        int doing = 0;
        while (1) {
            doing = 0;
            if (handle->stoping) return FFMPEG_CORE_ERR_OK;
            if (basic_event_handle(handle)) {
                doing = 1;
            }
            if (is_file_exists(handle)) break;
            if (!doing) Sleep(10);
        }
    } else {
        int doing = 0;
        FILETIME st;
        GetSystemTimePreciseAsFileTime(&st);
        FILETIME now;
        memcpy(&now, &st, sizeof(FILETIME));
        while ((ft2ts(now) - ft2ts(st)) < ((size_t)10000000 * handle->s->url_retry_interval)) {
            doing = 0;
            if (handle->stoping) return FFMPEG_CORE_ERR_OK;
            if (basic_event_handle(handle)) {
                doing = 1;
            }
            if (!doing) Sleep(10);
            GetSystemTimePreciseAsFileTime(&now);
        }
    }
    if (handle->fmt) avformat_close_input(&handle->fmt);
    if (handle->decoder) avcodec_free_context(&handle->decoder);
    if (handle->is_cda) {
        if ((re = open_cd_device(handle, handle->url))) {
            return re;
        }
    } else {
        if ((re = open_input(handle, handle->url))) {
            return re;
        }
    }
    if ((re = find_audio_stream(handle))) {
        return re;
    }
    if ((re = open_decoder(handle))) {
        return re;
    }
    av_log(NULL, AV_LOG_VERBOSE, "The target pts: %s\n", av_ts2timestr(handle->last_pkt_pts, &AV_TIME_BASE_Q));
    if ((re = av_seek_frame(handle->fmt, -1, handle->last_pkt_pts, AVSEEK_FLAG_ANY)) < 0) {
        return re;
    }
    AVPacket pkt;
    while (1) {
        int64_t tmppts = 0;
        if ((re = av_read_frame(handle->fmt, &pkt)) < 0) {
            return re;
        }
        if (pkt.stream_index != handle->is->index) {
            av_packet_unref(&pkt);
            continue;
        }
        tmppts = av_rescale_q_rnd(pkt.pts, handle->is->time_base, AV_TIME_BASE_Q, AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
        if (tmppts < handle->last_pkt_pts) {
            continue;
        }
        break;
    }
    av_log(NULL, AV_LOG_VERBOSE, "The packet pts after seek: %s\n", av_ts2timestr(pkt.pts, &handle->is->time_base));
    av_packet_unref(&pkt);
    handle->is_reopen = 0;
    return FFMPEG_CORE_ERR_OK;
}

int basic_event_handle(MusicHandle* h) {
    if (!h) return 0;
    if (h->need_reinit_filters) {
        int re = reinit_filters(h);
        if (re) {
            h->have_err = 1;
            h->err = re;
            av_log(NULL, AV_LOG_WARNING, "%s %i: Error when calling reinit_filters: %s (%i).\n", __FILE__, __LINE__, av_err2str(re), re);
        }
        h->need_reinit_filters = 0;
        return 1;
    }
    return 0;
}

DWORD WINAPI event_loop(LPVOID handle) {
    if (!handle) return FFMPEG_CORE_ERR_NULLPTR;
    MusicHandle* h = (MusicHandle*)handle;
    int samples = h->decoder->sample_rate * 15;
    /// 本次循环是否有做事
    char doing = 0;
    /// 是否往缓冲区加了数据
    char writed = 0;
    int buffered_size = h->sdl_spec.freq * h->s->cache_length;
    while (1) {
        doing = 0;
        if (h->stoping) break;
        if (basic_event_handle(h)) {
            doing = 1;
            goto end;
        }
        if (h->is_reopen) {
            /// 禁用了重试或重试次数达上限
            if (!h->s->max_retry_count || (h->s->max_retry_count > 0 && h->retry_count < h->s->max_retry_count)) {
                goto end;
            }
            h->retry_count += 1;
            av_log(NULL, AV_LOG_VERBOSE, "Try to reopen file \"%s\" %i times.\n", h->url, h->retry_count);
            int re = reopen_file(h);
            if (re) {
                av_log(NULL, AV_LOG_VERBOSE, "Reopen failed: %i.\n", re);
                h->err = re;
            } else {
                h->have_err = 0;
                h->err = 0;
                h->retry_count = 0;
            }
            doing = 1;
            goto end;
        }
        if (h->is_seek && h->first_pts != INT64_MIN) {
            int re = seek_to_pos(h);
            if (re) {
                h->have_err = 1;
                h->err = re;
                av_log(NULL, AV_LOG_WARNING, "%s %i: Error when calling seek_to_pos: %i.\n", __FILE__, __LINE__, re);
            }
            doing = 1;
            goto end;
        }
        if (!h->is_eof) {
            buffered_size = h->sdl_spec.freq * h->s->cache_length;
            if (av_audio_fifo_size(h->buffer) < buffered_size) {
                int re = decode_audio(handle, &writed);
                if (re) {
                    av_log(NULL, AV_LOG_WARNING, "%s %i: Error when calling decode_audio: %s (%i).\n", __FILE__, __LINE__, av_err2str(re), re);
                    h->have_err = 1;
                    h->err = re;
                    av_log(NULL, AV_LOG_VERBOSE, "Try to reopen file \"%s\".\n", h->url);
                    h->is_reopen = 1;
                    if (h->s->max_retry_count) {
                        h->retry_count += 1;
                        re = reopen_file(h);
                        if (re) {
                            av_log(NULL, AV_LOG_VERBOSE, "Reopen failed: %i.\n", re);
                            h->err = re;
                        } else {
                            h->have_err = 0;
                            h->err = 0;
                            h->retry_count = 0;
                        }
                    }
                }
                doing = 1;
            }
        } else {
            // 播放完毕，自动停止播放
            if (av_audio_fifo_size(h->buffer) == 0) {
                SDL_PauseAudioDevice(h->device_id, 1);
                h->is_playing = 0;
            }
        }
end:
        if (!doing) {
            Sleep(10);
        }
    }
    return FFMPEG_CORE_ERR_OK;
}

DWORD WINAPI filter_loop(LPVOID handle) {
    if (!handle) return FFMPEG_CORE_ERR_NULLPTR;
    MusicHandle* h = (MusicHandle*)handle;
    char doing = 0;
    while (1) {
        doing = 0;
        if (h->stoping) break;
        if (h->graph && !h->is_easy_filters) {
            int re = add_data_to_filters_buffer(h);
            if (re) {
                h->have_err = 1;
                h->err = re;
                av_log(NULL, AV_LOG_WARNING, "%s %i: Error when calling add_data_to_filters_buffer: %s (%i).\n", __FILE__, __LINE__, av_err2str(re), re);
            }
            doing = 1;
        }
        if (!doing) {
            Sleep(10);
        }
    }
    return FFMPEG_CORE_ERR_OK;
}
