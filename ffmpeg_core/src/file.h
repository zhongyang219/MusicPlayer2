#ifndef _MUSICPLAYER2_FILE_H
#define _MUSICPLAYER2_FILE_H
#include "core.h"
#if __cplusplus
extern "C" {
#endif
int is_file(UrlParseResult* url);
int is_file_exists(MusicHandle* handle);
#if __cplusplus
}
#endif
#endif
