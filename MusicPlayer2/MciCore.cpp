#include "stdafx.h"
#include "MciCore.h"
#include "AudioCommon.h"
#include "MusicPlayer2.h"


CMciCore::CMciCore()
{
    CDllLib::Init(L"winmm.dll");

    if (!IsSucceed())
    {
        CString strInfo = CCommon::LoadText(IDS_MCI_INIT_FAILED);
        theApp.WriteLog(wstring(strInfo));
    }
}


CMciCore::~CMciCore()
{
    //if (m_dll_module != NULL)
    //{
    //    FreeLibrary(m_dll_module);
    //    m_dll_module = NULL;
    //}
    CDllLib::UnInit();
}

bool CMciCore::GetFunction()
{
    bool rtn = true;
    //获取函数入口
    mciSendStringW = (_mciSendStringW)::GetProcAddress(m_dll_module, "mciSendStringW");
    mciGetErrorStringW = (_mciGetErrorStringW)::GetProcAddress(m_dll_module, "mciGetErrorStringW");
    //判断是否成功
    rtn &= (mciSendStringW != NULL);
    rtn &= (mciGetErrorStringW != NULL);
    return rtn;
}

void CMciCore::InitCore()
{
    //向支持的文件列表插入原生支持的文件格式
    CAudioCommon::m_surpported_format.clear();
    SupportedFormat format;
    format.description = CCommon::LoadText(IDS_BASIC_AUDIO_FORMAT);
    format.extensions.push_back(L"mp3");
    format.extensions.push_back(L"wma");
    format.extensions.push_back(L"wav");
    format.extensions.push_back(L"mid");
    format.extensions_list = L"*.mp3;*.wma;*.wav;*.mid";
    CAudioCommon::m_surpported_format.push_back(format);
    CAudioCommon::m_all_surpported_extensions = format.extensions;

}

void CMciCore::UnInitCore()
{
    Stop();
    Close();
}

unsigned int CMciCore::GetHandle()
{
    return 0;
}

std::wstring CMciCore::GetAudioType()
{
    return std::wstring();
}

int CMciCore::GetChannels()
{
    return 0;
}

int CMciCore::GetFReq()
{
    return 0;
}

wstring CMciCore::GetSoundFontName()
{
    return wstring();
}

void CMciCore::Open(const wchar_t * file_path)
{
    m_file_path = file_path;
    CFilePathHelper path_helper{ file_path };
    m_file_type = path_helper.GetFileExtension();
    if(m_success)
    {
        m_error_code = mciSendStringW((L"open \"" + m_file_path + L"\"").c_str(), NULL, 0, 0);

        //获取MIDI信息
        if (IsMidi())
        {
            wchar_t buff[16];
            m_error_code = mciSendStringW((L"status \"" + m_file_path + L"\" length").c_str(), buff, 15, 0);
            m_midi_info.midi_length = _wtoi(buff) / 4;
            m_error_code = mciSendStringW((L"status \"" + m_file_path + L"\" tempo").c_str(), buff, 15, 0);
            m_midi_info.speed = _wtoi(buff);
            if(m_midi_info.speed > 0)
                m_midi_info.tempo = 60000000 / m_midi_info.speed;
        }
    }

}

void CMciCore::Close()
{
    if (m_success)
    {
        m_error_code = mciSendStringW((L"close \"" + m_file_path + L"\"").c_str(), NULL, 0, 0);
        m_playing = PS_STOPED;
    }
}

void CMciCore::Play()
{
    if (m_success)
    {
        m_error_code = mciSendStringW((L"play \"" + m_file_path + L"\"").c_str(), NULL, 0, 0);
        m_playing = PS_PLAYING;
    }
}

void CMciCore::Pause()
{
    if (m_success)
    {
        m_error_code = mciSendStringW((L"pause \"" + m_file_path + L"\"").c_str(), NULL, 0, 0);
        m_playing = PS_PAUSED;
    }
}

void CMciCore::Stop()
{
    if (m_success)
    {
        m_error_code = mciSendStringW((L"stop \"" + m_file_path + L"\"").c_str(), NULL, 0, 0);
        m_playing = PS_STOPED;
        SetCurPosition(0);
    }
}

void CMciCore::SetVolume(int volume)
{
    if (m_success && !IsMidi())
    {
        wchar_t buff[16];
        _itow_s(volume * 10, buff, 10);		//设置音量100%时为1000
        m_error_code = mciSendStringW((L"setaudio \"" + m_file_path + L"\" volume to " + buff).c_str(), NULL, 0, 0);
    }
}

void CMciCore::SetSpeed(float speed)
{
}

bool CMciCore::SongIsOver()
{
    return false;
}

int CMciCore::GetCurPosition()
{
    if (m_success)
    {
        wchar_t buff[16];
        m_error_code = mciSendStringW((L"status \"" + m_file_path + L"\" position").c_str(), buff, 15, 0);
        int position = _wtoi(buff);
        if(IsMidi())        //如果是MIDI，MCI获取到的长度并不是毫秒数，而是MIDI的节拍数
        {
            m_midi_info.midi_position = position / 4;
            position = position * m_midi_info.speed;
        }
        return position;
    }
    return 0;
}

