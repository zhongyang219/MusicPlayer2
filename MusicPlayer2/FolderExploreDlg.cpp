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
#include "AddToPlaylistDlg.h"
#include "SongDataManager.h"


// CFolderExploreDlg 对话框

IMPLEMENT_DYNAMIC(CFolderExploreDlg, CMediaLibTabDlg)

CFolderExploreDlg::CFolderExploreDlg(CWnd* pParent /*=nullptr*/)
	: CMediaLibTabDlg(IDD_FOLDER_EXPLORE_DIALOG, pParent)
{

}

CFolderExploreDlg::~CFolderExploreDlg()
{
}

void CFolderExploreDlg::GetSongsSelected(std::vector<wstring>& song_list) const
{
    if (m_left_selected)
    {
        CAudioCommon::GetAudioFiles(wstring(m_folder_path_selected), song_list, 20000, true);
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
        SongInfo song = CSongDataManager::GetInstance().GetSongInfo(file);
        song_list.push_back(song);
    }
}

void CFolderExploreDlg::GetCurrentSongList(std::vector<SongInfo>& song_list) const
{
    for (int index = 0; index < m_song_list_ctrl.GetItemCount(); index++)
    {
        std::wstring file = m_song_list_ctrl.GetItemText(index, COL_PATH);
        SongInfo song = CSongDataManager::GetInstance().GetSongInfo(file);
        song_list.push_back(song);

    }
}

void CFolderExploreDlg::RefreshData()
{
	ShowFolderTree();
}

void CFolderExploreDlg::RefreshSongList()
{
    ShowSongList();
}

void CFolderExploreDlg::ShowFolderTree()
{
    m_initialized = true;
    CString search_key_word;
    m_search_edit.GetWindowText(search_key_word);
    m_searched = !search_key_word.IsEmpty();
    if (m_searched)     //要进入搜索状态前，先保存当前树的展开收缩状态
        m_folder_explore_tree.SaveExpandState();

    m_folder_explore_tree.DeleteAllItems();
    //wstring default_folder = CCommon::GetSpecialDir(CSIDL_MYMUSIC);
    for(const auto& default_folder : theApp.m_media_lib_setting_data.media_folders)
    {
        m_folder_explore_tree.InsertPath(default_folder.c_str(), NULL, [&](const CString& folder_path)
        {
            if (!CAudioCommon::IsPathContainsAudioFile(wstring(folder_path), true))       //排除不包含音频文件的目录
                return false;
            if (m_searched)
                return CCommon::IsFolderMatchKeyWord(wstring(folder_path), wstring(search_key_word));
            return true;
        });
    }

    if (m_searched)
        m_folder_explore_tree.ExpandAll();
    else
        m_folder_explore_tree.RestoreExpandState();     //不是搜索状态，树刷新后恢复展开收缩状态
}

void CFolderExploreDlg::ShowSongList()
{
    CWaitCursor wait_cursor;
    SetDlgItemText(IDC_PATH_STATIC, m_folder_path_selected);

    //获取歌曲列表
    std::vector<wstring> files;
    CAudioCommon::GetAudioFiles(wstring(m_folder_path_selected), files, 20000, false);
    std::vector<SongInfo> song_list;
    for (const auto& file : files)
    {
        SongInfo song = CSongDataManager::GetInstance().GetSongInfo(file);
        song_list.push_back(song);
    }

    //显示到列表控件中
	m_list_data.clear();
    for (const auto& item : song_list)
    {
        CListCtrlEx::RowData row_data;
        row_data[COL_FILE_NAME] = item.GetFileName();
        row_data[COL_TITLE] = item.GetTitle();
        row_data[COL_ARTIST] = item.GetArtist();
        row_data[COL_ALBUM] = item.GetAlbum();
        row_data[COL_PATH] = item.file_path;
		m_list_data.push_back(std::move(row_data));
    }
    m_song_list_ctrl.SetListData(&m_list_data);
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
    m_right_selected_item = -1;         // 点击左侧列表时清空右侧列表选中项
    m_right_selected_items.clear();
    m_song_list_ctrl.SelectNone();
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
    if (!m_initialized)
    {
        CWaitCursor wait_cursor;
        //注意，在这里向左侧树填充数据可能会比较缓慢，因此放到这里处理，而不在OnInitDialog()中处理，
        //即只有当标签切换到“文件夹浏览”时才填充数据，以加快“媒体库”对话框的打开速度
        ShowFolderTree();
    }
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
            SongInfo song = CSongDataManager::GetInstance().GetSongInfo(file_path);
            CMusicPlayerCmdHelper cmd_helper(pThis);
            cmd_helper.VeiwOnline(song);
        }
    }
    return 0;

}

