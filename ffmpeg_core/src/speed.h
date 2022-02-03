#ifndef _MUSICPLAYER2_SPEED_H
#define _MUSICPLAYER2_SPEED_H
#if __cplusplus
extern "C" {
#endif
#include "core.h"
/// 将speed转为1000为1.0倍的数字，例如1.5倍转为1500
int get_speed(float speed);
/**
 * @brief 创建 atempo Filter
 * @param index Filter 序号
 * @param graph Graph
 * @param src 输入
 * @param list Filters列表
 * @param speed  指向目标速度的指针，会返回还需要设置的速度
 * @return
*/
int create_speed_filter(int index, AVFilterGraph* graph, AVFilterContext* src, c_linked_list** list, int* speed);
#if __cplusplus
}
#endif
#endif
