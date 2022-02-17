#include "equalizer_settings.h"

#include "dict.h"

#define INTYPE Dict<int, int>*
#define OUTTYPE EqualizerChannels*

void free_equalizer_channels(EqualizerChannels** channels) {
    auto c = (INTYPE*)channels;
    if (c) {
        auto c2 = *c;
        dict_free(c2);
        *channels = (OUTTYPE)c2;
    }
}

int set_equalizer_channel(EqualizerChannels** channels, int channel, int gain) {
    if (!channels) return FFMPEG_CORE_ERR_NULLPTR;
    auto c = (INTYPE)(*channels);
    if (gain == 0) {
        dict_delete(c, channel);
    } else {
        if (!dict_set(c, channel, gain)) {
            *channels = (OUTTYPE)c;
            return FFMPEG_CORE_ERR_OOM;
        }
    }
    *channels = (OUTTYPE)c;
    return FFMPEG_CORE_ERR_OK;
}

size_t equalizer_channel_count(EqualizerChannels* channels) {
    return dict_count((INTYPE)channels);
}
