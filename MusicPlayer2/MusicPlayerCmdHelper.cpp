#include "stdafx.h"
#include "MusicPlayerCmdHelper.h"
#include "InternetCommon.h"
#include "Player.h"
#include "MusicPlayer2.h"
#include "MusicPlayerDlg.h"
#include "InputDlg.h"
#include "Playlist.h"
#include "AddToPlaylistDlg.h"
#include "AudioCommon.h"
#include "COSUPlayerHelper.h"
#include "SongDataManager.h"
#include "SelectItemDlg.h"
#include "CommonDialogMgr.h"

CMusicPlayerCmdHelper::CMusicPlayerCmdHelper(CWnd* pOwner)
    : m_pOwner(pOwner)
{
}

CMusicPlayerCmdHelper::~CMusicPlayerCmdHelper()
{
}

void CMusicPlayerCmdHelper::VeiwOnline(SongInfo& song)
{
    //查找歌曲并获取最佳匹配项的歌曲ID
    CSongDataManager::GetInstance().GetSongID(song, song.song_id);  // 从媒体库读取id
    if (song.song_id == 0)		//如果没有获取过ID，则获取一次ID
    {
        wstring song_id;
        song_id = CInternetCommon::SearchSongAndGetMatched(song.title, song.artist, song.album, song.GetFileName()).id;
        song.SetSongId(song_id);
        CSongDataManager::GetInstance().SetSongID(song, song.song_id);  // 与媒体库同步
    }

    if (song.song_id == 0)
        return;
    //获取网易云音乐中该歌曲的在线接听网址
    wstring song_url{ L"http://music.163.com/#/song?id=" + song.GetSongId() };

    //打开超链接
    ShellExecute(NULL, _T("open"), song_url.c_str(), NULL, NULL, SW_SHOW);

}

void CMusicPlayerCmdHelper::FormatConvert(const std::vector<SongInfo>& songs)
{
    if (!theApp.m_format_convert_dialog_exit)
        return;
    CMusicPlayerDlg* pPlayerDlg = CMusicPlayerDlg::GetInstance();
    if (pPlayerDlg == nullptr)
        return;

    CCommon::DeleteModelessDialog(pPlayerDlg->m_pFormatConvertDlg);

    if (CPlayer::GetInstance().IsMciCore())
    {
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_MCI_NO_THIS_FUNCTION_WARNING");
        GetOwner()->MessageBox(info.c_str(), NULL, MB_ICONWARNING | MB_OK);
        return;
    } else if (CPlayer::GetInstance().IsFfmpegCore()) {
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_FFMPEG_NO_THIS_FUNCTION_WARNING");
        GetOwner()->MessageBox(info.c_str(), NULL, MB_ICONWARNING | MB_OK);
        return;
    }

    pPlayerDlg->m_pFormatConvertDlg = new CFormatConvertDlg(songs, GetOwner());
    pPlayerDlg->m_pFormatConvertDlg->Create(IDD_FORMAT_CONVERT_DIALOG);
    pPlayerDlg->m_pFormatConvertDlg->ShowWindow(SW_SHOW);
}

