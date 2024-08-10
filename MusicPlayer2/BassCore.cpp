#include "stdafx.h"
#include "BassCore.h"
#include "Common.h"
#include "AudioCommon.h"
#include "MusicPlayer2.h"
#include "Player.h"
#include "CommonDialogMgr.h"

CBASSMidiLibrary CBassCore::m_bass_midi_lib;
CBassCore::MidiLyricInfo CBassCore::m_midi_lyric;
BASS_MIDI_FONT CBassCore::m_sfont{};
CCriticalSection CBassCore::m_critical;
CBASSEncodeLibrary CBassCore::m_bass_encode_lib;
CBASSWmaLibrary CBassCore::m_bass_wma_lib;
CBassMixLibrary CBassCore::m_bass_mix_lib;
bool CBassCore::m_fading = false;

#define CONVERTING_TEMP_FILE_NAME L"converting_5k2019u6271iyt8j"

CBassCore::CBassCore()
{
}


CBassCore::~CBassCore()
{
}

void CBassCore::InitCore()
{
    CSingleLock sync(&m_critical, TRUE);
    //获取当前的音频输出设备
    BASS_DEVICEINFO device_info;
    int rtn;
    int device_index{ 1 };
    theApp.m_output_devices.clear();
    DeviceInfo device{};
    device.index = -1;
    device.name = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_DEVICE_NAME_BASS_DEFAULT");
    theApp.m_output_devices.push_back(device);
    while (true)
    {
        device = DeviceInfo{};
        rtn = BASS_GetDeviceInfo(device_index, &device_info);
        if (rtn == 0)
            break;
        device.index = device_index;
        if (device_info.name != nullptr)
            device.name = CCommon::StrToUnicode(string(device_info.name));
        if (device_info.driver != nullptr)
            device.driver = CCommon::StrToUnicode(string(device_info.driver));
        device.flags = device_info.flags;
        theApp.m_output_devices.push_back(device);
        device_index++;
    }

    theApp.m_play_setting_data.device_selected = 0;
    for (size_t i{}; i < theApp.m_output_devices.size(); i++)
    {
        if (theApp.m_output_devices[i].name == theApp.m_play_setting_data.output_device)
        {
            theApp.m_play_setting_data.device_selected = i;
            break;
        }
    }

    //初始化BASE音频库
    BASS_Init(
        theApp.m_output_devices[theApp.m_play_setting_data.device_selected].index,		//播放设备
        44100,//输出采样率44100（常用值）
        BASS_DEVICE_CPSPEAKERS,//信号，BASS_DEVICE_CPSPEAKERS 注释原文如下：
        /* Use the Windows control panel setting to detect the number of speakers.*/
        /* Soundcards generally have their own control panel to set the speaker config,*/
        /* so the Windows control panel setting may not be accurate unless it matches that.*/
        /* This flag has no effect on Vista, as the speakers are already accurately detected.*/
        theApp.m_pMainWnd->m_hWnd,//程序窗口,0用于控制台程序
        NULL//类标识符,0使用默认值
    );

    //向支持的文件列表插入原生支持的文件格式
    CAudioCommon::m_surpported_format.clear();
    SupportedFormat format;
    format.description = theApp.m_str_table.LoadText(L"TXT_FILE_TYPE_BASE");
    format.extensions.insert(format.extensions.end(), theApp.m_nc_setting_data.default_file_type.begin(), theApp.m_nc_setting_data.default_file_type.end());
    for (const auto& f : theApp.m_nc_setting_data.default_file_type)
    {
        format.extensions_list += L"*.";
        format.extensions_list += f;
        format.extensions_list += L';';
    }
    if (!format.extensions_list.empty())
        format.extensions_list.pop_back();

    //format.extensions_list = L"*.mp3;*.wma;*.wav;*.flac;*.ogg;*.oga;*.m4a;*.mp4;*.cue;*.mp2;*.mp1;*.aif;*.aiff";
    CAudioCommon::m_surpported_format.push_back(format);
    CAudioCommon::m_all_surpported_extensions = format.extensions;
    //载入BASS插件
    wstring plugin_dir;
    plugin_dir = theApp.m_local_dir + L"Plugins\\";
    vector<wstring> plugin_files;
    CCommon::GetFiles(plugin_dir + L"*.dll", plugin_files);		//获取Plugins目录下所有的dll文件的文件名
    m_plugin_handles.clear();
    for (const auto& plugin_file : plugin_files)
    {
        //加载插件
        HPLUGIN handle = BASS_PluginLoad((plugin_dir + plugin_file).c_str(), 0);
        m_plugin_handles.push_back(handle);
        //获取插件支持的音频文件类型
        const BASS_PLUGININFO* plugin_info = BASS_PluginGetInfo(handle);
        if (plugin_info == nullptr)
            continue;
        format.file_name = plugin_file;
        format.description = CCommon::ASCIIToUnicode(plugin_info->formats->name);	//插件支持文件类型的描述
        format.extensions_list = CCommon::ASCIIToUnicode(plugin_info->formats->exts);	//插件支持文件扩展名列表
        //解析扩展名列表到vector
        format.extensions.clear();
        size_t index = 0, last_index = 0;
        while (true)
        {
            index = format.extensions_list.find(L"*.", index + 1);
            wstring ext{ format.extensions_list.substr(last_index + 2, index - last_index - 2) };
            if (!ext.empty() && ext.back() == L';')
                ext.pop_back();
            format.extensions.push_back(ext);
            if (!CCommon::IsItemInVector(CAudioCommon::m_all_surpported_extensions, ext))
                CAudioCommon::m_all_surpported_extensions.push_back(ext);
            if (index == wstring::npos)
                break;
            last_index = index;
        }
        CAudioCommon::m_surpported_format.push_back(format);

        //载入MIDI音色库，用于播放MIDI
        if (format.description == L"MIDI")
        {
            m_bass_midi_lib.Init(plugin_dir + plugin_file);
            m_sfont_name = theApp.m_str_table.LoadText(L"UI_TXT_SF2_NAME_NONE");
            m_sfont.font = 0;
            if (m_bass_midi_lib.IsSucceed())
            {
                wstring sf2_path = theApp.m_play_setting_data.sf2_path;
                if (!CCommon::FileExist(sf2_path))		//如果设置的音色库路径不存在，则从.\Plugins\soundfont\目录下查找音色库文件
                {
                    vector<wstring> sf2s;
                    CCommon::GetFiles(plugin_dir + L"soundfont\\*.sf2", sf2s);
                    if (!sf2s.empty())
                        sf2_path = plugin_dir + L"soundfont\\" + sf2s[0];
                }
                if (CCommon::FileExist(sf2_path))
                {
                    m_sfont.font = m_bass_midi_lib.BASS_MIDI_FontInit(sf2_path.c_str(), BASS_UNICODE);
                    if (m_sfont.font == 0)
                    {
                        wstring info = theApp.m_str_table.LoadTextFormat(L"LOG_SF2_LOAD_FAILED", { sf2_path });
                        theApp.WriteLog(info);
                        m_sfont_name = theApp.m_str_table.LoadText(L"UI_TXT_SF2_NAME_FAILDE");
                    }
                    else
                    {
                        //获取音色库信息
                        BASS_MIDI_FONTINFO sfount_info;
                        m_bass_midi_lib.BASS_MIDI_FontGetInfo(m_sfont.font, &sfount_info);
                        m_sfont_name = CCommon::StrToUnicode(sfount_info.name);
                    }
                    m_sfont.preset = -1;
                    m_sfont.bank = 0;
                }
            }
        }
    }
}

