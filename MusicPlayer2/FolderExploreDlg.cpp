// FolderExploreDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "FolderExploreDlg.h"
#include "afxdialogex.h"
#include "AudioCommon.h"
#include "MusicPlayerCmdHelper.h"
#include "PropertyDlg.h"
#include "Playlist.h"


// CFolderExploreDlg 对话框

IMPLEMENT_DYNAMIC(CFolderExploreDlg, CTabDlg)

CFolderExploreDlg::CFolderExploreDlg(CWnd* pParent /*=nullptr*/)
	: CTabDlg(IDD_FOLDER_EXPLORE_DIALOG, pParent)
{

}

CFolderExploreDlg::~CFolderExploreDlg()
{
}

void CFolderExploreDlg::GetSongsSelected(std::vector<wstring>& song_list) const
{
    if (m_left_selected)
    {
        CAudioCommon::GetAudioFiles(wstring(m_folder_path_selected), song_list);
    }
    else
    {
        for (int index : m_right_selected_items)
        {
            wstring file_path = m_song_list_ctrl.GetItemText(index, COL_PATH).GetString();
            song_list.push_back(file_path);
        }
    }
}

void CFolderExploreDlg::GetSongsSelected(std::vector<SongInfo>& song_list) const
{
    std::vector<wstring> file_list;
    GetSongsSelected(file_list);
    for (const auto& file : file_list)
    {
        SongInfo song;
        auto iter = theApp.m_song_data.find(file);
        if (iter != theApp.m_song_data.end())
        {
            song = iter->second;
            song.file_path = file;
            song_list.push_back(song);
        }
        else
        {
            song.file_path = file;
            song_list.push_back(song);
        }
    }
}

void CFolderExploreDlg::GetCurrentSongList(std::vector<SongInfo>& song_list) const
{
    for (int index = 0; index < m_song_list_ctrl.GetItemCount(); index++)
    {
        std::wstring file = m_song_list_ctrl.GetItemText(index, COL_PATH);
        SongInfo song;
        auto iter = theApp.m_song_data.find(file);
        if (iter != theApp.m_song_data.end())
        {
            song = iter->second;
        }
        song.file_path = file;
        song_list.push_back(song);

    }
}

void CFolderExploreDlg::ShowFolderTree()
{
    CString search_key_word;
    m_search_edit.GetWindowText(search_key_word);
    m_searched = !search_key_word.IsEmpty();
    if (m_searched)     //要进入搜索状态前，先保存当前树的展开收缩状态
        m_folder_explore_tree.SaveExpandState();

    m_folder_explore_tree.DeleteAllItems();
    wstring default_folder = CCommon::GetSpecialDir(CSIDL_MYMUSIC);
    m_folder_explore_tree.InsertPath(default_folder.c_str(), NULL, [&](const CString& folder_path)
    {
        if (!CAudioCommon::IsPathContainsAudioFile(wstring(folder_path), true))       //排除不包含音频文件的目录
            return false;
        if(m_searched)
            return CCommon::IsFolderMatchKeyWord(wstring(folder_path), wstring(search_key_word));
        return true;
    });

    if (m_searched)
        m_folder_explore_tree.ExpandAll();
    else
        m_folder_explore_tree.RestoreExpandState();     //不是搜索状态，树刷新后恢复展开收缩状态
}

void CFolderExploreDlg::ShowSongList(bool size_changed)
{
    CWaitCursor wait_cursor;
    SetDlgItemText(IDC_PATH_STATIC, m_folder_path_selected);
    std::vector<wstring> files;
    CAudioCommon::GetAudioFiles(wstring(m_folder_path_selected), files, 20000, false);
    std::vector<SongInfo> song_list;
    for (const auto& file : files)
    {
        SongInfo song;
        auto iter = theApp.m_song_data.find(file);
        if(iter != theApp.m_song_data.end())
        {
            song = iter->second;
        }
        song.file_path = file;
        song_list.push_back(song);
    }

    if (size_changed)
        m_song_list_ctrl.DeleteAllItems();

    int item_index = 0;
    for (const auto& item : song_list)
    {
        if (size_changed)
            m_song_list_ctrl.InsertItem(item_index, item.GetFileName().c_str());
        else
            m_song_list_ctrl.SetItemText(item_index, COL_FILE_NAME, item.GetFileName().c_str());
        m_song_list_ctrl.SetItemText(item_index, COL_TITLE, item.GetTitle().c_str());
        m_song_list_ctrl.SetItemText(item_index, COL_ARTIST, item.GetArtist().c_str());
        m_song_list_ctrl.SetItemText(item_index, COL_ALBUM, item.GetAlbum().c_str());
        //std::wstring track_str;
        //if (item.track != 0)
        //    track_str = std::to_wstring(item.track);
        //m_song_list_ctrl.SetItemText(item_index, COL_TRACK, track_str.c_str());
        //m_song_list_ctrl.SetItemText(item_index, COL_GENRE, item.GetGenre().c_str());
        m_song_list_ctrl.SetItemText(item_index, COL_PATH, item.file_path.c_str());
        item_index++;
    }
}

