#pragma once
#include "IPlayerCore.h"
#include "bass.h"
#include "BASSMidiLibrary.h"

class CBassCore :
    public IPlayerCore
{
public:
    CBassCore();
    ~CBassCore();

    virtual void InitCore() override;
    virtual void UnInitCore() override;
    virtual unsigned int GetHandle() override;

    virtual std::wstring GetAudioType() override;
    virtual int GetChannels() override;
    virtual int GetFReq() override;
    virtual const wstring& GetSoundFontName() override;

    virtual void Open(const wchar_t* file_path) override;
    virtual void Close() override;
    virtual void Play() override;
    virtual void Pause() override;
    virtual void Stop() override;
    virtual void SetVolume(int volume) override;

    virtual int GetCurPosition() override;
    virtual int GetSongLength() override;
    virtual void SetCurPosition(int position) override;

    virtual bool IsMidi() override;
    virtual bool IsMidiConnotPlay() override;
    virtual std::wstring GetMidiInnerLyric() override;
    virtual MidiInfo GetMidiInfo() override;
    virtual bool MidiNoLyric() override;

    virtual void ApplyEqualizer(int channel, int gain) override;
    virtual void SetReverb(int mix, int time) override;
    virtual void ClearReverb() override;

    struct MidiLyricInfo 
    {
        wstring midi_lyric;
        bool midi_no_lyric;
    };

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
    vector<HPLUGIN> m_plugin_handles;		//插件的句柄
    wstring m_sfont_name;		//MIDI音色库的名称
    static MidiLyricInfo m_midi_lyric;
    BASS_CHANNELINFO m_channel_info;	//音频通道的信息
    bool m_is_midi;
    MidiInfo m_midi_info;

    int m_equ_handle[EQU_CH_NUM]{};		//均衡器通道的句柄
    int m_reverb_handle{};		//混响的句柄
    const float FREQ_TABLE[EQU_CH_NUM]{ 80, 125, 250, 500, 1000, 1500, 2000, 4000, 8000, 1600 };		//每个均衡器通道的中心频率

};