void CBassCore::UnInitCore()
{
    CSingleLock sync(&m_critical, TRUE);
    BASS_Stop();	//停止输出
    BASS_Free();	//释放Bass资源
    if (m_bass_midi_lib.IsSucceed() && m_sfont.font != 0)
        m_bass_midi_lib.BASS_MIDI_FontFree(m_sfont.font);
    m_bass_midi_lib.UnInit();
    for (const auto& handle : m_plugin_handles)		//释放插件句柄
    {
        BASS_PluginFree(handle);
    }
    m_fading = false;
}

unsigned int CBassCore::GetHandle()
{
    return m_musicStream;
}

std::wstring CBassCore::GetAudioType()
{
    return CAudioCommon::GetBASSChannelDescription(m_channel_info.ctype);
}

void CBassCore::MidiLyricSync(HSYNC handle, DWORD channel, DWORD data, void * user)
{
    if (!m_bass_midi_lib.IsSucceed())
        return;
    m_midi_lyric.midi_no_lyric = false;
    BASS_MIDI_MARK mark;
    m_bass_midi_lib.BASS_MIDI_StreamGetMark(channel, (DWORD)user, data, &mark); // get the lyric/text
    if (mark.text[0] == '@') return; // skip info
    if (mark.text[0] == '\\')
    {
        // clear display
        m_midi_lyric.midi_lyric.clear();
    }
    else if (mark.text[0] == '/')
    {
        m_midi_lyric.midi_lyric += L"\r\n";
        const char* text = mark.text + 1;
        m_midi_lyric.midi_lyric += CCommon::StrToUnicode(text, CodeType::ANSI);
    }
    else
    {
        m_midi_lyric.midi_lyric += CCommon::StrToUnicode(mark.text, CodeType::ANSI);
    }
}

void CBassCore::MidiEndSync(HSYNC handle, DWORD channel, DWORD data, void * user)
{
    m_midi_lyric.midi_lyric.clear();
}

void CBassCore::GetMidiPosition()
{
    CSingleLock sync(&m_critical, TRUE);
    if (m_is_midi)
    {
        //获取midi音乐的进度并转换成节拍数。（其中+ (m_midi_info.ppqn / 4)的目的是修正显示的节拍不准确的问题）
        m_midi_info.midi_position = static_cast<int>((BASS_ChannelGetPosition(m_musicStream, BASS_POS_MIDI_TICK) + (m_midi_info.ppqn / 4)) / m_midi_info.ppqn);
    }
}

