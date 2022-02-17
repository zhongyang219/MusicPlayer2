#ifndef _MUSICPLAYER2_EQUALIZER_SETTINGS_H
#define _MUSICPLAYER2_EQUALIZER_SETTINGS_H
#if __cplusplus
#include "core.h"
extern "C" {
#endif
#if !__cplusplus
#include "core.h"
#endif
void free_equalizer_channels(EqualizerChannels** channels);
int set_equalizer_channel(EqualizerChannels** channels, int channel, int gain);
size_t equalizer_channel_count(EqualizerChannels* channels);
#if __cplusplus
}
#endif
#endif