bool CMusicPlayerCmdHelper::OnAddToNewPlaylist(std::function<void(std::vector<SongInfo>&)> get_song_list, std::wstring& playlist_path, const std::wstring& default_name /*= L""*/)
{
    CInputDlg imput_dlg(GetOwner());
    imput_dlg.SetTitle(theApp.m_str_table.LoadText(L"TITLE_NEW_PLAYLIST").c_str());
    imput_dlg.SetInfoText(theApp.m_str_table.LoadText(L"TXT_NEW_PLAYLIST_INPUT_PLAYLIST_NAME").c_str());
    imput_dlg.SetEditText(default_name.c_str());
    if (imput_dlg.DoModal() == IDOK)
    {
        wstring playlist_name = imput_dlg.GetEditText().GetString();
        if (playlist_name.empty())
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_PLAYLIST_NAME_EMPTY_WARNING");
            GetOwner()->MessageBox(info.c_str(), NULL, MB_ICONWARNING | MB_OK);
            return false;
        }
        if (!CCommon::IsFileNameValid(playlist_name))
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_FILE_NAME_INVALID_WARNING");
            GetOwner()->MessageBox(info.c_str(), NULL, MB_ICONWARNING | MB_OK);
            return false;
        }
        playlist_path = theApp.m_playlist_dir + playlist_name + PLAYLIST_EXTENSION;
        if (CCommon::FileExist(playlist_path))
        {
            wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_PLAYLIST_EXIST_WARNING", { playlist_name });
            GetOwner()->MessageBox(info.c_str(), NULL, MB_ICONWARNING | MB_OK);
            return false;
        }
        //添加空的播放列表
        CPlaylistMgr::Instance().AddNewPlaylist(playlist_path);

        //获取选中的曲目的路径
        std::vector<SongInfo> selected_item_path;
        get_song_list(selected_item_path);

        CMusicPlayerDlg* pPlayerDlg = CMusicPlayerDlg::GetInstance();

        CPlaylistFile playlist;
        playlist.LoadFromFile(playlist_path);
        if (!playlist.AddSongsToPlaylist(selected_item_path, theApp.m_media_lib_setting_data.insert_begin_of_playlist)) // 这里由于是空列表所以实际上设置无效不过仍然传递设置
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_FILE_EXIST_IN_PLAYLIST");
            pPlayerDlg->MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
            return false;
        }
        playlist.SaveToFile(playlist_path);
        // 此方法需要自行更新“添加到播放列表”菜单和媒体库标签页
        theApp.m_pMainWnd->SendMessage(WM_INIT_ADD_TO_MENU);
        RefreshMediaTabData(ML_PLAYLIST);

        return true;
    }
    return false;
}

bool CMusicPlayerCmdHelper::OnAddToPlaylistCommand(std::function<void(std::vector<SongInfo>&)> get_song_list, DWORD command)
{
    //响应播放列表右键菜单中的“添加到播放列表”
    if ((command >= ID_ADD_TO_DEFAULT_PLAYLIST && command <= ID_ADD_TO_MY_FAVOURITE + ADD_TO_PLAYLIST_MAX_SIZE) || command == ID_ADD_TO_OTHER_PLAYLIST)
    {
        //获取选中的曲目的路径
        std::vector<SongInfo> selected_item_path;
        get_song_list(selected_item_path);

        if (command == ID_ADD_TO_OTHER_PLAYLIST)
        {
            CAddToPlaylistDlg dlg;
            if (dlg.DoModal() == IDOK)
            {
                wstring playlist_path = theApp.m_playlist_dir + dlg.GetPlaylistSelected().GetString() + PLAYLIST_EXTENSION;
                if (CCommon::FileExist(playlist_path))
                {
                    AddToPlaylist(selected_item_path, playlist_path);
                }
            }
        }
        else if (command == ID_ADD_TO_DEFAULT_PLAYLIST)      //添加到默认播放列表
        {
            std::wstring default_playlist_path = CPlaylistMgr::Instance().m_default_playlist.path;
            AddToPlaylist(selected_item_path, default_playlist_path);
        }
        else if (command == ID_ADD_TO_MY_FAVOURITE)      //添加到“我喜欢”播放列表
        {
            std::wstring favourite_playlist_path = CPlaylistMgr::Instance().m_favourite_playlist.path;
            AddToPlaylist(selected_item_path, favourite_playlist_path);

            //添加到“我喜欢”播放列表后，为添加的项目设置favourite标记
            for (const auto& item : selected_item_path)
            {
                auto& cur_playlist{ CPlayer::GetInstance().GetPlayList() };
                auto iter = std::find_if(cur_playlist.begin(), cur_playlist.end(), [&](const SongInfo& song)
                    {
                        return item.IsSameSong(song);
                    });
                if (iter != cur_playlist.end())
                    iter->is_favourite = true;
            }
            //for (auto i : m_items_selected)
            //{
            //    if (i >= 0 && i < CPlayer::GetInstance().GetSongNum())
            //    {
            //        CPlayer::GetInstance().GetPlayList()[i].is_favourite = true;
            //    }
            //}

        }
        else        //添加到选中的播放列表
        {
            wstring playlist_path;
            const auto& recent_playlist = CPlaylistMgr::Instance().m_recent_playlists;
            int index = command - ID_ADD_TO_MY_FAVOURITE - 1;
            if (index >= 0 && index < static_cast<int>(recent_playlist.size()))
                playlist_path = recent_playlist[index].path;
            if (CCommon::FileExist(playlist_path))
            {
                AddToPlaylist(selected_item_path, playlist_path);
            }
            else
            {
                wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_PLAYLIST_ADD_SONGS_FAILED",
                    { CFilePathHelper(playlist_path).GetFileNameWithoutExtension() });
                GetOwner()->MessageBox(info.c_str(), NULL, MB_ICONWARNING | MB_OK);
            }
        }
        return true;
    }
    return false;
}

