#include "stdafx.h"
#include "Player.h"
#include "MusicPlayer2.h"
#include "COSUPlayerHelper.h"
#include "Playlist.h"
#include "BassCore.h"
#include "MciCore.h"
#include "FfmpegCore.h"
#include "MusicPlayerCmdHelper.h"
#include "SongDataManager.h"
#include "SongInfoHelper.h"
#include "RecentFolderAndPlaylist.h"
#include <random>
#include "IniHelper.h"
#include "MediaLibPlaylistMgr.h"

CPlayer CPlayer::m_instance;

CPlayer::CPlayer()
{
}

CPlayer& CPlayer::GetInstance()
{
    return m_instance;
}

CPlayer::~CPlayer()
{
    UnInitPlayerCore();
}

inline int CPlayer::GetNextShuffleIdx() const {
    // 获得下一m_shuffle_index
    int next = m_shuffle_index + 1;
    if (next >= static_cast<int>(m_shuffle_list.size()))
        next = 0;
    return next < 0 ? 0 : next;
}

inline int CPlayer::GetPrevShuffleIdx() const {
    // 获得前一m_shuffle_index
    int prev = m_shuffle_index - 1;
    if (prev < 0)
        prev = static_cast<int>(m_shuffle_list.size()) - 1;
    return prev < 0 ? 0 : prev;
}

inline void CPlayer::OnPlaylistChange() {
    //播放列表有修改时的相关操作，如清空下一首和随机播放记录
    m_random_list.clear();
    m_next_tracks.clear();
    m_shuffle_list.clear();
    m_is_shuffle_list_played = false;
}

void CPlayer::IniPlayerCore()
{
    if (m_pCore == nullptr)
    {
        if (theApp.m_play_setting_data.use_mci)
            m_pCore = new CMciCore();
        else if (theApp.m_play_setting_data.use_ffmpeg)
            m_pCore = new CFfmpegCore();

        //判断MCI或FFMPEG内核是否加载成功
        CDllLib* dll_lib = dynamic_cast<CDllLib*>(m_pCore);
        if (dll_lib != nullptr)
        {
            if (!dll_lib->IsSucceed())
            {
                dll_lib->UnInit();
                delete m_pCore;
                m_pCore = nullptr;
            }
        }

        if (m_pCore == nullptr)
        {
            m_pCore = new CBassCore();
            theApp.m_play_setting_data.use_mci = false;
            theApp.m_play_setting_data.use_ffmpeg = false;
        }

        m_pCore->InitCore();
        m_player_core_inited = true;
    }
}

void CPlayer::UnInitPlayerCore()
{
    if (m_pCore != nullptr)
    {
        m_player_core_inited = false;
        m_pCore->UnInitCore();
        delete m_pCore;
        m_pCore = nullptr;
    }
}

void CPlayer::Create()
{
    AfterSetTrack();     // 预先设置一次标题
    IniPlayerCore();
    LoadConfig();
    m_controls.InitSMTC(theApp.m_play_setting_data.use_media_trans_control);
    const PathInfo& path_info{ CRecentFolderMgr::Instance().GetCurrentItem() };
    bool change_to_default_playlist{ m_playlist_mode == PM_FOLDER && (CRecentFolderMgr::Instance().IsEmpty() || (!COSUPlayerHelper::IsOsuFolder(path_info.path) && !CAudioCommon::IsPathContainsAudioFile(path_info.path, path_info.contain_sub_folder)))};
    // 如果文件夹模式且当前文件夹没有音频文件那么切换到默认播放列表，清理无效（空）文件夹会在启动时更新媒体库进行（如果启用remove_file_not_exist_when_update）
    if (change_to_default_playlist)
    {
        const PlaylistInfo& playlist_info = CPlaylistMgr::Instance().GetDefaultPlaylist();
        SetPlaylist(playlist_info.path, playlist_info.track, playlist_info.position);
    }
    else if (m_playlist_mode == PM_FOLDER)
    {
        SetPath(path_info);
    }
    else if (m_playlist_mode == PM_MEDIA_LIB)
    {
        auto playlist_info = CMediaLibPlaylistMgr::Instance().GetCurrentPlaylistInfo();
        SetMediaLibPlaylist(playlist_info.medialib_type, playlist_info.path);
    }
    else
    {
        const PlaylistInfo& playlist_info = CPlaylistMgr::Instance().GetCurrentPlaylistInfo();
        SetPlaylist(playlist_info.path, playlist_info.track, playlist_info.position);
    }
}

void CPlayer::CreateWithFiles(const vector<wstring>& files)
{
    AfterSetTrack();
    IniPlayerCore();
    LoadConfig();
    m_controls.InitSMTC(theApp.m_play_setting_data.use_media_trans_control);
    OpenFilesInDefaultPlaylist(files);
}

void CPlayer::CreateWithPath(const wstring& path)
{
    AfterSetTrack();
    IniPlayerCore();
    LoadConfig();
    m_controls.InitSMTC(theApp.m_play_setting_data.use_media_trans_control);
    OpenFolder(path);
}

void CPlayer::CreateWithPlaylist(const wstring& playlist_path)
{
    AfterSetTrack();
    IniPlayerCore();
    LoadConfig();
    m_controls.InitSMTC(theApp.m_play_setting_data.use_media_trans_control);
    wstring playlist_path_{ playlist_path };
    OpenPlaylistFile(playlist_path_);
}

void CPlayer::IniPlayList(bool play, MediaLibRefreshMode refresh_mode)
{
    m_no_use = SongInfo{};  // 安全起见，防止意外写入被应用
    m_playlist.clear();
    //播放列表模式下
    if (m_playlist_mode == PM_PLAYLIST)
    {
        CPlaylistMgr::Instance().UpdateCurrentPlaylistType(m_playlist_path);
        CPlaylistFile playlist;
        playlist.LoadFromFile(m_playlist_path);
        playlist.MoveToSongList(m_playlist);
    }
    //媒体库播放列表模式下
    else if (m_playlist_mode == PM_MEDIA_LIB)
    {
        //根据类型和名称获取音频文件列表
        m_playlist = CMediaLibPlaylistMgr::Instance().GetSongList(m_media_lib_playlist_type, m_media_lib_playlist_name);
    }
    else
    {
        if (m_path.empty() || (m_path.back() != L'/' && m_path.back() != L'\\'))        //如果输入的新路径为空或末尾没有斜杠，则在末尾加上一个
            m_path.append(1, L'\\');
        if (COSUPlayerHelper::IsOsuFolder(m_path))
            COSUPlayerHelper::GetOSUAudioFiles(m_path, m_playlist);
        else
            CAudioCommon::GetAudioFiles(m_path, m_playlist, MAX_SONG_NUM, m_contain_sub_folder);
    }

    m_thread_info.refresh_mode = refresh_mode;
    m_thread_info.play = play;
    m_thread_info.playlist_mode = m_playlist_mode;
    m_thread_info.play_index = m_index;

    m_index = 0;            // 在初始化期间为维持程序其他部分不报错（可能需要（但不应该需要））保持m_playlist[m_index]有效
    if (m_playlist.empty())
        m_playlist.push_back(SongInfo{});       // 没有歌曲时向播放列表插入一个空的SongInfo对象

    //创建初始化播放列表的工作线程
    m_pThread = AfxBeginThread(IniPlaylistThreadFunc, &m_thread_info);
}

UINT CPlayer::IniPlaylistThreadFunc(LPVOID lpParam)
{
    CCommon::SetThreadLanguageList(theApp.m_str_table.GetLanguageTag());
    ThreadInfo* pInfo = (ThreadInfo*)lpParam;
    wstring remove_list_path{ pInfo->remove_list_path };
    SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_PLAYLIST_INI_START, (WPARAM)&remove_list_path, 0);

    // 播放列表模式下且play_index有效时重新查找play_index指向曲目，文件夹模式下play_index本就描述初始化完成的播放列表故无须改动
    SongInfo cur_song;
    vector<SongInfo>& play_list = GetInstance().m_playlist;
    if (pInfo->playlist_mode && pInfo->play_index >= 0 && pInfo->play_index < static_cast<int>(play_list.size()))
        cur_song = play_list[pInfo->play_index];

    bool exit_flag{};
    int update_cnt{};
    // 解析cue并更新所有曲目到媒体库，执行后仅file_path、track、is_cue可用
    // 此处不应设置ignore_short为true，因为现在正在初始化播放列表，如果读取到短文件却不保存有可能导致列表每次打开耗时都很长
    CAudioCommon::GetAudioInfo(play_list, update_cnt, exit_flag, pInfo->process_percent, pInfo->refresh_mode);
    // 将媒体库内信息更新到播放列表
    CSongDataManager::GetInstance().LoadSongsInfo(play_list);

    bool find_succeed{ cur_song.file_path.empty() };    // 为true说明不必继续查找cur_song
    // 重新查找当前播放
    if (!find_succeed)
    {
        for (auto iter = play_list.begin(); iter != play_list.end(); ++iter)
        {
            if (cur_song.IsSameSong(*iter))
            {
                pInfo->play_index = iter - play_list.begin();
                find_succeed = true;
                break;
            }
        }
    }
    // 如果没有找到那么说明受到了cue解析影响，有以下情况
    if (!find_succeed && cur_song.is_cue)
    {
        // cue解析前就是cue条目时上面IsSameSong搜索失败的可能性很低，只有cue编辑等偏门条件能触发
        // 这里按cue_file_path和track匹配查找
        for (auto iter = play_list.begin(); iter != play_list.end(); ++iter)
        {
            if (!iter->is_cue) continue;
            if (cur_song.track == iter->track && cur_song.cue_file_path == iter->cue_file_path)
            {
                pInfo->play_index = iter - play_list.begin();
                find_succeed = true;
                break;
            }
        }
        // 没有找到的话下面实质上是将此cue条目退化为“已被cue使用的原始音频”进行搜索
    }
    // cur_song是cue原始文件/已被cue使用的原始音频/含内嵌cue的原始音频，播放其(FILE标签中的)第一条音轨
    if (!find_succeed)
    {
        int track{ INT_MAX };
        const wstring& find_str = cur_song.file_path;
        for (auto iter = play_list.begin(); iter != play_list.end(); ++iter)
        {
            if (!iter->is_cue) continue;    // 先判断开销较小的条件
            if (iter->track > track) continue;
            if (find_str == iter->cue_file_path || find_str == iter->file_path)
            {
                track = iter->track;
                pInfo->play_index = iter - play_list.begin();
                find_succeed = true;
                if (track <= 1)     // cue不存在比1小的音轨号所以不必继续搜索
                    break;
            }
        }
    }

    PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_PLAYLIST_INI_COMPLATE, 0, 0);
    return 0;
}