void CFolderExploreDlg::FolderTreeClicked(HTREEITEM hItem)
{
    m_left_selected = true;
    CString folder_path_selected = m_folder_explore_tree.GetItemPath(hItem);
    if (folder_path_selected != m_folder_path_selected)
    {
        m_folder_path_selected = folder_path_selected;
        ShowSongList();
    }
    SetButtonsEnable(CCommon::FolderExist(wstring(folder_path_selected)));
}

void CFolderExploreDlg::SongListClicked(int index)
{
    m_left_selected = false;
    m_right_selected_item = index;
    m_song_list_ctrl.GetItemSelected(m_right_selected_items);
    SetButtonsEnable(!m_right_selected_items.empty());
}

void CFolderExploreDlg::SetButtonsEnable(bool enable)
{
    CWnd* pParent = GetParentWindow();
    ::SendMessage(pParent->GetSafeHwnd(), WM_PLAY_SELECTED_BTN_ENABLE, WPARAM(enable), 0);
}

bool CFolderExploreDlg::_OnAddToNewPlaylist(std::wstring& playlist_path)
{
    std::wstring default_name;
    //如果选中了左侧列表，则添加到新建播放列表时名称自动填上选中项的名称
    default_name = m_folder_explore_tree.GetItemText(m_tree_item_selected);
    CCommon::FileNameNormalize(default_name);

    auto getSongList = [&](std::vector<SongInfo>& song_list)
    {
        GetSongsSelected(song_list);
    };
    CMusicPlayerCmdHelper cmd_helper(this);
    return cmd_helper.OnAddToNewPlaylist(getSongList, playlist_path, default_name);
}

void CFolderExploreDlg::OnTabEntered()
{
    bool play_enable;
    if (m_left_selected)
        play_enable = CCommon::FolderExist(wstring(m_folder_path_selected));
    else
        play_enable = (!m_right_selected_items.empty());
    SetButtonsEnable(play_enable);
}

UINT CFolderExploreDlg::ViewOnlineThreadFunc(LPVOID lpParam)
{
    CFolderExploreDlg* pThis = (CFolderExploreDlg*)(lpParam);
    if (pThis == nullptr)
        return 0;
    CCommon::SetThreadLanguage(theApp.m_general_setting_data.language);
    //此命令用于跳转到歌曲对应的网易云音乐的在线页面
    if (pThis->m_right_selected_item >= 0)
    {
        wstring file_path = pThis->m_song_list_ctrl.GetItemText(pThis->m_right_selected_item, COL_PATH).GetString();
        if (CCommon::FileExist(file_path))
        {
            SongInfo song{ theApp.m_song_data[file_path] };
            song.file_path = file_path;
            CMusicPlayerCmdHelper cmd_helper(pThis);
            cmd_helper.VeiwOnline(song);
        }
    }
    return 0;

}

void CFolderExploreDlg::DoDataExchange(CDataExchange* pDX)
{
    CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_MFCEDITBROWSE1, m_search_edit);
    DDX_Control(pDX, IDC_FOLDER_EXPLORE_TREE, m_folder_explore_tree);
    DDX_Control(pDX, IDC_SONG_LIST, m_song_list_ctrl);
}