bool CMusicPlayerCmdHelper::DeleteSongsFromDisk(const std::vector<SongInfo>& files)
{
    if (theApp.m_media_lib_setting_data.disable_delete_from_disk)
        return false;

    wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_DELETE_SEL_AUDIO_FILE_INQUIRY", { files.size() });
    if (GetOwner()->MessageBox(info.c_str(), NULL, MB_ICONWARNING | MB_OKCANCEL) != IDOK)
        return false;

    vector<wstring> delected_files;
    for (const auto& song : files)
    {
        if (!song.is_cue && !COSUPlayerHelper::IsOsuFile(song.file_path))
            delected_files.push_back(song.file_path);

    }
    if (delected_files.empty())
        return false;

    wstring current_file = CPlayer::GetInstance().GetCurrentSongInfo().file_path;
    //如何要删除的文件是正在播放的文件，则先停止播放
    if (CCommon::IsItemInVector(delected_files, current_file))
    {
        CPlayer::GetInstance().GetPlayStatusMutex().lock();
        CPlayer::GetInstance().MusicControl(Command::STOP);
        CPlayer::GetInstance().MusicControl(Command::CLOSE);
        CPlayer::GetInstance().GetPlayStatusMutex().unlock();
    }
    int rtn{};
    rtn = CommonDialogMgr::DeleteFiles(GetOwner()->m_hWnd, delected_files);
    if (rtn == 0)
    {
        //如果文件删除成功，则从歌曲数据中删除
        for (const auto& file : delected_files)
        {
            CSongDataManager::GetInstance().RemoveItem(file);
        }
        //文件删除后同时删除和文件同名的图片文件和歌词文件
        for (auto& file : delected_files)
        {
            CFilePathHelper file_path(file);
            file = file_path.ReplaceFileExtension(L"jpg").c_str();
        }
        CommonDialogMgr::DeleteFiles(GetOwner()->m_hWnd, delected_files);
        for (const wstring& ext : CLyrics::m_surpported_lyric)      // 删除所有后缀的歌词
        {
            for (auto& file : delected_files)
            {
                CFilePathHelper file_path(file);
                file = file_path.ReplaceFileExtension(ext.c_str()).c_str();
            }
            CommonDialogMgr::DeleteFiles(GetOwner()->m_hWnd, delected_files);
        }
    }
    else if (rtn == 1223)	//如果在弹出的对话框中点击“取消”则返回值为1223
    {
        return false;
    }
    else
    {
        GetOwner()->MessageBox(theApp.m_str_table.LoadText(L"MSG_DELETE_FILE_FAILED").c_str(), NULL, MB_ICONWARNING);
        return false;
    }
    return true;
}

