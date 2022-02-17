#include "filter.h"

#include "output.h"
#include "volume.h"
#include "speed.h"
#include "equalizer.h"

int need_filters(FfmpegCoreSettings* s) {
    if (!s) return 0;
    if (!avfilter_get_by_name("abuffersink") || !avfilter_get_by_name("abuffer")) {
        return 0;
    }
    if (s->volume != 100 && avfilter_get_by_name("volume")) {
        return 1;
    }
    if (get_speed(s->speed) != 1000 && avfilter_get_by_name("atempo")) {
        return 1;
    }
    if (s->equalizer_channels && avfilter_get_by_name("equalizer")) {
        return 1;
    }
    return 0;
}

int init_filters(MusicHandle* handle) {
    if (!handle || !handle->s) return FFMPEG_CORE_ERR_NULLPTR;
    if (!need_filters(handle->s)) return FFMPEG_CORE_ERR_OK;
    int re = FFMPEG_CORE_ERR_OK;
    int is_easy_filters = 1;
    int speed = get_speed(handle->s->speed);
    if ((re = create_src_and_sink(&handle->graph, &handle->filter_inp, &handle->filter_out, handle))) {
        return re;
    }
    if (handle->s->volume != 100 && avfilter_get_by_name("volume")) {
        if ((re = create_volume_filter(0, handle->graph, handle->filter_inp, &handle->filters, handle->s->volume, handle->target_format))) {
            return re;
        }
    }
    if (speed != 1000 && avfilter_get_by_name("atempo")) {
        int index = 0;
        while (speed != 1000) {
            if ((re = create_speed_filter(index, handle->graph, handle->filter_inp, &handle->filters, &speed))) {
                return re;
            }
            index++;
        }
        is_easy_filters = 0;
    }
    if (handle->s->equalizer_channels && avfilter_get_by_name("equalizer")) {
        EqualizerChannels* now = handle->s->equalizer_channels;
        if ((re = create_equalizer_filter(handle->graph, handle->filter_inp, &handle->filters, now->d.channel, now->d.gain, handle->target_format))) {
            return re;
        }
        while (now->next) {
            now = now->next;
            if ((re = create_equalizer_filter(handle->graph, handle->filter_inp, &handle->filters, now->d.channel, now->d.gain, handle->target_format))) {
                return re;
            }
        }
        is_easy_filters = 0;
    }
    if (c_linked_list_count(handle->filters) == 0) {
        avfilter_graph_free(&handle->graph);
        handle->graph = NULL;
        handle->filter_inp = NULL;
        handle->filter_out = NULL;
        return FFMPEG_CORE_ERR_OK;
    }
    AVFilterContext* last = c_linked_list_tail(handle->filters)->d;
    if ((re = avfilter_link(last, 0, handle->filter_out, 0)) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Failed to link %s:%i -> %s:%i: %s (%i)\n", last->name, 0, handle->filter_out->name, 0, av_err2str(re), re);
        return re;
    }
    if ((re = avfilter_graph_config(handle->graph, NULL)) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Failed to check config of filters: %s (%i)\n", av_err2str(re), re);
        return re;
    }
    handle->is_easy_filters = is_easy_filters;
    if (!handle->is_easy_filters) {
        handle->is_wait_filters = 1;
    }
    return FFMPEG_CORE_ERR_OK;
}

