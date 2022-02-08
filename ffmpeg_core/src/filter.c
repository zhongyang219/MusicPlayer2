#include "filter.h"

#include "output.h"
#include "volume.h"
#include "speed.h"

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
    return 0;
}

int init_filters(MusicHandle* handle) {
    if (!handle || !handle->s) return FFMPEG_CORE_ERR_NULLPTR;
    if (!need_filters(handle->s)) return FFMPEG_CORE_ERR_OK;
    int re = FFMPEG_CORE_ERR_OK;
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
    return FFMPEG_CORE_ERR_OK;
}

int reinit_filters(MusicHandle* handle) {
    if (!handle || !handle->s) return FFMPEG_CORE_ERR_NULLPTR;
    if (!need_filters(handle->s)) {
        if (!handle->graph) return FFMPEG_CORE_ERR_OK;
        DWORD re = WaitForSingleObject(handle->mutex, INFINITE);
        if (re != WAIT_OBJECT_0) {
            return FFMPEG_CORE_ERR_WAIT_MUTEX_FAILED;
        }
        avfilter_graph_free(&handle->graph);
        handle->graph = NULL;
        handle->filter_inp = NULL;
        handle->filter_out = NULL;
        c_linked_list_clear(&handle->filters, NULL);
        ReleaseMutex(handle->mutex);
        return FFMPEG_CORE_ERR_OK;
    }
    int re = FFMPEG_CORE_ERR_OK;
    AVFilterGraph* graph = NULL;
    AVFilterContext* inc = NULL, * outc = NULL;
    c_linked_list* list = NULL;
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
    }
    if (c_linked_list_count(list) == 0) {
        if (handle->graph) {
            DWORD r = WaitForSingleObject(handle->mutex, INFINITE);
            if (r != WAIT_OBJECT_0) {
                re = FFMPEG_CORE_ERR_WAIT_MUTEX_FAILED;
                goto end;
            }
            avfilter_graph_free(&handle->graph);
            handle->graph = NULL;
            handle->filter_inp = NULL;
            handle->filter_out = NULL;
            c_linked_list_clear(&handle->filters, NULL);
            ReleaseMutex(handle->mutex);
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
    DWORD r = WaitForSingleObject(handle->mutex, INFINITE);
    if (r != WAIT_OBJECT_0) {
        re = FFMPEG_CORE_ERR_WAIT_MUTEX_FAILED;
        goto end;
    }
    if (handle->graph) {
        avfilter_graph_free(&handle->graph);
        handle->graph = NULL;
        handle->filter_inp = NULL;
        handle->filter_out = NULL;
        c_linked_list_clear(&handle->filters, NULL);
    }
    handle->graph = graph;
    handle->filter_inp = inc;
    handle->filter_out = outc;
    handle->filters = list;
    ReleaseMutex(handle->mutex);
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
    av_get_channel_layout_string(channel_layout, sizeof(channel_layout), handle->decoder->channels, layout);
    snprintf(args, sizeof(args), "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=%s:channels=%d", handle->is->time_base.num, handle->is->time_base.den, handle->sdl_spec.freq, av_get_sample_fmt_name(handle->target_format), channel_layout, handle->decoder->channels);
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
    int channel_counts[2] = { handle->decoder->channels, 0 };
    if ((re = av_opt_set_int_list(*sink, "channel_counts", channel_counts, 0, AV_OPT_SEARCH_CHILDREN)) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Failed to set channel_counts to output filter: %s (%i)\n", av_err2str(re), re);
        return re;
    }
    return FFMPEG_CORE_ERR_OK;
}
