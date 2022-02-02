#ifndef _MUSICPLAYER2_FFMPEG_CORE_H
#define _MUSICPLAYER2_FFMPEG_CORE_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include <wchar.h>
#if BUILD_FFMPEG_CORE
#define FFMPEG_CORE_API __declspec(dllexport)
#else
#define FFMPEG_CORE_API __declspec(dllimport)
#endif
typedef struct MusicHandle MusicHandle;
typedef struct MusicInfoHandle MusicInfoHandle;
// 负数即为来自ffmpeg的错误

#define FFMPEG_CORE_ERR_OK 0
#define FFMPEG_CORE_ERR_NULLPTR 1
#define FFMPEG_CORE_ERR_INVAILD_NAME 2
#define FFMPEG_CORE_ERR_OOM 3
#define FFMPEG_CORE_ERR_NO_AUDIO_OR_DECODER 4
#define FFMPEG_CORE_ERR_UNKNOWN_SAMPLE_FMT 5
#define FFMPEG_CORE_ERR_SDL 6
#define FFMPEG_CORE_ERR_FAILED_CREATE_THREAD 7
#define FFMPEG_CORE_ERR_FAILED_CREATE_MUTEX 8
#define FFMPEG_CORE_ERR_WAIT_MUTEX_FAILED 9
#define FFMPEG_CORE_ERR_NO_AUDIO 10
FFMPEG_CORE_API void free_music_handle(MusicHandle* handle);
FFMPEG_CORE_API void free_music_info_handle(MusicInfoHandle* handle);
FFMPEG_CORE_API int ffmpeg_core_open(const wchar_t* url, MusicHandle** handle);
FFMPEG_CORE_API int ffmpeg_core_info_open(const wchar_t* url, MusicInfoHandle** handle);
FFMPEG_CORE_API int ffmpeg_core_play(MusicHandle* handle);
FFMPEG_CORE_API int ffmpeg_core_pause(MusicHandle* handle);
FFMPEG_CORE_API int ffmpeg_core_seek(MusicHandle* handle, int64_t time);
/**
 * @brief 获取当前播放位置
 * @param handle Handle
 * @return 如果Handle为NULL，返回-1，反之返回以AV_TIME_BASE为基准的时间（1相当于1/1000000s)
*/
FFMPEG_CORE_API int64_t ffmpeg_core_get_cur_position(MusicHandle* handle);
/**
 * @brief 是否已经播放完
 * @param handle Handle
 * @return 如果Handle为NULL或未播放完，返回0，反之返回1
*/
FFMPEG_CORE_API int ffmpeg_core_song_is_over(MusicHandle* handle);
/**
 * @brief 获取长度（由demuxer回报，可能不准或者不存在）
 * @param handle Handle
 * @return 如果Handle为NULL，返回-1，反之返回以AV_TIME_BASE为基准的时间（1相当于1/1000000s)
*/
FFMPEG_CORE_API int64_t ffmpeg_core_get_song_length(MusicHandle* handle);
FFMPEG_CORE_API int64_t ffmpeg_core_info_get_song_length(MusicInfoHandle* handle);
/**
 * @brief 返回音频文件声道数（SDL可能会改如果音频设备不支持）
 * @param handle Handle
 * @return 如果Handle为NULL，返回-1，反之返回声道数
*/
FFMPEG_CORE_API int ffmpeg_core_get_channels(MusicHandle* handle);
FFMPEG_CORE_API int ffmpeg_core_info_get_channels(MusicInfoHandle* handle);
/**
 * @brief 返回音频文件采样频率（SDL可能会修改）
 * @param handle Handle
 * @return 如果Handle为NULL，返回-1，反之返回采样频率
*/
FFMPEG_CORE_API int ffmpeg_core_get_freq(MusicHandle* handle);
FFMPEG_CORE_API int ffmpeg_core_info_get_freq(MusicInfoHandle* handle);
/**
 * @brief 返回当前状态
 * @param handle Handle
 * @return 如果Handle为NULL，返回-1，正在播放返回1，未播放返回0
*/
FFMPEG_CORE_API int ffmpeg_core_is_playing(MusicHandle* handle);
/**
 * @brief 返回位数
 * @param handle Handle
 * @return 如果Handle为NULL，返回-1
*/
FFMPEG_CORE_API int ffmpeg_core_get_bits(MusicHandle* handle);
FFMPEG_CORE_API int ffmpeg_core_info_get_bits(MusicInfoHandle* handle);
/**
 * @brief 返回比特率
 * @param handle Handle
 * @return 如果Handle为NULL，返回-1
*/
FFMPEG_CORE_API int64_t ffmpeg_core_get_bitrate(MusicHandle* handle);
FFMPEG_CORE_API int64_t ffmpeg_core_info_get_bitrate(MusicInfoHandle* handle);
/**
 * @brief 获取元数据 
 * @param handle Handle
 * @param key 元数据Key
 * @return 结果，需要手动调用free释放内存
*/
FFMPEG_CORE_API wchar_t* ffmpeg_core_get_metadata(MusicHandle* handle, const char* key);
FFMPEG_CORE_API wchar_t* ffmpeg_core_info_get_metadata(MusicInfoHandle* handle, const char* key);
#ifdef __cplusplus
}
#endif
#endif