void CBassCore::SetFXHandle()
{
    if (m_musicStream == 0) return;
    //if (!m_equ_enable) return;
    //设置每个均衡器通道的句柄
    for (int i{}; i < EQU_CH_NUM; i++)
    {
        m_equ_handle[i] = BASS_ChannelSetFX(m_musicStream, BASS_FX_DX8_PARAMEQ, 1);
    }
    //设置混响的句柄
    m_reverb_handle = BASS_ChannelSetFX(m_musicStream, BASS_FX_DX8_REVERB, 1);
}

void CBassCore::RemoveFXHandle()
{
    if (m_musicStream == 0) return;
    //移除每个均衡器通道的句柄
    for (int i{}; i < EQU_CH_NUM; i++)
    {
        if (m_equ_handle[i] != 0)
        {
            BASS_ChannelRemoveFX(m_musicStream, m_equ_handle[i]);
            m_equ_handle[i] = 0;
        }
    }
    //移除混响的句柄
    if (m_reverb_handle != 0)
    {
        BASS_ChannelRemoveFX(m_musicStream, m_reverb_handle);
        m_reverb_handle = 0;
    }
}

void CBassCore::GetBASSAudioInfo(HSTREAM hStream, SongInfo & song_info, int flag)
{
    //获取长度
    if (flag&AF_LENGTH)
        song_info.end_pos = CBassCore::GetBASSSongLength(hStream);
    //获取比特率
    if(flag&AF_BITRATE)
    {
        float bitrate{};
        BASS_ChannelGetAttribute(hStream, BASS_ATTRIB_BITRATE, &bitrate);
        song_info.bitrate = static_cast<int>(bitrate + 0.5f);
    }
    //获取采样频率、通道数、位深度
    if (flag & AF_CHANNEL_INFO)
    {
        BASS_CHANNELINFO info{};
        BASS_ChannelGetInfo(hStream, &info);
        song_info.freq = info.freq;
        song_info.bits = static_cast<BYTE>(info.origres);
        song_info.channels = static_cast<BYTE>(info.chans);
    }
    if(flag&AF_TAG_INFO)
    {
        CAudioTag audio_tag(song_info, hStream);
        audio_tag.GetAudioTag();
        audio_tag.GetAudioRating();
        //获取midi音乐的标题
        if (CBassCore::m_bass_midi_lib.IsSucceed() && audio_tag.GetAudioType() == AU_MIDI)
        {
            BASS_MIDI_MARK mark;
            if (CBassCore::m_bass_midi_lib.BASS_MIDI_StreamGetMark(hStream, BASS_MIDI_MARK_TRACK, 0, &mark) && !mark.track)
            {
                song_info.title = CCommon::StrToUnicode(mark.text);
            }
        }
    }
}

int CBassCore::GetChannels()
{
    return m_channel_info.chans;
}

int CBassCore::GetFReq()
{
    return m_channel_info.freq;
}

wstring CBassCore::GetSoundFontName()
{
    return m_sfont_name;
}

void CBassCore::Open(const wchar_t * file_path)
{
    CSingleLock sync(&m_critical, TRUE);

    m_last_playing_state = PLAYING_STATE_DEFAULT_VALUE;
    if (m_musicStream != 0)     //打开前如果音频句柄没有关闭，先将其关闭，确保同时只能打开一个音频文件
        Close();

    m_file_path = file_path;
    if (CCommon::IsURL(m_file_path))
        m_musicStream = BASS_StreamCreateURL(file_path, 0, BASS_SAMPLE_FLOAT, NULL, NULL);
    else
        m_musicStream = BASS_StreamCreateFile(FALSE, /*(GetCurrentFilePath()).c_str()*/file_path, 0, 0, BASS_SAMPLE_FLOAT);
    BASS_ChannelGetInfo(m_musicStream, &m_channel_info);
    m_is_midi = (CAudioCommon::GetAudioTypeByBassChannel(m_channel_info.ctype) == AudioType::AU_MIDI);
    if (m_bass_midi_lib.IsSucceed() && m_is_midi && m_sfont.font != 0)
        m_bass_midi_lib.BASS_MIDI_StreamSetFonts(m_musicStream, &m_sfont, 1);

    //如果文件是MIDI音乐，则打开时获取MIDI音乐的信息
    if (m_is_midi && m_bass_midi_lib.IsSucceed())
    {
        //获取MIDI音乐信息
        BASS_ChannelGetAttribute(m_musicStream, BASS_ATTRIB_MIDI_PPQN, &m_midi_info.ppqn); // get PPQN value
        m_midi_info.midi_length = static_cast<int>(BASS_ChannelGetLength(m_musicStream, BASS_POS_MIDI_TICK) / m_midi_info.ppqn);
        m_midi_info.tempo = m_bass_midi_lib.BASS_MIDI_StreamGetEvent(m_musicStream, 0, MIDI_EVENT_TEMPO);
        m_midi_info.speed = 60000000 / m_midi_info.tempo;
        //获取MIDI音乐内嵌歌词
        BASS_MIDI_MARK mark;
        m_midi_lyric.midi_lyric.clear();
        if (m_bass_midi_lib.BASS_MIDI_StreamGetMark(m_musicStream, BASS_MIDI_MARK_LYRIC, 0, &mark)) // got lyrics
            BASS_ChannelSetSync(m_musicStream, BASS_SYNC_MIDI_MARK, BASS_MIDI_MARK_LYRIC, MidiLyricSync, (void*)BASS_MIDI_MARK_LYRIC);
        else if (m_bass_midi_lib.BASS_MIDI_StreamGetMark(m_musicStream, BASS_MIDI_MARK_TEXT, 20, &mark)) // got text instead (over 20 of them)
            BASS_ChannelSetSync(m_musicStream, BASS_SYNC_MIDI_MARK, BASS_MIDI_MARK_TEXT, MidiLyricSync, (void*)BASS_MIDI_MARK_TEXT);
        BASS_ChannelSetSync(m_musicStream, BASS_SYNC_END, 0, MidiEndSync, 0);
        m_midi_lyric.midi_no_lyric = true;
    }
    SetFXHandle();
    BASS_ChannelGetAttribute(m_musicStream, BASS_ATTRIB_FREQ, &m_freq);
}

