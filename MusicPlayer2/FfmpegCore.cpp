#include "stdafx.h"
#include "FfmpegCore.h"

#include "AudioCommon.h"
#include "Common.h"
#include "MusicPlayer2.h"
#include "Player.h"

#define ft2ts(t) (((size_t)t.dwHighDateTime << 32) | (size_t)t.dwLowDateTime)

static std::wstring last_ffmpeg_core_error_cache;

CFfmpegCore::CFfmpegCore() {
    handle = nullptr;
    err = 0;
    Init(L"ffmpeg_core.dll");
    if (!IsSucceed()) {
        const wstring& info = theApp.m_str_table.LoadText(L"LOG_FFMPEG_INIT_FAILED");
        theApp.WriteLog(info);
    }
}

CFfmpegCore::~CFfmpegCore() {
    if (handle || settings) UnInitCore();
    CDllLib::UnInit();
}

void CFfmpegCore::InitCore() {
    if (IsSucceed()) {
        CAudioCommon::m_surpported_format.clear();
        CAudioCommon::m_all_surpported_extensions.clear();
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"mp3 wma wav m4a ogg oga flac ape mp2 mp1 opus ape cda aif aiff cue mp4 mkv mka m2ts", theApp.m_str_table.LoadText(L"TXT_FILE_TYPE_BASE").c_str()));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"3gp 3g2 mj2 psp m4b ism ismv isma f4v", L"3gp"));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"aa", L"Audible Format 2, 3, and 4"));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"aac", L"AAC (Advanced Audio Coding)"));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"ac3", L"ATSC A/52A (AC-3)"));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"alac", L"ALAC (Apple Lossless Audio Codec)"));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"asf", L"Advanced Systems Format"));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"als", L"MPEG-4 Audio Lossless Coding (ALS)"));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"amr", L"AMR"));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"ape mac", L"Monkey's Audio"));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"aptx", L"aptX (Audio Processing Technology for Bluetooth)"));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"atrac", L"Adaptive TRansform Acoustic Coding"));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"dst", L"DST (Digital Stream Transfer)"));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"dca", L"DCA (DTS Coherent Acoustics)"));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"flac fla", L"FLAC (Free Lossless Audio Codec)"));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"flv live_flv kux", L"Adobe Flash Video"));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"gsm", L"GSM"));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"mp1 mp2 mp3 mp4", L"MPEG audio"));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"m4a mp4", L"MP4"));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"opus", L"Opus"));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"pcm", L"PCM"));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"spx", L"Speex"));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"tak", L"TAK (Tom's lossless Audio Kompressor)"));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"tta", L"TTA (True Audio)"));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"ogg oga", L"Vorbis"));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"wv", L"WavPack"));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"mpc mp+ mpp", L"Musepack"));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"wma", L"Windows Media Audio"));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"mov", L"mov"));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"avi", L"Audio Video Interleaved"));
        CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(L"cda", L"CD Audio"));

        if (!theApp.m_nc_setting_data.user_defined_type_ffmpeg.empty())
        {
            CAudioCommon::m_surpported_format.push_back(CAudioCommon::CreateSupportedFormat(theApp.m_nc_setting_data.user_defined_type_ffmpeg, theApp.m_str_table.LoadText(L"TXT_FILE_TYPE_FFMPEG_USER_DEFINE").c_str()));
        }

        for (const auto& item : CAudioCommon::m_surpported_format)
        {
            for (const auto& ext : item.extensions)
            {
                if (!ext.empty() && !CCommon::IsItemInVector(CAudioCommon::m_all_surpported_extensions, ext))
                    CAudioCommon::m_all_surpported_extensions.push_back(ext);
            }
        }

        settings = ffmpeg_core_init_settings();
        ffmpeg_core_log_set_flags(AV_LOG_SKIP_REPEATED | AV_LOG_PRINT_LEVEL);
        ffmpeg_core_log_set_callback(LogCallback);
        UpdateSettings();
        auto devices = GetAudioDevices();
        DeviceInfo d;
        d.index = -1;
        d.name = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_DEVICE_NAME_SDL_DEFAULT");
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
    if (theApp.m_play_setting_data.device_selected < static_cast<int>(theApp.m_output_devices.size()) && theApp.m_play_setting_data.device_selected) {
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
        return static_cast<int>(ffmpeg_core_get_cur_position(handle) / 1000);
    } else return 0;
}

