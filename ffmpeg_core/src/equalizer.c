#include "equalizer.h"

int get_equalizer_precision(enum AVSampleFormat f) {
    switch (f) {
    case AV_SAMPLE_FMT_U8:
    case AV_SAMPLE_FMT_S16:
    case AV_SAMPLE_FMT_U8P:
    case AV_SAMPLE_FMT_S16P:
        return 0;
    case AV_SAMPLE_FMT_S32:
    case AV_SAMPLE_FMT_S32P:
        return 1;
    case AV_SAMPLE_FMT_DBL:
    case AV_SAMPLE_FMT_DBLP:
        return 3;
    case AV_SAMPLE_FMT_FLT:
    case AV_SAMPLE_FMT_FLTP:
    default:
        return 2;
    }
}

int create_equalizer_filter(AVFilterGraph* graph, AVFilterContext* src, c_linked_list** list, int channel, int gain, enum AVSampleFormat f) {
    if (!graph || !src || !list) return FFMPEG_CORE_ERR_NULLPTR;
    char args[128];
    char name[32];
    const AVFilter* eq = avfilter_get_by_name("equalizer");
    snprintf(args, sizeof(args), "f=%d:g=%d:r=%d", channel, gain, get_equalizer_precision(f));
    snprintf(name, sizeof(name), "equalizer%d", channel);
    int re = 0;
    AVFilterContext* context = NULL;
    if ((re = avfilter_graph_create_filter(&context, eq, name, args, NULL, graph)) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Failed to create equalizer filter \"%s\": %s (%i)\n", name, av_err2str(re), re);
        return re;
    }
    if (!c_linked_list_append(list, (void*)context)) {
        av_log(NULL, AV_LOG_FATAL, "Failed to append filter \"%s\" to list.\n", context->name);
        return FFMPEG_CORE_ERR_OOM;
    }
    if (c_linked_list_count(*list) > 1) {
        AVFilterContext* last = c_linked_list_tail(*list)->prev->d;
        if ((re = avfilter_link(last, 0, context, 0)) < 0) {
            av_log(NULL, AV_LOG_FATAL, "Failed to link %s:%i -> %s:%i: %s (%i)\n", last->name, 0, context->name, 0, av_err2str(re), re);
            return re;
        }
    } else {
        if ((re = avfilter_link(src, 0, context, 0)) < 0) {
            av_log(NULL, AV_LOG_FATAL, "Failed to link %s:%i -> %s:%i: %s (%i)\n", src->name, 0, context->name, 0, av_err2str(re), re);
            return re;
        }
    }
    return FFMPEG_CORE_ERR_OK;
}
