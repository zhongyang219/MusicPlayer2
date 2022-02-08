#include "speed.h"

#include <math.h>

int get_speed(float speed) {
    return roundf(speed * 1000);
}

int create_speed_filter(int index, AVFilterGraph* graph, AVFilterContext* src, c_linked_list** list, int* speed) {
    if (!graph || !speed || !src || !list) return FFMPEG_CORE_ERR_NULLPTR;
    char args[64];
    char name[32];
    const AVFilter* atempo = avfilter_get_by_name("atempo");
    int speed_now = min(max(*speed, 500), 2000);
    *speed = 1000 * (*speed) / speed_now;
    snprintf(args, sizeof(args), "tempo=%.3f", speed_now / 1000.0);
    snprintf(name, sizeof(name), "atempo%d", index);
    int re = 0;
    AVFilterContext* context = NULL;
    if ((re = avfilter_graph_create_filter(&context, atempo, name, args, NULL, graph)) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Failed to create speed filter \"%s\": %s (%i)\n", name, av_err2str(re), re);
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
