#include "stdafx.h"
#include "MciCore.h"
#include "AudioCommon.h"


CMciCore::CMciCore()
{
	//载入DLL
	m_dll_module = ::LoadLibrary(_T("winmm.dll"));
	//获取函数入口
    mciSendStringW = (_mciSendStringW)::GetProcAddress(m_dll_module, "mciSendStringW");
	//判断是否成功
	m_successed = true;
	m_successed &= (m_dll_module != NULL);
	m_successed &= (mciSendStringW != NULL);
}


CMciCore::~CMciCore()
{
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
    m_error_code = mciSendStringW((L"open \"" + m_file_path + L"\"").c_str(), NULL, 0, 0);

}

void CMciCore::Close()
{
    m_error_code = mciSendStringW((L"close \"" + m_file_path + L"\"").c_str(), NULL, 0, 0);
}

void CMciCore::Play()
{
    m_error_code = mciSendStringW((L"play \"" + m_file_path + L"\"").c_str(), NULL, 0, 0);
}

void CMciCore::Pause()
{
    m_error_code = mciSendStringW((L"pause \"" + m_file_path + L"\"").c_str(), NULL, 0, 0);
}

void CMciCore::Stop()
{
    m_error_code = mciSendStringW((L"stop \"" + m_file_path + L"\"").c_str(), NULL, 0, 0);
}

void CMciCore::SetVolume(int volume)
{
    wchar_t buff[16];
    _itow_s(volume * 10, buff, 10);		//设置音量100%时为1000
    m_error_code = mciSendStringW((L"setaudio \"" + m_file_path + L"\" volume to " + buff).c_str(), NULL, 0, 0);

}

int CMciCore::GetCurPosition()
{
    wchar_t buff[16];
    m_error_code = mciSendStringW((L"status \"" + m_file_path + L"\" position").c_str(), buff, 15, 0);
    return _wtoi(buff);
}

int CMciCore::GetSongLength()
{
    wchar_t buff[16];
    m_error_code = mciSendStringW((L"status \"" + m_file_path + L"\" length").c_str(), buff, 15, 0);		//获取当前歌曲的长度，并储存在buff数组里
    return _wtoi(buff);
}

void CMciCore::SetCurPosition(int position)
{
    wchar_t buff[16];
    _itow_s(position, buff, 10);
    m_error_code = mciSendStringW((L"seek \"" + m_file_path + L"\" to " + buff).c_str(), NULL, 0, 0);		//定位到新的位置
}

void CMciCore::GetAudioInfo(SongInfo & song_info, bool get_tag)
{
    wchar_t buff[16];
    m_error_code = mciSendStringW((L"status \"" + song_info.file_path + L"\" length").c_str(), buff, 15, 0);		//获取当前歌曲的长度，并储存在buff数组里
    song_info.lengh = _wtoi(buff);

}

void CMciCore::GetAudioInfo(const wchar_t * file_path, SongInfo & song_info, bool get_tag)
{
    m_error_code = mciSendStringW((L"open \"" + wstring(file_path) + L"\"").c_str(), NULL, 0, 0);
    wchar_t buff[16];
    m_error_code = mciSendStringW((L"status \"" + wstring(file_path) + L"\" length").c_str(), buff, 15, 0);		//获取当前歌曲的长度，并储存在buff数组里
    song_info.lengh = _wtoi(buff);
    m_error_code = mciSendStringW((L"close \"" + wstring(file_path) + L"\"").c_str(), NULL, 0, 0);
}

bool CMciCore::IsMidi()
{
    return false;
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
    return MidiInfo();
}

bool CMciCore::MidiNoLyric()
{
    return true;
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
    return 0;
}
