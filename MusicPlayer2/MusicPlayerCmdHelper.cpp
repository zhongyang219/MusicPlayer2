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
    if (song.song_id == 0)		//如果没有获取过ID，则获取一次ID
    {
        wstring song_id;
        song_id = CInternetCommon::SearchSongAndGetMatched(song.title, song.artist, song.album, song.GetFileName()).id;
        song.SetSongId(song_id);
        if (!song.is_cue)
        {
            CSongDataManager::GetInstance().SaveSongInfo(song);
        }
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
    CMusicPlayerDlg* pPlayerDlg = dynamic_cast<CMusicPlayerDlg*>(theApp.m_pMainWnd);
    if (pPlayerDlg == nullptr)
        return;

    CCommon::DeleteModelessDialog(pPlayerDlg->m_pFormatConvertDlg);

    if (CPlayer::GetInstance().IsMciCore())
    {
        GetOwner()->MessageBox(CCommon::LoadText(IDS_MCI_NO_THIS_FUNCTION_WARNING), NULL, MB_ICONWARNING | MB_OK);
        return;
    }

    pPlayerDlg->m_pFormatConvertDlg = new CFormatConvertDlg(songs, GetOwner());
    pPlayerDlg->m_pFormatConvertDlg->Create(IDD_FORMAT_CONVERT_DIALOG);
    pPlayerDlg->m_pFormatConvertDlg->ShowWindow(SW_SHOW);
}

