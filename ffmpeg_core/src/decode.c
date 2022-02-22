#include "decode.h"

#include "libavutil/timestamp.h"

int open_decoder(MusicHandle* handle) {
    if (!handle || !handle->fmt || !handle->is) return FFMPEG_CORE_ERR_NULLPTR;
    handle->codec = avcodec_find_decoder(handle->is->codecpar->codec_id);
    if (!handle->codec) return FFMPEG_CORE_ERR_NO_AUDIO_OR_DECODER;
    handle->decoder = avcodec_alloc_context3(handle->codec);
    if (!handle->decoder) return FFMPEG_CORE_ERR_OOM;
    int re = 0;
    // 从输入流复制参数
    if ((re = avcodec_parameters_to_context(handle->decoder, handle->is->codecpar)) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Failed to copy parameters from input stream: %s (%i)\n", av_err2str(re), re);
        return re;
    }
    if (handle->decoder->channel_layout == 0) {
        // 如果未设置，设置为默认值
        handle->decoder->channel_layout = av_get_default_channel_layout(handle->decoder->channels);
    }
    // 打开解码器
    if ((re = avcodec_open2(handle->decoder, handle->codec, NULL)) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Failed to open decoder \"%s\": %s (%i)\n", handle->codec->name, av_err2str(re), re);
        return re;
    }
    return FFMPEG_CORE_ERR_OK;
}

int reopen_decoder(MusicHandle* handle) {
    if (!handle) return FFMPEG_CORE_ERR_NULLPTR;
    if (handle->decoder) {
        avcodec_free_context(&handle->decoder);
    }
    handle->decoder = avcodec_alloc_context3(handle->codec);
    if (!handle->decoder) return FFMPEG_CORE_ERR_OOM;
    int re = 0;
    // 从输入流复制参数
    if ((re = avcodec_parameters_to_context(handle->decoder, handle->is->codecpar)) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Failed to copy parameters from input stream: %s (%i)\n", av_err2str(re), re);
        return re;
    }
    if (handle->decoder->channel_layout == 0) {
        // 如果未设置，设置为默认值
        handle->decoder->channel_layout = av_get_default_channel_layout(handle->decoder->channels);
    }
    // 打开解码器
    if ((re = avcodec_open2(handle->decoder, handle->codec, NULL)) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Failed to open decoder \"%s\": %s (%i)\n", handle->codec->name, av_err2str(re), re);
        return re;
    }
    return FFMPEG_CORE_ERR_OK;
}