void CPlayer::IniPlaylistComplate()
{
    m_index = m_thread_info.play_index;

    if (m_index < 0 || m_index >= GetSongNum())
    {
        m_index = 0;                    // 确保当前歌曲序号不会超过歌曲总数
        m_current_position.fromInt(0);  // m_index失效时同时清除进度（这样略有不足，理论上只要m_index指向的歌曲改变就应当清除进度，不过这需要PathInfo和PlaylistInfo改track为SongInfo(SongKey)）
    }
    //统计列表总时长
    m_total_time = 0;
    for (const auto& song : m_playlist)
    {
        m_total_time += song.length().toInt();
    }

    //检查列表中的曲目是否在“我喜欢”播放列表中
    CPlaylistFile favourite_playlist;
    favourite_playlist.LoadFromFile(CPlaylistMgr::Instance().GetFavouritePlaylist().path);
    for (auto& item : m_playlist)
    {
        item.is_favourite = favourite_playlist.IsSongInPlaylist(item);
    }

    ASSERT(m_playing == 0);
    // 对播放列表排序
    if (m_playlist_mode == PM_PLAYLIST)                // 播放列表模式默认状态必须为未排序
    {
        ASSERT(m_sort_mode == SM_UNSORT);
        m_sort_mode = SM_UNSORT;
    }
    else if (m_playlist_mode == PM_FOLDER && m_sort_mode == SM_UNSORT)  // 文件夹模式不允许为未排序
    {
        ASSERT(FALSE);
        m_sort_mode = SM_U_FILE;
    }
    if ((m_playlist_mode == PM_FOLDER || m_playlist_mode == PM_MEDIA_LIB) && m_playlist.size() > 1)
        SortPlaylist(true);

    if (!IsPlaylistEmpty())         // 播放列表初始化完成，根据m_index,m_current_position,m_thread_info.play还原播放状态
    {
        bool tmp_find{ false };
        if (!m_current_song_tmp.IsEmpty())     // m_current_song_tmp不为空则改为查找播放此歌曲，同时定位到m_current_song_position_tmp
        {
            for (size_t i{}; i < m_playlist.size(); i++)
            {
                if (m_current_song_tmp.IsSameSong(m_playlist[i]))
                {
                    m_index = i;
                    m_current_position.fromInt(m_current_song_position_tmp);
                    m_thread_info.play = m_current_song_playing_tmp;
                    tmp_find = true;
                    break;
                }
            }
            m_current_song_tmp = SongInfo();
            m_current_song_position_tmp = 0;
            m_current_song_playing_tmp = false;
        }
        MusicControl(Command::OPEN);
        MusicControl(Command::SEEK);
        // 这行判断描述如下：m_current_song_tmp被找到时以m_current_song_playing_tmp为准，覆盖其他设置
        // 没有找到m_current_song_tmp则当(theApp.m_play_setting_data.auto_play_when_start || m_thread_info.play)为true时播放
        if ((theApp.m_play_setting_data.auto_play_when_start && !tmp_find) || m_thread_info.play)
            MusicControl(Command::PLAY);
    }
    else
    {
        // 列表为空没有执行OPEN时在这里复位显示
        m_index = 0;
        m_current_position.fromInt(0);
        m_song_length.fromInt(0);
        // 清除歌词和专辑封面
        m_album_cover.Destroy();
        m_album_cover_blur.Destroy();
        m_Lyrics = CLyrics();
        m_controls.UpdateControlsMetadata(SongInfo());
        MediaTransControlsLoadThumbnailDefaultImage();
    }

    OnPlaylistChange();
    AfterSetTrack();

    //初始化随机播放序号列表
    //在OnPlaylistChange后面以免被清空
    InitShuffleList(m_index);

    if (m_repeat_mode == RM_PLAY_RANDOM)
        m_random_list.push_back(m_index);

    SaveRecentInfoToFiles();

    // 移除文件夹后会切入默认播放列表，但此时实际上需要刷新媒体库文件夹/播放列表两个标签页
    if (!m_thread_info.remove_list_path.empty() && CCommon::IsFolder(m_thread_info.remove_list_path))
        CMusicPlayerCmdHelper::RefreshMediaTabData(CMusicPlayerCmdHelper::ML_FOLDER);
    // 刷新媒体库标签页（需要在SaveRecentInfoToFiles()之后，GetPlayStatusMutex().unlock()之前进行）
    if (m_playlist_mode == PM_FOLDER)
        CMusicPlayerCmdHelper::RefreshMediaTabData(CMusicPlayerCmdHelper::ML_FOLDER);
    else if (m_playlist_mode == PM_PLAYLIST)
        CMusicPlayerCmdHelper::RefreshMediaTabData(CMusicPlayerCmdHelper::ML_PLAYLIST);

    m_thread_info = ThreadInfo();
    // 检查过了只是保险起见
    ASSERT(m_loading);
    if (m_loading) GetPlayStatusMutex().unlock();
    m_loading = false;
}

void CPlayer::SearchLyrics(bool refresh)
{
    //检索正在播放的音频文件的歌词
    const SongInfo& cur_song = GetCurrentSongInfo();
    SongInfo cur_song_info{ CSongDataManager::GetInstance().GetSongInfo3(cur_song) };
    if (cur_song_info.lyric_file == NO_LYRIC_STR && !refresh)   // 歌曲标记为没有歌词且不要求强制刷新时返回
        return;

    wstring lyric_path;
    if (refresh || cur_song_info.lyric_file.empty() || !CCommon::FileExist(cur_song_info.lyric_file))
    {
        CMusicPlayerCmdHelper helper;
        lyric_path = helper.SearchLyricFile(cur_song, theApp.m_lyric_setting_data.lyric_fuzzy_match);
        cur_song_info.lyric_file = lyric_path;
        CSongDataManager::GetInstance().AddItem(cur_song_info);
    }
    else
    {
        lyric_path = cur_song_info.lyric_file;
    }
    GetCurrentSongInfo2().lyric_file = lyric_path;
}

void CPlayer::IniLyrics()
{
    //尝试获取内嵌歌词
    CLyrics inner_lyrics;		//音频文件内嵌歌词
    wstring lyric_str;			//内嵌歌词的原始文本
    if (theApp.m_lyric_setting_data.use_inner_lyric_first)
    {
        SongInfo song;
        song.file_path = GetCurrentFilePath();
        CAudioTag audio_tag(song, GetBassHandle());
        lyric_str = audio_tag.GetAudioLyric();
        inner_lyrics.LyricsFromRowString(lyric_str);
    }

    //获取关联歌词文件的歌词
    CLyrics file_lyrics;		//来自歌词文件
    if (!m_playlist.empty() && !GetCurrentSongInfo().lyric_file.empty())
    {
        file_lyrics = CLyrics{ GetCurrentSongInfo().lyric_file };
    }

    //判断使用内嵌歌词还是关联歌词文件的歌词
    if (inner_lyrics.IsEmpty() && !file_lyrics.IsEmpty())
    {
        m_Lyrics = file_lyrics;
        m_inner_lyric = false;
    }
    else if (theApp.m_lyric_setting_data.use_inner_lyric_first)
    {
        m_Lyrics = inner_lyrics;
        m_inner_lyric = !lyric_str.empty();
    }
    else
    {
        m_Lyrics = CLyrics();
        m_inner_lyric = false;
    }
}

void CPlayer::IniLyrics(const wstring& lyric_path)
{
    m_Lyrics = CLyrics{ lyric_path };
    GetCurrentSongInfo2().lyric_file = lyric_path;
    SongInfo song_info{ CSongDataManager::GetInstance().GetSongInfo3(GetCurrentSongInfo()) };
    song_info.lyric_file = lyric_path;
    CSongDataManager::GetInstance().AddItem(song_info);
}


void CPlayer::MusicControl(Command command, int volume_step)
{
    if (m_pCore == nullptr)
        return;

    // VOLUME_UP和VOLUME_DOWN可在无法播放时使用
    // stop和close也可以在m_index失效无法播放时使用（RemoveSong(s)）
    if (command != Command::VOLUME_ADJ && command != Command::STOP && command != Command::CLOSE)
    {
        if (!CCommon::IsURL(GetCurrentFilePath()) && !CCommon::FileExist(GetCurrentFilePath()))
        {
            m_error_state = ES_FILE_NOT_EXIST;
            return;
        }
    }

    switch (command)
    {
    case Command::OPEN:
    {
        m_file_opend = false;
        m_controls.ClearAll();  // Clear all metadata.
        SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_POST_MUSIC_STREAM_OPENED, 0, 0);
        m_error_code = 0;
        m_error_state = ES_NO_ERROR;
        SongInfo& cur_song = GetCurrentSongInfo2(); // 获取m_playlist[m_index]的引用，m_index无效时取得m_no_use
        m_is_osu = COSUPlayerHelper::IsOsuFile(cur_song.file_path);
        m_pCore->Open(cur_song.file_path.c_str());
        GetPlayerCoreError(L"Open");
        if (m_pCore->GetCoreType() == PT_BASS && GetBassHandle() == 0)
            m_error_state = ES_FILE_CANNOT_BE_OPEN;
        m_file_opend = true;
        //获取音频类型
        m_current_file_type = m_pCore->GetAudioType();  // 根据通道信息获取当前音频文件的类型
        if (m_current_file_type.empty())                // 如果获取不到音频文件的类型，则将其文件扩展名作为文件类型
        {
            CFilePathHelper file_path{ cur_song.file_path };
            m_current_file_type = file_path.GetFileExtension(true);
        }
        if (!cur_song.file_path.empty())
        {
            //打开时获取专辑封面
            SearchAlbumCover();
            //初始化歌词
            SearchLyrics();
            IniLyrics();
        }
        m_song_length = cur_song.length();
        SetVolume();
        if (std::fabs(m_speed - 1) > 0.01)
            SetSpeed(m_speed);
        memset(m_spectral_data, 0, sizeof(m_spectral_data));		//打开文件时清除频谱分析的数据
        //SetFXHandle();
        if (m_equ_enable)
            SetAllEqualizer();
        if (m_reverb_enable)
            m_pCore->SetReverb(m_reverb_mix, m_reverb_time);
        else
            m_pCore->ClearReverb();
        PostMessage(theApp.m_pMainWnd->m_hWnd, WM_MUSIC_STREAM_OPENED, 0, 0);
        m_controls.UpdateControlsMetadata(GetCurrentSongInfo());
        m_controls.UpdateControls(PlaybackStatus::Closed);          // OPEN时设置为停止，PLAY时再设置为PLAY
        m_enable_lastfm = theApp.m_media_lib_setting_data.enable_lastfm;
        if (m_enable_lastfm) {
            UpdateLastFMCurrentTrack(GetCurrentSongInfo());
        }
    }
    break;
    case Command::PLAY:
        ConnotPlayWarning();
        m_pCore->Play();
        m_playing = PS_PLAYING;
        GetPlayerCoreError(L"Play");
        m_controls.UpdateControls(PlaybackStatus::Playing);
        MediaTransControlsLoadThumbnailDefaultImage();
        break;
    case Command::CLOSE:
        //RemoveFXHandle();
        m_file_opend = false;   // 主窗口定时器使用此变量以阻止播放结束自动下一曲
        m_pCore->Close();
        m_playing = PS_STOPED;
        SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_AFTER_MUSIC_STREAM_CLOSED, 0, 0);
        m_controls.UpdateControls(PlaybackStatus::Closed);
        break;
    case Command::PAUSE:
        m_pCore->Pause();
        m_playing = PS_PAUSED;
        m_controls.UpdateControls(PlaybackStatus::Paused);
        MediaTransControlsLoadThumbnailDefaultImage();
        break;
    case Command::STOP:
        if (GetCurrentSongInfo().is_cue && GetCurrentSongInfo().start_pos > 0)
        {
            SeekTo(0);
            m_pCore->Pause();
        }
        else
        {
            m_pCore->Stop();
        }
        m_playing = PS_STOPED;
        m_current_position = Time();
        memset(m_spectral_data, 0, sizeof(m_spectral_data));		//停止时清除频谱分析的数据
        m_controls.UpdateControls(PlaybackStatus::Stopped);
        MediaTransControlsLoadThumbnailDefaultImage();
        break;
    case Command::FF:		//快进
        GetPlayerCoreCurrentPosition();		//获取当前位置（毫秒）
        m_current_position += 5000;		//每次快进5000毫秒
        if (m_current_position > m_song_length) m_current_position -= 5000;
        SeekTo(m_current_position.toInt());
        break;
    case Command::REW:		//快退
    {
        GetPlayerCoreCurrentPosition();		//获取当前位置（毫秒）
        int current_position = m_current_position.toInt();
        current_position -= 5000;		//每次快退5000毫秒
        if (current_position < 0) current_position = 0;		//防止快退到负的位置
        SeekTo(current_position);
    }
    break;
    case Command::PLAY_PAUSE:
        if (m_playing == PS_PLAYING)
        {
            m_pCore->Pause();
            m_playing = PS_PAUSED;
            m_controls.UpdateControls(PlaybackStatus::Paused);
        }
        else
        {
            ConnotPlayWarning();
            m_pCore->Play();
            m_playing = PS_PLAYING;
            GetPlayerCoreError(L"Play");
            m_controls.UpdateControls(PlaybackStatus::Playing);
        }
        MediaTransControlsLoadThumbnailDefaultImage();
        break;
    case Command::VOLUME_ADJ:
        m_volume += volume_step;
        if (m_volume > 100) m_volume = 100;
        if (m_volume < 0) m_volume = 0;
        SetVolume();
        break;
    case Command::SEEK:		//定位到m_current_position的位置
        if (m_current_position > m_song_length)
        {
            m_current_position = Time();
        }
        SeekTo(m_current_position.toInt());
        break;
    default:
        break;
    }
}

