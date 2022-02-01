#pragma once
#include "IPlayerCore.h"
#include "DllLib.h"

typedef struct MusicHandle MusicHandle;
typedef void(*_free_music_handle)(MusicHandle*);
typedef int(*_ffmpeg_core_open)(const wchar_t*, MusicHandle**);
typedef int(*_ffmpeg_core_play)(MusicHandle*);
typedef int(*_ffmpeg_core_pause)(MusicHandle*);
typedef int(*_ffmpeg_core_seek)(MusicHandle*, int64_t);
typedef int64_t(*_ffmpeg_core_get_cur_position)(MusicHandle*);
typedef int(*_ffmpeg_core_song_is_over)(MusicHandle*);
typedef int64_t(*_ffmpeg_core_get_song_length)(MusicHandle*);
typedef int(*_ffmpeg_core_get_channels)(MusicHandle*);
typedef int(*_ffmpeg_core_get_freq)(MusicHandle*);
typedef int(*_ffmpeg_core_is_playing)(MusicHandle*);
typedef int(*_ffmpeg_core_get_bits)(MusicHandle*);

class CFfmpegCore : public IPlayerCore, public CDllLib {
public:
    CFfmpegCore();
    ~CFfmpegCore();
    virtual void InitCore() override;
    virtual void UnInitCore() override;
    virtual unsigned int GetHandle() override;
    virtual std::wstring GetAudioType() override;
    virtual int GetChannels() override;
    virtual int GetFReq() override;
    virtual std::wstring GetSoundFontName() override;
    virtual void Open(const wchar_t* file_path) override;
    virtual void Close() override;
    virtual void Play() override;
    virtual void Pause() override;
    virtual void Stop() override;
    virtual void SetVolume(int volume) override;
    virtual void SetSpeed(float speed) override;
    virtual bool SongIsOver() override;
    virtual int GetCurPosition() override;
    virtual int GetSongLength() override;
    virtual void SetCurPosition(int position) override;
    virtual void GetAudioInfo(SongInfo& song_info, int flag = AF_LENGTH | AF_BITRATE | AF_TAG_INFO) override;
    virtual void GetAudioInfo(const wchar_t* file_path, SongInfo& song_info, int flag = AF_LENGTH | AF_BITRATE | AF_TAG_INFO) override;
    virtual bool IsMidi() override;
    virtual bool IsMidiConnotPlay() override;
    virtual MidiInfo GetMidiInfo() override;
    virtual std::wstring GetMidiInnerLyric() override;
    virtual bool MidiNoLyric() override;
    virtual PlayingState GetPlayingState() override;
    virtual void ApplyEqualizer(int channel, int gain) override;
    virtual void SetReverb(int mix, int time) override;
    virtual void ClearReverb() override;
    virtual void GetFFTData(float fft_data[FFT_SAMPLE]) override;
    virtual int GetErrorCode() override;
    virtual std::wstring GetErrorInfo(int error_code) override;
    virtual std::wstring GetErrorInfo() override;
    virtual PlayerCoreType GetCoreType() override;
    virtual int GetDeviceCount() override;
private:
    virtual bool GetFunction() override;
    _free_music_handle free_music_handle;
    _ffmpeg_core_open ffmpeg_core_open;
    _ffmpeg_core_play ffmpeg_core_play;
    _ffmpeg_core_pause ffmpeg_core_pause;
    _ffmpeg_core_seek ffmpeg_core_seek;
    _ffmpeg_core_get_cur_position ffmpeg_core_get_cur_position;
    _ffmpeg_core_song_is_over ffmpeg_core_song_is_over;
    _ffmpeg_core_get_song_length ffmpeg_core_get_song_length;
    _ffmpeg_core_get_channels ffmpeg_core_get_channels;
    _ffmpeg_core_get_freq ffmpeg_core_get_freq;
    _ffmpeg_core_is_playing ffmpeg_core_is_playing;
    _ffmpeg_core_get_bits ffmpeg_core_get_bits;
    MusicHandle* handle;
    int err;
};
