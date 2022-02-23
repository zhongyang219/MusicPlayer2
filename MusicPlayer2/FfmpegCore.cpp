#include "stdafx.h"
#include "FfmpegCore.h"

#include "AudioCommon.h"
#include "Common.h"
#include "MusicPlayer2.h"

#define ft2ts(t) (((size_t)t.dwHighDateTime << 32) | (size_t)t.dwLowDateTime)

static std::wstring last_ffmpeg_core_error_cache;

CFfmpegCore::CFfmpegCore() {
    handle = nullptr;
    err = 0;
    Init(L"ffmpeg_core.dll");
    if (!IsSucceed()) {
        CString strInfo = CCommon::LoadText(IDS_FFMPEG_INIT_FAILED);
        theApp.WriteLog(wstring(strInfo));
    }
}

CFfmpegCore::~CFfmpegCore() {
    if (handle || settings) UnInitCore();
    CDllLib::UnInit();
}

void CFfmpegCore::InitCore() {
    if (IsSucceed()) {
        CAudioCommon::m_surpported_format.clear();
        SupportedFormat format;
        format.description = CCommon::LoadText(IDS_BASIC_AUDIO_FORMAT);
        format.extensions.push_back(L"mp3");
        format.extensions.push_back(L"wma");
        format.extensions.push_back(L"wav");
        format.extensions.push_back(L"m4a");
        format.extensions.push_back(L"ogg");
        format.extensions.push_back(L"oga");
        format.extensions.push_back(L"flac");
        format.extensions.push_back(L"ape");
        format.extensions.push_back(L"mp2");
        format.extensions.push_back(L"mp1");
        format.extensions.push_back(L"opus");
        format.extensions.push_back(L"ape");
        format.extensions.push_back(L"aif");
        format.extensions.push_back(L"aiff");
        format.extensions.push_back(L"cda");
        format.extensions.push_back(L"cue");
        format.extensions.push_back(L"mp4");
        format.extensions.push_back(L"mkv");
        format.extensions.push_back(L"m2ts");
        format.extensions_list = L"*.mp3;*.wma;*.wav;*.m4a;*.ogg;*.oga;*.flac;*.ape;*.mp2;*.mp1;*.opus;*.ape;*.cda;*.aif;*.aiff;*.cue;*.mp4;*.mkv;*.m2ts";
        CAudioCommon::m_surpported_format.push_back(format);
        CAudioCommon::m_all_surpported_extensions = format.extensions;
        settings = ffmpeg_core_init_settings();
        ffmpeg_core_log_set_flags(AV_LOG_SKIP_REPEATED | AV_LOG_PRINT_LEVEL);
        ffmpeg_core_log_set_callback(LogCallback);
        UpdateSettings();
        auto devices = GetAudioDevices();
        DeviceInfo d;
        d.index = -1;
        d.name = CCommon::LoadText(IDS_SDL_DEFAULT_DEVICE);
        d.driver = L"";
        d.flags = 0;
        theApp.m_output_devices.clear();
        theApp.m_output_devices.push_back(d);
        int device_index = 0;
        for (auto i = devices.begin(); i != devices.end(); i++) {
            DeviceInfo d;
            d.index = device_index++;
            d.name = *i;
            d.driver = L"";
            d.flags = 0;
            theApp.m_output_devices.push_back(d);
        }
        std::string ver("ffmpeg core version: ");
        ver += ffmpeg_core_version_str();
        ver += "\n";
        OutputDebugStringA(ver.c_str());
        ffmpeg_core_dump_library_version(1, AV_LOG_INFO);
        ffmpeg_core_dump_ffmpeg_configuration(1, AV_LOG_INFO);
    }
}

void CFfmpegCore::UnInitCore() {
    if (settings && IsSucceed()) {
        free_ffmpeg_core_settings(settings);
        settings = nullptr;
    }
    if (handle) {
        Close();
    }
}

unsigned int CFfmpegCore::GetHandle() {
    return 0;
}