bool CPlayer::SongIsOver() const
{
    if (m_pCore->SongIsOver())
        return true;
    if (GetCurrentSongInfo().is_cue || IsMciCore())
        return (m_playing == PS_PLAYING && m_current_position >= m_song_length && m_current_position.toInt() != 0);
    return false;
}

void CPlayer::GetPlayerCoreCurrentPosition()
{
    int current_position_int = m_pCore->GetCurPosition();
    if (!IsPlaylistEmpty() && GetCurrentSongInfo().is_cue)
    {
        current_position_int -= GetCurrentSongInfo().start_pos.toInt();
    }
    m_current_position.fromInt(current_position_int);
}


void CPlayer::SetVolume()
{
    int volume = m_volume;
    volume = volume * theApp.m_nc_setting_data.volume_map / 100;
    m_pCore->SetVolume(volume);
    GetPlayerCoreError(L"SetVolume");
    SendMessage(theApp.m_pMainWnd->m_hWnd, WM_VOLUME_CHANGED, 0, 0);
}


void CPlayer::CalculateSpectralData()
{
    //memcpy_s(m_last_spectral_data, sizeof(m_last_spectral_data), m_spectral_data, sizeof(m_spectral_data));

    if (m_pCore != nullptr && ((GetBassHandle() && m_playing != 0 && m_current_position.toInt() < m_song_length.toInt() - 500)     //确保音频句柄不为空，并且歌曲最后500毫秒不显示频谱，以防止歌曲到达末尾无法获取频谱的错误
        || m_pCore->GetCoreType() == PT_FFMPEG) && m_pCore->GetPlayingState() != PS_STOPED)
    {
        int scale = (m_pCore->GetCoreType() == PT_FFMPEG ? 100 : 60);
        m_pCore->GetFFTData(m_fft);
        for (int i{}; i < FFT_SAMPLE; i++)
            m_fft[i] = std::abs(m_fft[i]);
        if (theApp.m_app_setting_data.use_old_style_specturm)
            CSpectralDataHelper::SpectralDataMapOld(m_fft, m_spectral_data, scale);
        else
            m_spectrum_data_helper.SpectralDataMap(m_fft, m_spectral_data, scale);
    }
    else
    {
        memset(m_spectral_data, 0, sizeof(m_spectral_data));
    }
}


void CPlayer::CalculateSpectralDataPeak()
{
    //计算频谱顶端的高度
    if (m_pCore != nullptr && m_pCore->GetPlayingState() != PS_PAUSED)
    {
        static int fall_count[SPECTRUM_COL];
        for (int i{}; i < SPECTRUM_COL; i++)
        {
            if (m_spectral_data[i] > m_spectral_peak[i])
            {
                m_spectral_peak[i] = m_spectral_data[i];		//如果当前的频谱比上一次的频谱高，则频谱顶端高度则为当前频谱的高度
                fall_count[i] = 0;
            }
            else if (m_spectral_data[i] < m_spectral_peak[i])
            {
                fall_count[i]++;
                float fall_distance = fall_count[i] * (8.18824f / theApp.m_fps - 0.082353f);
                if (fall_distance < 0)
                    fall_distance = 0;
                m_spectral_peak[i] -= fall_distance;		//如果当前频谱比上一次的频谱主低，则频谱顶端的高度逐渐下降
            }
        }
    }
}

bool CPlayer::IsPlaying() const
{
    return m_playing == PS_PLAYING;
}

bool CPlayer::PlayTrack(int song_track, bool auto_next)
{
    if (!auto_next)     // auto_next参数仅在主定时器被设置为true，此时已获取播放状态锁
    {
        if (CPlayer::GetInstance().m_loading) return false;
        if (!CPlayer::GetInstance().GetPlayStatusMutex().try_lock_for(std::chrono::milliseconds(1000))) return false;
    }

    if (song_track >= 0) {
        m_next_tracks.clear();     //手动播放时复位下一首列表
    }
    bool stop{};    // 根据循环模式和参数song_track判断应当停止时设置为true
    switch (m_repeat_mode)
    {
    case RM_PLAY_ORDER:		//顺序播放

        if (song_track == NEXT)		//播放下一曲
        {
            if (!m_next_tracks.empty()) {
                song_track = m_next_tracks.front();
                m_next_tracks.pop_front();
            }
            else
            {
                song_track = m_index + 1;
            }
        }
        else if (song_track == PREVIOUS)		//播放上一曲
        {
            song_track = m_index - 1;
        }
        break;

    case RM_PLAY_SHUFFLE:		//无序播放
        if (m_shuffle_list.size() != m_playlist.size())
            InitShuffleList();
        if (!m_shuffle_list.empty())
        {
            if (song_track == NEXT)
            {
                if (!m_next_tracks.empty()) {
                    song_track = m_next_tracks.front();
                    m_next_tracks.pop_front();
                }
                else
                {
                    m_shuffle_index = GetNextShuffleIdx();
                    if (m_shuffle_index == 0 && m_is_shuffle_list_played || m_shuffle_list.empty())
                    {
                        //如果列表中的曲目已经随机播放完了一遍，则重新生成一个新的顺序
                        InitShuffleList();
                        if (m_shuffle_list.empty())
                            break;
                    }
                    song_track = m_shuffle_list[m_shuffle_index];
                }
            }
            else if (song_track == PREVIOUS)
            {
                if (m_shuffle_list.empty())
                    break;
                m_shuffle_index = GetPrevShuffleIdx();
                song_track = m_shuffle_list[m_shuffle_index];
            }
            else if (m_is_shuffle_list_played)
            {
                //防止指定播放歌曲时，生成的列表第一个仍是该歌曲导致连续播放两遍
                InitShuffleList(song_track);      //手动指定播放曲目时重新生成无序列表
            }
            m_is_shuffle_list_played = true;      //否则手动指定时，可能会出现下一曲仍是同一曲
        }
        break;

    case RM_PLAY_RANDOM:		//随机播放
        if (song_track == NEXT)
        {
            if (!m_next_tracks.empty()) {
                song_track = m_next_tracks.front();
                m_next_tracks.pop_front();
            }
            else
            {
                if (GetSongNum() > 1)
                {
                    song_track = CCommon::Random(0, GetSongNum());
                }
                else
                {
                    song_track = 0;      //只有一首歌
                }
            }
            m_random_list.push_back(song_track);	//保存随机播放过的曲目
        }
        else if (song_track == PREVIOUS)		//回溯上一个随机播放曲目
        {
            if (m_random_list.size() >= 2)
            {
                if (m_index == m_random_list.back())
                    m_random_list.pop_back();
                song_track = m_random_list.back();
            }
            else
                stop = true;
        }
        else if (song_track >= 0 && song_track < GetSongNum() && song_track != m_index)     //手动指定歌曲时
        {
            m_random_list.push_back(song_track);	//保存随机播放过的曲目
        }
        break;

    case RM_LOOP_TRACK:		//单曲循环
        if (auto_next)
        {
            if (song_track == NEXT)
            {
                if (!m_next_tracks.empty()) {
                    song_track = m_next_tracks.front();
                    m_next_tracks.pop_front();
                }
                else { 
                    song_track = m_index; 
                }
            }
            else if (song_track == PREVIOUS)
                song_track = m_index;
        }
        else
        {
            LoopPlaylist(song_track);   //如果不是播放完成后自动下一曲，则执行列表循环中的代码
        }
        break;

    case RM_LOOP_PLAYLIST:		//列表循环
        if (!m_next_tracks.empty()) {
            song_track = m_next_tracks.front();
            m_next_tracks.pop_front();
        }
        else
        {
            LoopPlaylist(song_track);
        }
        break;

    case RM_PLAY_TRACK:
        if (auto_next)
        {
            if (song_track == NEXT || song_track == PREVIOUS)
                stop = true;
        }
        else
        {
            LoopPlaylist(song_track);   //如果不是播放完成后自动下一曲，则执行列表循环中的代码
        }
        break;
    }

    if (song_track < 0 || song_track >= GetSongNum())
    {
        song_track = 0;
        if (auto_next)
            stop = true;
    }
    if (stop)
        MusicControl(Command::STOP);
    else
    {
        m_current_position.fromInt(0);      //关闭前将当前播放位置清零
        MusicControl(Command::CLOSE);
        m_index = song_track;
        MusicControl(Command::OPEN);
        if (GetCurrentSongInfo().is_cue)
            SeekTo(0);
        MusicControl(Command::PLAY);
        GetPlayerCoreCurrentPosition();
    }
    AfterSetTrack();
    SaveConfig();
    SaveRecentInfoToFiles(false);
    if(!auto_next)
        CPlayer::GetInstance().GetPlayStatusMutex().unlock();
    return true;
}

bool CPlayer::PlayAfterCurrentTrack(const std::vector<int>& tracks_to_play)
{
    bool add{ false };
    for (auto it = tracks_to_play.rbegin(); it != tracks_to_play.rend(); ++it)  // 为维持次序不变此处逆序遍历
    {
        const int& track = *it;
        if (track >= 0 && track < static_cast<int>(m_playlist.size()))
        {
            m_next_tracks.push_front(track);
            add = true;
        }
    }
    return add;
}

bool CPlayer::PlayAfterCurrentTrack(const std::vector<SongInfo>& tracks_to_play)
{
    bool add{ false };
    for (auto it = tracks_to_play.rbegin(); it != tracks_to_play.rend(); ++it)  // 为维持次序不变此处逆序遍历
    {
        int index = IsSongInPlayList(*it);
        if(index != -1)
        {
            m_next_tracks.push_front(index);
            add = true;
        }
    }
    return add;
}

void CPlayer::LoopPlaylist(int& song_track)
{
    if (song_track == NEXT)		//播放下一曲
    {
        song_track = m_index + 1;
        if (song_track >= GetSongNum()) song_track = 0;
        if (song_track < 0) song_track = GetSongNum() - 1;
    }
    if (song_track == PREVIOUS)		//播放上一曲
    {
        song_track = m_index - 1;
        if (song_track >= GetSongNum()) song_track = 0;
        if (song_track < 0) song_track = GetSongNum() - 1;
    }
}

void CPlayer::SaveRecentInfoToFiles(bool save_playlist)
{
    static bool initialized{ false };
    // 程序启动后第一次调用时不需要保存（m_playlist为空时进行播放列表保存会导致清空列表）
    if (!initialized)
    {
        initialized = true;
        return;
    }
    int song_num = IsPlaylistEmpty() ? 0 : GetSongNum();
    if (m_playlist_mode == PM_PLAYLIST)
    {
        if (save_playlist)
            SaveCurrentPlaylist();
        CPlaylistMgr::Instance().EmplacePlaylist(m_playlist_path, m_index, m_current_position.toInt(), song_num, m_total_time, CCommon::GetCurTimeElapse());
        CPlaylistMgr::Instance().SavePlaylistData();
    }
    else if (m_playlist_mode == PM_MEDIA_LIB)
    {
        CMediaLibPlaylistMgr::Instance().EmplaceMediaLibPlaylist(m_media_lib_playlist_type, m_media_lib_playlist_name, m_index, m_current_position.toInt(), song_num, m_total_time, CCommon::GetCurTimeElapse(), m_sort_mode);
        CMediaLibPlaylistMgr::Instance().SavePlaylistData();
    }
    else if (m_playlist_mode == PM_FOLDER)
    {
        CRecentFolderMgr::Instance().EmplaceRecentFolder(m_path, m_index, m_current_position.toInt(), m_sort_mode, song_num, m_total_time, m_contain_sub_folder);
        CRecentFolderMgr::Instance().SaveData();
    }
    CRecentFolderAndPlaylist::Instance().Init();
}

