#pragma once

struct MidiInfo
{
    int midi_position;
    int midi_length;
    int speed;		//速度，bpm
    int tempo;		//每个四分音符的微秒数
    float ppqn;
};


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
    virtual const wstring& GetSoundFontName() = 0;

    virtual void Open(const wchar_t* file_path) = 0;
    virtual void Close() = 0;
    virtual void Play() = 0;
    virtual void Pause() = 0;
    virtual void Stop() = 0;
    virtual void SetVolume(int volume) = 0;

    virtual int GetCurPosition() = 0;
    virtual int GetSongLength() = 0;
    virtual void SetCurPosition(int position) = 0;

    virtual bool IsMidi() = 0;
    virtual bool IsMidiConnotPlay() = 0;
    virtual MidiInfo GetMidiInfo() = 0;
    virtual std::wstring GetMidiInnerLyric() = 0;
    virtual bool MidiNoLyric() = 0;

    virtual void ApplyEqualizer(int channel, int gain) = 0;
    virtual void SetReverb(int mix, int time) = 0;		//设置混响（mix为混响强度，取值为0~100，time为混响时间，取值为1~300，单位为10ms）
    virtual void ClearReverb() = 0;			//关闭混响

};