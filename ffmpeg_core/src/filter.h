#ifndef _MUSICPLAYER2_FILTER_H
#define _MUSICPLAYER2_FILTER_H
#if __cplusplus
extern "C" {
#endif
#include "core.h"
int need_filters(FfmpegCoreSettings* s);
int init_filters(MusicHandle* handle);
int reinit_filters(MusicHandle* handle);
/**
 * @brief 新建一个新的FilterGraph，并且分配好输出和输入
 * @param graph FilterGraph
 * @param src 输入节点
 * @param sink 输出节点
 * @param handle 读取必要的数据用
 * @return 
*/
int create_src_and_sink(AVFilterGraph** graph, AVFilterContext** src, AVFilterContext** sink, MusicHandle* handle);
/// 往filters_buffer塞数据
int add_data_to_filters_buffer(MusicHandle* handle);
void reset_filters_buffer(MusicHandle* handle);
#if __cplusplus
}
#endif
#endif
