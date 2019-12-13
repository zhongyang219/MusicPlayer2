// FolderExploreDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "FolderExploreDlg.h"
#include "afxdialogex.h"
#include "AudioCommon.h"


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
    for (int index : m_right_selected_items)
    {
        wstring file_path = m_song_list_ctrl.GetItemText(index, COL_PATH).GetString();
        song_list.push_back(file_path);
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
}

void CFolderExploreDlg::ShowFolderTree()
{
    m_folder_explore_tree.DeleteAllItems();
    wstring default_folder = CCommon::GetSpecialDir(CSIDL_MYMUSIC);
    m_folder_explore_tree.InsertPath(default_folder.c_str(), NULL);
}

void CFolderExploreDlg::ShowSongList(bool size_changed)
{
    CWaitCursor wait_cursor;
    
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
            song.file_path = file;
            song_list.push_back(song);
        }
        else
        {
            song.file_path = file;
            song_list.push_back(song);
        }
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
END_MESSAGE_MAP()


// CFolderExploreDlg 消息处理程序


BOOL CFolderExploreDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
    ShowFolderTree();

    //初始化右侧列表
    m_song_list_ctrl.SetExtendedStyle(m_song_list_ctrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
    //CRect rc_song_list;
    //m_song_list_ctrl.GetWindowRect(rc_song_list);
    m_song_list_ctrl.InsertColumn(0, CCommon::LoadText(IDS_FILE_NAME), LVCFMT_LEFT, theApp.DPI(200));
    m_song_list_ctrl.InsertColumn(1, CCommon::LoadText(IDS_TITLE), LVCFMT_LEFT, theApp.DPI(150));
    m_song_list_ctrl.InsertColumn(2, CCommon::LoadText(IDS_ARTIST), LVCFMT_LEFT, theApp.DPI(100));
    m_song_list_ctrl.InsertColumn(3, CCommon::LoadText(IDS_ALBUM), LVCFMT_LEFT, theApp.DPI(100));
    m_song_list_ctrl.InsertColumn(4, CCommon::LoadText(IDS_FILE_PATH), LVCFMT_LEFT, theApp.DPI(600));

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

    //弹出右键菜单
    CMenu* pMenu = theApp.m_menu_set.m_media_lib_popup_menu.GetSubMenu(1);
    ASSERT(pMenu != nullptr);
    if (pMenu != nullptr)
    {
        m_song_list_ctrl.ShowPopupMenu(pMenu, pNMItemActivate->iItem, this);
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

    CTabDlg::OnOK();
}