void CBassCore::Close()
{
    CSingleLock sync(&m_critical, TRUE);
    if (m_musicStream != 0)
    {
        if (KillTimer(theApp.m_pMainWnd->GetSafeHwnd(), FADE_TIMER_ID))
            BASS_ChannelStop(m_musicStream);
        RemoveFXHandle();
        BASS_StreamFree(m_musicStream);
        m_musicStream = 0;
    }
}

void CBassCore::Play()
{
    m_playing_state = PS_PLAYING;
    if (theApp.m_play_setting_data.fade_effect && theApp.m_play_setting_data.fade_time > 0)     //如果设置了播放时音量淡入淡出
    {
        KillTimer(theApp.m_pMainWnd->GetSafeHwnd(), FADE_TIMER_ID);
        int pos = GetCurPosition();
        pos -= (theApp.m_play_setting_data.fade_time / 2);
        if (pos < 0)
            pos = 0;
        SetCurPosition(pos);
        BASS_ChannelSetAttribute(m_musicStream, BASS_ATTRIB_VOL, 0);        //先将音量设为0
        BASS_ChannelPlay(m_musicStream, FALSE);
        float volume = static_cast<float>(m_volume) / 100;
        BASS_ChannelSlideAttribute(m_musicStream, BASS_ATTRIB_VOL, volume, theApp.m_play_setting_data.fade_time);   //音量渐变到原来的音量
    }
    else
    {
        BASS_ChannelPlay(m_musicStream, FALSE);
    }
}

void CBassCore::Pause()
{
    m_playing_state = PS_PAUSED;
    if (theApp.m_play_setting_data.fade_effect && theApp.m_play_setting_data.fade_time > 0)     //如果设置了播放时音量淡入淡出
    {
        BASS_ChannelSlideAttribute(m_musicStream, BASS_ATTRIB_VOL, 0, theApp.m_play_setting_data.fade_time);        //音量渐变到0
        KillTimer(theApp.m_pMainWnd->GetSafeHwnd(), FADE_TIMER_ID);
        m_fading = true;
        //设置一个淡出时间的定时器
        SetTimer(theApp.m_pMainWnd->GetSafeHwnd(), FADE_TIMER_ID, theApp.m_play_setting_data.fade_time, [](HWND Arg1, UINT Arg2, UINT_PTR Arg3, DWORD Arg4)
        {
            KillTimer(theApp.m_pMainWnd->GetSafeHwnd(), FADE_TIMER_ID);
            BASS_ChannelPause(CPlayer::GetInstance().GetBassHandle());     //当定时器器触发时，即音量已经渐变到0，执行暂停操作
            m_fading = false;
        });
    }
    else
    {
        BASS_ChannelPause(m_musicStream);
    }
}