const CListCtrlEx & CFolderExploreDlg::GetSongListCtrl() const
{
	return m_song_list_ctrl;
}

int CFolderExploreDlg::GetItemSelected() const
{
	return m_right_selected_item;
}

const vector<int>& CFolderExploreDlg::GetItemsSelected() const
{
	return m_right_selected_items;
}

void CFolderExploreDlg::AfterDeleteFromDisk(const std::vector<SongInfo>& files)
{
	//删除成功，则刷新列表
	ShowSongList();
}

int CFolderExploreDlg::GetPathColIndex() const
{
	return COL_PATH;
}

wstring CFolderExploreDlg::GetSelectedString() const
{
	return wstring(m_selected_string);
}

void CFolderExploreDlg::DoDataExchange(CDataExchange* pDX)
{
    CMediaLibTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_MFCEDITBROWSE1, m_search_edit);
    DDX_Control(pDX, IDC_FOLDER_EXPLORE_TREE, m_folder_explore_tree);
    DDX_Control(pDX, IDC_SONG_LIST, m_song_list_ctrl);
}


BEGIN_MESSAGE_MAP(CFolderExploreDlg, CMediaLibTabDlg)
    ON_NOTIFY(NM_RCLICK, IDC_FOLDER_EXPLORE_TREE, &CFolderExploreDlg::OnNMRClickFolderExploreTree)
    ON_NOTIFY(NM_CLICK, IDC_FOLDER_EXPLORE_TREE, &CFolderExploreDlg::OnNMClickFolderExploreTree)
    ON_NOTIFY(NM_CLICK, IDC_SONG_LIST, &CFolderExploreDlg::OnNMClickSongList)
    ON_NOTIFY(NM_RCLICK, IDC_SONG_LIST, &CFolderExploreDlg::OnNMRClickSongList)
    ON_NOTIFY(NM_DBLCLK, IDC_FOLDER_EXPLORE_TREE, &CFolderExploreDlg::OnNMDblclkFolderExploreTree)
    ON_NOTIFY(NM_DBLCLK, IDC_SONG_LIST, &CFolderExploreDlg::OnNMDblclkSongList)
    ON_EN_CHANGE(IDC_MFCEDITBROWSE1, &CFolderExploreDlg::OnEnChangeMfceditbrowse1)
    ON_MESSAGE(WM_SEARCH_EDIT_BTN_CLICKED, &CFolderExploreDlg::OnSearchEditBtnClicked)
    ON_COMMAND(ID_DELETE_FROM_DISK, &CFolderExploreDlg::OnDeleteFromDisk)
END_MESSAGE_MAP()


// CFolderExploreDlg 消息处理程序