bool CPlayer::BeforeIniPlayList(bool continue_play, bool force_continue_play)
{
    if (m_loading) return false;
    if (!GetPlayStatusMutex().try_lock_for(std::chrono::milliseconds(1000))) return false;
    m_loading = true;
    IniPlayerCore();
    SaveRecentInfoToFiles();
    // 实现同曲目播放保持
    if ((continue_play && theApp.m_play_setting_data.continue_when_switch_playlist) || force_continue_play)
    {
        m_current_song_tmp = GetCurrentSongInfo();
        m_current_song_position_tmp = GetCurrentPosition();
        m_current_song_playing_tmp = IsPlaying();
    }
    MusicControl(Command::CLOSE);
    return true;
}

#pragma region 列表初始化方法

bool CPlayer::SetPath(const PathInfo& path_info, bool play)
{
    if (!BeforeIniPlayList(true))
        return false;

    m_path = path_info.path;
    m_playlist_path.clear();
    m_playlist_mode = PM_FOLDER;
    m_sort_mode = path_info.sort_mode;
    m_contain_sub_folder = path_info.contain_sub_folder;
    m_index = path_info.track;
    m_current_position.fromInt(path_info.position);

    IniPlayList(play);
    return true;
}

bool CPlayer::OpenFolder(wstring path, bool contain_sub_folder, bool play)
{
    if (!BeforeIniPlayList())
        return false;

    // 按照新文件夹设置
    m_path = path;
    m_playlist_path.clear();
    m_playlist_mode = PM_FOLDER;
    m_sort_mode = SM_U_FILE;
    m_contain_sub_folder = contain_sub_folder;
    m_index = 0;
    m_current_position.fromInt(0);

    // 如果是打开过的文件夹那么用保存的设置覆盖默认值
    const auto& path_info = CRecentFolderMgr::Instance().FindItem(m_path);
    if (!path_info.IsEmpty())
    {
        m_sort_mode = path_info.sort_mode;
        m_index = path_info.track;
        m_current_position.fromInt(path_info.position);
    }

    IniPlayList(play);
    return true;
}

bool CPlayer::SetPlaylist(const wstring& playlist_path, int track, int position, bool play, bool force)
{
    if (!BeforeIniPlayList(!force))
        return false;

    m_path.clear();
    m_playlist_path = playlist_path;
    m_playlist_mode = PM_PLAYLIST;
    m_sort_mode = SM_UNSORT;    // 播放列表模式下默认未排序
    m_contain_sub_folder = false;
    m_index = track;
    m_current_position.fromInt(position);

    IniPlayList(play);
    return true;
}

bool CPlayer::OpenPlaylistFile(wstring& file_path)
{
    CFilePathHelper helper(file_path);
    if (!CCommon::StringCompareNoCase(helper.GetDir(), theApp.m_playlist_dir))      //如果打开的播放列表文件不是程序默认的播放列表目录，则将其转换为*.playlist格式并复制到默认的播放列表目录
    {
        //设置新的路径
        wstring playlist_name = helper.GetFileNameWithoutExtension();
        wstring new_path = theApp.m_playlist_dir + playlist_name + PLAYLIST_EXTENSION;
        CCommon::FileAutoRename(new_path);

        CPlaylistFile playlist;
        playlist.LoadFromFile(file_path);
        playlist.SaveToFile(new_path);
        file_path = new_path;

        return SetPlaylist(new_path, 0, 0);
    }
    else        //如果打开的播放文件就在默认播放列表目录下，则直接打开
    {
        const PlaylistInfo& path_info = CPlaylistMgr::Instance().FindPlaylistInfo(file_path);
        return SetPlaylist(file_path, path_info.track, path_info.position);
    }
}

bool CPlayer::OpenFilesInDefaultPlaylist(const vector<wstring>& files, bool play)
{
    vector<SongInfo> songs(files.size());
    for (size_t i{}; i < files.size(); ++i)
        songs[i].file_path = files[i];
    return OpenSongsInDefaultPlaylist(songs, play);
}

bool CPlayer::OpenSongsInDefaultPlaylist(const vector<SongInfo>& songs, bool play)
{
    if (songs.empty()) return false;
    if (!BeforeIniPlayList())
        return false;

    m_path.clear();
    m_playlist_path = CPlaylistMgr::Instance().GetDefaultPlaylist().path;
    m_playlist_mode = PM_PLAYLIST;
    m_sort_mode = SM_UNSORT;    // 播放列表模式下默认未排序
    m_contain_sub_folder = false;
    m_index = 0;
    m_current_position.fromInt(0);

    // 向播放列表文件追加songs
    CPlaylistFile playlist;
    playlist.LoadFromFile(m_playlist_path);
    playlist.AddSongsToPlaylist(songs, theApp.m_media_lib_setting_data.insert_begin_of_playlist);
    playlist.SaveToFile(m_playlist_path);
    // 设置播放songs的第一个文件
    m_index = playlist.GetSongIndexInPlaylist(songs.front());

    IniPlayList(play);
    return true;
}

bool CPlayer::OpenSongsInTempPlaylist(const vector<SongInfo>& songs, int play_index, bool play /*= true*/)
{
    if (songs.empty()) return false;
    if (!BeforeIniPlayList())
        return false;

    m_path.clear();
    m_playlist_path = CPlaylistMgr::Instance().GetTempPlaylist().path;
    m_playlist_mode = PM_PLAYLIST;
    m_sort_mode = SM_UNSORT;    // 播放列表模式下默认未排序
    m_contain_sub_folder = false;
    m_index = play_index;
    m_current_position.fromInt(0);

    // 向播放列表文件覆写songs
    CPlaylistFile::SavePlaylistToFile(songs, m_playlist_path);

    IniPlayList(play);
    return true;
}

bool CPlayer::OpenASongInFolderMode(const SongInfo& song, bool play)
{
    if (song.file_path.empty()) return false;
    if (!BeforeIniPlayList())
        return false;

    CFilePathHelper file_path(song.file_path);
    m_path = file_path.GetDir();
    m_playlist_path.clear();
    m_playlist_mode = PM_FOLDER;
    m_sort_mode = SM_U_FILE;
    m_contain_sub_folder = false;
    m_index = 0;
    m_current_position.fromInt(0);

    // 如果是打开过的文件夹那么用保存的设置覆盖默认值
    const auto& path_info = CRecentFolderMgr::Instance().FindItem(m_path);
    {
        m_sort_mode = path_info.sort_mode;
        m_contain_sub_folder = path_info.contain_sub_folder;
        m_index = path_info.track;
        m_current_position.fromInt(path_info.position);
    }
    // 使用切换播放列表继续播放实现初始化线程后的指定歌曲播放
    m_current_song_tmp = song;
    m_current_song_position_tmp = 0;
    m_current_song_playing_tmp = play;

    IniPlayList(play);
    return true;
}

bool CPlayer::SetMediaLibPlaylist(CMediaClassifier::ClassificationType type, const std::wstring& name, int play_index, const SongInfo& play_song, bool play, bool force)
{
    //如果播放模式没有变化，则不需要初始化播放列表
    if (IsMediaLibMode() && type == m_media_lib_playlist_type && name == m_media_lib_playlist_name)
    {
        //通过play_song指定了播放曲目
        if (!play_song.IsEmpty())
        {
            auto iter = std::find_if(m_playlist.begin(), m_playlist.end(), [&](const SongInfo& song) ->bool {
                return song.IsSameSong(play_song);
            });
            if (iter != m_playlist.end())
                PlayTrack(iter - m_playlist.begin());
        }
        //通过play_index指定了播放曲目
        else if (play_index >= 0)
        {
            PlayTrack(play_index);
        }
        return true;
    }

    if (!BeforeIniPlayList(!force))
        return false;

    m_path.clear();
    m_playlist_path.clear();
    m_playlist_mode = PM_MEDIA_LIB;
    m_sort_mode = CMediaLibPlaylistMgr::GetDefaultSortMode(type);
    m_media_lib_playlist_name = name;
    m_media_lib_playlist_type = type;
    m_index = 0;
    m_current_position.fromInt(0);
    auto playlistInfo = CMediaLibPlaylistMgr::Instance().FindItem(type, name);
    if (playlistInfo.isValid())
    {
        m_sort_mode = playlistInfo.sort_mode;
        m_index = playlistInfo.track;
        m_current_position.fromInt(playlistInfo.position);
    }
    //通过play_song指定了播放曲目
    if (!play_song.IsEmpty())
    {
        m_current_song_tmp = play_song;
        m_current_song_playing_tmp = play;
        m_current_position.fromInt(0);
    }
    //通过play_index指定了播放曲目
    else if (play_index >= 0)
    {
        m_index = play_index;
        m_current_position.fromInt(0);
    }

    IniPlayList(play);
    return true;

}

int CPlayer::AddFilesToPlaylist(const vector<wstring>& files)
{
    vector<SongInfo> songs(files.size());
    for (size_t i{}; i < files.size(); ++i)
        songs[i].file_path = files[i];
    return AddSongsToPlaylist(songs);
}

int CPlayer::AddSongsToPlaylist(const vector<SongInfo>& songs)
{
    // 此方法仅限已处于播放列表模式时使用
    if (m_playlist_mode != PM_PLAYLIST || m_playlist_path.empty()) return -2;
    // 这里有必要暂时关闭，故保存播放状态，AddSongsToPlaylist可能将歌曲插入到开头导致index不再准确
    // 待到将来万一m_playlist的多线程问题彻底修好以后或许可以做不停止的重新初始化
    if (!BeforeIniPlayList(true, true))
        return -1;

    // 向当前播放列表文件追加songs
    CPlaylistFile playlist;
    playlist.LoadFromFile(m_playlist_path);
    int added = playlist.AddSongsToPlaylist(songs, theApp.m_media_lib_setting_data.insert_begin_of_playlist);
    playlist.SaveToFile(m_playlist_path);

    m_sort_mode = SM_UNSORT;        // 播放列表模式下的修改会失去排序状态
    m_index = 0;
    m_current_position.fromInt(0);

    IniPlayList();
    return added;
}

bool CPlayer::ReloadPlaylist(MediaLibRefreshMode refresh_mode)
{
    if (!BeforeIniPlayList(true, true))
        return false;

    if (IsPlaylistMode())
        m_sort_mode = SM_UNSORT;        // 播放列表模式默认未排序
    m_index = 0;
    m_current_position.fromInt(0);

    IniPlayList(false, refresh_mode);
    return true;
}

bool CPlayer::SetContainSubFolder()
{
    if (m_playlist_mode == PM_FOLDER)
    {
        m_contain_sub_folder = !m_contain_sub_folder;
        if (ReloadPlaylist(MR_MIN_REQUIRED))
            return true;
        else
        {
            m_contain_sub_folder = !m_contain_sub_folder;   // ReloadPlaylist失败则此次翻转撤销，并返回false
            return false;
        }
    }
    return true;    // 播放列表模式返回true
}

bool CPlayer::RemoveCurPlaylistOrFolder()
{
    if (!BeforeIniPlayList(true))
        return false;

    if (m_playlist_mode == PM_MEDIA_LIB)
        return false;

    // 在列表初始化线程中通知主窗口移除当前列表
    if (m_playlist_mode == PM_PLAYLIST)
        m_thread_info.remove_list_path = m_playlist_path;
    else if (m_playlist_mode == PM_FOLDER)
        m_thread_info.remove_list_path = m_path;

    const PlaylistInfo& def_playlist = CPlaylistMgr::Instance().GetDefaultPlaylist();

    m_path.clear();
    m_playlist_path = def_playlist.path;
    m_playlist_mode = PM_PLAYLIST;
    m_sort_mode = SM_UNSORT;        // 播放列表模式默认未排序
    m_contain_sub_folder = false;
    m_index = def_playlist.track;
    m_current_position.fromInt(def_playlist.position);

    IniPlayList();
    return true;
}

#pragma endregion 列表初始化方法

void CPlayer::SetRepeatMode()
{
    int repeat_mode{ static_cast<int>(m_repeat_mode) };
    repeat_mode++;
    if (repeat_mode >= RM_MAX)
        repeat_mode = 0;
    m_repeat_mode = static_cast<RepeatMode>(repeat_mode);
    SaveConfig();
}