std::wstring CFfmpegCore::GetAudioType() {
    return L"";
}

int CFfmpegCore::GetChannels() {
    if (IsSucceed() && handle) {
        return ffmpeg_core_get_channels(handle);
    } else return 0;
}

int CFfmpegCore::GetFReq() {
    if (IsSucceed() && handle) {
        return ffmpeg_core_get_freq(handle);
    } else return 0;
}

std::wstring CFfmpegCore::GetSoundFontName() {
    return L"";
}

void CFfmpegCore::Open(const wchar_t* file_path) {
    if (!IsSucceed()) return;
    if (handle) {
        Close();
    }
    if (file_path) recent_file = file_path;
    const wchar_t* device = nullptr;
    if (theApp.m_play_setting_data.device_selected < theApp.m_output_devices.size() && theApp.m_play_setting_data.device_selected) {
        device = theApp.m_output_devices[theApp.m_play_setting_data.device_selected].name.c_str();
    }
    if (!settings) settings = ffmpeg_core_init_settings();
    int re = ffmpeg_core_open3(file_path, &handle, settings, device);
    if (re) {
        err = re;
    }
}

void CFfmpegCore::Close() {
    if (IsSucceed() && handle) {
        free_music_handle(handle);
        handle = nullptr;
        err = 0;
    }
}

void CFfmpegCore::Play() {
    if (!IsSucceed()) return;
    if (!handle && !recent_file.empty()) {
        Open(recent_file.c_str());
    }
    if (handle) {
        ffmpeg_core_play(handle);
    }
}

void CFfmpegCore::Pause() {
    if (IsSucceed() && handle) {
        ffmpeg_core_pause(handle);
    }
}

void CFfmpegCore::Stop() {
    Close();
}

void CFfmpegCore::SetVolume(int volume) {
    if (!IsSucceed()) return;
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
    if (!IsSucceed()) return;
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
    if (IsSucceed() && handle) {
        return ffmpeg_core_song_is_over(handle);
    } else return false;
}

int CFfmpegCore::GetCurPosition() {
    if (IsSucceed() && handle) {
        return ffmpeg_core_get_cur_position(handle) / 1000;
    } else return 0;
}

int CFfmpegCore::GetSongLength() {
    if (IsSucceed() && handle) {
        return ffmpeg_core_get_song_length(handle) / 1000;
    } else return 0;
}

void CFfmpegCore::SetCurPosition(int position) {
    if (IsSucceed() && handle) {
        int re = ffmpeg_core_seek(handle, (int64_t)position * 1000);
        if (re) {
            err = re;
        }
    }
}

void CFfmpegCore::GetAudioInfo(SongInfo& song_info, int flag) {
    if (!handle || !IsSucceed()) return;
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
    if (!IsSucceed()) return;
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
    if (IsSucceed() && handle) {
        return ffmpeg_core_is_playing(handle) ? PlayingState::PS_PLAYING : PlayingState::PS_PAUSED;
    } else return PlayingState::PS_STOPED;
}

void CFfmpegCore::ApplyEqualizer(int channel, int gain) {
    if (!IsSucceed()) return;
    channel = GetEqChannelFreq(channel);
    if (handle) {
        int re = ffmpeg_core_set_equalizer_channel(handle, channel, gain);
        if (re) {
            err = re;
        }
    } else {
        ffmpeg_core_settings_set_equalizer_channel(settings, channel, gain);
    }
}

void CFfmpegCore::SetReverb(int mix, int time) {
}

void CFfmpegCore::ClearReverb() {
}

void CFfmpegCore::GetFFTData(float fft_data[FFT_SAMPLE]) {
    if (handle && IsSucceed()) {
        memset(fft_data, 0, FFT_SAMPLE);
        ffmpeg_core_get_fft_data(handle, fft_data, FFT_SAMPLE);
    } else {
        memset(fft_data, 0, FFT_SAMPLE);
    }
}