void CBassCore::Stop()
{
    m_playing_state = PS_STOPED;
    DWORD playing_state = BASS_ChannelIsActive(m_musicStream);
    if (theApp.m_play_setting_data.fade_effect && theApp.m_play_setting_data.fade_time > 0 && playing_state == BASS_ACTIVE_PLAYING)
    {
        BASS_ChannelSlideAttribute(m_musicStream, BASS_ATTRIB_VOL, 0, theApp.m_play_setting_data.fade_time);
        KillTimer(theApp.m_pMainWnd->GetSafeHwnd(), FADE_TIMER_ID);
        m_fading = true;
        SetTimer(theApp.m_pMainWnd->GetSafeHwnd(), FADE_TIMER_ID, theApp.m_play_setting_data.fade_time, [](HWND Arg1, UINT Arg2, UINT_PTR Arg3, DWORD Arg4)
        {
            KillTimer(theApp.m_pMainWnd->GetSafeHwnd(), FADE_TIMER_ID);
            BASS_ChannelStop(CPlayer::GetInstance().GetBassHandle());
            BASS_ChannelSetPosition(CPlayer::GetInstance().GetBassHandle(), 0, BASS_POS_BYTE);
            m_fading = false;
        });
    }
    else
    {
        BASS_ChannelStop(m_musicStream);
        SetCurPosition(0);
    }
}

void CBassCore::SetVolume(int vol)
{
    if (!m_fading)
    {
        m_volume = vol;
        float volume = static_cast<float>(vol) / 100.0f;
        BASS_ChannelSetAttribute(m_musicStream, BASS_ATTRIB_VOL, volume);
    }
}

void CBassCore::SetSpeed(float speed)
{
    float freq;
    if (std::fabs(speed) < 0.01 || std::fabs(speed - 1) < 0.01 || speed < MIN_PLAY_SPEED || speed > MAX_PLAY_SPEED)
        speed = 0;
    freq = m_freq * speed;
    BASS_ChannelSetAttribute(m_musicStream, BASS_ATTRIB_FREQ, freq);
}

bool CBassCore::SongIsOver()
{
    DWORD state = BASS_ChannelIsActive(m_musicStream);
    bool is_over{ (m_last_playing_state == BASS_ACTIVE_PLAYING && state == BASS_ACTIVE_STOPPED)
        || m_error_code == BASS_ERROR_ENDED };
    m_last_playing_state = state;
    return is_over && m_playing_state == PS_PLAYING && m_musicStream != 0;
}

int CBassCore::GetCurPosition()
{
    CSingleLock sync(&m_critical, TRUE);
    if (m_musicStream == 0)
        return 0;
    QWORD pos_bytes;
    pos_bytes = BASS_ChannelGetPosition(m_musicStream, BASS_POS_BYTE);
    double pos_sec;
    pos_sec = BASS_ChannelBytes2Seconds(m_musicStream, pos_bytes);
    int current_position = static_cast<int>(pos_sec * 1000);
    if (current_position == -1000) current_position = 0;

    GetMidiPosition();
    return current_position;
}

int CBassCore::GetSongLength()
{
    CSingleLock sync(&m_critical, TRUE);
    QWORD lenght_bytes;
    lenght_bytes = BASS_ChannelGetLength(m_musicStream, BASS_POS_BYTE);
    double length_sec;
    length_sec = BASS_ChannelBytes2Seconds(m_musicStream, lenght_bytes);
    int song_length = static_cast<int>(length_sec * 1000);
    if (song_length <= -1000) song_length = 0;
    return song_length;
}

void CBassCore::SetCurPosition(int position)
{
    CSingleLock sync(&m_critical, TRUE);
    double pos_sec = static_cast<double>(position) / 1000.0;
    QWORD pos_bytes;
    pos_bytes = BASS_ChannelSeconds2Bytes(m_musicStream, pos_sec);
    BASS_ChannelSetPosition(m_musicStream, pos_bytes, BASS_POS_BYTE);
    m_midi_lyric.midi_lyric.clear();
    GetMidiPosition();
}

void CBassCore::GetAudioInfo(SongInfo & song_info, int flag)
{
    CSingleLock sync(&m_critical, TRUE);
    GetBASSAudioInfo(m_musicStream, song_info, flag);
}

void CBassCore::GetAudioInfo(const wchar_t * file_path, SongInfo & song_info, int flag)
{
    HSTREAM hStream;
    if (song_info.file_path.empty())
        song_info.file_path = file_path;
    hStream = BASS_StreamCreateFile(FALSE, file_path, 0, 0, BASS_SAMPLE_FLOAT);
    GetBASSAudioInfo(hStream, song_info, flag);
    BASS_StreamFree(hStream);
}