void CPlayer::SetRepeatMode(RepeatMode repeat_mode)
{
    m_repeat_mode = repeat_mode;
    SaveConfig();
}

RepeatMode CPlayer::GetRepeatMode() const
{
    return m_repeat_mode;
}

void CPlayer::SpeedUp()
{
    if (m_speed < MAX_PLAY_SPEED)
    {
        m_speed *= 1.0594631f;     //加速一次频率变为原来的(2的1/12次方=1.0594631)倍，即使单调提高一个半音，减速时同理
        if (m_speed > MAX_PLAY_SPEED)
            m_speed = MAX_PLAY_SPEED;
        if (std::fabs(m_speed - 1) < 0.01)
            m_speed = 1;
        m_pCore->SetSpeed(m_speed);
        m_controls.UpdateSpeed(m_speed);
    }
}

void CPlayer::SlowDown()
{
    if (m_speed > MIN_PLAY_SPEED)
    {
        m_speed /= 1.0594631f;
        if (m_speed < MIN_PLAY_SPEED)
            m_speed = MIN_PLAY_SPEED;
        if (std::fabs(m_speed - 1) < 0.01)
            m_speed = 1;
        m_pCore->SetSpeed(m_speed);
        m_controls.UpdateSpeed(m_speed);
    }
}

void CPlayer::SetSpeed(float speed) {
    if (speed > MIN_PLAY_SPEED && speed < MAX_PLAY_SPEED) {
        m_speed = speed;
        m_pCore->SetSpeed(m_speed);
        m_controls.UpdateSpeed(m_speed);
    }
}

void CPlayer::SetOrignalSpeed()
{
    m_speed = 1;
    m_pCore->SetSpeed(m_speed);
    m_controls.UpdateSpeed(m_speed);
}

bool CPlayer::GetPlayerCoreError(const wchar_t* function_name)
{
    if (m_loading)
        return false;
    int error_code_tmp = m_pCore->GetErrorCode();
    if (error_code_tmp && error_code_tmp != m_error_code)
    {
        wstring log_info = m_pCore->GetErrorInfo(error_code_tmp);
        log_info += L" function name: ";
        log_info += function_name;
        theApp.WriteLog(log_info);
    }
    m_error_code = error_code_tmp;
    return true;
}

bool CPlayer::IsError() const
{
    if (m_loading)		//如果播放列表正在加载，则不检测错误
        return false;
    else
        return (m_error_state != ES_NO_ERROR || m_error_code != 0 || m_pCore == nullptr || (m_file_opend && m_pCore->GetCoreType() == PT_BASS && GetBassHandle() == 0));
}

std::wstring CPlayer::GetErrorInfo()
{
    wstring error_info;
    if (m_error_state == ES_FILE_NOT_EXIST)
        error_info = theApp.m_str_table.LoadText(L"UI_TXT_PLAYSTATUS_ERROR_FILE_NOT_EXIST");
    else if (m_error_state == ES_FILE_CANNOT_BE_OPEN)
        error_info = theApp.m_str_table.LoadText(L"UI_TXT_PLAYSTATUS_ERROR_FILE_CANNOT_BE_OPEND");
    else
        error_info = m_pCore->GetErrorInfo();
    error_info = theApp.m_str_table.LoadTextFormat(L"UI_TXT_PLAYSTATUS_ERROR_2", { error_info });
    return error_info;
}

void CPlayer::AfterSetTrack() const
{
    SendMessage(theApp.m_pMainWnd->m_hWnd, WM_AFTER_SET_TRACK, 0, 0);
}

void CPlayer::SaveConfig() const
{
    CIniHelper ini(theApp.m_config_path);

    //ini.WriteString(L"config", L"path", m_path.c_str());
    //ini.WriteInt(L"config", L"track", m_index);
    ini.WriteInt(L"config", L"volume", m_volume);
    //ini.WriteInt(L"config", L"position", current_position_int);
    ini.WriteInt(L"config", L"repeat_mode", static_cast<int>(m_repeat_mode));
    ini.WriteBool(L"config", L"lyric_karaoke_disp", theApp.m_lyric_setting_data.lyric_karaoke_disp);
    ini.WriteString(L"config", L"lyric_path", theApp.m_lyric_setting_data.lyric_path);
    ini.WriteInt(L"config", L"sort_mode", static_cast<int>(m_sort_mode));
    ini.WriteBool(L"config", L"lyric_fuzzy_match", theApp.m_lyric_setting_data.lyric_fuzzy_match);
    ini.WriteString(L"config", L"default_album_file_name", CCommon::StringMerge(theApp.m_app_setting_data.default_album_name, L','));
    ini.WriteString(L"config", L"album_cover_path", theApp.m_app_setting_data.album_cover_path);
    ini.WriteInt(L"config", L"playlist_mode", m_playlist_mode);
    ini.WriteDouble(L"config", L"speed", m_speed);

    //保存均衡器设定
    ini.WriteBool(L"equalizer", L"equalizer_enable", m_equ_enable);
    //保存每个均衡器通道的增益
    //if (m_equ_style == 9)
    //{
    //	wchar_t buff[16];
    //	for (int i{}; i < EQU_CH_NUM; i++)
    //	{
    //		swprintf_s(buff, L"channel%d", i + 1);
    //		ini.WriteInt(L"equalizer", buff, m_equalizer_gain[i]);
    //	}
    //}
    //保存混响设定
    ini.WriteInt(L"reverb", L"reverb_enable", m_reverb_enable);
    ini.WriteInt(L"reverb", L"reverb_mix", m_reverb_mix);
    ini.WriteInt(L"reverb", L"reverb_time", m_reverb_time);

    ini.Save();
}

void CPlayer::LoadConfig()
{
    CIniHelper ini(theApp.m_config_path);

    //ini.GetString(L"config", L"path", L".\\songs\\");
    //m_path = buff;
    //m_index =ini.GetInt(L"config", L"track", 0);
    m_volume = ini.GetInt(L"config", L"volume", 60);
    //current_position_int =ini.GetInt(L"config", L"position", 0);
    //m_current_position.fromInt(current_position_int);
    m_repeat_mode = static_cast<RepeatMode>(ini.GetInt(L"config", L"repeat_mode", 0));
    theApp.m_lyric_setting_data.lyric_path = ini.GetString(L"config", L"lyric_path", L".\\lyrics\\");
    if (!theApp.m_lyric_setting_data.lyric_path.empty() && theApp.m_lyric_setting_data.lyric_path.back() != L'/' && theApp.m_lyric_setting_data.lyric_path.back() != L'\\')
        theApp.m_lyric_setting_data.lyric_path.append(1, L'\\');
    theApp.m_lyric_setting_data.lyric_karaoke_disp = ini.GetBool(L"config", L"lyric_karaoke_disp", true);
    m_sort_mode = static_cast<SortMode>(ini.GetInt(L"config", L"sort_mode", 0));
    theApp.m_lyric_setting_data.lyric_fuzzy_match = ini.GetBool(L"config", L"lyric_fuzzy_match", true);
    wstring default_album_name = ini.GetString(L"config", L"default_album_file_name", L"cover");
    CCommon::StringSplit(default_album_name, L',', theApp.m_app_setting_data.default_album_name);

    theApp.m_app_setting_data.album_cover_path = ini.GetString(L"config", L"album_cover_path", L".\\cover\\");
    if (!theApp.m_app_setting_data.album_cover_path.empty() && theApp.m_app_setting_data.album_cover_path.back() != L'/' && theApp.m_app_setting_data.album_cover_path.back() != L'\\')
        theApp.m_app_setting_data.album_cover_path.append(1, L'\\');

    bool playlist_mode_default = !CCommon::FileExist(theApp.m_recent_path_dat_path);
    m_playlist_mode = static_cast<PlaylistMode>(ini.GetInt(L"config", L"playlist_mode", playlist_mode_default));
    m_speed = static_cast<float>(ini.GetDouble(L"config", L"speed", 1));
    if (m_speed < MIN_PLAY_SPEED || m_speed > MAX_PLAY_SPEED)
        m_speed = 1;

    //读取均衡器设定
    m_equ_enable = ini.GetBool(L"equalizer", L"equalizer_enable", false);
    m_equ_style = ini.GetInt(L"equalizer", L"equalizer_style", 0);	//读取均衡器预设
    if (m_equ_style == 9)		//如果均衡器预设为“自定义”
    {
        //读取每个均衡器通道的增益
        for (int i{}; i < EQU_CH_NUM; i++)
        {
            wchar_t buff[16];
            swprintf_s(buff, L"channel%d", i + 1);
            m_equalizer_gain[i] = ini.GetInt(L"equalizer", buff, 0);
        }
    }
    else if (m_equ_style >= 0 && m_equ_style < 9)		//否则，根据均衡器预设设置每个通道的增益
    {
        for (int i{}; i < EQU_CH_NUM; i++)
        {
            m_equalizer_gain[i] = EQU_STYLE_TABLE[m_equ_style][i];
        }
    }
    //读取混响设定
    m_reverb_enable = ini.GetBool(L"reverb", L"reverb_enable", 0);
    m_reverb_mix = ini.GetInt(L"reverb", L"reverb_mix", 45);		//混响强度默认为50
    m_reverb_time = ini.GetInt(L"reverb", L"reverb_time", 100);	//混响时间默认为1s
}

void CPlayer::ExplorePath(int track) const
{
    if (GetSongNum() > 0)
    {
        CString str;
        if (track < 0)		//track小于0，打开资源管理器后选中当前播放的文件
            str.Format(_T("/select,\"%s\""), GetCurrentFilePath().c_str());
        else if (track < GetSongNum())		//track为播放列表中的一个序号，打开资源管理器后选中指定的文件
            str.Format(_T("/select,\"%s\""), m_playlist[track].file_path.c_str());
        else								//track超过播放列表中文件的数量，打开资源管理器后不选中任何文件
            str = m_path.c_str();
        ShellExecute(NULL, _T("open"), _T("explorer"), str, NULL, SW_SHOWNORMAL);
    }
}

void CPlayer::ExploreLyric() const
{
    if (!m_Lyrics.IsEmpty())
    {
        CString str;
        str.Format(_T("/select,\"%s\""), m_Lyrics.GetPathName().c_str());
        ShellExecute(NULL, _T("open"), _T("explorer"), str, NULL, SW_SHOWNORMAL);
    }
}

int CPlayer::IsSongInPlayList(const SongInfo& song)
{
    auto iter = std::find_if(m_playlist.begin(), m_playlist.end(),
        [&](const SongInfo& songinfo) { return song.IsSameSong(songinfo); });
    if (iter != m_playlist.end())
        return iter - m_playlist.begin();
    return -1;
}

bool CPlayer::IsSongsInPlayList(const vector<SongInfo>& songs_list)
{
    for (const SongInfo& song : songs_list)
    {
        auto iter = std::find_if(m_playlist.begin(), m_playlist.end(),
            [&](const SongInfo& songinfo) { return song.IsSameSong(songinfo); });
        if (iter == m_playlist.end())
            return false;
    }
    return true;    // 没有找到不存在于m_playlist中的songs_list元素，返回true
}

int CPlayer::GetSongNum() const
{
    return static_cast<int>(m_playlist.size());
}

wstring CPlayer::GetCurrentDir() const
{
    wstring current_file_path = GetCurrentFilePath();
    CFilePathHelper path_helper(current_file_path);
    return path_helper.GetDir();
}

wstring CPlayer::GetCurrentDir2() const
{
    if (m_playlist_mode == PM_PLAYLIST || m_playlist_mode == PM_MEDIA_LIB)
        return GetCurrentDir();
    else
        return m_path;
}