int CFfmpegCore::GetErrorCode() {
    if (err) {
        int tmp = err;
        err = 0;
        return tmp;
    }
    // 不返回下层的错误，以免当下层重新打开文件时，即使这里返回0，上层依旧认为有错误
    // if (handle) return ffmpeg_core_get_error(handle);
    return 0;
}

std::wstring CFfmpegCore::GetErrorInfo(int error_code) {
    if (error_code == 0 || !IsSucceed()) return L"";
    auto tmp = ffmpeg_core_get_err_msg(error_code);
    if (tmp) {
        std::wstring re(tmp);
        ffmpeg_core_free(tmp);
        return re;
    }
    return L"";
}

std::wstring CFfmpegCore::GetErrorInfo() {
    return GetErrorInfo(GetErrorCode());
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
    free_device_name_list = (_free_device_name_list)::GetProcAddress(m_dll_module, "free_device_name_list");
    ffmpeg_core_free = (_ffmpeg_core_free)::GetProcAddress(m_dll_module, "ffmpeg_core_free");
    ffmpeg_core_malloc = (_ffmpeg_core_malloc)::GetProcAddress(m_dll_module, "ffmpeg_core_malloc");
    ffmpeg_core_realloc = (_ffmpeg_core_realloc)::GetProcAddress(m_dll_module, "ffmpeg_core_realloc");
    ffmpeg_core_log_format_line = (_ffmpeg_core_log_format_line)::GetProcAddress(m_dll_module, "ffmpeg_core_log_format_line");
    ffmpeg_core_log_set_callback = (_ffmpeg_core_log_set_callback)::GetProcAddress(m_dll_module, "ffmpeg_core_log_set_callback");
    ffmpeg_core_log_set_flags = (_ffmpeg_core_log_set_flags)::GetProcAddress(m_dll_module, "ffmpeg_core_log_set_flags");
    ffmpeg_core_version_str = (_ffmpeg_core_version_str)::GetProcAddress(m_dll_module, "ffmpeg_core_version_str");
    ffmpeg_core_version = (_ffmpeg_core_version)::GetProcAddress(m_dll_module, "ffmpeg_core_version");
    ffmpeg_core_dump_library_version = (_ffmpeg_core_dump_library_version)::GetProcAddress(m_dll_module, "ffmpeg_core_dump_library_version");
    ffmpeg_core_dump_ffmpeg_configuration = (_ffmpeg_core_dump_ffmpeg_configuration)::GetProcAddress(m_dll_module, "ffmpeg_core_dump_ffmpeg_configuration");
    ffmpeg_core_open = (_ffmpeg_core_open)::GetProcAddress(m_dll_module, "ffmpeg_core_open");
    ffmpeg_core_open2 = (_ffmpeg_core_open2)::GetProcAddress(m_dll_module, "ffmpeg_core_open2");
    ffmpeg_core_open3 = (_ffmpeg_core_open3)::GetProcAddress(m_dll_module, "ffmpeg_core_open3");
    ffmpeg_core_info_open = (_ffmpeg_core_info_open)::GetProcAddress(m_dll_module, "ffmpeg_core_info_open");
    ffmpeg_core_play = (_ffmpeg_core_play)::GetProcAddress(m_dll_module, "ffmpeg_core_play");
    ffmpeg_core_pause = (_ffmpeg_core_pause)::GetProcAddress(m_dll_module, "ffmpeg_core_pause");
    ffmpeg_core_seek = (_ffmpeg_core_seek)::GetProcAddress(m_dll_module, "ffmpeg_core_seek");
    ffmpeg_core_set_volume = (_ffmpeg_core_set_volume)::GetProcAddress(m_dll_module, "ffmpeg_core_set_volume");
    ffmpeg_core_set_speed = (_ffmpeg_core_set_speed)::GetProcAddress(m_dll_module, "ffmpeg_core_set_speed");
    ffmpeg_core_set_equalizer_channel = (_ffmpeg_core_set_equalizer_channel)::GetProcAddress(m_dll_module, "ffmpeg_core_set_equalizer_channel");
    ffmpeg_core_get_error = (_ffmpeg_core_get_error)::GetProcAddress(m_dll_module, "ffmpeg_core_get_error");
    ffmpeg_core_get_err_msg = (_ffmpeg_core_get_err_msg)::GetProcAddress(m_dll_module, "ffmpeg_core_get_err_msg");
    ffmpeg_core_get_err_msg2 = (_ffmpeg_core_get_err_msg2)::GetProcAddress(m_dll_module, "ffmpeg_core_get_err_msg2");
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
    ffmpeg_core_get_fft_data = (_ffmpeg_core_get_fft_data)::GetProcAddress(m_dll_module, "ffmpeg_core_get_fft_data");
    ffmpeg_core_init_settings = (_ffmpeg_core_init_settings)::GetProcAddress(m_dll_module, "ffmpeg_core_init_settings");
    ffmpeg_core_settings_set_volume = (_ffmpeg_core_settings_set_volume)::GetProcAddress(m_dll_module, "ffmpeg_core_settings_set_volume");
    ffmpeg_core_settings_set_speed = (_ffmpeg_core_settings_set_speed)::GetProcAddress(m_dll_module, "ffmpeg_core_settings_set_speed");
    ffmpeg_core_settings_set_cache_length = (_ffmpeg_core_settings_set_cache_length)::GetProcAddress(m_dll_module, "ffmpeg_core_settings_set_cache_length");
    ffmpeg_core_settings_set_max_retry_count = (_ffmpeg_core_settings_set_max_retry_count)::GetProcAddress(m_dll_module, "ffmpeg_core_settings_set_max_retry_count");
    ffmpeg_core_settings_set_url_retry_interval = (_ffmpeg_core_settings_set_url_retry_interval)::GetProcAddress(m_dll_module, "ffmpeg_core_settings_set_url_retry_interval");
    ffmpeg_core_settings_set_equalizer_channel = (_ffmpeg_core_settings_set_equalizer_channel)::GetProcAddress(m_dll_module, "ffmpeg_core_settings_set_equalizer_channel");
    ffmpeg_core_get_audio_devices = (_ffmpeg_core_get_audio_devices)::GetProcAddress(m_dll_module, "ffmpeg_core_get_audio_devices");
    //判断是否成功
    rtn &= (free_device_name_list != NULL);
    rtn &= (free_music_handle != NULL);
    rtn &= (free_music_info_handle != NULL);
    rtn &= (free_ffmpeg_core_settings != NULL);
    rtn &= (ffmpeg_core_free != NULL);
    rtn &= (ffmpeg_core_malloc != NULL);
    rtn &= (ffmpeg_core_realloc != NULL);
    rtn &= (ffmpeg_core_log_format_line != NULL);
    rtn &= (ffmpeg_core_log_set_callback != NULL);
    rtn &= (ffmpeg_core_log_set_flags != NULL);
    rtn &= (ffmpeg_core_version_str != NULL);
    rtn &= (ffmpeg_core_version != NULL);
    rtn &= (ffmpeg_core_dump_library_version != NULL);
    rtn &= (ffmpeg_core_dump_ffmpeg_configuration != NULL);
    rtn &= (ffmpeg_core_open != NULL);
    rtn &= (ffmpeg_core_open2 != NULL);
    rtn &= (ffmpeg_core_open3 != NULL);
    rtn &= (ffmpeg_core_info_open != NULL);
    rtn &= (ffmpeg_core_play != NULL);
    rtn &= (ffmpeg_core_pause != NULL);
    rtn &= (ffmpeg_core_seek != NULL);
    rtn &= (ffmpeg_core_set_volume != NULL);
    rtn &= (ffmpeg_core_set_speed != NULL);
    rtn &= (ffmpeg_core_set_equalizer_channel != NULL);
    rtn &= (ffmpeg_core_get_error != NULL);
    rtn &= (ffmpeg_core_get_err_msg != NULL);
    rtn &= (ffmpeg_core_get_err_msg2 != NULL);
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
    rtn &= (ffmpeg_core_get_fft_data != NULL);
    rtn &= (ffmpeg_core_init_settings != NULL);
    rtn &= (ffmpeg_core_settings_set_volume != NULL);
    rtn &= (ffmpeg_core_settings_set_speed != NULL);
    rtn &= (ffmpeg_core_settings_set_cache_length != NULL);
    rtn &= (ffmpeg_core_settings_set_max_retry_count != NULL);
    rtn &= (ffmpeg_core_settings_set_url_retry_interval != NULL);
    rtn &= (ffmpeg_core_settings_set_equalizer_channel != NULL);
    rtn &= (ffmpeg_core_get_audio_devices != NULL);
    return rtn;
}

