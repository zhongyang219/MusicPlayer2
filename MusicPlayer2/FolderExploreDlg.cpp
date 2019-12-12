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
    CString folder_path_selected = m_folder_explore_tree.GetItemPath(hItem);
    if (folder_path_selected != m_folder_path_selected)
    {
        m_folder_path_selected = folder_path_selected;
        ShowSongList();
    }
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
        if ((nFlags & TVHT_ONITEM) && (hItem != NULL))
        {
            FolderTreeClicked(hItem);
        }

        //m_folder_explore_tree.SetFocus();
        //m_folder_explore_tree.SelectItem(hItem);
    }
    *pResult = 0;
}
