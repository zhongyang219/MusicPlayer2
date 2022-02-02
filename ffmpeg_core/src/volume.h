#ifndef _MUSICPLAYER2_VOLUME_H
#define _MUSICPLAYER2_VOLUME_H
#if __cplusplus
extern "C" {
#endif
#include "core.h"
int get_volume_precision(enum AVSampleFormat f);
/**
 * @brief 创建 volume Filter
 * @param index Filter 序号
 * @param graph Graph
 * @param src 输入
 * @param list Filters列表
 * @param volume 声音大小百分比
 * @param f 输入的格式
 * @return 
*/
int create_volume_filter(int index, AVFilterGraph* graph, AVFilterContext* src, c_linked_list** list, int volume, enum AVSampleFormat f);
#if __cplusplus
}
#endif
#endif