BEGIN_MESSAGE_MAP(CFolderExploreDlg, CTabDlg)
    ON_NOTIFY(NM_RCLICK, IDC_FOLDER_EXPLORE_TREE, &CFolderExploreDlg::OnNMRClickFolderExploreTree)
    ON_NOTIFY(NM_CLICK, IDC_FOLDER_EXPLORE_TREE, &CFolderExploreDlg::OnNMClickFolderExploreTree)
    ON_COMMAND(ID_PLAY_ITEM, &CFolderExploreDlg::OnPlayItem)
    ON_NOTIFY(NM_CLICK, IDC_SONG_LIST, &CFolderExploreDlg::OnNMClickSongList)
    ON_NOTIFY(NM_RCLICK, IDC_SONG_LIST, &CFolderExploreDlg::OnNMRClickSongList)
    ON_COMMAND(ID_ADD_TO_NEW_PLAYLIST, &CFolderExploreDlg::OnAddToNewPlaylist)
    ON_COMMAND(ID_ADD_TO_NEW_PALYLIST_AND_PLAY, &CFolderExploreDlg::OnAddToNewPalylistAndPlay)
    ON_COMMAND(ID_PLAY_ITEM_IN_FOLDER_MODE, &CFolderExploreDlg::OnPlayItemInFolderMode)
    ON_COMMAND(ID_EXPLORE_ONLINE, &CFolderExploreDlg::OnExploreOnline)
    ON_COMMAND(ID_FORMAT_CONVERT, &CFolderExploreDlg::OnFormatConvert)
    ON_COMMAND(ID_EXPLORE_TRACK, &CFolderExploreDlg::OnExploreTrack)
    ON_COMMAND(ID_ITEM_PROPERTY, &CFolderExploreDlg::OnItemProperty)
    ON_COMMAND(ID_COPY_TEXT, &CFolderExploreDlg::OnCopyText)
    ON_NOTIFY(NM_DBLCLK, IDC_FOLDER_EXPLORE_TREE, &CFolderExploreDlg::OnNMDblclkFolderExploreTree)
    ON_NOTIFY(NM_DBLCLK, IDC_SONG_LIST, &CFolderExploreDlg::OnNMDblclkSongList)
    ON_EN_CHANGE(IDC_MFCEDITBROWSE1, &CFolderExploreDlg::OnEnChangeMfceditbrowse1)
    ON_MESSAGE(WM_SEARCH_EDIT_BTN_CLICKED, &CFolderExploreDlg::OnSearchEditBtnClicked)
    ON_WM_INITMENU()
END_MESSAGE_MAP()


// CFolderExploreDlg 消息处理程序


BOOL CFolderExploreDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
    CCommon::SetDialogFont(this, theApp.m_pMainWnd->GetFont());     //由于此对话框资源由不同语言共用，所以这里要设置一下字体
    
    ShowFolderTree();

    //初始化右侧列表
    m_song_list_ctrl.SetExtendedStyle(m_song_list_ctrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
    //CRect rc_song_list;
    //m_song_list_ctrl.GetWindowRect(rc_song_list);
    m_song_list_ctrl.InsertColumn(0, CCommon::LoadText(IDS_FILE_NAME), LVCFMT_LEFT, theApp.DPI(200));
    m_song_list_ctrl.InsertColumn(1, CCommon::LoadText(IDS_TITLE), LVCFMT_LEFT, theApp.DPI(180));
    m_song_list_ctrl.InsertColumn(2, CCommon::LoadText(IDS_ARTIST), LVCFMT_LEFT, theApp.DPI(100));
    m_song_list_ctrl.InsertColumn(3, CCommon::LoadText(IDS_ALBUM), LVCFMT_LEFT, theApp.DPI(100));
    m_song_list_ctrl.InsertColumn(4, CCommon::LoadText(IDS_FILE_PATH), LVCFMT_LEFT, theApp.DPI(600));

    m_search_edit.SetCueBanner(CCommon::LoadText(IDS_SEARCH_FORDER), TRUE);
    SetDlgItemText(IDC_SETTINGS_BUTTON, CCommon::LoadText(IDS_BTN_SETTINGS));

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CFolderExploreDlg::OnNMRClickFolderExploreTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: 在此添加控件通知处理程序代码
    if (pNMHDR->hwndFrom == m_folder_explore_tree.GetSafeHwnd())
    {
        CPoint point(GetMessagePos());
        unsigned int nFlags = 0;
        m_folder_explore_tree.ScreenToClient(&point);
        HTREEITEM hItem = m_folder_explore_tree.HitTest(point, &nFlags);
        m_tree_item_selected = hItem;
        m_selected_string = m_folder_explore_tree.GetItemText(hItem);

        m_folder_explore_tree.SetFocus();
        m_folder_explore_tree.SelectItem(hItem);
        if ((nFlags & TVHT_ONITEM || nFlags & TVHT_ONITEMRIGHT || nFlags & TVHT_ONITEMINDENT) && (hItem != NULL))
        {
            FolderTreeClicked(hItem);
            CMenu* pMenu = theApp.m_menu_set.m_media_lib_popup_menu.GetSubMenu(0);
            GetCursorPos(&point);
            pMenu->TrackPopupMenu(TPM_LEFTBUTTON | TPM_LEFTALIGN, point.x, point.y, this);
        }
    }

    *pResult = 0;
}


