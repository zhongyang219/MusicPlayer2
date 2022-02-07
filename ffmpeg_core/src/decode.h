#ifndef _MUSICPLAYER2_DECODE_H
#define _MUSICPLAYER2_DECODE_H
#if __cplusplus
extern "C" {
#endif
#include "core.h"
int open_decoder(MusicHandle* handle);
int reopen_decoder(MusicHandle* handle);
/**
 * @brief 解码
 * @param handle Handle
 * @param writed 是否成功往缓冲区添加数据
 * @return 非0如果发生错误
*/
int decode_audio(MusicHandle* handle, char* writed);
int convert_samples_and_add_to_fifo(MusicHandle* handle, AVFrame* frame, char* writed);
#if __cplusplus
}
#endif
#endif