bool CBassCore::EncodeAudio(SongInfo song_info, const wstring& dest_file_path, EncodeFormat encode_format, void* encode_para, int dest_freq, EncodeAudioProc proc)
{
    wstring out_file_path_temp = CCommon::GetTemplatePath() + CONVERTING_TEMP_FILE_NAME;     //转换时的临时文件名

    //创建解码通道
    const int BUFF_SIZE{ 20000 };
    // char buff[BUFF_SIZE];
    std::unique_ptr<char[]> buff(new char[BUFF_SIZE]);
    HSTREAM hStream = BASS_StreamCreateFile(FALSE, song_info.file_path.c_str(), 0, 0, BASS_STREAM_DECODE/* | BASS_SAMPLE_FLOAT*/);
    if (hStream == 0)
    {
        proc(CONVERT_ERROR_FILE_CANNOT_OPEN);
        return false;
    }

    //获取通道信息
    BASS_CHANNELINFO channel_info;
    BASS_ChannelGetInfo(hStream, &channel_info);
    bool is_midi = (CAudioCommon::GetAudioTypeByBassChannel(channel_info.ctype) == AU_MIDI);
    if (is_midi)
    {
        bool sf2_loaded = (CBassCore::m_bass_midi_lib.IsSucceed() && CBassCore::m_sfont.font != 0);
        if (sf2_loaded)
        {
            CBassCore::m_bass_midi_lib.BASS_MIDI_StreamSetFonts(hStream, &CBassCore::m_sfont, 1);
        }
        else
        {
            BASS_StreamFree(hStream);
            //::PostMessage(pthis->GetSafeHwnd(), WM_CONVERT_PROGRESS, file_index, CONVERT_ERROR_MIDI_NO_SF2);
            proc(CONVERT_ERROR_MIDI_NO_SF2);
            return false;
        }
    }

    //转换采样频率
    HSTREAM hStreamOld = 0;
    if (dest_freq > 0 && dest_freq != channel_info.freq)
    {
        hStreamOld = hStream;
        hStream = m_bass_mix_lib.BASS_Mixer_StreamCreate(dest_freq, channel_info.chans, BASS_MIXER_END | BASS_STREAM_DECODE);
        if (hStream != 0)
        {
            m_bass_mix_lib.BASS_Mixer_StreamAddChannel(hStream, hStreamOld, 0);
        }
        else
        {
            hStream = hStreamOld;
            hStreamOld = 0;
        }
    }

    //获取流的长度
    QWORD length = BASS_ChannelGetLength(hStreamOld != 0 ? hStreamOld : hStream, 0);
    if (hStreamOld != 0 && channel_info.freq != 0)     //如果开启了转换采样频率
        length = length * dest_freq / channel_info.freq;
    if (length == 0) length = 1;	//防止length作为除数时为0

    //如果转换的音频是cue音轨，则先定位到曲目开始处
    if (song_info.is_cue)
    {
        CBassCore::SetCurrentPosition(hStreamOld != 0 ? hStreamOld : hStream, song_info.start_pos.toInt());
    }

    HENCODE hEncode;
    if (encode_format != EncodeFormat::WMA)
    {
        //设置解码器命令行参数
        wstring cmdline;
        switch (encode_format)
        {
        case EncodeFormat::MP3:
        {
            if (encode_para == nullptr)
                break;
            MP3EncodePara* mp3_para = (MP3EncodePara*)encode_para;
            //设置lame命令行参数
            cmdline = m_encode_dir;
            cmdline += L"lame.exe ";
            cmdline += mp3_para->cmd_para;
            cmdline += L" - \"";
            cmdline += out_file_path_temp;
            cmdline.push_back(L'\"');
        }
        break;
        case EncodeFormat::OGG:
        {
            if (encode_para == nullptr)
                break;
            OggEncodePara* ogg_para = (OggEncodePara*)encode_para;

            cmdline = m_encode_dir;
            CString str;
            str.Format(_T("oggenc -q %d"), ogg_para->encode_quality);
            cmdline += str;
            cmdline += L" -o \"";
            cmdline += dest_file_path;
            cmdline += L"\" -";
        }
        break;
        case EncodeFormat::FLAC:
        {
            if (encode_para == nullptr)
                break;
            FlacEncodePara* flac_para = (FlacEncodePara*)encode_para;
            cmdline = m_encode_dir;
            cmdline += _T("flac.exe ");
            cmdline += flac_para->cmd_para.c_str();
            cmdline += L" -o \"";
            cmdline += dest_file_path;
            cmdline += L"\" -";
        }
        break;
        default:
            cmdline = dest_file_path;
        }
        //开始解码
        hEncode = m_bass_encode_lib.BASS_Encode_StartW(hStream, cmdline.c_str(), BASS_ENCODE_AUTOFREE | (encode_format == EncodeFormat::WAV ? BASS_ENCODE_PCM : 0), NULL, 0);
        if (hEncode == 0)
        {
            BASS_StreamFree(hStream);
            if (hStreamOld != 0)
                BASS_StreamFree(hStreamOld);
            proc(CONVERT_ERROR_ENCODE_CHANNEL_FAILED);
            return false;
        }
    }
    else		//输出为WMA时使用专用的编码器
    {
        if (!m_bass_wma_lib.IsSucceed())
        {
            BASS_StreamFree(hStream);
            if (hStreamOld != 0)
                BASS_StreamFree(hStreamOld);
            return false;
        }
        //开始解码
        if (encode_para == nullptr)
            return false;
        WmaEncodePara* wma_para = (WmaEncodePara*)encode_para;
        int wma_bitrate;
        if (wma_para->cbr)
            wma_bitrate = wma_para->cbr_bitrate * 1000;
        else
            wma_bitrate = wma_para->vbr_quality;
        hEncode = m_bass_wma_lib.BASS_WMA_EncodeOpenFileW(hStream, NULL, BASS_WMA_ENCODE_STANDARD | BASS_WMA_ENCODE_SOURCE, wma_bitrate, dest_file_path.c_str());
        int error = BASS_ErrorGetCode();
        int error_code{ CONVERT_ERROR_ENCODE_CHANNEL_FAILED };
        //如果出现了错误，则写入错误日志
        if (error != 0)
        {
            wstring log_str = theApp.m_str_table.LoadTextFormat(L"LOG_BASS_FORMAT_CONVERT_ERROR", { song_info.file_path, L"WMA", error });
            theApp.WriteLog(log_str);
            switch (error)
            {
            case BASS_ERROR_WMA:
                error_code = CONVERT_ERROR_WMA_NO_WMP9_OR_LATER;
                break;
            case BASS_ERROR_NOTAVAIL:
                error_code = CONVERT_ERROR_WMA_NO_SUPPORTED_ENCODER;
                break;
            default:
                break;
            }
        }
        if (hEncode == 0)
        {
            BASS_StreamFree(hStream);
            if (hStreamOld != 0)
                BASS_StreamFree(hStreamOld);
            proc(error_code);
            return false;
        }
    }

    while (BASS_ChannelIsActive(hStream) != BASS_ACTIVE_STOPPED)
    {
        if (theApp.m_format_convert_dialog_exit)
        {
            m_bass_encode_lib.BASS_Encode_Stop(hEncode);
            BASS_StreamFree(hStream);
            if (hStreamOld != 0)
                BASS_StreamFree(hStreamOld);
            return false;
        }
        // BASS_ChannelGetData(hStream, buff, BUFF_SIZE);
        BASS_ChannelGetData(hStream, buff.get(), BUFF_SIZE);
        //if (m_bass_encode_lib.BASS_Encode_IsActive(hStream) == 0)
        //{
        //	m_bass_encode_lib.BASS_Encode_Stop(hEncode);
        //	BASS_StreamFree(hStream);
        //}

        //获取转换百分比
        int percent;
        static int last_percent{ -1 };
        if (!song_info.is_cue)
        {
            QWORD position = BASS_ChannelGetPosition(hStream, 0);
            percent = static_cast<int>(position * 100 / length);
        }
        else
        {
            int cue_position = CBassCore::GetBASSCurrentPosition(hStreamOld != 0 ? hStreamOld : hStream) - song_info.start_pos.toInt();
            int cue_length = song_info.length().toInt();
            percent = static_cast<int>(cue_position * 100 / cue_length);
            if (percent == 100)
                break;
        }
        if (percent < 0 || percent>100)
        {
            proc(CONVERT_ERROR_ENCODE_PARA_ERROR);
            BASS_StreamFree(hStream);
            if (hStreamOld != 0)
                BASS_StreamFree(hStreamOld);
            return false;
        }
        if (last_percent != percent)
        {
            proc(percent);
        }
        last_percent = percent;
    }

    BASS_StreamFree(hStream);
    if (hStreamOld != 0)
        BASS_StreamFree(hStreamOld);

    //由于在转换mp3文件时将目标文件输出到了临时目录，因此转换完成后将此文件移动到输出目录
    if (encode_format == EncodeFormat::MP3)
    {
        CFilePathHelper out_file_path_helper{ dest_file_path };
        CommonDialogMgr::MoveAFile(AfxGetMainWnd()->GetSafeHwnd(), out_file_path_temp, out_file_path_helper.GetDir());
        if (CCommon::FileExist(out_file_path_helper.GetFilePath()))
            CommonDialogMgr::DeleteAFile(AfxGetMainWnd()->GetSafeHwnd(), out_file_path_helper.GetFilePath());
        CCommon::FileRename(out_file_path_helper.GetDir() + CONVERTING_TEMP_FILE_NAME, out_file_path_helper.GetFileName());
    }
    return true;
}