int decode_audio_internal(MusicHandle* handle, char* writed, AVFrame* frame) {
    if (!handle || !writed || !frame) return FFMPEG_CORE_ERR_NULLPTR;
    int re = 0;
    re = avcodec_receive_frame(handle->decoder, frame);
    if (re >= 0) {
        if (handle->first_pts == INT64_MIN) {
            handle->first_pts = av_rescale_q_rnd(frame->pts, handle->is->time_base, AV_TIME_BASE_Q, AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
            av_log(NULL, AV_LOG_VERBOSE, "first_pts: %s\n", av_ts2timestr(handle->first_pts, &AV_TIME_BASE_Q));
            if (handle->only_part) {
                // 定位到开始位置
                if (handle->part_start_pts > 0) {
                    handle->is_seek = 1;
                    handle->seek_pos = handle->part_start_pts;
                }
            }
        }
        if (handle->set_new_pts && frame->pts != AV_NOPTS_VALUE) {
            av_log(NULL, AV_LOG_VERBOSE, "pts: %s\n", av_ts2timestr(frame->pts, &handle->is->time_base));
            handle->pts = av_rescale_q_rnd(frame->pts, handle->is->time_base, AV_TIME_BASE_Q, AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX) - handle->first_pts;
            handle->end_pts = handle->pts;
            handle->set_new_pts = 0;
        } else if (handle->set_new_pts) {
            av_log(NULL, AV_LOG_VERBOSE, "skip NOPTS frame.\n");
            // 跳过NOPTS的frame
            goto end;
        }
        // 整段数据在结束位置之后，跳过
        if (handle->only_part && (frame->pts - handle->first_pts) >= handle->part_end_pts) {
            handle->is_eof = 1;
            goto end;
        }
        re = convert_samples_and_add_to_fifo(handle, frame, writed);
        goto end;
    } else if (re == AVERROR(EAGAIN)) {
        // 数据不够，继续读取
        re = FFMPEG_CORE_ERR_OK;
        goto end;
    } else if (re == AVERROR_EOF) {
        handle->is_eof = 1;
        re = FFMPEG_CORE_ERR_OK;
        goto end;
    }
end:
    return re;
}

int decode_audio(MusicHandle* handle, char* writed) {
    if (!handle || !writed) return FFMPEG_CORE_ERR_NULLPTR;
    AVPacket pkt;
    AVFrame* frame = av_frame_alloc();
    *writed = 0;
    if (!frame) {
        return FFMPEG_CORE_ERR_OOM;
    }
    int re = FFMPEG_CORE_ERR_OK;
    while (1) {
        if ((re = decode_audio_internal(handle, writed, frame))) {
            goto end;
        }
        if (*writed || handle->is_eof) {
            goto end;
        }
        if ((re = av_read_frame(handle->fmt, &pkt)) < 0) {
            if (re == AVERROR_EOF) {
                handle->is_eof = 1;
                re = FFMPEG_CORE_ERR_OK;
                goto end;
            }
            goto end;
        }
        if (pkt.stream_index != handle->is->index) {
            // 其他流，跳过并释放引用
            av_packet_unref(&pkt);
            continue;
        }
        handle->last_pkt_pts = av_rescale_q_rnd(pkt.pts, handle->is->time_base, AV_TIME_BASE_Q, AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
        if ((re = avcodec_send_packet(handle->decoder, &pkt)) < 0) {
            if (re == AVERROR(EAGAIN)) {
                re = 0;
                av_packet_unref(&pkt);
                continue;
            }
            av_packet_unref(&pkt);
            goto end;
        }
        av_packet_unref(&pkt);
        if ((re = decode_audio_internal(handle, writed, frame))) {
            goto end;
        }
        if (*writed || handle->is_eof) break;
    }
end:
    if (frame) av_frame_free(&frame);
    return re;
}

int convert_samples_and_add_to_fifo(MusicHandle* handle, AVFrame* frame, char* writed) {
    if (!handle || !frame || !writed) return FFMPEG_CORE_ERR_NULLPTR;
    uint8_t** converted_input_samples = NULL;
    int re = FFMPEG_CORE_ERR_OK;
    AVRational base = { 1, handle->decoder->sample_rate }, target = { 1, handle->sdl_spec.freq };
    int samples = frame->nb_samples;
    if (handle->only_part) {
        int tmp = av_rescale_q_rnd(handle->part_end_pts - handle->end_pts + handle->first_pts, AV_TIME_BASE_Q, base, AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
        if (samples >= tmp) {
            samples = tmp;
            handle->is_eof = 1;
        }
    }
    /// 输出的样本数
    int64_t frames = av_rescale_q_rnd(samples, base, target, AV_ROUND_UP | AV_ROUND_PASS_MINMAX);
    /// 实际输出样本数
    int converted_samples = 0;
    DWORD res = 0;
    if (!(converted_input_samples = malloc(sizeof(void*) * handle->sdl_spec.channels))) {
        re = FFMPEG_CORE_ERR_OOM;
        goto end;
    }
    memset(converted_input_samples, 0, sizeof(void*) * handle->sdl_spec.channels);
    if ((re = av_samples_alloc(converted_input_samples, NULL, handle->sdl_spec.channels, frames, handle->target_format, 0)) < 0) {
        re = FFMPEG_CORE_ERR_OOM;
        goto end;
    }
    re = 0;
    if ((converted_samples = swr_convert(handle->swrac, converted_input_samples, frames, (const uint8_t**)frame->extended_data, samples)) < 0) {
        re = converted_samples;
        goto end;
    }
    res = WaitForSingleObject(handle->mutex, INFINITE);
    if (res != WAIT_OBJECT_0) {
        re = FFMPEG_CORE_ERR_WAIT_MUTEX_FAILED;
        goto end;
    }
    if ((converted_samples = av_audio_fifo_write(handle->buffer, (void**)converted_input_samples, converted_samples)) < 0) {
        ReleaseMutex(handle->mutex);
        re = converted_samples;
        goto end;
    }
    handle->end_pts += av_rescale_q_rnd(converted_samples, target, AV_TIME_BASE_Q, AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
    *writed = 1;
    ReleaseMutex(handle->mutex);
end:
    if (converted_input_samples) {
        av_freep(&converted_input_samples[0]);
        free(converted_input_samples);
    }
    return re;
}