void CMusicPlayerCmdHelper::SearchLyricFiles(const wstring& lyric_name, const wstring& cur_dir, std::vector<std::wstring>& result, bool fuzzy_match)
{
    result.clear();

    // 查找歌词文件名和搜索关键词完全匹配的歌词
    CFilePathHelper lyric_path{ cur_dir + lyric_name + L".lrc"};     // 得到路径+文件名的字符串，预先加扩展名防止之后ReplaceFileExtension替换误伤

    // 按顺序列出所有需要查找的目录
    vector<wstring> path_list{ cur_dir };
    if (CCommon::FolderExist(theApp.m_lyric_setting_data.lyric_path))
    {
        path_list.push_back(theApp.m_lyric_setting_data.lyric_path);
    }

    // 完全匹配搜索，使用CCommon::FileExist的快速查找
    for (const wstring& pa : path_list)
    {
        lyric_path.SetFilePath(pa + lyric_path.GetFileName());
        for (const wstring& ext : CLyrics::m_surpported_lyric)
        {
            lyric_path.ReplaceFileExtension(ext.c_str());
            if (CCommon::FileExist(lyric_path.GetFilePath()))
                result.push_back(lyric_path.GetFilePath());
        }
    }

    // 列出所有搜索路径内的歌词文件
    vector<vector<wstring>> path_lyrics;
    path_lyrics.resize(path_list.size());
    for (int i{}; i < static_cast<int>(path_list.size()); ++i)
    {
        CAudioCommon::GetLyricFiles(path_list[i], path_lyrics[i]);
    }

    //将查找的歌词名称拆分成若干个子字符串
    vector<wstring> key_words;
    CCommon::StringSplit(lyric_name, L'-', key_words);
    for (auto& key_word : key_words)
    {
        CCommon::StringNormalize(key_word);
        CCommon::FileNameNormalize(key_word);
    }

    // 判断一个字符串是否匹配key_words中任意一个关键字（根据是否模糊搜索使用不同方式）
    auto isMatched = [&](const wstring& str, const vector<wstring>& key_words)
    {
        if (fuzzy_match)
        {
            //如果是部分匹配，则只需要有一个关键字匹配，就返回true
            for (const auto& key_word : key_words)
            {
                if (CCommon::StringNatchWholeWord(str, key_word) != -1)
                    return true;
            }
            return false;
        }
        else
        {
            //否则，需要所有关键字都匹配才返回true
            for (const auto& key_word : key_words)
            {
                if (CCommon::StringNatchWholeWord(str, key_word) == -1)
                    return false;
            }
            return true;
        }
    };

    for (int i{}; i < static_cast<int>(path_lyrics.size()); ++i)
    {
        const vector<wstring>& cur_path = path_lyrics[i];
        for (const wstring& str : cur_path)
        {
            if (isMatched(str, key_words))
            {
                wstring matched_lyric = path_list[i] + str;
                if (!CCommon::IsItemInVector(result, matched_lyric))
                    result.push_back(matched_lyric);
            }
        }
    }
}

std::wstring CMusicPlayerCmdHelper::SearchLyricFile(const SongInfo& song, bool fuzzy_match)
{
    if (song.GetFileName().size() < 3) return wstring();

    // cue与osu文件禁止按音频文件名搜索，无视模糊搜索，优先搜索“艺术家 - 标题”
    bool find_org_name{ !song.is_cue && !COSUPlayerHelper::IsOsuFile(song.file_path) };
    CFilePathHelper lyric_path{ song.file_path };
    if (!find_org_name)
    {
        wstring ar_ti{ song.artist + L" - " + song.title + L".lrc"};    // 预先加扩展名防止之后ReplaceFileExtension替换误伤
        CCommon::FileNameNormalize(ar_ti);
        lyric_path.SetFilePath(lyric_path.GetDir() + ar_ti);
    }

    // 按顺序列出所有需要查找的目录
    vector<wstring> path_list{ lyric_path.GetDir() };
    if (CCommon::FolderExist(theApp.m_lyric_setting_data.lyric_path))
    {
        path_list.push_back(theApp.m_lyric_setting_data.lyric_path);
    }

    // 完全匹配搜索，使用CCommon::FileExist的快速查找
    for (const wstring& pa : path_list)
    {
        lyric_path.SetFilePath(pa + lyric_path.GetFileName());
        for (const wstring& ext : CLyrics::m_surpported_lyric)
        {
            lyric_path.ReplaceFileExtension(ext.c_str());
            if (CCommon::FileExist(lyric_path.GetFilePath()))
                return lyric_path.GetFilePath();
        }
    }
    // 进行模糊查找，由于需要列出所有歌词文件所以较慢
    if (fuzzy_match)
    {
        // 列出所有搜索路径内的歌词文件
        vector<vector<wstring>> path_lyrics;
        path_lyrics.resize(path_list.size());
        for (int i{}; i < static_cast<int>(path_list.size()); ++i)
        {
            CAudioCommon::GetLyricFiles(path_list[i], path_lyrics[i]);
        }
        wstring matched_lyric;      // 匹配的歌词的路径
        wstring title{ song.title }, artist{ song.artist };
        CCommon::FileNameNormalize(title);
        CCommon::FileNameNormalize(artist);
        // 先寻找歌词文件中同时包含歌曲标题和艺术家的歌词文件
        for (int i{}; i < static_cast<int>(path_lyrics.size()); ++i)
        {
            const vector<wstring>& cur_path = path_lyrics[i];
            for (const wstring& str : cur_path)
            {
                if (CCommon::StringNatchWholeWord(str, artist) != -1 && CCommon::StringNatchWholeWord(str, title) != -1)
                {
                    matched_lyric = path_list[i] + str;
                    return matched_lyric;
                }
            }
        }
        // 没有找到的话就寻找歌词文件中只包含歌曲标题的歌词文件
        for (int i{}; i < static_cast<int>(path_lyrics.size()); ++i)
        {
            const vector<wstring>& cur_path = path_lyrics[i];
            for (const wstring& str : cur_path)
            {
                if (CCommon::StringNatchWholeWord(str, title) != -1)
                {
                    matched_lyric = path_list[i] + str;
                    return matched_lyric;
                }
            }
        }
    }
    return wstring();
}