BOOL CFolderExploreDlg::OnInitDialog()
{
    CMediaLibTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
    CCommon::SetDialogFont(this, theApp.m_pMainWnd->GetFont());     //由于此对话框资源由不同语言共用，所以这里要设置一下字体
    
    //为树控件设置图标
    CImageList image_list;
    image_list.Create(theApp.DPI(16), theApp.DPI(16), ILC_COLOR32 | ILC_MASK, 2, 2);
    image_list.Add(CDrawCommon::LoadIconResource(IDI_EXPLORE_FOLDER, theApp.DPI(16), theApp.DPI(16)));
    m_folder_explore_tree.SetImageList(&image_list, TVSIL_NORMAL);
    image_list.Detach();

    //设置行高
    m_folder_explore_tree.SetItemHeight(theApp.DPI(22));

    if(!theApp.m_media_lib_setting_data.show_tree_tool_tips)
        m_folder_explore_tree.ModifyStyle(0, TVS_NOTOOLTIPS);

    //填充数据
    //注意，在这里向左侧树填充数据可能会比较缓慢，因此放到OnTabEntered()函数中处理，
    //即只有当标签切换到“文件夹浏览”时才填充数据，以加载“媒体库”对话框的打开速度
    //ShowFolderTree();

    ////
    //CImageList image_list1;
    //image_list1.Create(theApp.DPI(16), theApp.DPI(16), ILC_COLOR32 | ILC_MASK, 2, 2);
    //image_list1.Add(theApp.LoadIcon(IDI_FILE_ICON));
    //m_song_list_ctrl.SetImageList(&image_list1, LVSIL_SMALL);

    //初始化右侧列表
    m_song_list_ctrl.SetExtendedStyle(m_song_list_ctrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
    //CRect rc_song_list;
    //m_song_list_ctrl.GetWindowRect(rc_song_list);
    m_song_list_ctrl.InsertColumn(0, CCommon::LoadText(IDS_FILE_NAME), LVCFMT_LEFT, theApp.DPI(200));
    m_song_list_ctrl.InsertColumn(1, CCommon::LoadText(IDS_TITLE), LVCFMT_LEFT, theApp.DPI(180));
    m_song_list_ctrl.InsertColumn(2, CCommon::LoadText(IDS_ARTIST), LVCFMT_LEFT, theApp.DPI(100));
    m_song_list_ctrl.InsertColumn(3, CCommon::LoadText(IDS_ALBUM), LVCFMT_LEFT, theApp.DPI(100));
    m_song_list_ctrl.InsertColumn(4, CCommon::LoadText(IDS_FILE_PATH), LVCFMT_LEFT, theApp.DPI(600));
	m_song_list_ctrl.SetCtrlAEnable(true);

    m_search_edit.SetCueBanner(CCommon::LoadText(IDS_SEARCH_FORDER), TRUE);
    //SetDlgItemText(IDC_SETTINGS_BUTTON, CCommon::LoadText(IDS_BTN_SETTINGS));

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

            GetCursorPos(&point);
            if(hItem != NULL)
            {
                CRect item_rect;
                m_folder_explore_tree.GetItemRect(hItem, item_rect, FALSE);
                CRect window_rect;
                m_folder_explore_tree.GetWindowRect(window_rect);		//获取列表控件的矩形区域（以屏幕左上角为原点）
                point.y = window_rect.top + item_rect.bottom;	//设置鼠标要弹出的y坐标为选中项目的下边框位置，防止右键菜单挡住选中的项目
            }
            CMenu* pMenu = theApp.m_menu_set.m_media_lib_popup_menu.GetSubMenu(0);
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


void CFolderExploreDlg::OnNMDblclkFolderExploreTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: 在此添加控件通知处理程序代码
    CPoint point(GetMessagePos());
    unsigned int nFlags = 0;
    m_folder_explore_tree.ScreenToClient(&point);
    HTREEITEM hItem = m_folder_explore_tree.HitTest(point, &nFlags);
    if ((nFlags & TVHT_ONITEM) && (hItem != NULL))
    {
        OnOK();
    }

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
    // 发送此通知，除非重写 CMediaLibTabDlg::OnInitDialog()
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
    CMediaLibTabDlg::OnInitMenu(pMenu);

    // TODO: 在此处添加消息处理程序代码
    pMenu->SetDefaultItem(ID_PLAY_ITEM);
}


//void CFolderExploreDlg::OnDeleteFromDisk()
//{
//    // TODO: 在此添加命令处理程序代码
//    vector<SongInfo> songs_selected;
//    GetSongsSelected(songs_selected);
//    CMusicPlayerCmdHelper helper;
//    if (helper.DeleteSongsFromDisk(songs_selected))
//    {
//        //删除成功，则刷新列表
//        ShowSongList();
//    }
//}


void CFolderExploreDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	if (m_left_selected)        //选中左侧树时，播放选中文件夹
	{
		wstring folder_path = m_folder_explore_tree.GetItemPath(m_tree_item_selected);
		CPlayer::GetInstance().OpenFolder(folder_path, true, true);
		CTabDlg::OnOK();
		CWnd* pParent = GetParentWindow();
		if (pParent != nullptr)
			::SendMessage(pParent->GetSafeHwnd(), WM_COMMAND, IDOK, 0);
	}
	else
	{
		CMediaLibTabDlg::OnOK();
	}
}
