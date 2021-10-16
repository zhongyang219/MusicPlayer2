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
    PT_MCI
};

enum PlayingState       //正在播放标志
{
    PS_STOPED,          //已停止
    PS_PAUSED,          //已暂停
    PS_PLAYING          //正在播放
};

#define MAX_PLAY_SPEED 4
#define MIN_PLAY_SPEED 0.1

class IPlayerCore
{
public:
    virtual ~IPlayerCore() {}

    virtual void InitCore() = 0;
    virtual void UnInitCore() = 0;
    virtual unsigned int GetHandle() = 0;

    virtual std::wstring GetAudioType() = 0;
    virtual int GetChannels() = 0;
    virtual int GetFReq() = 0;
    virtual std::wstring GetSoundFontName() = 0;

    virtual void Open(const wchar_t* file_path) = 0;
    virtual void Close() = 0;
    virtual void Play() = 0;
    virtual void Pause() = 0;
    virtual void Stop() = 0;
    virtual void SetVolume(int volume) = 0;
    virtual void SetSpeed(float speed) = 0;         //设置播放速度（1为原速）
    virtual bool SongIsOver() = 0;                  //曲目是否播放完毕

    virtual int GetCurPosition() = 0;
    virtual int GetSongLength() = 0;
    virtual void SetCurPosition(int position) = 0;
    virtual void GetAudioInfo(SongInfo& song_info, int flag = AF_LENGTH | AF_BITRATE | AF_TAG_INFO) = 0;        //获取打开的音频的长度、比特率和标签信息，flag用于指定获取哪些信息
    virtual void GetAudioInfo(const wchar_t* file_path, SongInfo& song_info, int flag = AF_LENGTH | AF_BITRATE | AF_TAG_INFO) = 0;        //获取指定音频文件的长度、比特率和标签信息

    virtual bool IsMidi() = 0;
    virtual bool IsMidiConnotPlay() = 0;
    virtual MidiInfo GetMidiInfo() = 0;
    virtual std::wstring GetMidiInnerLyric() = 0;
    virtual bool MidiNoLyric() = 0;
    virtual PlayingState GetPlayingState() = 0;

    virtual void ApplyEqualizer(int channel, int gain) = 0;
    virtual void SetReverb(int mix, int time) = 0;		//设置混响（mix为混响强度，取值为0~100，time为混响时间，取值为1~300，单位为10ms）
    virtual void ClearReverb() = 0;			//关闭混响
    virtual void GetFFTData(float fft_data[FFT_SAMPLE]) = 0;       //获取频谱分析数据

    virtual int GetErrorCode() = 0;                         //获取错误代码
    virtual std::wstring GetErrorInfo(int error_code) = 0;  //根据错误代码获取错误信息
    virtual std::wstring GetErrorInfo() = 0;  //获取错误信息

    virtual PlayerCoreType GetCoreType() = 0;

    virtual int GetDeviceCount() = 0;       //获取播放设备数量
};
