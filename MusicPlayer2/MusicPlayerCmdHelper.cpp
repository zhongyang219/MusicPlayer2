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
    if (song.song_id.empty())		//如果没有获取过ID，则获取一次ID
    {
        wstring song_id;
        song_id = CInternetCommon::SearchSongAndGetMatched(song.title, song.artist, song.album, song.GetFileName()).id;
        song.song_id = song_id;
        if (!song.is_cue)
        {
            theApp.SaveSongInfo(song);
        }
    }

    if (song.song_id.empty())
        return;
    //获取网易云音乐中该歌曲的在线接听网址
    wstring song_url{ L"http://music.163.com/#/song?id=" + song.song_id };

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

        CPlaylist playlist;
        playlist.LoadFromFile(playlist_path);
        playlist.AddFiles(selected_item_path);
        playlist.SaveToFile(playlist_path);
        theApp.m_pMainWnd->SendMessage(WM_INIT_ADD_TO_MENU);
        return true;
    }
    return false;
}

void CMusicPlayerCmdHelper::OnAddToPlaylistCommand(std::function<void(std::vector<SongInfo>&)> get_song_list, DWORD command)
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
                    CPlaylist playlist;
                    playlist.LoadFromFile(playlist_path);
                    playlist.AddFiles(selected_item_path);
                    playlist.SaveToFile(playlist_path);
                }
            }
        }
        else if (command == ID_ADD_TO_DEFAULT_PLAYLIST)      //添加到默认播放列表
        {
            std::wstring default_playlist_path = CPlayer::GetInstance().GetRecentPlaylist().m_default_playlist.path;
            CPlaylist playlist;
            playlist.LoadFromFile(default_playlist_path);
            playlist.AddFiles(selected_item_path);
            playlist.SaveToFile(default_playlist_path);

        }
        else if (command == ID_ADD_TO_MY_FAVOURITE)      //添加到“我喜欢”播放列表
        {
            std::wstring favourite_playlist_path = CPlayer::GetInstance().GetRecentPlaylist().m_favourite_playlist.path;
            CPlaylist playlist;
            playlist.LoadFromFile(favourite_playlist_path);
            playlist.AddFiles(selected_item_path);
            playlist.SaveToFile(favourite_playlist_path);

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
                    CPlaylist playlist;
                    playlist.LoadFromFile(playlist_path);
                    playlist.AddFiles(selected_item_path);
                    playlist.SaveToFile(playlist_path);
                }
                else
                {
                    GetOwner()->MessageBox(CCommon::LoadText(IDS_ADD_TO_PLAYLIST_FAILED_WARNING), NULL, MB_ICONWARNING | MB_OK);
                }
            }
        }
    }

}

bool CMusicPlayerCmdHelper::DeleteSongsFromDisk(const std::vector<SongInfo>& files)
{
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
            auto iter = theApp.m_song_data.find(file);
            if (iter != theApp.m_song_data.end())
            {
                theApp.m_song_data.erase(iter);
            }
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

void CMusicPlayerCmdHelper::SearchLyricFiles(const SongInfo & song, std::vector<std::wstring>& result, bool fuzzy_match)
{
	result.clear();
	if (song.GetFileName().size() < 3) return;
	
	CFilePathHelper lyric_path{ song.file_path };		//得到路径+文件名的字符串
	lyric_path.ReplaceFileExtension(L"lrc");		//将文件扩展替换成lrc
	CFilePathHelper lyric_path2{ theApp.m_lyric_setting_data.lyric_path + song.GetFileName() };
	lyric_path2.ReplaceFileExtension(L"lrc");
	//查找歌词文件名和歌曲文件名完全匹配的歌词
	if (CCommon::FileExist(lyric_path.GetFilePath()))
	{
		result.push_back(lyric_path.GetFilePath());
	}
	if (CCommon::FileExist(lyric_path2.GetFilePath()))
	{
		result.push_back(lyric_path2.GetFilePath());
	}
	if (fuzzy_match)
	{
		vector<wstring> current_path_lyrics;	//储存当前路径下的歌词文件的文件名
		vector<wstring> lyric_path_lyrics;		//储存歌词文件夹下的歌词文件的文件名
		CAudioCommon::GetLyricFiles(lyric_path.GetDir(), current_path_lyrics);
		CAudioCommon::GetLyricFiles(theApp.m_lyric_setting_data.lyric_path, lyric_path_lyrics);

		//寻找歌词文件中同时包含歌曲标题的歌词文件
		for (const auto& str : current_path_lyrics)	//在当前目录下寻找
		{
			//if (str.find(song.artist) != string::npos && str.find(song.title) != string::npos)
			if (CCommon::StringNatchWholeWord(str, song.title) != -1)
			{
				wstring matched_lyric = lyric_path.GetDir() + str;
				result.push_back(matched_lyric);
			}
		}

		for (const auto& str : lyric_path_lyrics)	//在歌词目录下寻找
		{
			//if (str.find(song.artist) != string::npos && str.find(song.title) != string::npos)
			if (CCommon::StringNatchWholeWord(str, song.title) != -1)
			{
				wstring matched_lyric = theApp.m_lyric_setting_data.lyric_path + str;
				result.push_back(matched_lyric);
			}
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
		//先寻找歌词文件中同时包含歌曲标题和艺术家的歌词文件
		for (const auto& str : current_path_lyrics)	//在当前目录下寻找
		{
			//if (str.find(song.artist) != string::npos && str.find(song.title) != string::npos)
			if (CCommon::StringNatchWholeWord(str, song.artist) != -1 && CCommon::StringNatchWholeWord(str, song.title) != -1)
			{
				matched_lyric = lyric_path.GetDir() + str;
				return matched_lyric;
			}
		}

		if (matched_lyric.empty())		//如果当前目录下没找到
		{
			for (const auto& str : lyric_path_lyrics)	//在歌词目录下寻找
			{
				//if (str.find(song.artist) != string::npos && str.find(song.title) != string::npos)
				if (CCommon::StringNatchWholeWord(str, song.artist) != -1 && CCommon::StringNatchWholeWord(str, song.title) != -1)
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

int CMusicPlayerCmdHelper::UpdateMediaLib()
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
        auto iter = theApp.m_song_data.find(file_path);
        if (iter == theApp.m_song_data.end())       //如果还没有获取到该歌曲的信息，则在这里获取
        {
            SongInfo song_info;
            CPlayer::GetInstance().GetPlayerCore()->GetAudioInfo(file_path.c_str(), song_info);
            theApp.m_song_data[file_path] = song_info;
            theApp.m_media_num_added++;
        }
    }

    //for (const auto& item : new_songs_map)
    //{
    //    theApp.m_song_data[item.first] = item.second;
    //}

    if(theApp.m_media_num_added > 0)
        theApp.SetSongDataModified();
    return theApp.m_media_num_added;
}

int CMusicPlayerCmdHelper::CleanUpSongData()
{
    int clear_cnt{};		//统计删除的项目的数量
    //遍历映射容器，删除不必要的条目。
    for (auto iter{ theApp.m_song_data.begin() }; iter != theApp.m_song_data.end();)
    {
        if (!CCommon::FileExist(iter->first))
        {
            iter = theApp.m_song_data.erase(iter);		//删除条目之后将迭代器指向被删除条目的前一个条目
            clear_cnt++;
        }
        else
        {
            iter++;
        }
    }
    return clear_cnt;
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

CWnd* CMusicPlayerCmdHelper::GetOwner()
{
    if (m_pOwner != nullptr)
        return m_pOwner;
    else
        return theApp.m_pMainWnd;
}
