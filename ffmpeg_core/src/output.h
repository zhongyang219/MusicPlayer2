#ifndef _MUSICPLAYER2_OUTPUT_H
#define _MUSICPLAYER2_OUTPUT_H
#if __cplusplus
extern "C" {
#endif
#include "core.h"
int init_output(MusicHandle* handle);
enum AVSampleFormat convert_to_sdl_supported_format(enum AVSampleFormat fmt);
void SDL_callback(void* userdata, uint8_t* stream, int len);
SDL_AudioFormat convert_to_sdl_format(enum AVSampleFormat fmt);
#if __cplusplus
}
#endif
#endif