bool CBassCore::InitEncoder()
{
    wstring encode_dll_path;
    wstring wma_dll_path;
    wstring plugin_dir;
    m_encode_dir = theApp.m_local_dir + L"Encoder\\";
    plugin_dir = theApp.m_local_dir + L"Plugins\\";
    encode_dll_path = m_encode_dir + L"bassenc.dll";
    m_bass_encode_lib.Init(encode_dll_path);

    wma_dll_path = plugin_dir + L"basswma.dll";
    m_bass_wma_lib.Init(wma_dll_path);

    m_bass_mix_lib.Init(m_encode_dir + L"bassmix.dll");

    return m_bass_encode_lib.IsSucceed();
}

void CBassCore::UnInitEncoder()
{
    m_bass_encode_lib.UnInit();
    m_bass_wma_lib.UnInit();
    m_bass_mix_lib.UnInit();
}

bool CBassCore::IsFreqConvertAvailable()
{
    return m_bass_mix_lib.IsSucceed();
}

bool CBassCore::IsMidi()
{
    return m_is_midi;
}

bool CBassCore::IsMidiConnotPlay()
{
    return (m_is_midi && m_sfont.font == 0);
}

std::wstring CBassCore::GetMidiInnerLyric()
{
    return m_midi_lyric.midi_lyric;
}