int reinit_filters(MusicHandle* handle) {
    if (!handle || !handle->s) return FFMPEG_CORE_ERR_NULLPTR;
    if (!need_filters(handle->s)) {
        if (!handle->graph) return FFMPEG_CORE_ERR_OK;
        DWORD re = WaitForSingleObject(handle->mutex2, INFINITE);
        if (re != WAIT_OBJECT_0) {
            return FFMPEG_CORE_ERR_WAIT_MUTEX_FAILED;
        }
        re = WaitForSingleObject(handle->mutex, INFINITE);
        if (re != WAIT_OBJECT_0) {
            ReleaseMutex(handle->mutex2);
            return FFMPEG_CORE_ERR_WAIT_MUTEX_FAILED;
        }
        avfilter_graph_free(&handle->graph);
        handle->graph = NULL;
        handle->filter_inp = NULL;
        handle->filter_out = NULL;
        c_linked_list_clear(&handle->filters, NULL);
        av_audio_fifo_reset(handle->filters_buffer);
        handle->filters_buffer_offset = 0;
        ReleaseMutex(handle->mutex);
        ReleaseMutex(handle->mutex2);
        return FFMPEG_CORE_ERR_OK;
    }
    int re = FFMPEG_CORE_ERR_OK;
    AVFilterGraph* graph = NULL;
    AVFilterContext* inc = NULL, * outc = NULL;
    c_linked_list* list = NULL;
    int is_easy_filters = 1;
    int speed = get_speed(handle->s->speed);
    if ((re = create_src_and_sink(&graph, &inc, &outc, handle)) < 0) {
        goto end;
    }
    if (handle->s->volume != 100 && avfilter_get_by_name("volume")) {
        if ((re = create_volume_filter(0, graph, inc, &list, handle->s->volume, handle->target_format))) {
            goto end;
        }
    }
    if (speed != 1000 && avfilter_get_by_name("atempo")) {
        int index = 0;
        while (speed != 1000) {
            if ((re = create_speed_filter(index, graph, inc, &list, &speed))) {
                goto end;
            }
            index++;
        }
        is_easy_filters = 0;
    }
    if (handle->s->equalizer_channels && avfilter_get_by_name("equalizer")) {
        EqualizerChannels* now = handle->s->equalizer_channels;
        if ((re = create_equalizer_filter(graph, inc, &list, now->d.channel, now->d.gain, handle->target_format))) {
            goto end;
        }
        while (now->next) {
            now = now->next;
            if ((re = create_equalizer_filter(graph, inc, &list, now->d.channel, now->d.gain, handle->target_format))) {
                goto end;
            }
        }
        is_easy_filters = 0;
    }
    if (c_linked_list_count(list) == 0) {
        if (handle->graph) {
            DWORD r = WaitForSingleObject(handle->mutex2, INFINITE);
            if (r != WAIT_OBJECT_0) {
                re = FFMPEG_CORE_ERR_WAIT_MUTEX_FAILED;
                goto end;
            }
            r = WaitForSingleObject(handle->mutex, INFINITE);
            if (r != WAIT_OBJECT_0) {
                ReleaseMutex(handle->mutex2);
                re = FFMPEG_CORE_ERR_WAIT_MUTEX_FAILED;
                goto end;
            }
            avfilter_graph_free(&handle->graph);
            handle->graph = NULL;
            handle->filter_inp = NULL;
            handle->filter_out = NULL;
            c_linked_list_clear(&handle->filters, NULL);
            av_audio_fifo_reset(handle->filters_buffer);
            handle->filters_buffer_offset = 0;
            ReleaseMutex(handle->mutex);
            ReleaseMutex(handle->mutex2);
        }
        re = FFMPEG_CORE_ERR_OK;
        goto end;
    }
    AVFilterContext* last = c_linked_list_tail(list)->d;
    if ((re = avfilter_link(last, 0, outc, 0)) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Failed to link %s:%i -> %s:%i: %s (%i)\n", last->name, 0, outc->name, 0, av_err2str(re), re);
        goto end;
    }
    if ((re = avfilter_graph_config(graph, NULL)) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Failed to check config of filters: %s (%i)\n", av_err2str(re), re);
        goto end;
    }
    DWORD r = WaitForSingleObject(handle->mutex2, INFINITE);
    if (r != WAIT_OBJECT_0) {
        re = FFMPEG_CORE_ERR_WAIT_MUTEX_FAILED;
        goto end;
    }
    r = WaitForSingleObject(handle->mutex, INFINITE);
    if (r != WAIT_OBJECT_0) {
        ReleaseMutex(handle->mutex2);
        re = FFMPEG_CORE_ERR_WAIT_MUTEX_FAILED;
        goto end;
    }
    if (handle->graph) {
        avfilter_graph_free(&handle->graph);
        handle->graph = NULL;
        handle->filter_inp = NULL;
        handle->filter_out = NULL;
        av_audio_fifo_reset(handle->filters_buffer);
        handle->filters_buffer_offset = 0;
        c_linked_list_clear(&handle->filters, NULL);
    }
    handle->graph = graph;
    handle->filter_inp = inc;
    handle->filter_out = outc;
    handle->filters = list;
    handle->is_easy_filters = is_easy_filters;
    if (!handle->is_easy_filters) {
        handle->is_wait_filters = 1;
    }
    ReleaseMutex(handle->mutex);
    ReleaseMutex(handle->mutex2);
    return FFMPEG_CORE_ERR_OK;
end:
    if (graph) {
        avfilter_graph_free(&graph);
        c_linked_list_clear(&list, NULL);
    }
    return re;
}

