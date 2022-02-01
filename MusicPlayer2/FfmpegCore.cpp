#include "stdafx.h"
#include "FfmpegCore.h"

#include "AudioCommon.h"
#include "MusicPlayer2.h"


CFfmpegCore::CFfmpegCore() {
    handle = nullptr;
    err = 0;
    Init(L"ffmpeg_core.dll");
    if (!IsSucceed()) {
    }
}

CFfmpegCore::~CFfmpegCore() {
    if (handle) {
        Close();
    }
    CDllLib::UnInit();
}

void CFfmpegCore::InitCore() {
    if (IsSucceed()) {
        CAudioCommon::m_surpported_format.clear();
        SupportedFormat format;
        format.description = L"FFMPEG";
        format.extensions.push_back(L"mp3");
        format.extensions.push_back(L"wma");
        format.extensions.push_back(L"wav");
        format.extensions.push_back(L"m4a");
        format.extensions.push_back(L"flac");
        format.extensions.push_back(L"ape");
        format.extensions.push_back(L"mp4");
        format.extensions.push_back(L"mkv");
        format.extensions.push_back(L"m2ts");
        format.extensions_list = L"*.mp3;*.wma;*.wav;*.m4a;*.flac;*.ape;*.mp4;*.mkv;*.m2ts";
        CAudioCommon::m_surpported_format.push_back(format);
        CAudioCommon::m_all_surpported_extensions = format.extensions;
    }
}

void CFfmpegCore::UnInitCore() {
}

unsigned int CFfmpegCore::GetHandle() {
    return 0;
}

std::wstring CFfmpegCore::GetAudioType() {
    return L"";
}

int CFfmpegCore::GetChannels() {
    if (ffmpeg_core_get_channels && handle) {
        return ffmpeg_core_get_channels(handle);
    } else return 0;
}

int CFfmpegCore::GetFReq() {
    if (ffmpeg_core_get_freq && handle) {
        return ffmpeg_core_get_freq(handle);
    } else return 0;
}

std::wstring CFfmpegCore::GetSoundFontName() {
    return L"";
}

void CFfmpegCore::Open(const wchar_t* file_path) {
    if (ffmpeg_core_open) {
        int re = ffmpeg_core_open(file_path, &handle);
        if (re) {
            err = re;
        }
    }
}

void CFfmpegCore::Close() {
    if (free_music_handle && handle) {
        free_music_handle(handle);
        handle = nullptr;
    }
}

void CFfmpegCore::Play() {
    if (ffmpeg_core_play && handle) {
        ffmpeg_core_play(handle);
    }
}

void CFfmpegCore::Pause() {
    if (ffmpeg_core_pause && handle) {
        ffmpeg_core_pause(handle);
    }
}

void CFfmpegCore::Stop() {
    // 忽略Stop
}

void CFfmpegCore::SetVolume(int volume) {
}

void CFfmpegCore::SetSpeed(float speed) {
}

bool CFfmpegCore::SongIsOver() {
    if (ffmpeg_core_song_is_over && handle) {
        return ffmpeg_core_song_is_over(handle);
    } else return false;
}

int CFfmpegCore::GetCurPosition() {
    if (ffmpeg_core_get_cur_position && handle) {
        return ffmpeg_core_get_cur_position(handle) / 1000;
    } else return 0;
}

int CFfmpegCore::GetSongLength() {
    if (ffmpeg_core_get_song_length && handle) {
        return ffmpeg_core_get_song_length(handle) / 1000;
    } else return 0;
}

void CFfmpegCore::SetCurPosition(int position) {
    if (ffmpeg_core_seek && handle) {
        int re = ffmpeg_core_seek(handle, (int64_t)position * 1000);
        if (re) {
            err = re;
        }
    }
}

void CFfmpegCore::GetAudioInfo(SongInfo& song_info, int flag) {
    if (!handle) return;
    if (flag & AF_LENGTH) song_info.lengh = GetSongLength();
    if (flag & AF_CHANNEL_INFO) {
        song_info.freq = ffmpeg_core_get_freq(handle);
        song_info.bits = ffmpeg_core_get_bits(handle);
        song_info.channels = ffmpeg_core_get_channels(handle);
    }
}

