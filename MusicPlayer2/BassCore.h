﻿#pragma once
#include "IPlayerCore.h"
#include "bass.h"
#include "BASSMidiLibrary.h"
#include "PlayTime.h"
#include "BASSEncodeLibrary.h"
#include "BASSWmaLibrary.h"
#include "BassMixLibrary.h"

#define PLAYING_STATE_DEFAULT_VALUE 99

class CBassCore :
    public IPlayerCore
{
public:
    CBassCore();
    ~CBassCore();

    virtual void InitCore() override;
    virtual void UnInitCore() override;
    unsigned int GetHandle();

    virtual std::wstring GetAudioType() override;
    virtual int GetChannels() override;
    virtual int GetFReq() override;
    virtual wstring GetSoundFontName() override;

    virtual void Open(const wchar_t* file_path) override;
    virtual void Close() override;
    virtual void Play() override;
    virtual void Pause() override;
    virtual void Stop() override;
    virtual void SetVolume(int volume) override;
    virtual void SetSpeed(float speed) override;
    virtual void SetPitch(int pitch) override;
    virtual bool SongIsOver() override;

    virtual int GetCurPosition() override;
    virtual int GetSongLength() override;
    virtual void SetCurPosition(int position) override;
    virtual void GetAudioInfo(const wchar_t* file_path, AudioInfo* audio_info, AudioTag* audio_tag) override;

    virtual bool EncodeAudio(const std::wstring& src_file_path, const wstring& dest_file_path, EncodeFormat encode_format, void* encode_para, int dest_freq, EncodeAudioProc proc, int start_pos = 0, int end_pos = 0) override;
    virtual bool InitEncoder() override;
    virtual void UnInitEncoder() override;
    virtual bool IsFreqConvertAvailable() override;

    virtual bool IsMidi() override;
    virtual bool IsMidiConnotPlay() override;
    virtual std::wstring GetMidiInnerLyric() override;
    virtual MidiInfo GetMidiInfo() override;
    virtual bool MidiNoLyric() override;
    virtual PlayingState GetPlayingState() override;

    struct MidiLyricInfo
    {
        wstring midi_lyric;
        bool midi_no_lyric;
    };

    virtual void ApplyEqualizer(int channel, int gain) override;
    void ApplyReplayGain(float gain);
    virtual void SetReverb(int mix, int time) override;
    virtual void ClearReverb() override;
    virtual void GetFFTData(float fft_data[FFT_SAMPLE]) override;

    virtual int GetErrorCode() override;
    virtual std::wstring GetErrorInfo(int error_code) override;
    virtual std::wstring GetErrorInfo() override;

    virtual PlayerCoreType GetCoreType() override { return PT_BASS; }

    virtual bool IsVolumeFadingOut() override;

    static int GetBASSCurrentPosition(HSTREAM hStream);
    static CPlayTime GetBASSSongLength(HSTREAM hStream);
    static void SetCurrentPosition(HSTREAM hStream, int position);

    wstring GetEncoderDir() { return m_encode_dir; }

public:
    static CBASSMidiLibrary m_bass_midi_lib;
    static BASS_MIDI_FONT m_sfont;	//MIDI音色库

private:
    //获取Midi音乐内嵌歌词的回调函数
    static void CALLBACK MidiLyricSync(HSYNC handle, DWORD channel, DWORD data, void *user);
    static void CALLBACK MidiEndSync(HSYNC handle, DWORD channel, DWORD data, void *user);

    void GetMidiPosition();
    void SetFXHandle();		//设置音效句柄
    void RemoveFXHandle();		//移除音效句柄

private:
    HSTREAM m_musicStream{};		//当前的音频句柄
    wstring m_file_path;            //当前音频文件的路径
    vector<HPLUGIN> m_plugin_handles;		//插件的句柄
    wstring m_sfont_name;		//MIDI音色库的名称
    static MidiLyricInfo m_midi_lyric;
    BASS_CHANNELINFO m_channel_info{};	//音频通道的信息
    bool m_is_midi{};
    MidiInfo m_midi_info{};
    int m_error_code{};
    int m_last_playing_state{ PLAYING_STATE_DEFAULT_VALUE };     //保存上一次的播放状态（BASS_ChannelIsActive的返回值）

    int m_equ_handle[EQU_CH_NUM]{};		//均衡器通道的句柄
    int m_reverb_handle{};		//混响的句柄
    int m_replaygain_handle{};  // 响度均衡的句柄
    const float FREQ_TABLE[EQU_CH_NUM]{ 80, 125, 250, 500, 1000, 1500, 2000, 4000, 8000, 1600 };		//每个均衡器通道的中心频率

    int m_volume{60};

    enum { FADE_TIMER_ID = 1010 };

    static CCriticalSection m_critical;
    PlayingState m_playing_state{};

    static CBASSEncodeLibrary m_bass_encode_lib;
    static CBASSWmaLibrary m_bass_wma_lib;
    static CBassMixLibrary m_bass_mix_lib;

    wstring m_encode_dir;

    static bool m_fading;        //音量是否处于淡入淡出状态
    };