int CFfmpegCore::GetSongLength() {
    if (IsSucceed() && handle) {
        int64_t length = ffmpeg_core_get_song_length(handle);
        if (length != INT64_MIN)
            return static_cast<int>(length / 1000);
        else
            return 0;
    } else return 0;
}

void CFfmpegCore::SetCurPosition(int position) {
    if (IsSucceed() && handle && GetSongLength() != 0) {    // 时长为0(获取失败)时seek(0)会卡死
        int re = ffmpeg_core_seek(handle, (int64_t)position * 1000);
        if (re) {
            err = re;
        }
    }
}

void CFfmpegCore::GetAudioInfo(SongInfo& song_info, int flag) {
    if (!handle || !IsSucceed()) return;
    if (flag & AF_LENGTH) song_info.end_pos.fromInt(GetSongLength());
    if (flag & AF_CHANNEL_INFO) {
        song_info.freq = ffmpeg_core_get_freq(handle);
        song_info.bits = ffmpeg_core_get_bits(handle);
        song_info.channels = ffmpeg_core_get_channels(handle);
    }
    if (flag & AF_BITRATE) {
        song_info.bitrate = static_cast<short>(ffmpeg_core_get_bitrate(handle) / 1000);
    }
    if (flag & AF_TAG_INFO) {
        CAudioTag audio_tag(song_info);
        audio_tag.GetAudioRating();
        if (!audio_tag.GetAudioTag())       //如果taglib获取信息失败，则使用ffmpeg获取标签信息
        {
            song_info.title = GetTitle();
            song_info.artist = GetArtist();
            song_info.album = GetAlbum();
            song_info.comment = GetComment();
            song_info.genre = GetGenre();
            song_info.year = GetYear();
            song_info.track = GetTrackNum();
        }
    }
}