void CFolderExploreDlg::OnNMClickFolderExploreTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: 在此添加控件通知处理程序代码
    if (pNMHDR->hwndFrom == m_folder_explore_tree.GetSafeHwnd())
    {
        CPoint point(GetMessagePos());
        unsigned int nFlags = 0;
        m_folder_explore_tree.ScreenToClient(&point);
        HTREEITEM hItem = m_folder_explore_tree.HitTest(point, &nFlags);
        m_tree_item_selected = hItem;
        if ((nFlags & TVHT_ONITEM) && (hItem != NULL))
        {
            FolderTreeClicked(hItem);
        }

        //点击加减号时
        if ((nFlags & TVHT_ONITEMBUTTON) && (hItem != NULL))
        {
            bool expand = (m_folder_explore_tree.GetItemState(hItem, TVIS_EXPANDED)&TVIS_EXPANDED) == TVIS_EXPANDED;
            //保存当前点击节点的展开状态，由于这时item的状态还没有改变，所以要对expand取反
            m_folder_explore_tree.SaveItemExpandState(hItem, !expand);
        }

        //m_folder_explore_tree.SetFocus();
        //m_folder_explore_tree.SelectItem(hItem);
    }
    *pResult = 0;
}


void CFolderExploreDlg::OnPlayItem()
{
    // TODO: 在此添加命令处理程序代码
    OnOK();
}


void CFolderExploreDlg::OnNMClickSongList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    SongListClicked(pNMItemActivate->iItem);
    *pResult = 0;
}


void CFolderExploreDlg::OnNMRClickSongList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    SongListClicked(pNMItemActivate->iItem);
    m_selected_string = m_song_list_ctrl.GetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem);

    if(!m_right_selected_items.empty())
    {
        //弹出右键菜单
        CMenu* pMenu = theApp.m_menu_set.m_media_lib_popup_menu.GetSubMenu(1);
        ASSERT(pMenu != nullptr);
        if (pMenu != nullptr)
        {
            m_song_list_ctrl.ShowPopupMenu(pMenu, pNMItemActivate->iItem, this);
        }
    }
    *pResult = 0;
}


void CFolderExploreDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    if (m_left_selected)        //选中左侧树时，播放选中文件夹
    {
        wstring folder_path = m_folder_explore_tree.GetItemPath(m_tree_item_selected);
        CPlayer::GetInstance().OpenFolder(folder_path, true);
    }
    else
    {
        std::vector<wstring> files;
        GetSongsSelected(files);
        if (!files.empty())
        {
            CPlayer::GetInstance().OpenFilesInTempPlaylist(files);
        }
    }
    CTabDlg::OnOK();
    CWnd* pParent = GetParentWindow();
    if (pParent != nullptr)
        ::SendMessage(pParent->GetSafeHwnd(), WM_COMMAND, IDOK, 0);
}


void CFolderExploreDlg::OnAddToNewPlaylist()
{
    // TODO: 在此添加命令处理程序代码
    wstring playlist_path;
    _OnAddToNewPlaylist(playlist_path);
}


void CFolderExploreDlg::OnAddToNewPalylistAndPlay()
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


void CFolderExploreDlg::OnPlayItemInFolderMode()
{
    // TODO: 在此添加命令处理程序代码
    if (m_right_selected_item >= 0)
    {
        std::wstring file_path = m_song_list_ctrl.GetItemText(m_right_selected_item, COL_PATH);
        CPlayer::GetInstance().OpenAFile(file_path, true);

        CTabDlg::OnOK();
        CWnd* pParent = GetParentWindow();
        if (pParent != nullptr)
            ::SendMessage(pParent->GetSafeHwnd(), WM_COMMAND, IDOK, 0);
    }
}


