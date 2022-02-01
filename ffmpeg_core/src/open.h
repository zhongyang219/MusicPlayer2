#ifndef _MUSICPLAYER2_OPEN_H
#define _MUSICPLAYER2_OPEN_H
#if __cplusplus
extern "C" {
#endif
#include "core.h"
/// 打开文件
int open_input(MusicHandle* handle, const char* url);
int find_audio_stream(MusicHandle* handle);
#if __cplusplus
}
#endif
#endif
