#ifndef _MUSICPLAYER2_CDA_H
#define _MUSICPLAYER2_CDA_H
#if __cplusplus
extern "C" {
#endif
#include "core.h"
int is_cda_file(const char* url);
int read_cda_file(MusicHandle* handle, const char* url);
int read_cda_file2(MusicInfoHandle* handle, const char* url);
const AVInputFormat* find_libcdio();
int open_cd_device(MusicHandle* handle, const char* device);
int open_cd_device2(MusicInfoHandle* handle, const char* device);
int64_t get_cda_duration(CDAData* d);
#if __cplusplus
}
#endif
#endif