std::wstring CFfmpegCore::GetMetadata(std::string key, MusicInfoHandle* h) {
    if (!IsSucceed()) return L"";
    if (h) {
        auto r = ffmpeg_core_info_get_metadata(h, key.c_str());
        if (!r) return L"";
        std::wstring re(r);
        ffmpeg_core_free(r);
        return re;
    }
    if (!handle) return L"";
    auto r = ffmpeg_core_get_metadata(handle, key.c_str());
    if (!r) return L"";
    std::wstring re(r);
    ffmpeg_core_free(r);
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
        int r = ffmpeg_core_log_format_line(ptr, level, fmt, vl, buf, sizeof(buf), &print);
        if (r > 0) {
            std::wstring s = CCommon::StrToUnicode(std::string(buf, r), CodeType::UTF8);
            if (s.find(L"Last message repeated") != -1) {
                FreeLibrary(re);
                return;
            }
            OutputDebugStringW(s.c_str());
            if (level <= AV_LOG_ERROR) {
                if (s.back() == '\n') {
                    s.pop_back();
                }
                if (last_ffmpeg_core_error_cache.empty() || last_ffmpeg_core_error_cache != s) {
                    last_ffmpeg_core_error_cache = s;
                    theApp.WriteLog(s);
                }
            }
        }
    }
    FreeLibrary(re);
}