void CFfmpegCore::GetAudioInfo(const wchar_t* file_path, SongInfo& song_info, int flag) {
    if (!IsSucceed()) return;
    MusicInfoHandle* h = nullptr;
    int re = ffmpeg_core_info_open(file_path, &h);
    if (re || !h) return;
    if (flag & AF_LENGTH) {
        int64_t length = ffmpeg_core_info_get_song_length(h);
        if (length != INT64_MIN)
            song_info.end_pos.fromInt(static_cast<int>(length / 1000));
        else
            song_info.end_pos.fromInt(0);
    }
    if (flag & AF_CHANNEL_INFO) {
        song_info.freq = ffmpeg_core_info_get_freq(h);
        song_info.bits = ffmpeg_core_info_get_bits(h);
        song_info.channels = ffmpeg_core_info_get_channels(h);
    }
    if (flag & AF_BITRATE) {
        song_info.bitrate = static_cast<short>(ffmpeg_core_info_get_bitrate(h) / 1000);
    }
    if (flag & AF_TAG_INFO) {
        if (song_info.file_path.empty())
            song_info.file_path = file_path;
        CAudioTag audio_tag(song_info);
        audio_tag.GetAudioRating();
        if (!audio_tag.GetAudioTag())       //如果taglib获取信息失败，则使用ffmpeg获取标签信息
        {
            song_info.title = GetTitle(h);
            song_info.artist = GetArtist(h);
            song_info.album = GetAlbum(h);
            song_info.comment = GetComment(h);
            song_info.genre = GetGenre(h);
            song_info.year = GetYear(h);
            song_info.track = GetTrackNum(h);
        }
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
        memset(fft_data, 0, sizeof(fft_data));
        ffmpeg_core_get_fft_data(handle, fft_data, FFT_SAMPLE);
    } else {
        memset(fft_data, 0, sizeof(fft_data));
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
    if (ffmpeg_core_version) {
        version = ffmpeg_core_version();
        if (version > FFMPEG_CORE_VERSION(1, 0, 0, 0)) {
            ffmpeg_core_is_wasapi_supported = (_ffmpeg_core_is_wasapi_supported)::GetProcAddress(m_dll_module, "ffmpeg_core_is_wasapi_supported");
            ffmpeg_core_settings_set_use_WASAPI = (_ffmpeg_core_settings_set_use_WASAPI)::GetProcAddress(m_dll_module, "ffmpeg_core_settings_set_use_WASAPI");
            ffmpeg_core_settings_set_enable_exclusive = (_ffmpeg_core_settings_set_enable_exclusive)::GetProcAddress(m_dll_module, "ffmpeg_core_settings_set_enable_exclusive");
            ffmpeg_core_settings_set_max_wait_time = (_ffmpeg_core_settings_set_max_wait_time)::GetProcAddress(m_dll_module, "ffmpeg_core_settings_set_max_wait_time");
            rtn &= (ffmpeg_core_is_wasapi_supported != NULL);
            rtn &= (ffmpeg_core_settings_set_use_WASAPI != NULL);
            rtn &= (ffmpeg_core_settings_set_enable_exclusive != NULL);
            rtn &= (ffmpeg_core_settings_set_max_wait_time != NULL);
        }
    }
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
    CFfmpegCore* ffmpeg_core = dynamic_cast<CFfmpegCore*>(CPlayer::GetInstance().GetPlayerCore());
    if (ffmpeg_core == nullptr)
        return;
    char buf[1024];
    int print = 1;
    int r = ffmpeg_core->ffmpeg_core_log_format_line(ptr, level, fmt, vl, buf, sizeof(buf), &print);
    if (r > 0) {
        std::wstring s = CCommon::StrToUnicode(std::string(buf, r), CodeType::UTF8);
        if (s.find(L"Last message repeated") != -1) {
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

void CFfmpegCore::UpdateSettings() {
    SetCacheLength(theApp.m_play_setting_data.ffmpeg_core_cache_length);
    SetMaxRetryCount(theApp.m_play_setting_data.ffmpeg_core_max_retry_count);
    SetUrlRetryInterval(theApp.m_play_setting_data.ffmpeg_core_url_retry_interval);
    EnableWASAPI(theApp.m_play_setting_data.ffmpeg_core_enable_WASAPI);
    EnableExclusiveMode(theApp.m_play_setting_data.ffmpeg_core_enable_WASAPI_exclusive_mode);
    SetMaxWaitTime(theApp.m_play_setting_data.ffmpeg_core_max_wait_time);
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

bool CFfmpegCore::EncodeAudio(SongInfo song_info, const wstring& dest_file_path, EncodeFormat encode_format, void* encode_para, int dest_freq, EncodeAudioProc proc)
{
    return false;
}

bool CFfmpegCore::InitEncoder()
{
    return false;
}

void CFfmpegCore::UnInitEncoder()
{
}

bool CFfmpegCore::IsFreqConvertAvailable()
{
    return false;
}

int32_t CFfmpegCore::GetVersion() {
    return version;
}

bool CFfmpegCore::IsWASAPISupported() {
    if (ffmpeg_core_is_wasapi_supported) {
        return ffmpeg_core_is_wasapi_supported();
    }
    return false;
}

void CFfmpegCore::EnableWASAPI(bool enable) {
    if (settings && IsSucceed() && ffmpeg_core_settings_set_use_WASAPI) {
        ffmpeg_core_settings_set_use_WASAPI(settings, enable ? 1 : 0);
    }
}

void CFfmpegCore::EnableExclusiveMode(bool enable) {
    if (settings && IsSucceed() && ffmpeg_core_settings_set_enable_exclusive) {
        ffmpeg_core_settings_set_enable_exclusive(settings, enable ? 1 : 0);
    }
}

void CFfmpegCore::SetMaxWaitTime(int max_wait_time) {
    if (settings && IsSucceed() && ffmpeg_core_settings_set_max_wait_time) {
        ffmpeg_core_settings_set_max_wait_time(settings, max_wait_time);
    }
}