wstring CPlayer::GetCurrentFolderOrPlaylistName() const
{
    if (m_playlist_mode == PM_PLAYLIST)
    {
        CFilePathHelper file_path{ m_playlist_path };
        wstring playlist_name = file_path.GetFileName();
        if (playlist_name == DEFAULT_PLAYLIST_NAME)
            return theApp.m_str_table.LoadText(L"TXT_PLAYLIST_NAME_DEFAULT");
        else if (playlist_name == FAVOURITE_PLAYLIST_NAME)
            return theApp.m_str_table.LoadText(L"TXT_PLAYLIST_NAME_FAVOURITE");
        else if (playlist_name == TEMP_PLAYLIST_NAME)
            return theApp.m_str_table.LoadText(L"TXT_PLAYLIST_NAME_TEMP");
        else
            return file_path.GetFileNameWithoutExtension();
    }
    else if (m_playlist_mode == PM_MEDIA_LIB)
    {
        return CMediaLibPlaylistMgr::GetMediaLibItemDisplayName(m_media_lib_playlist_type, m_media_lib_playlist_name);
    }
    else
    {
        return m_path;
    }
}

wstring CPlayer::GetCurrentFilePath() const
{
    if (m_index >= 0 && m_index < GetSongNum())
    {
        //if (m_playlist[m_index].file_path.empty())
        //    return m_path + m_playlist[m_index].file_name;
        //else
        return m_playlist[m_index].file_path;
    }
    else
        return wstring();
}

wstring CPlayer::GetDisplayName() const
{
    const SongInfo& song = GetCurrentSongInfo();
    if (song.is_cue && !song.IsArtistEmpty() && !song.IsTitleEmpty())
        return song.artist + L" - " + song.title;
    if (IsOsuFile() && !song.comment.empty())
        return song.comment;
    wstring file_name = GetCurrentSongInfo().GetFileName();
    if (!file_name.empty())
        return file_name;
    return theApp.m_str_table.LoadText(L"TXT_EMPTY_FILE_NAME");
}

CImage& CPlayer::GetAlbumCover()
{
    CSingleLock sync(&m_album_cover_sync, TRUE);
    return m_album_cover;
}

ATL::CImage& CPlayer::GetAlbumCoverBlur()
{
    CSingleLock sync(&m_album_cover_sync, TRUE);
    return m_album_cover_blur;
}

bool CPlayer::AlbumCoverExist()
{
    CSingleLock slock(&m_album_cover_sync);
    if (slock.IsLocked())
        return false;
    else
        return !m_album_cover.IsNull();
}

wstring CPlayer::GetAlbumCoverType() const
{ 
    // TagLibHelper.cpp中GetPicType的反向方法，之后需要重构，跳过int转手
    switch (m_album_cover_type)
    {
    case 0: return L"jpg";
    case 1: return L"png";
    case 2: return L"gif";
    case 3: return L"bmp";
    default: return L"other";
    }
}

void CPlayer::AfterRemoveSong(bool is_current)
{
    if (is_current)
    {
        if (m_playlist.empty()) // 播放列表为空时清除显示
        {
            MusicControl(Command::CLOSE);
            m_playlist.push_back(SongInfo());
            m_index = 0;
            m_current_position.fromInt(0);
            m_song_length.fromInt(0);
            m_album_cover.Destroy();
            m_album_cover_blur.Destroy();
            m_Lyrics = CLyrics();
            m_controls.UpdateControlsMetadata(SongInfo());
            MediaTransControlsLoadThumbnailDefaultImage();
        }
        else    // 播放列表不为空时先确保索引有效再打开/播放（最接近的曲目）
        {
            if (m_index < 0) m_index = 0;
            if (m_index >= GetSongNum()) m_index = GetSongNum() - 1;
            bool play{ IsPlaying() };
            m_current_position.fromInt(0);      //关闭前将当前播放位置清零
            MusicControl(Command::CLOSE);
            MusicControl(Command::OPEN);
            if (GetCurrentSongInfo().is_cue)
                SeekTo(0);
            if (play)
                MusicControl(Command::PLAY);
            GetPlayerCoreCurrentPosition();
        }
    }
    // 重新统计列表总时长
    m_total_time = 0;
    for (const auto& song : m_playlist)
    {
        m_total_time += song.length().toInt();
    }
    OnPlaylistChange();
    AfterSetTrack();
    SaveRecentInfoToFiles();
    // 文件夹模式“从磁盘删除”时刷新媒体库路径标签页否则刷新播放列表标签页
    if (m_playlist_mode == PM_PLAYLIST)
        CMusicPlayerCmdHelper::RefreshMediaTabData(CMusicPlayerCmdHelper::ML_PLAYLIST);
    else if (m_playlist_mode == PM_FOLDER)
        CMusicPlayerCmdHelper::RefreshMediaTabData(CMusicPlayerCmdHelper::ML_FOLDER);
}

bool CPlayer::RemoveSong(int index, bool skip_locking)
{
    if (IsPlaylistEmpty()) return false;                    // 播放列表为空（或有一个占位SongInfo）返回
    // if (!m_playlist_mode) return false;                     // 不是播放列表模式返回（文件夹模式可以“从磁盘删除”）
    if (index < 0 || index >= GetSongNum()) return false;   // index无效返回
    if (m_loading) return false;                            // 播放列表载入中返回
    if (!skip_locking)
        if (!GetPlayStatusMutex().try_lock_for(std::chrono::milliseconds(1000))) return false;  // 取得播放状态锁失败返回

    bool rm_is_index{ index == m_index };
    m_playlist.erase(m_playlist.begin() + index);
    if (index < m_index) --m_index;                         //如果要删除的曲目在正在播放的曲目之前，则正在播放的曲目序号减1

    AfterRemoveSong(rm_is_index);

    if (!skip_locking)
        GetPlayStatusMutex().unlock();
    return true;
}

void CPlayer::RemoveSongs(vector<int> indexes, bool skip_locking)
{
    if (IsPlaylistEmpty()) return;                          // 播放列表为空（或有一个占位SongInfo）返回
    // if (!m_playlist_mode) return;                           // 不是播放列表模式返回（文件夹模式可以“从磁盘删除”）
    if (m_loading) return;                                  // 播放列表载入中返回
    int list_size{ GetSongNum() };
    vector<int> indexes_;   // 存储检查过的未越界待移除index
    for (const auto& index : indexes)
        if (index >= 0 && index < list_size)
            indexes_.push_back(index);
    if (indexes_.empty()) return;                           // 没有有效的移除index返回
    if (!skip_locking)
        if (!GetPlayStatusMutex().try_lock_for(std::chrono::milliseconds(1000))) return;    // 取得播放状态锁失败返回

    std::sort(indexes_.rbegin(), indexes_.rend());      // 降序排序以免移除时需要修改索引值
    bool rm_is_index{ std::find(indexes_.begin(), indexes_.end(), m_index) != indexes_.end() };
    for (int rm_index : indexes_)
    {
        m_playlist.erase(m_playlist.begin() + rm_index);
        if (rm_index < m_index) --m_index;
    }

    AfterRemoveSong(rm_is_index);

    if (!skip_locking)
        GetPlayStatusMutex().unlock();
}

int CPlayer::RemoveSameSongs()
{
    if (GetSongNum() < 2) return 0;                             // 播放列表为空或仅含有一个元素则返回
    if (m_playlist_mode != PM_PLAYLIST) return 0;               // 不是播放列表模式返回
    if (m_loading) return 0;                                    // 播放列表载入中返回
    if (!GetPlayStatusMutex().try_lock_for(std::chrono::milliseconds(1000))) return 0;  // 取得播放状态锁失败返回

    int removed = 0;
    for (int i = 0; i < GetSongNum(); i++)
    {
        for (int j = i + 1; j < GetSongNum(); j++)
        {
            if (m_playlist[i].IsSameSong(m_playlist[j]))
            {
                if (j == m_index)
                    m_index = i;
                else if (j > m_index)
                    --m_index;
                m_playlist.erase(m_playlist.begin() + j);
                ++removed;
                --j;
            }
        }
    }

    if (removed)
        AfterRemoveSong(false); // 移除重复歌曲时m_index总是能够保持指向相同歌曲（虽然本身值可能改变）所以参数总为false

    GetPlayStatusMutex().unlock();
    return removed;
}

int CPlayer::RemoveInvalidSongs()
{
    if (IsPlaylistEmpty()) return 0;                            // 播放列表为空或仅含有一个元素则返回
    if (m_playlist_mode != PM_PLAYLIST) return 0;               // 不是播放列表模式返回
    if (m_loading) return 0;                                    // 播放列表载入中返回
    if (!GetPlayStatusMutex().try_lock_for(std::chrono::milliseconds(1000))) return 0;  // 取得播放状态锁失败返回

    bool rm_is_index{};
    int removed = 0;
    for (int i = 0; i < GetSongNum(); i++)
    {
        if (!CCommon::FileExist(m_playlist[i].file_path) || m_playlist[i].length().isZero())
        {
            m_playlist.erase(m_playlist.begin() + i);
            if (i == m_index)
                rm_is_index = true;
            removed++;
            i--;
        }
    }

    if (removed)
        AfterRemoveSong(rm_is_index);

    GetPlayStatusMutex().unlock();
    return removed;
}

void CPlayer::ClearPlaylist()
{
    if (IsPlaylistEmpty()) return;                          // 播放列表为空（或有一个占位SongInfo）返回
    if (m_playlist_mode != PM_PLAYLIST) return;             // 不是播放列表模式返回
    if (m_loading) return;                                  // 播放列表载入中返回
    if (!GetPlayStatusMutex().try_lock_for(std::chrono::milliseconds(1000))) return;  // 取得播放状态锁失败返回

    m_playlist.clear();

    AfterRemoveSong(true);

    GetPlayStatusMutex().unlock();
}

bool CPlayer::MoveUp(int first, int last)
{
    if (m_loading)
        return false;
    if (m_playlist_mode != PM_PLAYLIST)
        return false;

    if (first <= 0 || last >= GetSongNum() || last < first)
        return false;

    if (m_index >= first && m_index <= last)
        m_index--;
    else if (m_index == first - 1)
        m_index = last;

    for (int i = first; i <= last; i++)
    {
        std::swap(m_playlist[i - 1], m_playlist[i]);
    }
    m_sort_mode = SM_UNSORT;        // 修改会失去排序状态
    OnPlaylistChange();
    SaveCurrentPlaylist();
    return true;
}

bool CPlayer::MoveDown(int first, int last)
{
    if (m_loading)
        return false;
    if (m_playlist_mode != PM_PLAYLIST)
        return false;

    if (first < 0 || last >= GetSongNum() - 1 || last < first)
        return false;

    if (m_index >= first && m_index <= last)
        m_index++;
    else if (m_index == last + 1)
        m_index = first;

    for (int i = last + 1; i > first; i--)
    {
        std::swap(m_playlist[i], m_playlist[i - 1]);
    }
    m_sort_mode = SM_UNSORT;        // 修改会失去排序状态
    OnPlaylistChange();
    SaveCurrentPlaylist();
    return true;
}

int CPlayer::MoveItems(std::vector<int> indexes, int dest)
{
    if (m_loading)
        return -1;
    if (m_playlist_mode != PM_PLAYLIST)
        return -1;

    if (std::find(indexes.begin(), indexes.end(), dest) != indexes.end())
        return -1;

    std::wstring dest_file_path;        //保存目标位置的文件路径
    int dest_track = 0;                      //保存目标位置的音轨
    if (dest >= 0 && dest < GetSongNum())
    {
        dest_file_path = m_playlist[dest].file_path;
        dest_track = m_playlist[dest].track;
    }

    SongInfo current_file{ GetCurrentSongInfo() };

    //把要移动的项目取出并删除要移动的项目
    std::vector<SongInfo> moved_items;
    int size = indexes.size();
    for (int i{}; i < size; i++)
    {
        if (indexes[i] >= 0 && indexes[i] < GetSongNum())
        {
            moved_items.push_back(m_playlist[indexes[i]]);
            m_playlist.erase(m_playlist.begin() + indexes[i]);
            if (i <= size - 2 && indexes[i + 1] > indexes[i])
            {
                for (int j{ i + 1 }; j < size; j++)
                    indexes[j]--;
            }
        }
    }

    //重新查找目标文件的位置
    int dest_index;
    auto iter_dest = std::find_if(m_playlist.begin(), m_playlist.end(), [&](const SongInfo& song)
        {
            return song.file_path == dest_file_path && song.track == dest_track;
        });
    if (dest >= 0 && iter_dest != m_playlist.end())
    {
        //把要移动的项目插入到目标位置
        dest_index = iter_dest - m_playlist.begin();
        m_playlist.insert(iter_dest, moved_items.begin(), moved_items.end());
    }
    else        //dest为负，则把要移动的项目插入到列表最后
    {
        dest_index = GetSongNum();
        for (const auto& song : moved_items)
        {
            m_playlist.push_back(song);
        }
    }

    //查找正在播放的曲目
    auto iter_play = std::find_if(m_playlist.begin(), m_playlist.end(), [&](const SongInfo& song)
        {
            return song.IsSameSong(current_file);
        });
    if (iter_play == m_playlist.end())
        m_index = 0;
    else
        m_index = iter_play - m_playlist.begin();

    m_sort_mode = SM_UNSORT;        // 修改会失去排序状态
    OnPlaylistChange();
    SaveCurrentPlaylist();
    return dest_index;
}