void CFolderExploreDlg::OnExploreOnline()
{
    // TODO: 在此添加命令处理程序代码
    AfxBeginThread(ViewOnlineThreadFunc, (void*)this);
}


void CFolderExploreDlg::OnFormatConvert()
{
    // TODO: 在此添加命令处理程序代码
    std::vector<SongInfo> songs;
    GetSongsSelected(songs);
    CMusicPlayerCmdHelper cmd_helper(this);
    cmd_helper.FormatConvert(songs);
}


void CFolderExploreDlg::OnExploreTrack()
{
    // TODO: 在此添加命令处理程序代码
    if (m_right_selected_item < 0)
        return;
    wstring file_path = m_song_list_ctrl.GetItemText(m_right_selected_item, COL_PATH).GetString();
    if (!file_path.empty())
    {
        CString str;
        str.Format(_T("/select,\"%s\""), file_path.c_str());
        ShellExecute(NULL, _T("open"), _T("explorer"), str, NULL, SW_SHOWNORMAL);
    }
}


void CFolderExploreDlg::OnItemProperty()
{
    // TODO: 在此添加命令处理程序代码
    if (m_right_selected_item < 0)
        return;
    std::vector<SongInfo> songs;
    GetCurrentSongList(songs);
    CPropertyDlg propertyDlg(songs, this);
    propertyDlg.m_index = m_right_selected_item;
    propertyDlg.DoModal();
    if (propertyDlg.GetListRefresh())
        ShowSongList();
}


void CFolderExploreDlg::OnCopyText()
{
    // TODO: 在此添加命令处理程序代码
    if (!CCommon::CopyStringToClipboard(wstring(m_selected_string)))
        MessageBox(CCommon::LoadText(IDS_COPY_CLIPBOARD_FAILED), NULL, MB_ICONWARNING);
}


BOOL CFolderExploreDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    // TODO: 在此添加专用代码和/或调用基类
    WORD command = LOWORD(wParam);
    //响应播放列表右键菜单中的“添加到播放列表”
    if (command >= ID_ADD_TO_DEFAULT_PLAYLIST && command <= ID_ADD_TO_MY_FAVOURITE + ADD_TO_PLAYLIST_MAX_SIZE)
    {
        //获取选中的曲目的路径
        std::vector<SongInfo> selected_item_path;
        GetSongsSelected(selected_item_path);
        if (command == ID_ADD_TO_DEFAULT_PLAYLIST)      //添加到默认播放列表
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
                    MessageBox(CCommon::LoadText(IDS_ADD_TO_PLAYLIST_FAILED_WARNING), NULL, MB_ICONWARNING | MB_OK);
                }
            }
        }
    }

    return CTabDlg::OnCommand(wParam, lParam);
}


void CFolderExploreDlg::OnCancel()
{
    // TODO: 在此添加专用代码和/或调用基类

    CTabDlg::OnCancel();

    CWnd* pParent = GetParentWindow();
    if (pParent != nullptr)
        ::SendMessage(pParent->GetSafeHwnd(), WM_COMMAND, IDCANCEL, 0);
}


void CFolderExploreDlg::OnNMDblclkFolderExploreTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: 在此添加控件通知处理程序代码
    OnOK();
    *pResult = 0;
}


void CFolderExploreDlg::OnNMDblclkSongList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    OnOK();
    *pResult = 0;
}


void CFolderExploreDlg::OnEnChangeMfceditbrowse1()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CTabDlg::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    ShowFolderTree();
}


afx_msg LRESULT CFolderExploreDlg::OnSearchEditBtnClicked(WPARAM wParam, LPARAM lParam)
{
    if(m_searched)
    {
        m_search_edit.SetWindowText(_T(""));
    }
    return 0;
}


void CFolderExploreDlg::OnInitMenu(CMenu* pMenu)
{
    CTabDlg::OnInitMenu(pMenu);

    // TODO: 在此处添加消息处理程序代码
    pMenu->SetDefaultItem(ID_PLAY_ITEM);
}