void CFfmpegCore::UpdateSettings() {
    SetCacheLength(theApp.m_play_setting_data.ffmpeg_core_cache_length);
    SetMaxRetryCount(theApp.m_play_setting_data.ffmpeg_core_max_retry_count);
    SetUrlRetryInterval(theApp.m_play_setting_data.ffmpeg_core_url_retry_interval);
}

void CFfmpegCore::SetCacheLength(int cache_length) {
    if (settings && IsSucceed()) {
        ffmpeg_core_settings_set_cache_length(settings, cache_length);
    }
}

void CFfmpegCore::SetMaxRetryCount(int max_retry_count) {
    if (settings && IsSucceed()) {
        ffmpeg_core_settings_set_max_retry_count(settings, max_retry_count);
    }
}

void CFfmpegCore::SetUrlRetryInterval(int url_retry_interval) {
    if (settings && IsSucceed()) {
        ffmpeg_core_settings_set_url_retry_interval(settings, url_retry_interval);
    }
}

int CFfmpegCore::GetEqChannelFreq(int channel) {
    switch (channel) {
        case 0:
            return 80;
        case 1:
            return 125;
        case 2:
            return 250;
        case 3:
            return 500;
        case 4:
            return 1000;
        case 5:
            return 1500;
        case 6:
            return 2000;
        case 7:
            return 4000;
        case 8:
            return 8000;
        case 9:
            return 16000;
        default:
            return 0;
    }
}

std::list<std::wstring> CFfmpegCore::GetAudioDevices() {
    if (!IsSucceed()) return {};
    auto d = ffmpeg_core_get_audio_devices();
    if (!d) return {};
    std::list<std::wstring> l;
    l.push_back(CCommon::StrToUnicode(d->device, CodeType::UTF8));
    auto t = d;
    while (t->next) {
        t = t->next;
        l.push_back(CCommon::StrToUnicode(t->device, CodeType::UTF8));
    }
    free_device_name_list(&d);
    return l;
}