int CMciCore::GetSongLength()
{
    if (m_success)
    {
        return GetMciSongLength(m_file_path);
    }
    return 1;
}

void CMciCore::SetCurPosition(int position)
{
    if (m_success)
    {
        if (IsMidi())
        {
            if (m_midi_info.speed > 0)
                position = position / m_midi_info.speed;
        }

        wchar_t buff[16];
        _itow_s(position, buff, 10);
        m_error_code = mciSendStringW((L"seek \"" + m_file_path + L"\" to " + buff).c_str(), NULL, 0, 0);		//定位到新的位置
        if (m_playing)
            m_error_code = mciSendStringW((L"play \"" + m_file_path + L"\"").c_str(), NULL, 0, 0);		//继续播放

    }
}

void CMciCore::GetAudioInfo(SongInfo & song_info, int flag)
{
    if (m_success)
    {
        if (flag&AF_LENGTH)
            song_info.lengh = GetMciSongLength(song_info.file_path);
        if (flag&AF_BITRATE)
            song_info.bitrate = GetMciBitrate(song_info.file_path);
        if (flag&AF_TAG_INFO)
        {
            CAudioTag audio_tag(song_info);
            audio_tag.GetAudioTag();
        }
    }

}

void CMciCore::GetAudioInfo(const wchar_t * file_path, SongInfo & song_info, int flag)
{
    if (m_success && ((flag&AF_LENGTH) || (flag&AF_BITRATE)))
    {
        m_error_code = mciSendStringW((L"open \"" + wstring(file_path) + L"\"").c_str(), NULL, 0, 0);
        if (flag&AF_LENGTH)
        {
            wchar_t buff[16];
            m_error_code = mciSendStringW((L"status \"" + wstring(file_path) + L"\" length").c_str(), buff, 15, 0);		//获取当前歌曲的长度，并储存在buff数组里
            song_info.lengh = _wtoi(buff);
        }
        if (flag&AF_BITRATE)
            song_info.bitrate = GetMciBitrate(file_path);
        if (flag&AF_TAG_INFO)
        {
            CAudioTag audio_tag(song_info);
            audio_tag.GetAudioTag();
        }

        m_error_code = mciSendStringW((L"close \"" + wstring(file_path) + L"\"").c_str(), NULL, 0, 0);
    }
}

bool CMciCore::IsMidi()
{
    return m_file_type==L"mid" || m_file_type == L"midi";
}

bool CMciCore::IsMidiConnotPlay()
{
    return false;
}

std::wstring CMciCore::GetMidiInnerLyric()
{
    return std::wstring();
}

MidiInfo CMciCore::GetMidiInfo()
{
    return m_midi_info;
}

bool CMciCore::MidiNoLyric()
{
    return true;
}

PlayingState CMciCore::GetPlayingState()
{
    return m_playing;
}

void CMciCore::ApplyEqualizer(int channel, int gain)
{
}

void CMciCore::SetReverb(int mix, int time)
{
}

void CMciCore::ClearReverb()
{
}

void CMciCore::GetFFTData(float fft_data[128])
{
    memset(fft_data, 0, sizeof(fft_data));
}

int CMciCore::GetErrorCode()
{
    return m_error_code;
}

std::wstring CMciCore::GetErrorInfo(int error_code)
{
    wchar_t buff[128]{};
    mciGetErrorStringW(error_code, buff, sizeof(buff) / sizeof(wchar_t));		//根据错误代码获取错误信息
    return L"MCI: " + wstring(buff) + m_file_path;
}

std::wstring CMciCore::GetErrorInfo()
{
    wchar_t buff[128]{};
    mciGetErrorStringW(m_error_code, buff, sizeof(buff) / sizeof(wchar_t));
    return buff;
}

int CMciCore::GetDeviceCount()
{
    return 0;
}

void CMciCore::GetMidiPosition()
{
    if (IsMidi())
    {
        wchar_t buff[16];
        m_error_code = mciSendStringW((L"status \"" + m_file_path + L"\" position").c_str(), buff, 15, 0);
        m_midi_info.midi_position = _wtoi(buff) / 4;
    }
}

int CMciCore::GetMciSongLength(const std::wstring& file_path)
{
    wchar_t buff[16];
    m_error_code = mciSendStringW((L"status \"" + file_path + L"\" length").c_str(), buff, 15, 0);		//获取当前歌曲的长度，并储存在buff数组里
    int length = _wtoi(buff);
    if (IsMidi())
    {
        m_midi_info.midi_length = length / 4;
        if (m_midi_info.speed > 0)
            length = length * m_midi_info.speed;
    }
    return length;
}

int CMciCore::GetMciBitrate(const std::wstring & file_path)
{
    wchar_t buff[16];
    m_error_code = mciSendStringW((L"status \"" + file_path + L"\" bytespersec").c_str(), buff, 15, 0);
    int bitrate = _wtoi(buff);
    bitrate = bitrate * 8 / 1000;
    return bitrate;
}