bool CMusicPlayerCmdHelper::OnAddToNewPlaylist(std::function<void(std::vector<SongInfo>&)> get_song_list, std::wstring& playlist_path, const std::wstring& default_name /*= L""*/)
{
    CInputDlg imput_dlg(GetOwner());
    imput_dlg.SetTitle(CCommon::LoadText(IDS_NEW_PLAYLIST));
    imput_dlg.SetInfoText(CCommon::LoadText(IDS_INPUT_PLAYLIST_NAME));
    imput_dlg.SetEditText(default_name.c_str());
    if (imput_dlg.DoModal() == IDOK)
    {
        CString playlist_name = imput_dlg.GetEditText();
        if (playlist_name.IsEmpty())
        {
            GetOwner()->MessageBox(CCommon::LoadText(IDS_PLAYLIST_NAME_EMPTY_WARNING), NULL, MB_ICONWARNING | MB_OK);
            return false;
        }
        if (!CCommon::IsFileNameValid(wstring(playlist_name.GetString())))
        {
            GetOwner()->MessageBox(CCommon::LoadText(IDS_FILE_NAME_INVALID_WARNING), NULL, MB_ICONWARNING | MB_OK);
            return false;
        }
        playlist_path = theApp.m_playlist_dir + playlist_name.GetString() + PLAYLIST_EXTENSION;
        if (CCommon::FileExist(playlist_path))
        {
            GetOwner()->MessageBox(CCommon::LoadTextFormat(IDS_PLAYLIST_EXIST_WARNING, { playlist_name }), NULL, MB_ICONWARNING | MB_OK);
            return false;
        }
        //添加空的播放列表
        CPlayer::GetInstance().GetRecentPlaylist().AddNewPlaylist(playlist_path);

        //获取选中的曲目的路径
        std::vector<SongInfo> selected_item_path;
        get_song_list(selected_item_path);

        CMusicPlayerDlg* pPlayerDlg = dynamic_cast<CMusicPlayerDlg*>(theApp.m_pMainWnd);

        CPlaylistFile playlist;
        playlist.LoadFromFile(playlist_path);
        if (!playlist.AddFiles(selected_item_path, theApp.m_media_lib_setting_data.ignore_songs_already_in_playlist))
        {
            pPlayerDlg->MessageBox(CCommon::LoadText(IDS_FILE_EXIST_IN_PLAYLIST_INFO), NULL, MB_ICONWARNING | MB_OK);
            return false;
        }
        playlist.SaveToFile(playlist_path);
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

        CMusicPlayerDlg* pPlayerDlg = dynamic_cast<CMusicPlayerDlg*>(theApp.m_pMainWnd);

        if (command == ID_ADD_TO_OTHER_PLAYLIST)
        {
            CAddToPlaylistDlg dlg;
            if (dlg.DoModal() == IDOK)
            {
                wstring playlist_path = theApp.m_playlist_dir + dlg.GetPlaylistSelected().GetString() + PLAYLIST_EXTENSION;
                if (CCommon::FileExist(playlist_path))
                {
                    if (!AddToPlaylist(selected_item_path, playlist_path))
                        pPlayerDlg->MessageBox(CCommon::LoadText(IDS_FILE_EXIST_IN_PLAYLIST_INFO), NULL, MB_ICONWARNING | MB_OK);

                }
            }
        }
        else if (command == ID_ADD_TO_DEFAULT_PLAYLIST)      //添加到默认播放列表
        {
            std::wstring default_playlist_path = CPlayer::GetInstance().GetRecentPlaylist().m_default_playlist.path;
            if (!AddToPlaylist(selected_item_path, default_playlist_path))
                pPlayerDlg->MessageBox(CCommon::LoadText(IDS_FILE_EXIST_IN_PLAYLIST_INFO), NULL, MB_ICONWARNING | MB_OK);
        }
        else if (command == ID_ADD_TO_MY_FAVOURITE)      //添加到“我喜欢”播放列表
        {
            std::wstring favourite_playlist_path = CPlayer::GetInstance().GetRecentPlaylist().m_favourite_playlist.path;
            if (!AddToPlaylist(selected_item_path, favourite_playlist_path))
                pPlayerDlg->MessageBox(CCommon::LoadText(IDS_FILE_EXIST_IN_PLAYLIST_INFO), NULL, MB_ICONWARNING | MB_OK);

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
            CString menu_string;
            theApp.m_menu_set.m_list_popup_menu.GetMenuString(command, menu_string, 0);
            if (!menu_string.IsEmpty())
            {
                wstring playlist_path = theApp.m_playlist_dir + menu_string.GetString() + PLAYLIST_EXTENSION;
                if (CCommon::FileExist(playlist_path))
                {
                    if (!AddToPlaylist(selected_item_path, playlist_path))
                        pPlayerDlg->MessageBox(CCommon::LoadText(IDS_FILE_EXIST_IN_PLAYLIST_INFO), NULL, MB_ICONWARNING | MB_OK);
                }
                else
                {
                    GetOwner()->MessageBox(CCommon::LoadText(IDS_ADD_TO_PLAYLIST_FAILED_WARNING), NULL, MB_ICONWARNING | MB_OK);
                }
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

    CString info = CCommon::LoadTextFormat(IDS_DELETE_FILE_INQUARY, { files.size() });
    if (GetOwner()->MessageBox(info, NULL, MB_ICONWARNING | MB_OKCANCEL) != IDOK)
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
        CPlayer::GetInstance().MusicControl(Command::STOP);
        CPlayer::GetInstance().MusicControl(Command::CLOSE);
    }
    int rtn{};
    rtn = CCommon::DeleteFiles(GetOwner()->m_hWnd, delected_files);
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
        CCommon::DeleteFiles(GetOwner()->m_hWnd, delected_files);
        for (auto& file : delected_files)
        {
            CFilePathHelper file_path(file);
            file = file_path.ReplaceFileExtension(L"lrc").c_str();
        }
        CCommon::DeleteFiles(GetOwner()->m_hWnd, delected_files);
    }
    else if (rtn == 1223)	//如果在弹出的对话框中点击“取消”则返回值为1223
    {
        return false;
    }
    else
    {
        GetOwner()->MessageBox(CCommon::LoadText(IDS_CONNOT_DELETE_FILE), NULL, MB_ICONWARNING);
        return false;
    }
    return true;
}

void CMusicPlayerCmdHelper::SearchLyricFiles(const wstring& lyric_name, const wstring& cur_dir, std::vector<std::wstring>& result, bool fuzzy_match)
{
    result.clear();
    //if (song.GetFileName().size() < 3) return;

    wstring lyric_path{ cur_dir + lyric_name + L".lrc" };		//得到路径+文件名的字符串
    wstring lyric_path2{ theApp.m_lyric_setting_data.lyric_path + lyric_name + L".lrc" };
    //查找歌词文件名和歌曲文件名完全匹配的歌词
    if (CCommon::FileExist(lyric_path))
    {
        result.push_back(lyric_path);
    }
    if (CCommon::FileExist(lyric_path2))
    {
        result.push_back(lyric_path2);
    }
    vector<wstring> current_path_lyrics;	//储存当前路径下的歌词文件的文件名
    vector<wstring> lyric_path_lyrics;		//储存歌词文件夹下的歌词文件的文件名
    CAudioCommon::GetLyricFiles(cur_dir, current_path_lyrics);
    CAudioCommon::GetLyricFiles(theApp.m_lyric_setting_data.lyric_path, lyric_path_lyrics);

    //将查找的歌词名称拆分成若干个子字符串
    vector<wstring> key_words;
    CCommon::StringSplit(lyric_name, L'-', key_words);
    for (auto& key_word : key_words)
    {
        CCommon::StringNormalize(key_word);
        CCommon::FileNameNormalize(key_word);
    }

    //判断一个字符串是否匹配key_words中任意一个关键字
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

    //寻找歌词文件中同时包含歌曲标题的歌词文件
    for (const auto& str : current_path_lyrics)	//在当前目录下寻找
    {
        if (isMatched(str, key_words))
        {
            wstring matched_lyric = cur_dir + str;
            if (!CCommon::IsItemInVector(result, matched_lyric))
                result.push_back(matched_lyric);
        }
    }

    for (const auto& str : lyric_path_lyrics)	//在歌词目录下寻找
    {
        if (isMatched(str, key_words))
        {
            wstring matched_lyric = theApp.m_lyric_setting_data.lyric_path + str;
            if (!CCommon::IsItemInVector(result, matched_lyric))
                result.push_back(matched_lyric);
        }
    }
}

std::wstring CMusicPlayerCmdHelper::SearchLyricFile(const SongInfo& song, bool fuzzy_match)
{
    if (song.GetFileName().size() < 3) return wstring();

    CFilePathHelper lyric_path{ song.file_path };		//得到路径+文件名的字符串
    lyric_path.ReplaceFileExtension(L"lrc");		//将文件扩展替换成lrc
    CFilePathHelper lyric_path2{ theApp.m_lyric_setting_data.lyric_path + song.GetFileName() };
    lyric_path2.ReplaceFileExtension(L"lrc");
    //查找歌词文件名和歌曲文件名完全匹配的歌词
    if (CCommon::FileExist(lyric_path.GetFilePath()))
    {
        return lyric_path.GetFilePath();
    }
    else if (CCommon::FileExist(lyric_path2.GetFilePath()))		//当前目录下没有对应的歌词文件时，就在theApp.m_lyric_setting_data.m_lyric_path目录下寻找歌词文件
    {
        return lyric_path2.GetFilePath();
    }
    else if (fuzzy_match)
    {
        vector<wstring> current_path_lyrics;	//储存当前路径下的歌词文件的文件名
        vector<wstring> lyric_path_lyrics;		//储存歌词文件夹下的歌词文件的文件名
        CAudioCommon::GetLyricFiles(lyric_path.GetDir(), current_path_lyrics);
        CAudioCommon::GetLyricFiles(theApp.m_lyric_setting_data.lyric_path, lyric_path_lyrics);

        wstring matched_lyric;		//匹配的歌词的路径
        wstring title{ song.title }, artist{ song.artist };
        CCommon::FileNameNormalize(title);
        CCommon::FileNameNormalize(artist);
        //先寻找歌词文件中同时包含歌曲标题和艺术家的歌词文件
        for (const auto& str : current_path_lyrics)	//在当前目录下寻找
        {
            if (CCommon::StringNatchWholeWord(str, artist) != -1 && CCommon::StringNatchWholeWord(str, title) != -1)
            {
                matched_lyric = lyric_path.GetDir() + str;
                return matched_lyric;
            }
        }

        if (matched_lyric.empty())		//如果当前目录下没找到
        {
            for (const auto& str : lyric_path_lyrics)	//在歌词目录下寻找
            {
                if (CCommon::StringNatchWholeWord(str, artist) != -1 && CCommon::StringNatchWholeWord(str, title) != -1)
                {
                    matched_lyric = theApp.m_lyric_setting_data.lyric_path + str;
                    return matched_lyric;
                }
            }
        }

        //没有找到的话就寻找歌词文件中只包含歌曲标题的歌词文件
        if (matched_lyric.empty())
        {
            for (const auto& str : current_path_lyrics)	//在当前目录下寻找
            {
                //if (str.find(song.title) != string::npos)
                if (CCommon::StringNatchWholeWord(str, song.title) != -1)
                {
                    matched_lyric = lyric_path.GetDir() + str;
                    return matched_lyric;
                }
            }
        }

        if (matched_lyric.empty())
        {
            for (const auto& str : lyric_path_lyrics)	//在歌词目录下寻找
            {
                //if (str.find(song.title) != string::npos)
                if (CCommon::StringNatchWholeWord(str, song.title) != -1)
                {
                    matched_lyric = theApp.m_lyric_setting_data.lyric_path + str;
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
            for (const auto& album_name : theApp.m_app_setting_data.default_album_name)
            {
                if (!album_name.empty())
                {
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
        }
    }
    return album_cover_path;
}

bool CMusicPlayerCmdHelper::OnRating(const wstring& file_path, DWORD command)
{
    if (command - ID_RATING_1 <= 5)     //如果命令是歌曲分级（应确保分级命令的ID是连续的）
    {
        int rating = command - ID_RATING_1 + 1;
        SongInfo song = CSongDataManager::GetInstance().GetSongInfo(file_path);
        song.rating = static_cast<BYTE>(rating);
        CAudioTag audio_tag(song);
        bool succeed{ audio_tag.WriteAudioRating() };
        if (!CAudioTag::IsFileRatingSupport(CFilePathHelper(file_path).GetFileExtension()))
            succeed = true;     //如果文件格式不支持写入分级，也返回true
        //if (succeed)
        //{
        CSongDataManager::GetInstance().AddItem(file_path, song);
        CSongDataManager::GetInstance().SetSongDataModified();
        //}
        return succeed;
    }
    return true;
}

int CMusicPlayerCmdHelper::UpdateMediaLib(bool refresh)
{
    if (CPlayer::GetInstance().IsMciCore())
        return 0;

    theApp.m_media_num_added = 0;
    std::vector<wstring> all_media_files;
    //获取所有音频文件的路径
    for (const auto& item : theApp.m_media_lib_setting_data.media_folders)
    {
        CAudioCommon::GetAudioFiles(item, all_media_files, 50000, true);
    }

    //std::unordered_map<wstring, SongInfo> new_songs_map;
    for (const auto& file_path : all_media_files)
    {
        if (!CSongDataManager::GetInstance().IsItemExist(file_path) || (refresh && IsSongNewer(file_path)))       //如果还没有获取到该歌曲的信息，或者文件的最后修改时间比上次获取到的新，则在这里获取
        {
            SongInfo song_info = CSongDataManager::GetInstance().GetSongInfo(file_path);
            song_info.file_path = file_path;
            IPlayerCore* pPlayerCore = CPlayer::GetInstance().GetPlayerCore();
            if (pPlayerCore == nullptr)
                break;
            pPlayerCore->GetAudioInfo(file_path.c_str(), song_info);
            if (!song_info.lengh.isZero() || CFilePathHelper(file_path).GetFileExtension() == L"cue")
            {
                CAudioTag audio_tag(song_info);
                audio_tag.GetAudioRating();
                CSongDataManager::GetInstance().AddItem(file_path, song_info);
                theApp.m_media_num_added++;
            }
        }
    }

    //for (const auto& item : new_songs_map)
    //{
    //    theApp.m_song_data[item.first] = item.second;
    //}

    if (theApp.m_media_num_added > 0)
        CSongDataManager::GetInstance().SetSongDataModified();
    return theApp.m_media_num_added;
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
        if (!CCommon::FolderExist(recent_folders[i].path))
        {
            recent_folders.erase(recent_folders.begin() + i);		//删除不存在的路径
            i--;
            cleard_cnt++;
        }
    }
    return cleard_cnt;
}

bool CMusicPlayerCmdHelper::Rename(SongInfo& song, const wstring& new_name)
{
    CPlayer::ReOpen reopen(song.IsSameSong(CPlayer::GetInstance().GetCurrentSongInfo()));
    wstring new_file_path = CCommon::FileRename(song.file_path, new_name);
    if (new_file_path.empty())
        return false;
    CSongDataManager::GetInstance().ChangeFilePath(song.file_path, new_file_path);
    song.file_path = new_file_path;
    return true;
}

void CMusicPlayerCmdHelper::ShowMediaLib(int cur_tab /*= -1*/, int tab_force_show)
{
    CMusicPlayerDlg* pPlayerDlg = dynamic_cast<CMusicPlayerDlg*>(theApp.m_pMainWnd);
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
            tab_index = CPlayer::GetInstance().IsPlaylistMode() ? 1 : 0;
        pPlayerDlg->m_pMediaLibDlg = new CMediaLibDlg(tab_index);
        pPlayerDlg->m_pMediaLibDlg->SetTabForceShow(tab_force_show);
        pPlayerDlg->m_pMediaLibDlg->Create(IDD_MEDIA_LIB_DIALOG/*, GetDesktopWindow()*/);
        pPlayerDlg->m_pMediaLibDlg->ShowWindow(SW_SHOW);
    }
}

void CMusicPlayerCmdHelper::RefreshMediaTabData(enum eMediaLibTab tab_index)
{
    CMusicPlayerDlg* pPlayerDlg = dynamic_cast<CMusicPlayerDlg*>(theApp.m_pMainWnd);
    if (pPlayerDlg != nullptr && pPlayerDlg->m_pMediaLibDlg != nullptr && IsWindow(pPlayerDlg->m_pMediaLibDlg->GetSafeHwnd()))
    {
        if (tab_index == ML_FOLDER)
            pPlayerDlg->m_pMediaLibDlg->m_path_dlg.RefreshTabData();         // 刷新媒体库文件夹列表
        else if(tab_index == ML_PLAYLIST)
            pPlayerDlg->m_pMediaLibDlg->m_playlist_dlg.RefreshSongList();    // 刷新媒体库播放列表列表
    }
}

void CMusicPlayerCmdHelper::OnViewArtist(const SongInfo& song_info)
{
    vector<wstring> artist_list;
    song_info.GetArtistList(artist_list);     //获取艺术家（可能有多个）
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
        dlg.SetTitle(CCommon::LoadText(IDS_SELECT_ARTIST));
        dlg.SetDlgIcon(theApp.m_icon_set.artist.GetIcon());
        if (dlg.DoModal() == IDOK)
            artist = dlg.GetSelectedItem();
        else
            return;
    }
    ShowMediaLib(CMusicPlayerCmdHelper::ML_ARTIST, MLDI_ARTIST);
    CMusicPlayerDlg* pPlayerDlg = dynamic_cast<CMusicPlayerDlg*>(theApp.m_pMainWnd);
    if (!pPlayerDlg->m_pMediaLibDlg->NavigateToItem(artist))
    {
        pPlayerDlg->MessageBox(CCommon::LoadTextFormat(IDS_CONNOT_FIND_ARTIST_WARNING, { artist }), NULL, MB_OK | MB_ICONWARNING);
    }
}

void CMusicPlayerCmdHelper::OnViewAlbum(const SongInfo& song_info)
{
    wstring album = song_info.GetAlbum();
    ShowMediaLib(CMusicPlayerCmdHelper::ML_ALBUM, MLDI_ALBUM);
    CMusicPlayerDlg* pPlayerDlg = dynamic_cast<CMusicPlayerDlg*>(theApp.m_pMainWnd);
    if (!pPlayerDlg->m_pMediaLibDlg->NavigateToItem(album))
    {
        pPlayerDlg->MessageBox(CCommon::LoadTextFormat(IDS_CONNOT_FIND_ALBUM_WARNING, { album }), NULL, MB_OK | MB_ICONWARNING);
    }
}

bool CMusicPlayerCmdHelper::AddToPlaylist(const std::vector<SongInfo>& songs, const std::wstring& playlist_path)
{
    if (CPlayer::GetInstance().IsPlaylistMode() && playlist_path == CPlayer::GetInstance().GetPlaylistPath())
    {
        std::vector<wstring> files;
        for (const auto& song : songs)
            files.push_back(song.file_path);
        return CPlayer::GetInstance().AddFiles(files, theApp.m_media_lib_setting_data.ignore_songs_already_in_playlist);
    }
    else
    {
        CPlaylistFile playlist;
        playlist.LoadFromFile(playlist_path);
        if (playlist.AddFiles(songs, theApp.m_media_lib_setting_data.ignore_songs_already_in_playlist))
        {
            playlist.SaveToFile(playlist_path);
            return true;
        }
        return false;
    }
}

bool CMusicPlayerCmdHelper::IsSongNewer(const std::wstring& file_path)
{
    if (!CCommon::FileExist(file_path))
        return false;

    SongInfo& song_info = CSongDataManager::GetInstance().GetSongInfoRef(file_path);
    unsigned __int64 last_modified = CCommon::GetFileLastModified(file_path);
    bool is_newer = (last_modified > song_info.modified_time);
    song_info.modified_time = last_modified;
    return is_newer;
}

CWnd* CMusicPlayerCmdHelper::GetOwner()
{
    if (m_pOwner != nullptr)
        return m_pOwner;
    else
        return theApp.m_pMainWnd;
}