void CFfmpegCore::GetAudioInfo(const wchar_t* file_path, SongInfo& song_info, int flag) {
}

bool CFfmpegCore::IsMidi() {
    return false;
}

bool CFfmpegCore::IsMidiConnotPlay() {
    return false;
}

MidiInfo CFfmpegCore::GetMidiInfo() {
    return MidiInfo();
}

std::wstring CFfmpegCore::GetMidiInnerLyric() {
    return L"";
}

bool CFfmpegCore::MidiNoLyric() {
    return true;
}

PlayingState CFfmpegCore::GetPlayingState() {
    if (ffmpeg_core_is_playing && handle) {
        return ffmpeg_core_is_playing(handle) ? PlayingState::PS_PLAYING : PlayingState::PS_PAUSED;
    } else return PlayingState::PS_STOPED;
}

void CFfmpegCore::ApplyEqualizer(int channel, int gain) {
}

void CFfmpegCore::SetReverb(int mix, int time) {
}

void CFfmpegCore::ClearReverb() {
}

void CFfmpegCore::GetFFTData(float fft_data[FFT_SAMPLE]) {
}

int CFfmpegCore::GetErrorCode() {
    return err;
}

std::wstring CFfmpegCore::GetErrorInfo(int error_code) {
    return L"";
}

std::wstring CFfmpegCore::GetErrorInfo() {
    return L"";
}

PlayerCoreType CFfmpegCore::GetCoreType() {
    return PlayerCoreType::PT_FFMPEG;
}

int CFfmpegCore::GetDeviceCount() {
    return 0;
}

bool CFfmpegCore::GetFunction() {
    bool rtn = true;
    //获取函数入口
    free_music_handle = (_free_music_handle)::GetProcAddress(m_dll_module, "free_music_handle");
    ffmpeg_core_open = (_ffmpeg_core_open)::GetProcAddress(m_dll_module, "ffmpeg_core_open");
    ffmpeg_core_play = (_ffmpeg_core_play)::GetProcAddress(m_dll_module, "ffmpeg_core_play");
    ffmpeg_core_pause = (_ffmpeg_core_pause)::GetProcAddress(m_dll_module, "ffmpeg_core_pause");
    ffmpeg_core_seek = (_ffmpeg_core_seek)::GetProcAddress(m_dll_module, "ffmpeg_core_seek");
    ffmpeg_core_get_cur_position = (_ffmpeg_core_get_cur_position)::GetProcAddress(m_dll_module, "ffmpeg_core_get_cur_position");
    ffmpeg_core_song_is_over = (_ffmpeg_core_song_is_over)::GetProcAddress(m_dll_module, "ffmpeg_core_song_is_over");
    ffmpeg_core_get_song_length = (_ffmpeg_core_get_song_length)::GetProcAddress(m_dll_module, "ffmpeg_core_get_song_length");
    ffmpeg_core_get_channels = (_ffmpeg_core_get_channels)::GetProcAddress(m_dll_module, "ffmpeg_core_get_channels");
    ffmpeg_core_get_freq = (_ffmpeg_core_get_freq)::GetProcAddress(m_dll_module, "ffmpeg_core_get_freq");
    ffmpeg_core_is_playing = (_ffmpeg_core_is_playing)::GetProcAddress(m_dll_module, "ffmpeg_core_is_playing");
    ffmpeg_core_get_bits = (_ffmpeg_core_get_bits)::GetProcAddress(m_dll_module, "ffmpeg_core_get_bits");
    //判断是否成功
    rtn &= (free_music_handle != NULL);
    rtn &= (ffmpeg_core_open != NULL);
    rtn &= (ffmpeg_core_play != NULL);
    rtn &= (ffmpeg_core_pause != NULL);
    rtn &= (ffmpeg_core_seek != NULL);
    rtn &= (ffmpeg_core_get_cur_position != NULL);
    rtn &= (ffmpeg_core_song_is_over != NULL);
    rtn &= (ffmpeg_core_get_song_length != NULL);
    rtn &= (ffmpeg_core_get_channels != NULL);
    rtn &= (ffmpeg_core_get_freq != NULL);
    rtn &= (ffmpeg_core_is_playing != NULL);
    rtn &= (ffmpeg_core_get_bits != NULL);
    return rtn;
}
