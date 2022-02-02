#ifndef _MUSICPLAYER2_OPEN_H
#define _MUSICPLAYER2_OPEN_H
#if __cplusplus
extern "C" {
#endif
#include "core.h"
/// 打开文件
int open_input(MusicHandle* handle, const char* url);
int find_audio_stream(MusicHandle* handle);
int open_input2(MusicInfoHandle* handle, const char* url);
int find_audio_stream2(MusicInfoHandle* handle);
#if __cplusplus
}
#endif
#endif