int create_src_and_sink(AVFilterGraph** graph, AVFilterContext** src, AVFilterContext** sink, MusicHandle* handle) {
    if (!graph || !src || !sink) return FFMPEG_CORE_ERR_NULLPTR;
    const AVFilter* buffersink = avfilter_get_by_name("abuffersink"), * buffer = avfilter_get_by_name("abuffer");
    if (!(*graph = avfilter_graph_alloc())) {
        av_log(NULL, AV_LOG_FATAL, "Failed to allocate filter graph.\n");
        return FFMPEG_CORE_ERR_OOM;
    }
    int re = 0;
    char args[1024];
    char channel_layout[512];
    // 输入的设置：描述见 ffmpeg -h filter=abuffer
    uint64_t layout = handle->output_channel_layout;
    av_get_channel_layout_string(channel_layout, sizeof(channel_layout), handle->sdl_spec.channels, layout);
    snprintf(args, sizeof(args), "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=%s:channels=%d", handle->is->time_base.num, handle->is->time_base.den, handle->sdl_spec.freq, av_get_sample_fmt_name(handle->target_format), channel_layout, handle->sdl_spec.channels);
    if ((re = avfilter_graph_create_filter(src, buffer, "in", args, NULL, *graph)) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Failed to create input filter: %s (%i)\n", av_err2str(re), re);
        return re;
    }
    if ((re = avfilter_graph_create_filter(sink, buffersink, "out", NULL, NULL, *graph)) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Failed to create output filter: %s (%i)\n", av_err2str(re), re);
        return re;
    }
    // 输出设置
    // 描述见 ffmpeg -h filter=abuffersink
    // 具体类型参考 ffmpeg 源代码 libavfilter/buffersink.c 里的 abuffersink_options
    enum AVSampleFormat sample_fmts[2] = { handle->target_format , AV_SAMPLE_FMT_NONE };
    if ((re = av_opt_set_int_list(*sink, "sample_fmts", sample_fmts, AV_SAMPLE_FMT_NONE, AV_OPT_SEARCH_CHILDREN)) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Failed to set sample_fmts to output filter: %s (%i)\n", av_err2str(re), re);
        return re;
    }
    int sample_rates[2] = { handle->sdl_spec.freq , 0 };
    if ((re = av_opt_set_int_list(*sink, "sample_rates", sample_rates, 0, AV_OPT_SEARCH_CHILDREN)) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Failed to set sample_rates to output filter: %s (%i)\n", av_err2str(re), re);
        return re;
    }
    int64_t channel_layouts[2] = { layout , 0 };
    if ((re = av_opt_set_int_list(*sink, "channel_layouts", channel_layouts, 0, AV_OPT_SEARCH_CHILDREN)) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Failed to set channel_layouts to output filter: %s (%i)\n", av_err2str(re), re);
        return re;
    }
    int channel_counts[2] = { handle->sdl_spec.channels, 0 };
    if ((re = av_opt_set_int_list(*sink, "channel_counts", channel_counts, 0, AV_OPT_SEARCH_CHILDREN)) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Failed to set channel_counts to output filter: %s (%i)\n", av_err2str(re), re);
        return re;
    }
    return FFMPEG_CORE_ERR_OK;
}