std::wstring CMusicPlayerCmdHelper::SearchAlbumCover(const SongInfo& song)
{
    wstring album_cover_path;
    if (COSUPlayerHelper::IsOsuFile(song.file_path))
    {
        album_cover_path = COSUPlayerHelper::GetAlbumCover(song.file_path);
        if (album_cover_path.empty())
            album_cover_path = theApp.m_nc_setting_data.default_osu_img;
    }
    else
    {
        vector<wstring> files;
        wstring file_name;
        //查找文件和歌曲名一致的图片文件
        CFilePathHelper c_file_path(song.file_path);
        //file_name = m_path + c_file_name.GetFileNameWithoutExtension() + L".*";
        c_file_path.ReplaceFileExtension(L"*");
        wstring dir{ c_file_path.GetDir() };
        CCommon::GetImageFiles(c_file_path.GetFilePath(), files);
        if (files.empty() && !song.album.empty())
        {
            //没有找到和歌曲名一致的图片文件，则查找文件名为“唱片集”的文件
            wstring album_name{ song.album };
            CCommon::FileNameNormalize(album_name);
            file_name = dir + album_name + L".*";
            CCommon::GetImageFiles(file_name, files);
        }
        if (!files.empty())
            album_cover_path = dir + files[0];
        // 没有找到唱片集为文件名的文件，查找文件名为设置的专辑封面名的文件
        else if (theApp.m_app_setting_data.use_out_image)
        {
            wstring absolute_dir;
            // 按照默认封面文件名列表查找歌曲所在目录，如遇到绝对路径则存入absolute_dir
            for (const auto& album_name : theApp.m_app_setting_data.default_album_name)
            {
                if (!album_name.empty())
                {
                    if (absolute_dir.empty() && CCommon::IsPath(album_name) && CCommon::FileExist(album_name))
                    {
                        absolute_dir = album_name;
                        continue;
                    }
                    file_name = CCommon::RelativePathToAbsolutePath(album_name + L".*", dir);
                    CCommon::GetImageFiles(file_name, files);
                }
                if (!files.empty())
                {
                    // 处理album_name可能含有相对路径的情况，files[0]仅有文件名
                    // 由于album_name中文件名部分可能含有通配符所以不能只替换后缀，需要替换整个文件名
                    size_t index = file_name.rfind('\\');
                    album_cover_path = file_name.substr(0, index + 1) + files[0];
                    break;
                }
            }
            // 按照歌曲名、唱片名查找封面文件夹album_path
            if (album_cover_path.empty())
            {
                CCommon::GetImageFiles(theApp.m_app_setting_data.album_cover_path + c_file_path.GetFileName(), files);
                if (files.empty() && !song.album.empty())
                {
                    // 没有找到和歌曲名一致的图片文件，则查找文件名为“唱片集”的文件
                    wstring album_name{ song.album };
                    CCommon::FileNameNormalize(album_name);
                    file_name = theApp.m_app_setting_data.album_cover_path + album_name + L".*";
                    CCommon::GetImageFiles(file_name, files);
                }
                if (!files.empty())
                    album_cover_path = theApp.m_app_setting_data.album_cover_path + files[0];
            }
            // 使用默认封面文件名列表中的绝对路径
            if (album_cover_path.empty())
                album_cover_path = absolute_dir;
        }
    }
    return album_cover_path;
}