void CPlayer::SeekTo(int position)
{
    if (position > m_song_length.toInt())
        position = m_song_length.toInt();
    m_current_position.fromInt(position);
    if (m_playlist[m_index].is_cue)
    {
        position += m_playlist[m_index].start_pos.toInt();
    }
    m_pCore->SetCurPosition(position);
    GetPlayerCoreError(L"SetCurPosition");
}

void CPlayer::SeekTo(double position)
{
    int pos = static_cast<int>(m_song_length.toInt() * position);
    SeekTo(pos);
}

//void CPlayer::SeekTo(HSTREAM hStream, int position)
//{
//    double pos_sec = static_cast<double>(position) / 1000.0;
//    QWORD pos_bytes;
//    pos_bytes = BASS_ChannelSeconds2Bytes(hStream, pos_sec);
//    BASS_ChannelSetPosition(hStream, pos_bytes, BASS_POS_BYTE);
//    GetInstance().GetPlayerCoreError();
//}

void CPlayer::ClearLyric()
{
    m_Lyrics = CLyrics{};
    GetCurrentSongInfo2().lyric_file.clear();
    SongInfo song_info{ CSongDataManager::GetInstance().GetSongInfo3(GetCurrentSongInfo()) };
    song_info.lyric_file.clear();
    CSongDataManager::GetInstance().AddItem(song_info);
}

wstring CPlayer::GetTimeString() const
{
    wchar_t buff[64];
    swprintf_s(buff, L"%d:%.2d/%d:%.2d", m_current_position.min, m_current_position.sec, m_song_length.min, m_song_length.sec);
    return wstring(buff);
}

wstring CPlayer::GetPlayingState() const
{
    static wstring str_paly_error = theApp.m_str_table.LoadText(L"UI_TXT_PLAYSTATUS_ERROR");
    static wstring str_stoped = theApp.m_str_table.LoadText(L"UI_TXT_PLAYSTATUS_STOPED");
    static wstring str_paused = theApp.m_str_table.LoadText(L"UI_TXT_PLAYSTATUS_PAUSED");
    static wstring str_playing = theApp.m_str_table.LoadText(L"UI_TXT_PLAYSTATUS_PLAYING");
    if (m_error_code != 0)
        return str_paly_error;
    switch (m_playing)
    {
    case PS_STOPED:
        return str_stoped;
    case PS_PAUSED:
        return str_paused;
    case PS_PLAYING:
        return str_playing;
    default:
        return wstring();
    }
}

const SongInfo& CPlayer::GetCurrentSongInfo() const
{
    if (m_index >= 0 && m_index < GetSongNum())
        return m_playlist[m_index];
    else return m_no_use;
}

SongInfo& CPlayer::GetCurrentSongInfo2()
{
    if (m_index >= 0 && m_index < GetSongNum())
        return m_playlist[m_index];
    else return m_no_use;
}

SongInfo CPlayer::GetNextTrack() const
{
    auto GetLegitSongInfo = [this](int x) { return x >= 0 && x < static_cast<int>(m_playlist.size()) ? m_playlist[x] : SongInfo(); };
    if (!m_next_tracks.empty())
    {
        return GetLegitSongInfo(m_next_tracks.front());
    }
    switch (m_repeat_mode)
    {
    case RM_PLAY_ORDER:
    {
        return GetLegitSongInfo(m_index + 1);
    }

    case RM_PLAY_SHUFFLE:
    {
        int shuffle_index = GetNextShuffleIdx();
        if (shuffle_index == 0 && m_is_shuffle_list_played || m_shuffle_list.empty())
        {
            //如果shuffle_index == 0且列表播放过，说明列表中的曲目已经无序播放完一遍，此时无序列表要重新生成，因此下一首曲目是不确定的
            //以及shuffle之前m_shuffle_list为空
            return SongInfo();
        }
        else
        {
            return GetLegitSongInfo(m_shuffle_list[shuffle_index]);
        }
    }

    case RM_PLAY_RANDOM:
        return SongInfo();

    case RM_LOOP_PLAYLIST:
    {
        int index = m_index + 1;
        if (index >= GetSongNum() || index < 0)
            index = 0;
        return m_playlist[index];
    }

    case RM_LOOP_TRACK:
        return GetCurrentSongInfo();

    case RM_PLAY_TRACK:
        return SongInfo();

    default:
        return SongInfo();
    }
}

void CPlayer::SetFavourite(int index, bool favourite)
{
    if (IsError())
        return;
    if (index >= 0 && index < GetSongNum())
    {
        m_playlist[index].is_favourite = favourite;
    }
    if (theApp.m_media_lib_setting_data.enable_lastfm) {
        theApp.UpdateLastFMFavourite(favourite);
    }
}

void CPlayer::SetFavourite(bool favourite)
{
    SetFavourite(m_index, favourite);
}

bool CPlayer::IsFavourite(int index)
{
    if (m_playlist_mode == PM_PLAYLIST && CPlaylistMgr::Instance().GetCurPlaylistType() == PT_FAVOURITE)
        return true;
    if (index >= 0 && index < GetSongNum())
    {
        return m_playlist[index].is_favourite;
    }
    return false;
}

bool CPlayer::IsFavourite()
{
    return IsFavourite(m_index);
}

void CPlayer::AddListenTime(int sec)
{
    if (m_index >= 0 && m_index < GetSongNum())
    {
        m_playlist[m_index].listen_time += sec; // m_playlist的信息不会保存到媒体库，此处仅为排序维护
        SongInfo song_info{ CSongDataManager::GetInstance().GetSongInfo3(m_playlist[m_index]) };
        song_info.listen_time += sec;
        CSongDataManager::GetInstance().AddItem(song_info);
    }
    if (m_enable_lastfm) {
        int speed = static_cast<int>(m_speed * 1000);
        theApp.m_lastfm.AddCurrentPlayedTime(sec * speed);
        if (!theApp.m_lastfm.IsPushed()) {
            if (theApp.m_lastfm.CurrentTrackScrobbleable()) {
                theApp.m_lastfm.PushCurrentTrackToCache();
            }
        }
        if (theApp.m_media_lib_setting_data.lastfm_auto_scrobble && theApp.m_lastfm.IsScrobbeable()) {
            theApp.LastFMScrobble();
        }
    }
}

int CPlayer::GetChannels()
{
    return m_pCore == nullptr ? 0 : m_pCore->GetChannels();
}

int CPlayer::GetFreq()
{
    return m_pCore == nullptr ? 0 : m_pCore->GetFReq();
}

unsigned int CPlayer::GetBassHandle() const
{
    if (IsBassCore())
    {
        CBassCore* bass_core = dynamic_cast<CBassCore*>(m_pCore);
        if (bass_core != nullptr)
        {
            return bass_core->GetHandle();
        }
    }
    return 0;
}

void CPlayer::ReIniPlayerCore(bool replay)
{
    if (m_loading) return;
    if (!GetPlayStatusMutex().try_lock_for(std::chrono::milliseconds(5000))) return;    // 这里多等一会，系统从挂起中恢复可能很卡
    int playing = m_playing;
    int current_position = GetCurrentPosition();
    MusicControl(Command::CLOSE);   // stop可以忽略但close中有不应忽略的保存歌词操作
    UnInitPlayerCore();
    IniPlayerCore();
    MusicControl(Command::OPEN);
    SeekTo(current_position);
    if (replay && playing == PS_PLAYING)
    {
        MusicControl(Command::PLAY);
    }
    else
    {
        m_playing = PS_STOPED;
    }
    GetPlayStatusMutex().unlock();
}

void CPlayer::SortPlaylist(bool is_init)
{
    if (m_loading && !is_init) return;
    CWaitCursor wait_cursor;
    SongInfo current_song = GetCurrentSongInfo();
    auto sort_fun = SongInfo::ByFileName;
    switch (m_sort_mode)
    {
    case SM_U_FILE: sort_fun = SongInfo::ByFileName; break;
    case SM_D_FILE: sort_fun = SongInfo::ByFileNameDecending; break;
    case SM_U_PATH: sort_fun = SongInfo::ByPath; break;
    case SM_D_PATH: sort_fun = SongInfo::ByPathDecending; break;
    case SM_U_TITLE: sort_fun = SongInfo::ByTitle; break;
    case SM_D_TITLE: sort_fun = SongInfo::ByTitleDecending; break;
    case SM_U_ARTIST: sort_fun = SongInfo::ByArtist; break;
    case SM_D_ARTIST: sort_fun = SongInfo::ByArtistDecending; break;
    case SM_U_ALBUM: sort_fun = SongInfo::ByAlbum; break;
    case SM_D_ALBUM: sort_fun = SongInfo::ByAlbumDecending; break;
    case SM_U_TRACK: sort_fun = SongInfo::ByTrack; break;
    case SM_D_TRACK: sort_fun = SongInfo::ByTrackDecending; break;
    case SM_U_LISTEN: sort_fun = SongInfo::ByListenTime; break;
    case SM_D_LISTEN: sort_fun = SongInfo::ByListenTimeDecending; break;
    case SM_U_TIME: sort_fun = SongInfo::ByModifiedTime; break;
    case SM_D_TIME: sort_fun = SongInfo::ByModifiedTimeDecending; break;
    default: ASSERT(FALSE); break;
    }
    std::stable_sort(m_playlist.begin(), m_playlist.end(), sort_fun);

    if (!is_init)   // 由初始化完成方法调用时不重新查找index
    {
        //播放列表排序后，查找正在播放项目的序号
        for (int i{}; i < GetSongNum(); i++)
        {
            if (current_song.IsSameSong(m_playlist[i]))
            {
                m_index = i;
                break;
            }
        }
    }
    OnPlaylistChange();
    SaveCurrentPlaylist();
}

void CPlayer::OnExit()
{
    SaveConfig();
    //退出时保存最后播放的曲目和位置
    SaveRecentInfoToFiles();
}

void CPlayer::SaveCurrentPlaylist()
{
    if (m_playlist_mode == PM_PLAYLIST)
        CPlaylistFile::SavePlaylistToFile(m_playlist, m_playlist_path);
}

//void CPlayer::SetFXHandle()
//{
//    GetPlayerCoreError();
//}
//
//void CPlayer::RemoveFXHandle()
//{
//    GetPlayerCoreError();
//}

void CPlayer::ApplyEqualizer(int channel, int gain)
{
    m_pCore->ApplyEqualizer(channel, gain);
    GetPlayerCoreError(L"ApplyEqualizer");
}

void CPlayer::SetEqualizer(int channel, int gain)
{
    if (channel < 0 || channel >= EQU_CH_NUM) return;
    m_equalizer_gain[channel] = gain;
    ApplyEqualizer(channel, gain);
}

int CPlayer::GeEqualizer(int channel)
{
    if (channel < 0 || channel >= EQU_CH_NUM) return 0;
    //BASS_DX8_PARAMEQ parameq;
    //int rtn;
    //rtn = BASS_FXGetParameters(m_equ_handle[channel], &parameq);
    //return static_cast<int>(parameq.fGain);
    return m_equalizer_gain[channel];
}

void CPlayer::SetAllEqualizer()
{
    for (int i{}; i < EQU_CH_NUM; i++)
    {
        ApplyEqualizer(i, m_equalizer_gain[i]);
    }
}