int add_data_to_filters_buffer(MusicHandle* handle) {
    if (!handle) return FFMPEG_CORE_ERR_NULLPTR;
    DWORD re = WaitForSingleObject(handle->mutex2, INFINITE);
    int r = FFMPEG_CORE_ERR_OK;
    AVFrame* in = NULL, * out = NULL;
    int samples_need = 1000;
    int samples_need_in = 0;
    /// 音频缓冲区buffer要peek的起始位置
    int input_samples_offset = 0;
    int buffer_size = 0;
    int writed = 0;
    unsigned char have_mutex = 0;
    AVRational base = { 1000, 1000 }, target = { 1, 1 };
    if (re == WAIT_TIMEOUT) return FFMPEG_CORE_ERR_OK;
    else if (re != WAIT_OBJECT_0) return FFMPEG_CORE_ERR_WAIT_MUTEX_FAILED;
    re = WaitForSingleObject(handle->mutex, 10);
    if (re == WAIT_TIMEOUT) {
        ReleaseMutex(handle->mutex2);
        return FFMPEG_CORE_ERR_OK;
    } else if (re != WAIT_OBJECT_0) {
        ReleaseMutex(handle->mutex2);
        return FFMPEG_CORE_ERR_WAIT_MUTEX_FAILED;
    }
    have_mutex = 1;
    if (!handle->graph || handle->is_easy_filters) {
        ReleaseMutex(handle->mutex);
        ReleaseMutex(handle->mutex2);
        return FFMPEG_CORE_ERR_OK;
    }
    buffer_size = av_audio_fifo_size(handle->filters_buffer);
    if (buffer_size > handle->sdl_spec.freq) {
        r = FFMPEG_CORE_ERR_OK;
        goto end;
    }
    base.num = get_speed(handle->s->speed);
    input_samples_offset = handle->filters_buffer_offset;
    samples_need_in = av_rescale_q_rnd(samples_need, base, target, AV_ROUND_UP | AV_ROUND_PASS_MINMAX);
    if (av_audio_fifo_size(handle->buffer) <= input_samples_offset) {
        ReleaseMutex(handle->mutex);
        have_mutex = 0;
        if (handle->is_eof) {
            if ((r = av_buffersrc_add_frame(handle->filter_inp, NULL)) < 0) {
                goto end;
            }
            out = av_frame_alloc();
            if (!out) {
                r = FFMPEG_CORE_ERR_OOM;
                goto end;
            }
            goto outp;
        }
        r = FFMPEG_CORE_ERR_OK;
        goto end;
    }
    in = av_frame_alloc();
    out = av_frame_alloc();
    if (!in || !out) {
        r = FFMPEG_CORE_ERR_OOM;
        goto end;
    }
    in->channels = handle->sdl_spec.channels;
    in->channel_layout = handle->output_channel_layout;
    in->format = handle->target_format;
    in->sample_rate = handle->sdl_spec.freq;
    samples_need_in = min(samples_need_in, av_audio_fifo_size(handle->buffer) - input_samples_offset);
    in->nb_samples = samples_need_in;
    if ((r = av_frame_get_buffer(in, 0)) < 0) {
        goto end;
    }
    writed = av_audio_fifo_peek_at(handle->buffer, (void**)in->data, samples_need_in, input_samples_offset);
    if (writed < 0) {
        r = writed;
        goto end;
    }
    handle->filters_buffer_offset += writed;
    in->nb_samples = writed;
    ReleaseMutex(handle->mutex);
    have_mutex = 0;
    if ((r = av_buffersrc_add_frame(handle->filter_inp, in)) < 0) {
        goto end;
    }
outp:
    if ((r = av_buffersink_get_frame(handle->filter_out, out)) < 0) {
        if (r == AVERROR(EAGAIN)) r = FFMPEG_CORE_ERR_OK;
        goto end;
    }
    re = WaitForSingleObject(handle->mutex, INFINITE);
    if (re != WAIT_OBJECT_0) {
        r = FFMPEG_CORE_ERR_WAIT_MUTEX_FAILED;
        goto end;
    }
    have_mutex = 1;
    if ((r = av_audio_fifo_write(handle->filters_buffer, (void*)out->data, out->nb_samples)) < 0) {
        goto end;
    }
    r = FFMPEG_CORE_ERR_OK;
end:
    if (in) av_frame_free(&in);
    if (out) av_frame_free(&out);
    if (have_mutex) ReleaseMutex(handle->mutex);
    ReleaseMutex(handle->mutex2);
    return r;
}

void reset_filters_buffer(MusicHandle* handle) {
    if (!handle) return;
    av_audio_fifo_reset(handle->filters_buffer);
    handle->filters_buffer_offset = 0;
    if (!handle->is_easy_filters) {
        handle->is_wait_filters = 1;
    }
}
