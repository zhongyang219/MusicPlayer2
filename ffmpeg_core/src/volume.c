#include "volume.h"

int get_volume_precision(enum AVSampleFormat f) {
    switch (f) {
        case AV_SAMPLE_FMT_U8:
        case AV_SAMPLE_FMT_S16:
        case AV_SAMPLE_FMT_S32:
        case AV_SAMPLE_FMT_U8P:
        case AV_SAMPLE_FMT_S16P:
        case AV_SAMPLE_FMT_S32P:
            return 0;
        case AV_SAMPLE_FMT_DBL:
        case AV_SAMPLE_FMT_DBLP:
            return 2;
        case AV_SAMPLE_FMT_FLT:
        case AV_SAMPLE_FMT_FLTP:
        default:
            return 1;
    }
}

int create_volume_filter(int index, AVFilterGraph* graph, AVFilterContext* src, c_linked_list** list, int volume, enum AVSampleFormat f) {
    if (!graph || !src || !list) return FFMPEG_CORE_ERR_NULLPTR;
    char args[128];
    char name[32];
    const AVFilter* vol = avfilter_get_by_name("volume");
    snprintf(args, sizeof(args), "volume=%.2f:precision=%d", volume / 100.0, get_volume_precision(f));
    snprintf(name, sizeof(name), "volume%d", index);
    int re = 0;
    AVFilterContext* context = NULL;
    if ((re = avfilter_graph_create_filter(&context, vol, name, args, NULL, graph)) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Failed to create volume filter \"%s\": %s (%i)\n", name, av_err2str(re), re);
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