MidiInfo CBassCore::GetMidiInfo()
{
    return m_midi_info;
}

bool CBassCore::MidiNoLyric()
{
    return m_midi_lyric.midi_no_lyric;
}

PlayingState CBassCore::GetPlayingState()
{
    DWORD state = BASS_ChannelIsActive(m_musicStream);
    if (state == BASS_ACTIVE_PLAYING)
        return PS_PLAYING;
    else if (state == BASS_ACTIVE_PAUSED)
        return PS_PAUSED;
    else
        return PS_STOPED;
}

void CBassCore::ApplyEqualizer(int channel, int gain)
{
    if (channel < 0 || channel >= EQU_CH_NUM) return;
    //if (!m_equ_enable) return;
    if (gain < -15) gain = -15;
    if (gain > 15) gain = 15;
    BASS_DX8_PARAMEQ parameq;
    parameq.fBandwidth = 30;
    parameq.fCenter = FREQ_TABLE[channel];
    parameq.fGain = static_cast<float>(gain);
    BASS_FXSetParameters(m_equ_handle[channel], &parameq);

}

void CBassCore::SetReverb(int mix, int time)
{
    BASS_DX8_REVERB parareverb;
    parareverb.fInGain = 0;
    //parareverb.fReverbMix = static_cast<float>(mix) / 100 * 96 - 96;
    parareverb.fReverbMix = static_cast<float>(std::pow(static_cast<double>(mix) / 100, 0.1) * 96 - 96);
    parareverb.fReverbTime = static_cast<float>(time * 10);
    parareverb.fHighFreqRTRatio = 0.001f;
    BASS_FXSetParameters(m_reverb_handle, &parareverb);
}

void CBassCore::ClearReverb()
{
    BASS_DX8_REVERB parareverb;
    parareverb.fInGain = 0;
    parareverb.fReverbMix = -96;
    parareverb.fReverbTime = 0.001f;
    parareverb.fHighFreqRTRatio = 0.001f;
    BASS_FXSetParameters(m_reverb_handle, &parareverb);
}

void CBassCore::GetFFTData(float fft_data[FFT_SAMPLE])
{
    BASS_ChannelGetData(m_musicStream, fft_data, BASS_DATA_FFT512);
}

int CBassCore::GetErrorCode()
{
    m_error_code = BASS_ErrorGetCode();
    return m_error_code;
}

std::wstring CBassCore::GetErrorInfo(int error_code)
{
    // 这个方法获取写入错误日志的字符串，后面会附加CPlayer的方法名
    wstring info = theApp.m_str_table.LoadTextFormat(L"LOG_BASS_ERROR", { error_code, m_file_path });
    return info;
}

std::wstring CBassCore::GetErrorInfo()
{
    // 这个方法获取错误时状态栏显示的错误字符串“播放出错: <%1%>”
    int error_code = BASS_ErrorGetCode();
    wstring info = theApp.m_str_table.LoadTextFormat(L"UI_TXT_PLAYSTATUS_ERROR_CORE_BASS", { error_code });
    return info;
}

//int CBassCore::GetDeviceCount()
//{
//    BASS_DEVICEINFO info;
//    int count{};
//    for (int i = 0; BASS_GetDeviceInfo(i, &info); i++)
//        if (info.flags&BASS_DEVICE_ENABLED) // device is enabled
//            count++; // count it
//    return count;
//}

bool CBassCore::IsVolumeFadingOut()
{
    return m_fading;
}

int CBassCore::GetBASSCurrentPosition(HSTREAM hStream)
{
    QWORD pos_bytes;
    pos_bytes = BASS_ChannelGetPosition(hStream, BASS_POS_BYTE);
    double pos_sec;
    pos_sec = BASS_ChannelBytes2Seconds(hStream, pos_bytes);
    return static_cast<int>(pos_sec * 1000);
}

Time CBassCore::GetBASSSongLength(HSTREAM hStream)
{
    QWORD lenght_bytes;
    lenght_bytes = BASS_ChannelGetLength(hStream, BASS_POS_BYTE);
    double length_sec;
    length_sec = BASS_ChannelBytes2Seconds(hStream, lenght_bytes);
    int song_length_int = static_cast<int>(length_sec * 1000);
    if (song_length_int <= -1000) song_length_int = 0;
    return Time(song_length_int);		//将长度转换成Time结构
}

void CBassCore::SetCurrentPosition(HSTREAM hStream, int position)
{
    double pos_sec = static_cast<double>(position) / 1000.0;
    QWORD pos_bytes;
    pos_bytes = BASS_ChannelSeconds2Bytes(hStream, pos_sec);
    BASS_ChannelSetPosition(hStream, pos_bytes, BASS_POS_BYTE);
}