bool CMusicPlayerCmdHelper::OnRating(const SongInfo& song, DWORD command)
{
    if ((command >= ID_RATING_1 && command <= ID_RATING_5) || command == ID_RATING_NONE)     //如果命令是歌曲分级（应确保分级命令的ID是连续的）
    {
        int rating = 0;
        if (command >= ID_RATING_1 && command <= ID_RATING_5)
            rating = command - ID_RATING_1 + 1;
        SongInfo song_info{ CSongDataManager::GetInstance().GetSongInfo3(song) };
        song_info.rating = static_cast<BYTE>(rating);
        bool succeed{};
        // cue、osu!、不支持写入的文件分级只保存到媒体库
        if (CAudioTag::IsFileRatingSupport(CFilePathHelper(song_info.file_path).GetFileExtension()) && !song_info.is_cue && !COSUPlayerHelper::IsOsuFile(song_info.file_path))
        {
            CAudioTag audio_tag(song_info);
            succeed = audio_tag.WriteAudioRating();
        }
        else
        {
            succeed = true;     //如果文件格式不支持写入分级，也返回true
        }
        CSongDataManager::GetInstance().AddItem(song_info);
        return succeed;
    }
    return true;
}

int CMusicPlayerCmdHelper::UpdateMediaLib()
{
    if (CPlayer::GetInstance().IsMciCore())
        return 0;

    vector<SongInfo> all_media_songs;
    //获取所有音频文件的路径
    for (const auto& item : theApp.m_media_lib_setting_data.media_folders)
    {
        // 这里还是特殊处理比较好，避免自动加入大量无关文件（比如配的视频等等）
        if (!COSUPlayerHelper::IsOsuFolder(item))
            CAudioCommon::GetAudioFiles(item, all_media_songs, MAX_SONG_NUM, true);
        else
            COSUPlayerHelper::GetOSUAudioFiles(item, all_media_songs);
    }

    CAudioCommon::GetAudioInfo(all_media_songs,
        theApp.m_media_update_para.num_added,
        theApp.m_media_update_para.thread_exit,
        theApp.m_media_update_para.process_percent,
        theApp.m_media_update_para.force ? MR_FOECE_FULL : MR_FILE_MODIFICATION,
        theApp.m_media_lib_setting_data.ignore_too_short_when_update
    );

    return theApp.m_media_update_para.num_added;
}

int CMusicPlayerCmdHelper::CleanUpSongData(std::function<bool(const SongInfo&)> fun_condition)
{
    return CSongDataManager::GetInstance().RemoveItemIf(fun_condition);
}

int CMusicPlayerCmdHelper::CleanUpRecentFolders()
{
    int cleard_cnt{};
    auto& recent_folders{ CPlayer::GetInstance().GetRecentPath() };
    for (size_t i{}; i < recent_folders.size(); i++)
    {
        if (!CAudioCommon::IsPathContainsAudioFile(recent_folders[i].path, recent_folders[i].contain_sub_folder) && !COSUPlayerHelper::IsOsuFolder(recent_folders[i].path))
        {
            recent_folders.erase(recent_folders.begin() + i);		//删除不存在的路径
            i--;
            cleard_cnt++;
        }
    }
    return cleard_cnt;
}

