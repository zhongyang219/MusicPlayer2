#include "stdafx.h"
#include "MediaLibTabDlg.h"
#include "MusicPlayer2.h"
#include "MusicPlayerCmdHelper.h"
#include "PropertyDlg.h"
#include "SongDataManager.h"


IMPLEMENT_DYNAMIC(CMediaLibTabDlg, CTabDlg)

CMediaLibTabDlg::CMediaLibTabDlg(UINT nIDTemplate, CWnd *pParent)
	: CTabDlg(nIDTemplate, pParent)
{
}


CMediaLibTabDlg::~CMediaLibTabDlg()
{
}

BEGIN_MESSAGE_MAP(CMediaLibTabDlg, CTabDlg)
	ON_WM_INITMENU()
	ON_COMMAND(ID_PLAY_ITEM, &CMediaLibTabDlg::OnPlayItem)
	ON_COMMAND(ID_PLAY_ITEM_IN_FOLDER_MODE, &CMediaLibTabDlg::OnPlayItemInFolderMode)
	ON_COMMAND(ID_ADD_TO_NEW_PLAYLIST, &CMediaLibTabDlg::OnAddToNewPlaylist)
	ON_COMMAND(ID_ADD_TO_NEW_PALYLIST_AND_PLAY, &CMediaLibTabDlg::OnAddToNewPalylistAndPlay)
	ON_COMMAND(ID_EXPLORE_ONLINE, &CMediaLibTabDlg::OnExploreOnline)
	ON_COMMAND(ID_EXPLORE_TRACK, &CMediaLibTabDlg::OnExploreTrack)
	ON_COMMAND(ID_FORMAT_CONVERT, &CMediaLibTabDlg::OnFormatConvert)
	ON_COMMAND(ID_DELETE_FROM_DISK, &CMediaLibTabDlg::OnDeleteFromDisk)
	ON_COMMAND(ID_ITEM_PROPERTY, &CMediaLibTabDlg::OnItemProperty)
	ON_COMMAND(ID_COPY_TEXT, &CMediaLibTabDlg::OnCopyText)
END_MESSAGE_MAP()


void CMediaLibTabDlg::GetSongsSelected(std::vector<SongInfo>& song_list) const
{
	song_list.clear();
	std::vector<wstring> file_list;
	GetSongsSelected(file_list);
	for (const auto& file : file_list)
		song_list.push_back(CSongDataManager::GetInstance().GetSongInfo(file));
}

void CMediaLibTabDlg::GetSongsSelected(std::vector<wstring>& song_list) const
{
	song_list.clear();
	for (int index : GetItemsSelected())
	{
		if (index < 0 || index >= GetSongListCtrl().GetItemCount())
			continue;
		wstring file_path = GetSongListCtrl().GetItemText(index, GetPathColIndex());
		song_list.push_back(file_path);
	}
}

void CMediaLibTabDlg::GetCurrentSongList(std::vector<SongInfo>& song_list) const
{
	song_list.clear();
	std::vector<wstring> file_list;
	GetCurrentSongList(file_list);
	for (const auto& file : file_list)
		song_list.push_back(CSongDataManager::GetInstance().GetSongInfo(file));
}


void CMediaLibTabDlg::GetCurrentSongList(std::vector<wstring>& song_list) const
{
	song_list.clear();
	for (int index{}; index < GetSongListCtrl().GetItemCount(); index++)
	{
		wstring file_path = GetSongListCtrl().GetItemText(index, GetPathColIndex()).GetString();
		song_list.push_back(file_path);
	}
}

bool CMediaLibTabDlg::_OnAddToNewPlaylist(std::wstring& playlist_path)
{
	auto getSongList = [&](std::vector<SongInfo>& song_list)
	{
		GetSongsSelected(song_list);
	};
	CMusicPlayerCmdHelper cmd_helper(this);
	return cmd_helper.OnAddToNewPlaylist(getSongList, playlist_path);
}

UINT CMediaLibTabDlg::ViewOnlineThreadFunc(LPVOID lpParam)
{
	CMediaLibTabDlg* pThis = (CMediaLibTabDlg*)(lpParam);
	if (pThis == nullptr)
		return 0;
	CCommon::SetThreadLanguage(theApp.m_general_setting_data.language);
	//此命令用于跳转到歌曲对应的网易云音乐的在线页面
	if (pThis->GetItemSelected() >= 0)
	{
		wstring file_path = pThis->GetSongListCtrl().GetItemText(pThis->GetItemSelected(), pThis->GetPathColIndex()).GetString();
		if (CCommon::FileExist(file_path))
		{
			SongInfo song{ CSongDataManager::GetInstance().GetSongInfo(file_path) };
			CMusicPlayerCmdHelper cmd_helper(pThis);
			cmd_helper.VeiwOnline(song);
		}
	}
	return 0;
}

void CMediaLibTabDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	std::vector<wstring> files;
	GetSongsSelected(files);
	if (!files.empty())
	{
        if (files.size() > 1 || CFilePathHelper(files[0]).GetFileExtension() == L"cue")
        {
            CPlayer::GetInstance().OpenFilesInTempPlaylist(files);
        }
        else
		{
			std::vector<wstring> all_song_list;
			GetCurrentSongList(all_song_list);
			CPlayer::GetInstance().OpenFilesInTempPlaylist(all_song_list, GetItemSelected());
		}

		CTabDlg::OnOK();
		CWnd* pParent = GetParentWindow();
		if (pParent != nullptr)
			::SendMessage(pParent->GetSafeHwnd(), WM_COMMAND, IDOK, 0);
	}
}


void CMediaLibTabDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	CTabDlg::OnCancel();

	CWnd* pParent = GetParentWindow();
	if (pParent != nullptr)
		::SendMessage(pParent->GetSafeHwnd(), WM_COMMAND, IDCANCEL, 0);
}


BOOL CMediaLibTabDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类

	WORD command = LOWORD(wParam);
	auto getSelectedItems = [&](std::vector<SongInfo>& item_list)
	{
		GetSongsSelected(item_list);
	};
	//响应播放列表右键菜单中的“添加到播放列表”
	CMusicPlayerCmdHelper cmd_helper;
	cmd_helper.OnAddToPlaylistCommand(getSelectedItems, command);

	return CTabDlg::OnCommand(wParam, lParam);
}

void CMediaLibTabDlg::OnInitMenu(CMenu* pMenu)
{
	CTabDlg::OnInitMenu(pMenu);

	// TODO: 在此处添加消息处理程序代码
	pMenu->SetDefaultItem(ID_PLAY_ITEM);
    pMenu->EnableMenuItem(ID_DELETE_FROM_DISK, MF_BYCOMMAND | (theApp.m_media_lib_setting_data.disable_delete_from_disk ? MF_GRAYED : MF_ENABLED));
}


void CMediaLibTabDlg::OnPlayItem()
{
	// TODO: 在此添加命令处理程序代码
	OnOK();
}


void CMediaLibTabDlg::OnPlayItemInFolderMode()
{
	// TODO: 在此添加命令处理程序代码
	if (GetItemSelected() >= 0)
	{
		std::wstring file_path = GetSongListCtrl().GetItemText(GetItemSelected(), GetPathColIndex());
		CPlayer::GetInstance().OpenAFile(file_path, true);

		CTabDlg::OnOK();
		CWnd* pParent = GetParentWindow();
		if (pParent != nullptr)
			::SendMessage(pParent->GetSafeHwnd(), WM_COMMAND, IDOK, 0);
	}
}


void CMediaLibTabDlg::OnAddToNewPlaylist()
{
	// TODO: 在此添加命令处理程序代码
	wstring playlist_path;
	_OnAddToNewPlaylist(playlist_path);
}


void CMediaLibTabDlg::OnAddToNewPalylistAndPlay()
{
	// TODO: 在此添加命令处理程序代码
	wstring playlist_path;
	if (_OnAddToNewPlaylist(playlist_path))
	{
		CPlayer::GetInstance().SetPlaylist(playlist_path, 0, 0, false, true);
		CPlayer::GetInstance().SaveRecentPath();
		OnCancel();
	}
}


void CMediaLibTabDlg::OnExploreOnline()
{
	// TODO: 在此添加命令处理程序代码
	AfxBeginThread(ViewOnlineThreadFunc, (void*)this);
}


void CMediaLibTabDlg::OnExploreTrack()
{
	// TODO: 在此添加命令处理程序代码
	if (GetItemSelected() < 0)
		return;
	wstring file_path = GetSongListCtrl().GetItemText(GetItemSelected(), GetPathColIndex()).GetString();
	if (!file_path.empty())
	{
		CString str;
		str.Format(_T("/select,\"%s\""), file_path.c_str());
		ShellExecute(NULL, _T("open"), _T("explorer"), str, NULL, SW_SHOWNORMAL);
	}
}


void CMediaLibTabDlg::OnFormatConvert()
{
	// TODO: 在此添加命令处理程序代码
	std::vector<SongInfo> songs;
	GetSongsSelected(songs);
	CMusicPlayerCmdHelper cmd_helper(this);
	cmd_helper.FormatConvert(songs);
}


void CMediaLibTabDlg::OnDeleteFromDisk()
{
	// TODO: 在此添加命令处理程序代码
	vector<SongInfo> songs_selected;
	GetSongsSelected(songs_selected);
	CMusicPlayerCmdHelper helper;
	if (helper.DeleteSongsFromDisk(songs_selected))
	{
		AfterDeleteFromDisk(songs_selected);
	}
}


void CMediaLibTabDlg::OnItemProperty()
{
	// TODO: 在此添加命令处理程序代码
    if (GetItemSelected() < 0)
        return;
    CPropertyDlg* pDlg;
    std::vector<SongInfo> songs_selected;
    std::vector<SongInfo> songs;
    if (GetItemsSelected().size() > 1)        //批量编辑
    {
        GetSongsSelected(songs_selected);
        pDlg = new CPropertyDlg(songs_selected);
    }
    else
    {
        GetCurrentSongList(songs);
        pDlg = new CPropertyDlg(songs, GetItemSelected(), false);
    }
    pDlg->DoModal();
    if (pDlg->GetModified())
        RefreshSongList();
    SAFE_DELETE(pDlg);
}


void CMediaLibTabDlg::OnCopyText()
{
	// TODO: 在此添加命令处理程序代码
	if (!CCommon::CopyStringToClipboard(GetSelectedString()))
		MessageBox(CCommon::LoadText(IDS_COPY_CLIPBOARD_FAILED), NULL, MB_ICONWARNING);
}
