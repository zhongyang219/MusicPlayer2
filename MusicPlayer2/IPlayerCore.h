#pragma once
#include "SongInfo.h"

struct MidiInfo
{
    int midi_position;
    int midi_length;
    int speed;		//速度，bpm
    int tempo;		//每个四分音符的微秒数
    float ppqn;
};

enum AudioInfoFlag
{
    AF_LENGTH = 1,
    AF_BITRATE = 2,
    AF_TAG_INFO = 4,
    AF_CHANNEL_INFO = 8,

    AF_ALL = AF_LENGTH | AF_BITRATE | AF_TAG_INFO | AF_CHANNEL_INFO
};

enum PlayerCoreType
{
    PT_BASS,
    PT_MCI,
    PT_FFMPEG,
};

enum PlayingState       //正在播放标志
{
    PS_STOPED,          //已停止
    PS_PAUSED,          //已暂停
    PS_PLAYING          //正在播放
};

#define MAX_PLAY_SPEED 4.0f
#define MIN_PLAY_SPEED 0.1f

//MP3编码参数
struct MP3EncodePara
{
    int encode_type{ 0 };               // 0:CBR, 1:ABR, 2:VBR, （更改：<3:自定义参数>不再使用，但加载配置时保持兼容）
    int cbr_bitrate{ 128 };             // CBR比特率
    int abr_bitrate{ 128 };             // ABR比特率
    int vbr_quality{ 4 };               // VBR质量（0~9）
    bool joint_stereo{ true };
    bool user_define_para{ false };     // 使用用户自定义参数（此项不保存，当ini中cmd_para非空时加载为true）
    wstring cmd_para;                   // 命令行参数
};

//wma 编码参数
struct WmaEncodePara
{
    bool cbr{ true };			//true: CBR; false: VBR
    int cbr_bitrate{ 64 };		//CBR比特率，单位kbps
    int vbr_quality{ 75 };		//VBR质量
};

//ogg编码参数
struct OggEncodePara
{
    int encode_quality{ 3 };
};

//Flac 编码参数
struct FlacEncodePara
{
    int compression_level{ 8 }; //压缩级别，0~8，0：最小压缩，速度最快；8：最大压缩，速度最慢
    bool user_define_para{ false }; //用户自定义编码参数
    wstring cmd_para;		//命令行参数
};

//转换格式时的输出编码格式
enum class EncodeFormat { WAV, MP3, WMA, OGG, FLAC };

//格式转换错误代码
#define CONVERT_ERROR_FILE_CANNOT_OPEN (-1)	        //源文件无法读取
#define CONVERT_ERROR_ENCODE_CHANNEL_FAILED (-2)	//编码通道创建失败
#define CONVERT_ERROR_ENCODE_PARA_ERROR (-3)		//找不到编码器或编码器参数错误
#define CONVERT_ERROR_MIDI_NO_SF2 (-4)				//没有MIDI音色库
#define CONVERT_ERROR_WMA_NO_WMP9_OR_LATER (-5)     //没有安装 Windows Media Player 9 或更高版本。
#define CONVERT_ERROR_WMA_NO_SUPPORTED_ENCODER (-6) //无法找到可支持请求的采样格式和比特率的编解码器。


/////////////////////////////////////////////////////////////////////////////////////////////////

class IPlayerCore
{
public:
    virtual ~IPlayerCore() {}

    virtual void InitCore() = 0;
    virtual void UnInitCore() = 0;

    virtual std::wstring GetAudioType() = 0;    //获取音频格式的类型，如果返回空字符串，则会显示为文件的扩展名
    virtual int GetChannels() = 0;      //获取声道数
    virtual int GetFReq() = 0;          //获取采样频率，单位为Hz
    virtual std::wstring GetSoundFontName() = 0;    //播放midi音乐时，获取midi音色库的名称

    virtual void Open(const wchar_t* file_path) = 0;
    virtual void Close() = 0;
    virtual void Play() = 0;
    virtual void Pause() = 0;
    virtual void Stop() = 0;
    virtual void SetVolume(int volume) = 0;
    virtual void SetSpeed(float speed) = 0;         //设置播放速度（1为原速）
    virtual bool SongIsOver() = 0;                  //曲目是否播放完毕

