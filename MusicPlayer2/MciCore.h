#pragma once
#include "IPlayerCore.h"
#include "DllLib.h"

class CMciCore :
    public IPlayerCore, public CDllLib
{
    typedef DWORD   MCIERROR;       /* error return code, 0 means no error */

    //使用动态加载的方式加载MCI的API，如果没有使用MCI内核，则不加载
    typedef MCIERROR(WINAPI *_mciSendStringW)(LPCWSTR lpstrCommand, LPWSTR lpstrReturnString, UINT uReturnLength, HWND hwndCallback);
    typedef BOOL(WINAPI *_mciGetErrorStringW)(MCIERROR mcierr, LPWSTR pszText, UINT cchText);

public:
    CMciCore();
    ~CMciCore();

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
    virtual std::wstring GetMidiInnerLyric() override;
    virtual MidiInfo GetMidiInfo() override;
    virtual bool MidiNoLyric() override;
    virtual PlayingState GetPlayingState() override;

    virtual void ApplyEqualizer(int channel, int gain) override;
    virtual void SetReverb(int mix, int time) override;
    virtual void ClearReverb() override;
    virtual void GetFFTData(float fft_data[128]) override;

    virtual int GetErrorCode() override;
    virtual std::wstring GetErrorInfo(int error_code) override;
    virtual std::wstring GetErrorInfo() override;

    virtual PlayerCoreType GetCoreType() override { return PT_MCI; }
    virtual int GetDeviceCount() override;

private:
    void GetMidiPosition();
    int GetMciSongLength(const std::wstring& file_path);
    int GetMciBitrate(const std::wstring& file_path);

    virtual bool GetFunction() override;

private:
    std::wstring m_file_path;
    PlayingState m_playing{};
    std::wstring m_file_type;
    MidiInfo m_midi_info;

    MCIERROR m_error_code{ 0 };

    _mciSendStringW mciSendStringW;
    _mciGetErrorStringW mciGetErrorStringW;
};
