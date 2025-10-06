#pragma once

struct MidiInfo
{
    int midi_position;
    int midi_length;
    int speed;		//速度，bpm
    int tempo;		//每个四分音符的微秒数
    float ppqn;
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
#define MAX_PLAY_PITCH 12
#define MIN_PLAY_PITCH -12

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
    struct AudioInfo
    {
        int length{};
        int bitrate{};
        int freq{};
        int bits{};
        int channels{};
    };

    struct AudioTag
    {
        std::wstring title;
        std::wstring artist;
        std::wstring album;
        std::wstring comment;
        std::wstring genre;
        unsigned short year{};
        int track{};
    };

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
    virtual void SetPitch(int pitch) = 0;           //设置播放变调，半音为一个单位，[-12, 12]，0为原调
    virtual bool IsSpeedAvailable() = 0;            //设置播放速度是否可用
    virtual bool IsPitchAvailable() = 0;            //设置播放变调是否可用
    virtual bool SongIsOver() = 0;                  //曲目是否播放完毕

    virtual int GetCurPosition() = 0;               //获取当前播放进度，单位为毫秒
    virtual int GetSongLength() = 0;                //获取歌曲长度，单位为毫秒
    virtual void SetCurPosition(int position) = 0;  //设置播放进度，单位为毫秒

    /**
     * @brief   获取一个音频文件的音频信息和标签信息（需要支持并发且不影响当前播放）
     * @param[in]   file_path 文件路径
     * @param[out]   audio_info 保存音频信息
     * @param[out]   audio_tag 保存标签信息（主程序会优先通过Taglib获取标签信息，在Taglib无法获取到标签信息的情况下，才会使用这里的值）
     */
    virtual void GetAudioInfo(const wchar_t* file_path, AudioInfo* audio_info, AudioTag* audio_tag) = 0;

    /**
     * @brief   音频编码的回调函数
     * @param   int progress 转换的进度。0~100: 已完成的百分比; <0:出错，值为此文件中以CONVERT_ERROR_开头的宏定义
     */
    typedef void(*EncodeAudioProc)(int progress);

    /**
     * @brief   在转换格式时对音频进行编码
     * @param   const std::wstring& src_file_path 要转换的音频文件路径
     * @param   const wstring & dest_file_path 输出的文件路径
     * @param   EncodeFormat encode_type 转换的编码格式
     * @param   int dest_freq 转换的采样频率。如果为0，则不转换采样频率
     * @param   void * encode_para 编码参数。如果encode_type为ET_WAV，则encode_para应该被忽略；
                如果encode_type为ET_MP3，则encode_para为一个指向MP3EncodePara对象的指针；
                如果encode_type为ET_WMA，则encode_para为一个指向WmaEncodePara对象的指针；
                如果encode_type为ET_OGG，则encode_para为一个指向OggEncodePara对象的指针；
     * @param   EncodeAudioProc proc 通知转换进度的回调函数
     * @param   int start_pos 截取的起始位置（毫秒）（仅当end_pos > start_pos时有效）
     * @param   int end_pos 截取的结束位置（毫秒）（仅当end_pos > start_pos时有效）
     * @return  bool 转换成功返回true，否则返回false
     */
    virtual bool EncodeAudio(const std::wstring& src_file_path, const wstring& dest_file_path, EncodeFormat encode_format, void* encode_para, int dest_freq, EncodeAudioProc proc, int start_pos = 0, int end_pos = 0) = 0;
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
    virtual void ApplyReplayGain(float gain) = 0;           //设置响度均衡（gain为增益，取值一般在0附近）
    virtual void SetReverb(int mix, int time) = 0;		//设置混响（mix为混响强度，取值为0~100，time为混响时间，取值为1~300，单位为10ms）
    virtual void ClearReverb() = 0;			//关闭混响
    virtual void GetFFTData(float fft_data[FFT_SAMPLE]) = 0;       //获取频谱分析数据

    virtual int GetErrorCode() = 0;                         //获取错误代码
    virtual std::wstring GetErrorInfo(int error_code) = 0;  //根据错误代码获取错误信息
    virtual std::wstring GetErrorInfo() = 0;  //获取错误信息

    virtual PlayerCoreType GetCoreType() = 0;

    virtual bool IsVolumeFadingOut() { return false; }    //是否处于音量淡出状态
};