    virtual int GetCurPosition() = 0;               //获取当前播放进度，单位为毫秒
    virtual int GetSongLength() = 0;                //获取歌曲长度，单位为毫秒
    virtual void SetCurPosition(int position) = 0;  //设置播放进度，单位为毫秒
    // 获取打开的音频的长度、比特率和标签信息，flag用于指定获取哪些信息
    virtual void GetAudioInfo(SongInfo& song_info, int flag = AF_LENGTH | AF_BITRATE | AF_TAG_INFO | AF_CHANNEL_INFO) = 0;
    // 获取file_path属性写入song_info，flag用于指定获取哪些信息（需要支持并发且不影响当前播放）
    // AF_LENGTH读取文件时长直接写入end_pos，AF_BITRATE读取比特率，AF_TAG_INFO读取标签/分级，AF_CHANNEL_INFO读取采样率/位深度/通道数
    virtual void GetAudioInfo(const wchar_t* file_path, SongInfo& song_info, int flag = AF_LENGTH | AF_BITRATE | AF_TAG_INFO | AF_CHANNEL_INFO) = 0;

    /**
     * @brief   音频编码的回调函数
     * @param   int progress 转换的进度。0~100: 已完成的百分比; <0:出错，值为此文件中以CONVERT_ERROR_开头的宏定义
     */
    typedef void(*EncodeAudioProc)(int progress);

    /**
     * @brief   在转换格式时对音频进行编码
     * @param   SongInfo song_info 要转换的音频文件信息
     * @param   const wstring & dest_file_path 输出的文件路径
     * @param   EncodeFormat encode_type 转换的编码格式
     * @param   int dest_freq 转换的采样频率。如果为0，则不转换采样频率
     * @param   void * encode_para 编码参数。如果encode_type为ET_WAV，则encode_para应该被忽略；
                如果encode_type为ET_MP3，则encode_para为一个指向MP3EncodePara对象的指针；
                如果encode_type为ET_WMA，则encode_para为一个指向WmaEncodePara对象的指针；
                如果encode_type为ET_OGG，则encode_para为一个指向OggEncodePara对象的指针；
     * @param   EncodeAudioProc proc 通知转换进度的回调函数
     * @return  bool 转换成功返回true，否则返回false
     */
    virtual bool EncodeAudio(SongInfo song_info, const wstring& dest_file_path, EncodeFormat encode_format, void* encode_para, int dest_freq, EncodeAudioProc proc) = 0;
    virtual bool InitEncoder() = 0;         //对编码器执行一些初始化工作，成功返回true，失败返回false
    virtual void UnInitEncoder() = 0;       //对编码器执行一些清理工作
    virtual bool IsFreqConvertAvailable() = 0;  //转换采样频率是否可用

    virtual bool IsMidi() = 0;
    virtual bool IsMidiConnotPlay() = 0;
    virtual MidiInfo GetMidiInfo() = 0;
    virtual std::wstring GetMidiInnerLyric() = 0;
    virtual bool MidiNoLyric() = 0;
    virtual PlayingState GetPlayingState() = 0;

    virtual void ApplyEqualizer(int channel, int gain) = 0; //设置均衡器（channel为均衡器通道，取值为0~9，gain为增益，取值为-15~15）
    virtual void SetReverb(int mix, int time) = 0;		//设置混响（mix为混响强度，取值为0~100，time为混响时间，取值为1~300，单位为10ms）
    virtual void ClearReverb() = 0;			//关闭混响
    virtual void GetFFTData(float fft_data[FFT_SAMPLE]) = 0;       //获取频谱分析数据

    virtual int GetErrorCode() = 0;                         //获取错误代码
    virtual std::wstring GetErrorInfo(int error_code) = 0;  //根据错误代码获取错误信息
    virtual std::wstring GetErrorInfo() = 0;  //获取错误信息

    virtual PlayerCoreType GetCoreType() = 0;

    virtual bool IsVolumeFadingOut() { return false; }    //是否处于音量淡出状态
};