void CMusicPlayerCmdHelper::ShowMediaLib(int cur_tab /*= -1*/, int tab_force_show)
{
    CMusicPlayerDlg* pPlayerDlg = CMusicPlayerDlg::GetInstance();
    if (pPlayerDlg == nullptr)
        return;

    bool dlg_exist = (pPlayerDlg->m_pMediaLibDlg != nullptr && IsWindow(pPlayerDlg->m_pMediaLibDlg->m_hWnd));   //媒体库对话框是否已经存在
    bool tab_not_show = (~theApp.m_media_lib_setting_data.display_item & tab_force_show);       //如果有要强制显示的标签但是媒体库设置中此标签不显示

    //计算实际的tab序号（参数cur_tab为媒体库中标签的序号，但是如果有标签不显示，则此序号将不正确，因此这里需要根据实际显示的标签计算出真正的tab序号）
    int shown_tab = theApp.m_media_lib_setting_data.display_item | tab_force_show;  //实际要显示的标签
    int tab_num = cur_tab - 2;             //除去“文件夹”和“播放列表”两个标签（因为这两个标签总是显示，无法隐藏）
    for (int i = 0; i < tab_num; i++)      //遍历cur_tab及前面的所有标签，检查是否有未显示出来的标签
    {
        int tab_mask = 1 << i;
        if ((shown_tab & tab_mask) == 0)  //如果有未显示出来的标签，则当前标签序号减1
            cur_tab--;
    }

    if (dlg_exist && !tab_not_show)
    {
        pPlayerDlg->m_pMediaLibDlg->SetTabForceShow(tab_force_show);
        pPlayerDlg->m_pMediaLibDlg->ShowWindow(SW_SHOWNORMAL);
        pPlayerDlg->m_pMediaLibDlg->SetForegroundWindow();
        if (cur_tab >= 0)
            pPlayerDlg->m_pMediaLibDlg->SetCurTab(cur_tab);
    }
    else    //如果对话框不存在，或有需要强制显示但是媒体库库中设置不显示的的标签，则需要重新打开媒体库对话框
    {
        CCommon::DeleteModelessDialog(pPlayerDlg->m_pMediaLibDlg);
        int tab_index = cur_tab;
        if (tab_index < 0)
            tab_index = CPlayer::GetInstance().IsFolderMode() ? 0 : 1;
        pPlayerDlg->m_pMediaLibDlg = new CMediaLibDlg(tab_index);
        pPlayerDlg->m_pMediaLibDlg->SetTabForceShow(tab_force_show);
        pPlayerDlg->m_pMediaLibDlg->Create(IDD_MEDIA_LIB_DIALOG/*, GetDesktopWindow()*/);
        pPlayerDlg->m_pMediaLibDlg->ShowWindow(SW_SHOW);
    }
}

void CMusicPlayerCmdHelper::RefreshMediaTabData(eMediaLibTab tab_index)
{
    CMusicPlayerDlg* pPlayerDlg = CMusicPlayerDlg::GetInstance();
    if (pPlayerDlg != nullptr && pPlayerDlg->m_pMediaLibDlg != nullptr && IsWindow(pPlayerDlg->m_pMediaLibDlg->GetSafeHwnd()))
    {
        if (tab_index == ML_FOLDER)
            pPlayerDlg->m_pMediaLibDlg->m_path_dlg->RefreshTabData();         // 刷新媒体库文件夹列表
        else if(tab_index == ML_PLAYLIST)
            pPlayerDlg->m_pMediaLibDlg->m_playlist_dlg->RefreshTabData();    // 刷新媒体库播放列表列表
    }
}

void CMusicPlayerCmdHelper::OnViewArtist(const SongInfo& song_info)
{
    vector<wstring> artist_list;
    song_info.GetArtistList(artist_list);     // 获取艺术家（可能有多个）
    wstring artist;
    if (artist_list.empty())
    {
        return;
    }
    else if (artist_list.size() == 1)
    {
        artist = artist_list.front();
    }
    else
    {
        //如果有多个艺术家，弹出“选择艺术家”对话框
        CSelectItemDlg dlg(artist_list);
        dlg.SetTitle(theApp.m_str_table.LoadText(L"TITLE_SELECT_ARTIST").c_str());
        dlg.SetDlgIcon(IconMgr::IconType::IT_Artist);
        if (dlg.DoModal() == IDOK)
            artist = dlg.GetSelectedItem();
        else
            return;
    }
    ShowMediaLib(CMusicPlayerCmdHelper::ML_ARTIST, MLDI_ARTIST);
    CMusicPlayerDlg* pPlayerDlg = CMusicPlayerDlg::GetInstance();
    if (!pPlayerDlg->m_pMediaLibDlg->NavigateToItem(artist))
    {
        wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_CANNOT_FIND_ARTIST_WARNING", { artist });
        pPlayerDlg->MessageBox(info.c_str(), NULL, MB_OK | MB_ICONWARNING);
    }
}

