#pragma once
#include "IPlayerCore.h"

class CMciCore :
    public IPlayerCore
{
    typedef DWORD   MCIERROR;       /* error return code, 0 means no error */

    //使用动态加载的方式加载MCI的API，如果没有使用MCI内核，则不加载
    typedef MCIERROR(WINAPI *_mciSendStringW)(LPCWSTR lpstrCommand, LPWSTR lpstrReturnString, UINT uReturnLength, HWND hwndCallback);

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

    virtual int GetCurPosition() override;
    virtual int GetSongLength() override;
    virtual void SetCurPosition(int position) override;
    virtual void GetAudioInfo(SongInfo& song_info, bool get_tag = true) override;
    virtual void GetAudioInfo(const wchar_t* file_path, SongInfo& song_info, bool get_tag = true) override;

    virtual bool IsMidi() override;
    virtual bool IsMidiConnotPlay() override;
    virtual std::wstring GetMidiInnerLyric() override;
    virtual MidiInfo GetMidiInfo() override;
    virtual bool MidiNoLyric() override;

    virtual void ApplyEqualizer(int channel, int gain) override;
    virtual void SetReverb(int mix, int time) override;
    virtual void ClearReverb() override;
    virtual void GetFFTData(float fft_data[128]) override;

    virtual int GetErrorCode() override;


private:
    std::wstring m_file_path;

    MCIERROR m_error_code{ 0 };

    //
    HMODULE m_dll_module{};
    _mciSendStringW mciSendStringW;
    bool m_successed;
};

