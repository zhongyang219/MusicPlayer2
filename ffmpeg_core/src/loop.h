#ifndef _MUSICPLAYER2_LOOP_H
#define _MUSICPLAYER2_LOOP_H
#if __cplusplus
extern "C" {
#endif
#include "core.h"
int seek_to_pos(MusicHandle* handle);
DWORD WINAPI event_loop(LPVOID handle);
#if __cplusplus
}
#endif
#endif
