#ifndef _MUSICPLAYER2_EQUALIZER_H
#define _MUSICPLAYER2_EQUALIZER_H
#if __cplusplus
extern "C" {
#endif
#include "core.h"
int get_equalizer_precision(enum AVSampleFormat f);
/**
 * @brief 创建 equalizer filter
 * @param index Filter 序号
 * @param graph Graph
 * @param src Graph
 * @param list Filters 列表
 * @param channel 中心频率(hz)
 * @param gain 
 * @return 
*/
int create_equalizer_filter(AVFilterGraph* graph, AVFilterContext* src, c_linked_list** list, int channel, int gain, enum AVSampleFormat f);
#if __cplusplus
}
#endif
#endif