void CPlayer::ClearAllEqulizer()
{
    for (int i{}; i < EQU_CH_NUM; i++)
    {
        ApplyEqualizer(i, 0);
    }
}

void CPlayer::EnableEqualizer(bool enable)
{
    if (enable)
        SetAllEqualizer();
    else
        ClearAllEqulizer();
    m_equ_enable = enable;
}

void CPlayer::EnableReverb(bool enable)
{
    if (enable)
    {
        if (m_reverb_mix < 0) m_reverb_mix = 0;
        if (m_reverb_mix > 100) m_reverb_mix = 100;
        if (m_reverb_time < 1) m_reverb_time = 1;
        if (m_reverb_time > 300) m_reverb_time = 300;
        m_pCore->SetReverb(m_reverb_mix, m_reverb_time);
        GetPlayerCoreError(L"SetReverb");
    }
    else
    {
        m_pCore->ClearReverb();
        GetPlayerCoreError(L"ClearReverb");
    }
    m_reverb_enable = enable;
}


bool CPlayer::SetARepeatPoint()
{
    m_a_repeat = m_current_position;
    m_ab_repeat_mode = AM_A_SELECTED;
    return true;
}

bool CPlayer::SetBRepeatPoint()
{
    if (m_ab_repeat_mode != AM_NONE)
    {
        Time time_span = m_current_position - m_a_repeat;
        if (time_span > 200 && time_span < m_song_length)		//B点位置必须至少超过A点200毫秒
        {
            m_b_repeat = m_current_position;
            m_ab_repeat_mode = AM_AB_REPEAT;
            return true;
        }
    }
    return false;
}

bool CPlayer::ContinueABRepeat()
{
    if (m_ab_repeat_mode == AM_AB_REPEAT)		//在AB重复状态下，将当前重复B点设置为下一次的重复A点
    {
        m_a_repeat = m_b_repeat;
        m_ab_repeat_mode = AM_A_SELECTED;
        if (GetPlayStatusMutex().try_lock_for(std::chrono::milliseconds(1000)))
        {
            SeekTo(m_a_repeat.toInt());
            GetPlayStatusMutex().unlock();
        }
        return true;
    }
    return false;
}

void CPlayer::DoABRepeat()
{
    switch (m_ab_repeat_mode)
    {
    case CPlayer::AM_NONE:
        SetARepeatPoint();
        break;
    case CPlayer::AM_A_SELECTED:
        if (!SetBRepeatPoint())
            ResetABRepeat();
        break;
    case CPlayer::AM_AB_REPEAT:
        ResetABRepeat();
        break;
    default:
        break;
    }
}

void CPlayer::ResetABRepeat()
{
    m_ab_repeat_mode = AM_NONE;
}

void CPlayer::ConnotPlayWarning() const
{
    if (m_pCore->IsMidiConnotPlay())
        PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CONNOT_PLAY_WARNING, 0, 0);
}

void CPlayer::SearchAlbumCover()
{
    CSingleLock sync(&m_album_cover_sync, TRUE);
    //static wstring last_file_path;
    //if (last_file_path != GetCurrentFilePath())		//防止同一个文件多次获取专辑封面
    //{
    m_album_cover.Destroy();
    SongInfo song_info{ CSongDataManager::GetInstance().GetSongInfo3(GetCurrentSongInfo()) };
    bool always_use_external_album_cover{ song_info.AlwaysUseExternalAlbumCover() };
    if ((!theApp.m_app_setting_data.use_out_image || theApp.m_app_setting_data.use_inner_image_first) && !IsOsuFile() && !always_use_external_album_cover)
    {
        //从文件获取专辑封面
        CAudioTag audio_tag(GetCurrentSongInfo2(), GetBassHandle());
        m_album_cover_path = audio_tag.GetAlbumCover(m_album_cover_type);
        if (!m_album_cover_path.empty())
        {
            m_album_cover.Load(m_album_cover_path.c_str());
            AlbumCoverResize();
            MediaTransControlsLoadThumbnail();
        }
    }
    m_inner_cover = !m_album_cover.IsNull();

    if (/*theApp.m_app_setting_data.use_out_image && */m_album_cover.IsNull())
    {
        //获取不到专辑封面时尝试使用外部图片作为封面
        SearchOutAlbumCover();
    }
    //AlbumCoverGaussBlur();
    //}
    //last_file_path = GetCurrentFilePath();

    ////如果专辑封面过大，则将其缩小，以提高性能
    //if (!m_album_cover.IsNull() && (m_album_cover.GetWidth() > 800 || m_album_cover.GetHeight() > 800))
    //{
    //    CSize image_size(m_album_cover.GetWidth(), m_album_cover.GetHeight());
    //    CCommon::SizeZoom(image_size, 800);

    //    CImage img_temp;
    //    if (CDrawCommon::BitmapStretch(&m_album_cover, &img_temp, image_size))
    //    {
    //        m_album_cover = img_temp;
    //    }
    //}
}

void CPlayer::AlbumCoverGaussBlur()
{
    if (!theApp.m_app_setting_data.background_gauss_blur || !theApp.m_app_setting_data.enable_background)
        return;
    CSingleLock sync(&m_album_cover_sync, TRUE);
    if (m_album_cover.IsNull())
    {
        m_album_cover_blur.Destroy();
    }
    else
    {
        CImage image_tmp;
        CSize image_size(m_album_cover.GetWidth(), m_album_cover.GetHeight());
        //将图片缩小以减小高斯模糊的计算量
        CCommon::SizeZoom(image_size, 300);		//图片大小按比例缩放，使长边等于300
        CDrawCommon::ImageResize(m_album_cover, image_tmp, image_size);		//拉伸图片
#ifdef _DEBUG
        image_tmp.Save(_T("..\\Debug\\image_tmp.bmp"), Gdiplus::ImageFormatBMP);
#endif // _DEBUG

        //执行高斯模糊
        CGaussBlur gauss_blur;
        gauss_blur.SetSigma(static_cast<double>(theApp.m_app_setting_data.gauss_blur_radius) / 10);		//设置高斯模糊半径
        gauss_blur.DoGaussBlur(image_tmp, m_album_cover_blur);
    }
}

void CPlayer::AlbumCoverResize()
{
    m_album_cover_info.GetInfo(m_album_cover);
    m_album_cover_info.size_exceed = false;
    if (!m_album_cover.IsNull() && theApp.m_nc_setting_data.max_album_cover_size > 0)
    {
        CSize image_size;
        image_size.cx = m_album_cover.GetWidth();
        image_size.cy = m_album_cover.GetHeight();
        if (max(image_size.cx, image_size.cy) > theApp.m_nc_setting_data.max_album_cover_size)      //如果专辑封面的尺寸大于设定的最大值，则将其缩小
        {
            wstring temp_img_path{ CCommon::GetTemplatePath() + ALBUM_COVER_TEMP_NAME };
            //缩小图片大小并保存到临时目录
            CDrawCommon::ImageResize(m_album_cover, temp_img_path, theApp.m_nc_setting_data.max_album_cover_size, IT_PNG);
            m_album_cover.Destroy();
            m_album_cover.Load(temp_img_path.c_str());
            m_album_cover_info.size_exceed = true;
        }
    }
}

void CPlayer::InitShuffleList(int first_song)
{
    if (first_song < 0 && first_song != -1 || first_song > static_cast<int>(m_shuffle_list.size()) - 1)
    {
        first_song = 0;
    }
    m_shuffle_list.resize(m_playlist.size());
    //为无序播放列表生成[0, n)的序号
    for (size_t i{}; i < m_shuffle_list.size(); i++)
        m_shuffle_list[i] = i;

    //将生成的序号打乱
    if (m_shuffle_list.size() > 1)
    {
        // 创建随机数引擎
        std::random_device rd;
        std::mt19937 generator(rd());
        if (first_song != -1)  // 指定第一首
        {
            if (first_song != 0)
            {
                //交换
                m_shuffle_list[first_song] = 0;
                m_shuffle_list[0] = first_song;
            }
            std::shuffle(m_shuffle_list.begin() + 1, m_shuffle_list.end(), generator);
        }
        else
        {
            std::shuffle(m_shuffle_list.begin(), m_shuffle_list.end(), generator);
        }
    }
    m_shuffle_index = 0;
    m_is_shuffle_list_played = false;
}

void CPlayer::SearchOutAlbumCover()
{
    CMusicPlayerCmdHelper helper;
    m_album_cover_path = helper.SearchAlbumCover(GetCurrentSongInfo());
    if (!m_album_cover.IsNull())
        m_album_cover.Destroy();
    m_album_cover.Load(m_album_cover_path.c_str());
    AlbumCoverResize();
    MediaTransControlsLoadThumbnail();
}

bool CPlayer::IsOsuFile() const
{
    return m_is_osu;
}

bool CPlayer::IsPlaylistMode() const
{
    return m_playlist_mode == PM_PLAYLIST;
}

bool CPlayer::IsFolderMode() const
{
    return m_playlist_mode == PM_FOLDER;
}

bool CPlayer::IsMediaLibMode() const
{
    return m_playlist_mode == PM_MEDIA_LIB;
}

CMediaClassifier::ClassificationType CPlayer::GetMediaLibPlaylistType() const
{
    return m_media_lib_playlist_type;
}

bool CPlayer::IsPlaylistEmpty() const
{
    return m_playlist.empty() || (m_playlist.size() == 1 /*&& m_playlist[0].file_name.empty()*/ && m_playlist[0].file_path.empty());
}

void CPlayer::SetPlaylistPath(const wstring& playlist_path)
{
    m_playlist_path = playlist_path;
}

wstring CPlayer::GetPlaylistPath() const
{
    return m_playlist_path;
}

bool CPlayer::IsMciCore() const
{
    return m_pCore ? m_pCore->GetCoreType() == PT_MCI : false;
}

bool CPlayer::IsBassCore() const
{
    return m_pCore ? m_pCore->GetCoreType() == PT_BASS : false;
}

bool CPlayer::IsFfmpegCore() const {
    return m_pCore ? m_pCore->GetCoreType() == PT_FFMPEG : false;
}

void CPlayer::MediaTransControlsLoadThumbnail()
{
    if (CCommon::FileExist(m_album_cover_path))
    {
        if (CCommon::IsFileHidden(m_album_cover_path))
        {
            //如果专辑封面图片文件已隐藏，先将文件复制到Temp目录，再取消隐藏属性
            wstring temp_img_path{ CCommon::GetTemplatePath() + ALBUM_COVER_TEMP_NAME2 };
            CopyFile(m_album_cover_path.c_str(), temp_img_path.c_str(), FALSE);
            CCommon::SetFileHidden(temp_img_path, false);
            m_controls.loadThumbnail(temp_img_path);
        }
        else
        {
            //专辑封面图片文件未隐藏
            m_controls.loadThumbnail(m_album_cover_path);
        }
    }
    else
    {
        MediaTransControlsLoadThumbnailDefaultImage();
    }
}

void CPlayer::MediaTransControlsLoadThumbnailDefaultImage()
{
    if (m_album_cover.IsNull())
    {
        if (IsPlaying())
            m_controls.loadThumbnail((const BYTE*)theApp.m_image_set.default_cover_img_data.data(), theApp.m_image_set.default_cover_img_data.size());
        else
            m_controls.loadThumbnail((const BYTE*)theApp.m_image_set.default_cover_not_played_img_data.data(), theApp.m_image_set.default_cover_not_played_img_data.size());
    }
}

void CPlayer::UpdateLastFMCurrentTrack(const SongInfo& info) {
    LastFMTrack track;
    track.ReadDataFrom(info);
    auto& current = theApp.m_lastfm.CurrentTrack();
    if (track == current) {
        int duration = track.duration.toInt() / 1000 * 9 / 10;
        if (track.timestamp - current.timestamp < duration) return;
    }
    theApp.m_lastfm.UpdateCurrentTrack(track);
    if (theApp.m_media_lib_setting_data.lastfm_enable_nowplaying) {
        theApp.UpdateLastFMNowPlaying();
    }
}
