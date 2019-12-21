#include "stdafx.h"
#include "MusicPlayerCmdHelper.h"
#include "InternetCommon.h"
#include "Player.h"
#include "MusicPlayer2.h"
#include "MusicPlayerDlg.h"
#include "InputDlg.h"
#include "Playlist.h"
#include "AddToPlaylistDlg.h"

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

CWnd* CMusicPlayerCmdHelper::GetOwner()
{
    if (m_pOwner != nullptr)
        return m_pOwner;
    else
        return theApp.m_pMainWnd;
}