void CMusicPlayerCmdHelper::OnViewAlbum(const SongInfo& song_info)
{
    wstring album = song_info.GetAlbum();
    ShowMediaLib(CMusicPlayerCmdHelper::ML_ALBUM, MLDI_ALBUM);
    CMusicPlayerDlg* pPlayerDlg = CMusicPlayerDlg::GetInstance();
    if (!pPlayerDlg->m_pMediaLibDlg->NavigateToItem(album))
    {
        wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_CANNOT_FIND_ALBUM_WARNING", { album });
        pPlayerDlg->MessageBox(info.c_str(), NULL, MB_OK | MB_ICONWARNING);
    }
}

int CMusicPlayerCmdHelper::FixPlaylistPathError(const std::wstring& path)
{
    vector<SongInfo> song_list;
    CPlaylistFile playlist_file;
    playlist_file.LoadFromFile(path);
    playlist_file.MoveToSongList(song_list);    // move后playlist_file对象不再可用
    int fixed_count{};
    for (auto& song : song_list)
    {
        if (!CCommon::FileExist(song.file_path))
        {
            //std::wstring file_name{ song.GetFileName() };
            ////文件不存在，从媒体库中寻找匹配的文件
            //for (const auto& song_info : CSongDataManager::GetInstance().GetSongData())
            //{
            //    if (CCommon::FileExist(song_info.second.file_path) && song_info.second.GetFileName() == file_name)
            //    {
            //        song.file_path = song_info.second.file_path;
            //        fixed_count++;
            //    }
            //}
            if (CSongDataManager::GetInstance().FixWrongFilePath(song.file_path))
                fixed_count++;
        }
    }
    if (fixed_count > 0)
    {
        //保存播放列表到文件
        CPlaylistFile::SavePlaylistToFile(song_list, path);

        //如果处理的是正在播放的播放列表
        if (CPlayer::GetInstance().IsPlaylistMode() && CPlayer::GetInstance().GetPlaylistPath() == path)
        {
            CPlayer::GetInstance().ReloadPlaylist(MR_MIN_REQUIRED);
        }
    }
    return fixed_count;
}

void CMusicPlayerCmdHelper::OnRecentItemSelected(const CRecentFolderAndPlaylist::Item* item)
{
    if (item != nullptr && !item->IsItemCurrentPlaying())
    {
        if (item->IsPlaylist())
        {
            if (item->playlist_info != nullptr)
            {
                if (!CPlayer::GetInstance().SetPlaylist(item->playlist_info->path, item->playlist_info->track, item->playlist_info->position))
                {
                    const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
                    AfxMessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
                }
            }
        }
        else if (item->IsFolder())
        {
            if (item->folder_info != nullptr)
            {
                if (!CPlayer::GetInstance().SetPath(*item->folder_info))
                {
                    const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
                    AfxMessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
                }
            }
        }
        else if (item->IsMedialib())
        {
            if (item->medialib_info != nullptr)
            {
                if (!CPlayer::GetInstance().SetMediaLibPlaylist(item->medialib_info->medialib_type, item->medialib_info->path))
                {
                    const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
                    AfxMessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
                }
            }
        }
    }
}

void CMusicPlayerCmdHelper::AddToPlaylist(const std::vector<SongInfo>& songs, const std::wstring& playlist_path)
{
    CMusicPlayerDlg* pPlayerDlg = CMusicPlayerDlg::GetInstance();
    if (CPlayer::GetInstance().IsPlaylistMode() && playlist_path == CPlayer::GetInstance().GetPlaylistPath())
    {
        int rtn = CPlayer::GetInstance().AddSongsToPlaylist(songs);
        if (rtn == 0)
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_FILE_EXIST_IN_PLAYLIST");
            pPlayerDlg->MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
        }
        else if (rtn == -1)
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
            pPlayerDlg->MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
        }
    }
    else
    {
        CPlaylistFile playlist;
        playlist.LoadFromFile(playlist_path);
        if (playlist.AddSongsToPlaylist(songs, theApp.m_media_lib_setting_data.insert_begin_of_playlist))
            playlist.SaveToFile(playlist_path);
        else
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_FILE_EXIST_IN_PLAYLIST");
            pPlayerDlg->MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
        }
    }
}

CWnd* CMusicPlayerCmdHelper::GetOwner()
{
    if (m_pOwner != nullptr)
        return m_pOwner;
    else
        return theApp.m_pMainWnd;
}
