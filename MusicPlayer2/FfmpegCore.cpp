#include "stdafx.h"
#include "FfmpegCore.h"

#include "AudioCommon.h"
#include "Common.h"
#include "MusicPlayer2.h"


CFfmpegCore::CFfmpegCore() {
    handle = nullptr;
    err = 0;
    Init(L"ffmpeg_core.dll");
    if (!IsSucceed()) {
    }
}

CFfmpegCore::~CFfmpegCore() {
    if (settings) {
        free_ffmpeg_core_settings(settings);
    }
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
        settings = ffmpeg_core_init_settings();
        ffmpeg_core_log_set_callback(LogCallback);
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
    if (handle) {
        Close();
    }
    if (ffmpeg_core_open) {
        if (file_path) recent_file = file_path;
        if (!settings) settings = ffmpeg_core_init_settings();
        int re = ffmpeg_core_open2(file_path, &handle, settings);
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
    if (!handle && !recent_file.empty()) {
        Open(recent_file.c_str());
    }
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
    Close();
}

void CFfmpegCore::SetVolume(int volume) {
    if (handle) {
        int re = ffmpeg_core_set_volume(handle, volume);
        if (re) {
            err = re;
        }
    } else {
        ffmpeg_core_settings_set_volume(settings, volume);
    }
}

void CFfmpegCore::SetSpeed(float speed) {
    if (handle) {
        int re = ffmpeg_core_set_speed(handle, speed);
        if (re) {
            err = re;
        }
    } else {
        ffmpeg_core_settings_set_speed(settings, speed);
    }
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
    if (flag & AF_BITRATE) {
        song_info.bitrate = ffmpeg_core_get_bitrate(handle) / 1000;
    }
    if (flag & AF_TAG_INFO) {
        song_info.title = GetTitle();
        song_info.artist = GetArtist();
        song_info.album = GetAlbum();
        song_info.comment = GetComment();
        song_info.genre = GetGenre();
        song_info.year = GetYear();
        song_info.track = GetTrackNum();
    }
}

void CFfmpegCore::GetAudioInfo(const wchar_t* file_path, SongInfo& song_info, int flag) {
    MusicInfoHandle* h = nullptr;
    int re = ffmpeg_core_info_open(file_path, &h);
    if (re || !h) return;
    if (flag & AF_LENGTH) song_info.lengh = ffmpeg_core_info_get_song_length(h) / 1000;
    if (flag & AF_CHANNEL_INFO) {
        song_info.freq = ffmpeg_core_info_get_freq(h);
        song_info.bits = ffmpeg_core_info_get_bits(h);
        song_info.channels = ffmpeg_core_info_get_channels(h);
    }
    if (flag & AF_BITRATE) {
        song_info.bitrate = ffmpeg_core_info_get_bitrate(h) / 1000;
    }
    if (flag & AF_TAG_INFO) {
        song_info.title = GetTitle(h);
        song_info.artist = GetArtist(h);
        song_info.album = GetAlbum(h);
        song_info.comment = GetComment(h);
        song_info.genre = GetGenre(h);
        song_info.year = GetYear(h);
        song_info.track = GetTrackNum(h);
    }
    free_music_info_handle(h);
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
    free_music_info_handle = (_free_music_info_handle)::GetProcAddress(m_dll_module, "free_music_info_handle");
    free_ffmpeg_core_settings = (_free_ffmpeg_core_settings)::GetProcAddress(m_dll_module, "free_ffmpeg_core_settings");
    ffmpeg_core_log_format_line = (_ffmpeg_core_log_format_line)::GetProcAddress(m_dll_module, "ffmpeg_core_log_format_line");
    ffmpeg_core_log_set_callback = (_ffmpeg_core_log_set_callback)::GetProcAddress(m_dll_module, "ffmpeg_core_log_set_callback");
    ffmpeg_core_open = (_ffmpeg_core_open)::GetProcAddress(m_dll_module, "ffmpeg_core_open");
    ffmpeg_core_open2 = (_ffmpeg_core_open2)::GetProcAddress(m_dll_module, "ffmpeg_core_open2");
    ffmpeg_core_info_open = (_ffmpeg_core_info_open)::GetProcAddress(m_dll_module, "ffmpeg_core_info_open");
    ffmpeg_core_play = (_ffmpeg_core_play)::GetProcAddress(m_dll_module, "ffmpeg_core_play");
    ffmpeg_core_pause = (_ffmpeg_core_pause)::GetProcAddress(m_dll_module, "ffmpeg_core_pause");
    ffmpeg_core_seek = (_ffmpeg_core_seek)::GetProcAddress(m_dll_module, "ffmpeg_core_seek");
    ffmpeg_core_set_volume = (_ffmpeg_core_set_volume)::GetProcAddress(m_dll_module, "ffmpeg_core_set_volume");
    ffmpeg_core_set_speed = (_ffmpeg_core_set_speed)::GetProcAddress(m_dll_module, "ffmpeg_core_set_speed");
    ffmpeg_core_get_cur_position = (_ffmpeg_core_get_cur_position)::GetProcAddress(m_dll_module, "ffmpeg_core_get_cur_position");
    ffmpeg_core_song_is_over = (_ffmpeg_core_song_is_over)::GetProcAddress(m_dll_module, "ffmpeg_core_song_is_over");
    ffmpeg_core_get_song_length = (_ffmpeg_core_get_song_length)::GetProcAddress(m_dll_module, "ffmpeg_core_get_song_length");
    ffmpeg_core_info_get_song_length = (_ffmpeg_core_info_get_song_length)::GetProcAddress(m_dll_module, "ffmpeg_core_info_get_song_length");
    ffmpeg_core_get_channels = (_ffmpeg_core_get_channels)::GetProcAddress(m_dll_module, "ffmpeg_core_get_channels");
    ffmpeg_core_info_get_channels = (_ffmpeg_core_info_get_channels)::GetProcAddress(m_dll_module, "ffmpeg_core_info_get_channels");
    ffmpeg_core_get_freq = (_ffmpeg_core_get_freq)::GetProcAddress(m_dll_module, "ffmpeg_core_get_freq");
    ffmpeg_core_info_get_freq = (_ffmpeg_core_info_get_freq)::GetProcAddress(m_dll_module, "ffmpeg_core_info_get_freq");
    ffmpeg_core_is_playing = (_ffmpeg_core_is_playing)::GetProcAddress(m_dll_module, "ffmpeg_core_is_playing");
    ffmpeg_core_get_bits = (_ffmpeg_core_get_bits)::GetProcAddress(m_dll_module, "ffmpeg_core_get_bits");
    ffmpeg_core_info_get_bits = (_ffmpeg_core_info_get_bits)::GetProcAddress(m_dll_module, "ffmpeg_core_info_get_bits");
    ffmpeg_core_get_bitrate = (_ffmpeg_core_get_bitrate)::GetProcAddress(m_dll_module, "ffmpeg_core_get_bitrate");
    ffmpeg_core_info_get_bitrate = (_ffmpeg_core_info_get_bitrate)::GetProcAddress(m_dll_module, "ffmpeg_core_info_get_bitrate");
    ffmpeg_core_get_metadata = (_ffmpeg_core_get_metadata)::GetProcAddress(m_dll_module, "ffmpeg_core_get_metadata");
    ffmpeg_core_info_get_metadata = (_ffmpeg_core_info_get_metadata)::GetProcAddress(m_dll_module, "ffmpeg_core_info_get_metadata");
    ffmpeg_core_init_settings = (_ffmpeg_core_init_settings)::GetProcAddress(m_dll_module, "ffmpeg_core_init_settings");
    ffmpeg_core_settings_set_volume = (_ffmpeg_core_settings_set_volume)::GetProcAddress(m_dll_module, "ffmpeg_core_settings_set_volume");
    ffmpeg_core_settings_set_speed = (_ffmpeg_core_settings_set_speed)::GetProcAddress(m_dll_module, "ffmpeg_core_settings_set_speed");
    //判断是否成功
    rtn &= (free_music_handle != NULL);
    rtn &= (free_music_info_handle != NULL);
    rtn &= (free_ffmpeg_core_settings != NULL);
    rtn &= (ffmpeg_core_log_format_line != NULL);
    rtn &= (ffmpeg_core_log_set_callback != NULL);
    rtn &= (ffmpeg_core_open != NULL);
    rtn &= (ffmpeg_core_open2 != NULL);
    rtn &= (ffmpeg_core_info_open != NULL);
    rtn &= (ffmpeg_core_play != NULL);
    rtn &= (ffmpeg_core_pause != NULL);
    rtn &= (ffmpeg_core_seek != NULL);
    rtn &= (ffmpeg_core_set_volume != NULL);
    rtn &= (ffmpeg_core_set_speed != NULL);
    rtn &= (ffmpeg_core_get_cur_position != NULL);
    rtn &= (ffmpeg_core_song_is_over != NULL);
    rtn &= (ffmpeg_core_get_song_length != NULL);
    rtn &= (ffmpeg_core_info_get_song_length != NULL);
    rtn &= (ffmpeg_core_get_channels != NULL);
    rtn &= (ffmpeg_core_info_get_channels != NULL);
    rtn &= (ffmpeg_core_get_freq != NULL);
    rtn &= (ffmpeg_core_info_get_freq != NULL);
    rtn &= (ffmpeg_core_is_playing != NULL);
    rtn &= (ffmpeg_core_get_bits != NULL);
    rtn &= (ffmpeg_core_info_get_bits != NULL);
    rtn &= (ffmpeg_core_get_bitrate != NULL);
    rtn &= (ffmpeg_core_info_get_bitrate != NULL);
    rtn &= (ffmpeg_core_get_metadata != NULL);
    rtn &= (ffmpeg_core_info_get_metadata != NULL);
    rtn &= (ffmpeg_core_init_settings != NULL);
    rtn &= (ffmpeg_core_settings_set_volume != NULL);
    rtn &= (ffmpeg_core_settings_set_speed != NULL);
    return rtn;
}

std::wstring CFfmpegCore::GetMetadata(std::string key, MusicInfoHandle* h) {
    if (h) {
        auto r = ffmpeg_core_info_get_metadata(h, key.c_str());
        if (!r) return L"";
        std::wstring re(r);
        free(r);
        return re;
    }
    if (!handle) return L"";
    auto r = ffmpeg_core_get_metadata(handle, key.c_str());
    if (!r) return L"";
    std::wstring re(r);
    free(r);
    return re;
}

std::wstring CFfmpegCore::GetTitle(MusicInfoHandle* h) {
    return GetMetadata("title", h);
}

std::wstring CFfmpegCore::GetArtist(MusicInfoHandle* h) {
    return GetMetadata("artist", h);
}

std::wstring CFfmpegCore::GetAlbum(MusicInfoHandle* h) {
    return GetMetadata("album", h);
}

std::wstring CFfmpegCore::GetComment(MusicInfoHandle* h) {
    auto re = GetMetadata("comment", h);
    if (!re.empty()) return re;
    return GetMetadata("description", h);
}

std::wstring CFfmpegCore::GetGenre(MusicInfoHandle* h) {
    return GetMetadata("genre", h);
}

std::wstring CFfmpegCore::GetDate(MusicInfoHandle* h) {
    return GetMetadata("date", h);
}

unsigned short CFfmpegCore::GetYear(MusicInfoHandle* h) {
    auto r = GetDate(h);
    if (r.empty()) return 0;
    unsigned short year = 0;
    if (swscanf_s(r.c_str(), L"%hu", &year) == 1) return year;
    return 0;
}

std::wstring CFfmpegCore::GetTrack(MusicInfoHandle* h) {
    return GetMetadata("track", h);
}

int CFfmpegCore::GetTrackNum(MusicInfoHandle* h) {
    auto r = GetTrack(h);
    if (r.empty()) return 0;
    int track;
    if (swscanf_s(r.c_str(), L"%i", &track) == 1) return track;
    return 0;
}

void CFfmpegCore::LogCallback(void* ptr, int level, const char* fmt, va_list vl) {
    if (level > AV_LOG_VERBOSE) return;
    auto re = LoadLibraryW(L"ffmpeg_core.dll");
    if (!re) return;
    auto ffmpeg_core_log_format_line = (_ffmpeg_core_log_format_line)::GetProcAddress(re, "ffmpeg_core_log_format_line");
    char buf[1024];
    if (ffmpeg_core_log_format_line) {
        int print = 1;
        int re = ffmpeg_core_log_format_line(ptr, level, fmt, vl, buf, sizeof(buf), &print);
        if (re > 0) {
            std::wstring s = CCommon::StrToUnicode(std::string(buf, re), CodeType::UTF8);
            OutputDebugStringW(s.c_str());
        }
    }
    FreeLibrary(re);
}
