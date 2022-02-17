#ifndef _MUSICPLAYER2_CORE_H
#define _MUSICPLAYER2_CORE_H
#if __cplusplus
#include <string>
extern "C" {
#endif
#include "../ffmpeg_core.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "libavutil/avutil.h"
#include "libavutil/audio_fifo.h"
#include "libavutil/opt.h"
#include "libavutil/rational.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#include "libswresample/swresample.h"
#include "SDL2/SDL.h"
#include <Windows.h>
#include "c_linked_list.h"
#include "urlparse.h"

#ifndef __cplusplus
#ifndef min
#define min(x,y) (((x) < (y)) ? (x) : (y))
#endif
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#endif

#define FFT_SAMPLE 1024

typedef struct CDAData {
/// version of the CD format. In May 2006, always equal to 1.
uint16_t cd_format_version;
/// number of the range. The first track has the number 1.
uint16_t no;
/// range offset, in number of frames.
uint32_t range_offset;
/// duration of the track, total number of frames
uint32_t duration;
} CDAData;
typedef struct EqualizerChannel {
/// The filter’s central frequency in Hz.
int channel;
/// The required gain or attenuation in dB.
int gain;
} EqualizerChannel;
typedef struct EqualizerChannels {
EqualizerChannel d;
struct EqualizerChannels* prev;
struct EqualizerChannels* next;
} EqualizerChannels;
typedef struct MusicHandle {
/// Demux 用
AVFormatContext* fmt;
/// 要解码的流
AVStream* is;
/// 解码器类型
const AVCodec* codec;
/// 解码器
AVCodecContext* decoder;
/// 用于转换音频格式
struct SwrContext* swrac;
/// 指定的SDL输出格式
SDL_AudioSpec sdl_spec;
/// 事件处理线程
HANDLE thread;
/// 事件处理线程线程ID
DWORD thread_id;
/// 维护filters处理后缓冲区线程
HANDLE filter_thread;
/// 维护filters处理后缓冲区线程线程ID
DWORD filter_thread_id;
/// 音频缓冲区
AVAudioFifo* buffer;
/// 经过filters处理后的缓冲区
AVAudioFifo* filters_buffer;
/// 输出格式
enum AVSampleFormat target_format;
/// 每样本的字节数
int target_format_pbytes;
/// SDL音频设备ID
SDL_AudioDeviceID device_id;
/// 错误信息（ffmpeg错误或Core错误
int err;
/// Mutex对象，作为线程锁（用于保护缓冲区和时间）
HANDLE mutex;
/// 用来确保filter graph对象可用
HANDLE mutex2;
/// 缓冲区开始时间
int64_t pts;
/// 缓冲区结束时间
int64_t end_pts;
/// 第一个sample的pts
int64_t first_pts;
int64_t seek_pos;
/// 要播放的部分的开始时间（相对于first_pts的偏移量）
int64_t part_start_pts;
/// 要播放的部分的结束时间（相对于first_pts的偏移量）
int64_t part_end_pts;
/// 设置
FfmpegCoreSettings* s;
/// 用于设置filter
AVFilterGraph* graph;
/// filter 输入口
AVFilterContext* filter_inp;
/// filter 输出口
AVFilterContext* filter_out;
/// filter 链
c_linked_list* filters;
/// CDA 文件信息
CDAData* cda;
/// 输出时的声道布局
uint64_t output_channel_layout;
/// 播放地址
char* url;
/// 解析后的播放地址
UrlParseResult* parsed_url;
/// 当前重新打开次数
int retry_count;
/// 最近一个包的时间
int64_t last_pkt_pts;
/// 当去filters链从buffer读入的数据量（仅复杂的filters链）
int filters_buffer_offset;
/// SDL是否被初始化
unsigned char sdl_initialized : 1;
/// 让事件处理线程退出标志位
unsigned char stoping : 1;
/// 是否已读到文件尾部/读取位置达到要播放的部分的结束时间
unsigned char is_eof : 1;
/// 是否有错误
unsigned char have_err : 1;
/// 是否需要Seek
unsigned char is_seek : 1;
/// 是否需要设置新的缓冲区时间
unsigned char set_new_pts : 1;
unsigned char is_playing : 1;
/// 设置是内部分配
unsigned char settings_is_alloc : 1;
/// 需要设置新的filters链
unsigned char need_reinit_filters : 1;
/// 是否正在播放CDA文件
unsigned char is_cda : 1;
/// 是否仅播放一部分内容
unsigned char only_part : 1;
/// 是否为本地文件
unsigned char is_file : 1;
/// 是否正在重新打开文件
unsigned char is_reopen : 1;
/// 是否是简单的filters链
unsigned char is_easy_filters : 1;
/// 刚初始化完复杂的filters，等待filters填充数据
unsigned char is_wait_filters : 1;
} MusicHandle;
typedef struct MusicInfoHandle {
AVFormatContext* fmt;
AVStream* is;
CDAData* cda;
} MusicInfoHandle;
typedef struct FfmpegCoreSettings {
/// 音量
int volume;
/// 速度
float speed;
/// 缓存长度（单位s）
int cache_length;
/// 最大重试次数
int max_retry_count;
/// 非本地文件重试间隔时间（单位s）
int url_retry_interval;
/// 均衡器
EqualizerChannels* equalizer_channels;
} FfmpegCoreSettings;
#if __cplusplus
}
std::wstring get_metadata_str(AVDictionary* dict, const char* key, int flags);
#endif
#endif
