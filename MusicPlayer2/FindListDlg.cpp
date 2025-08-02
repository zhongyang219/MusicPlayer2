// FindListDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "afxdialogex.h"
#include "FindListDlg.h"
#include "UiMediaLibItemMgr.h"
#include "MusicPlayerCmdHelper.h"
#include "FolderPropertiesDlg.h"
#include "PlaylistPropertiesDlg.h"
#include "MediaLibItemPropertiesDlg.h"
#include "UIElement/FolderExploreTree.h"

// CFindListDlg 对话框

IMPLEMENT_DYNAMIC(CFindListDlg, CTabDlg)

CFindListDlg::CFindListDlg(CWnd* pParent /*=nullptr*/)
    : CTabDlg(IDD_FIND_LIST_DIALOG, pParent)
{

}

CFindListDlg::~CFindListDlg()
{
}

void CFindListDlg::InitListData()
{
    if (!m_initialized)
    {
        m_list_data.clear();
        //添加所有最近播放项目
        CListCache list_cache(CListCache::SubsetType::ST_ALL);
        list_cache.reload();
        AddListCacheData(list_cache);
        //添加媒体库项目
        AddMediaLibItem(ListItem::ClassificationType::CT_ARTIST);
        AddMediaLibItem(ListItem::ClassificationType::CT_ALBUM);
        AddMediaLibItem(ListItem::ClassificationType::CT_GENRE);
        AddMediaLibItem(ListItem::ClassificationType::CT_YEAR);
        AddMediaLibItem(ListItem::ClassificationType::CT_TYPE);
        //添加媒体库中的所有文件夹
        AddAllFolders();

        //设置到列表
        ShowList();

        m_initialized = true;
    }
}

void CFindListDlg::OnTabEntered()
{
    if (!m_initialized)
        SetPlaySelectedEnable(false);
    else
        SetPlaySelectedEnable(m_list_ctrl.GetCurSel() > 0);
    InitListData();
}

void CFindListDlg::AddListCacheData(const CListCache& list_cache)
{
    for (int i{}; i < list_cache.size(); i++)
    {
        CListCtrlEx::RowData row_data;
        const auto& list_data = list_cache.at(i);
        row_data[COL_NAME] = list_data.GetDisplayName();
        row_data[COL_TRACK_NUM] = std::to_wstring(list_data.total_num);
        m_list_data.push_back(std::move(row_data));
        FindListItem list_item;
        list_item.list_data = list_data;
        list_item.item_from = ItemFrom::RECENT_PLAYED;
        m_all_list_items.push_back(list_item);
    }
}

void CFindListDlg::AddMediaLibItem(ListItem::ClassificationType type)
{
    //这里考虑即使没有在媒体库设置中勾选“将只有一项的分类归到其他类中”，也应该添加所有项目
	int item_count = CUiMediaLibItemMgr::Instance().GetItemCount(type);
	for (int i{}; i < item_count; i++)
	{
		ListItem list_data;
		list_data.type = LT_MEDIA_LIB;
		list_data.medialib_type = type;
		list_data.path = CUiMediaLibItemMgr::Instance().GetItemName(type, i);
		list_data.total_num = CUiMediaLibItemMgr::Instance().GetItemSongCount(type, i);
		auto iter = std::find(m_all_list_items.begin(), m_all_list_items.end(), list_data);
		//不添加重复的项目
		if (iter == m_all_list_items.end())
		{
			FindListItem list_item;
			list_item.list_data = list_data;
			list_item.item_from = ItemFrom::MEDIALIB_ITEM;
			m_all_list_items.push_back(list_item);
			CListCtrlEx::RowData row_data;
			row_data[COL_NAME] = CUiMediaLibItemMgr::Instance().GetItemDisplayName(type, i);
			row_data[COL_TRACK_NUM] = std::to_wstring(list_data.total_num);
			m_list_data.push_back(std::move(row_data));
		}
	}
}

void CFindListDlg::AddAllFolders()
{
    const auto& folder_nodes = CUiFolderExploreMgr::Instance().GetRootNodes();
    for (const auto& folder_node : folder_nodes)
    {
        //遍历每个顶级文件夹节点
        folder_node->IterateNodeInOrder([&](UiElement::TreeElement::Node* node) -> bool {
            std::wstring folder_path = UiElement::FolderExploreTree::GetNodePath(node);		//文件夹路径
            std::wstring track_num = node->texts[UiElement::FolderExploreTree::COL_COUNT];	//曲目数
            ListItem list_data;
            list_data.type = LT_FOLDER;
            list_data.path = folder_path + L'\\';	//文件夹路径末尾添加一个反斜杠，使得其格式和CListCache中的文件夹一致
            list_data.total_num = _wtoi(track_num.c_str());
            auto iter = std::find(m_all_list_items.begin(), m_all_list_items.end(), list_data);
            //不添加重复的项目
            if (iter == m_all_list_items.end())
            {
                FindListItem list_item;
                list_item.list_data = list_data;
                list_item.item_from = ItemFrom::FOLDER_EXPLORE;
                m_all_list_items.push_back(list_item);
                CListCtrlEx::RowData row_data;
                row_data[COL_NAME] = folder_path;
                row_data[COL_TRACK_NUM] = track_num;
                m_list_data.push_back(std::move(row_data));
            }
            return false;
        }, false);
    }
}

void CFindListDlg::ShowList()
{
    if (m_searched)
        m_list_ctrl.SetListData(&m_list_data_searched);
    else
        m_list_ctrl.SetListData(&m_list_data);

    //更新图标
    if (m_searched)
    {
        for (int i{}; i < static_cast<int>(m_search_result.size()); i++)
        {
            int index = m_search_result[i];
            if (index >= 0 && index < static_cast<int>(m_all_list_items.size()))
            {
                IconMgr::IconType icon = m_all_list_items[index].list_data.GetTypeIcon();
                m_list_ctrl.SetItemIcon(i, theApp.m_icon_mgr.GetHICON(icon, IconMgr::IconStyle::IS_OutlinedDark, IconMgr::IconSize::IS_DPI_16));
            }
        }
    }
    else
    {
        for (int i{}; i < static_cast<int>(m_all_list_items.size()); i++)
        {
            IconMgr::IconType icon = m_all_list_items[i].list_data.GetTypeIcon();
            m_list_ctrl.SetItemIcon(i, theApp.m_icon_mgr.GetHICON(icon, IconMgr::IconStyle::IS_OutlinedDark, IconMgr::IconSize::IS_DPI_16));
        }
    }
}

void CFindListDlg::QuickSearch(const wstring& key_word)
{
    m_list_data_searched.clear();
    m_search_result.clear();
    if (!key_word.empty())
    {
        for (int i{}; i < static_cast<int>(m_list_data.size()); ++i)
        {
            const auto& item{ m_list_data[i] };
            const vector<int> search_col{ COL_NAME };
            for (int col : search_col)
            {
                if (theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(key_word, item.at(col)))
                {
                    m_list_data_searched.push_back(item);
                    m_search_result.push_back(i);
                    break;
                }
            }
        }
    }
}

void CFindListDlg::SetPlaySelectedEnable(bool enable)
{
    CWnd* pParent = GetParentWindow();
    ::SendMessage(pParent->GetSafeHwnd(), WM_PLAY_SELECTED_BTN_ENABLE, WPARAM(enable), 0);
}

CFindListDlg::FindListItem CFindListDlg::GetSelectedItem() const
{
    FindListItem list_item;
    int cur_sel = m_list_ctrl.GetCurSel();
    if (m_searched)
    {
        if (cur_sel >= 0 && cur_sel < static_cast<int>(m_search_result.size()))
        {
            int index = m_search_result[cur_sel];
            if (index >= 0 && index < static_cast<int>(m_all_list_items.size()))
            {
                list_item = m_all_list_items[index];
            }
        }
    }
    else
    {
        if (cur_sel >= 0 && cur_sel < static_cast<int>(m_all_list_items.size()))
        {
            list_item = m_all_list_items[cur_sel];
        }
    }
    return list_item;
}

void CFindListDlg::DoDataExchange(CDataExchange* pDX)
{
    CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SONG_LIST, m_list_ctrl);
    DDX_Control(pDX, IDC_SEARCH_EDIT, m_search_edit);
}


BEGIN_MESSAGE_MAP(CFindListDlg, CTabDlg)
    ON_MESSAGE(WM_SEARCH_EDIT_BTN_CLICKED, &CFindListDlg::OnSearchEditBtnClicked)
    ON_EN_CHANGE(IDC_SEARCH_EDIT, &CFindListDlg::OnEnChangeSearchEdit)
    ON_NOTIFY(NM_CLICK, IDC_SONG_LIST, &CFindListDlg::OnNMClickSongList)
    ON_WM_SIZE()
    ON_NOTIFY(NM_DBLCLK, IDC_SONG_LIST, &CFindListDlg::OnNMDblclkSongList)
    ON_NOTIFY(NM_RCLICK, IDC_SONG_LIST, &CFindListDlg::OnNMRClickSongList)
    ON_COMMAND(ID_PLAY_ITEM, &CFindListDlg::OnPlayItem)
    ON_COMMAND(ID_COPY_TEXT, &CFindListDlg::OnCopyText)
    ON_COMMAND(ID_VIEW_IN_MEDIA_LIB, &CFindListDlg::OnViewInMediaLib)
    ON_COMMAND(ID_LIB_RECENT_PLAYED_ITEM_PROPERTIES, &CFindListDlg::OnLibRecentPlayedItemProperties)
    ON_WM_INITMENU()
END_MESSAGE_MAP()


// CFindListDlg 消息处理程序


BOOL CFindListDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    //初始化列表控件
    m_list_ctrl.SetExtendedStyle(m_list_ctrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
    m_list_ctrl.InsertColumn(COL_NAME, theApp.m_str_table.LoadText(L"TXT_NAME").c_str(), LVCFMT_LEFT, theApp.DPI(320));
    m_list_ctrl.InsertColumn(COL_TRACK_NUM, theApp.m_str_table.LoadText(L"TXT_NUM_OF_TRACK").c_str(), LVCFMT_LEFT, theApp.DPI(140));
    m_list_ctrl.SetCtrlAEnable(true);
    m_list_ctrl.SetRowHeight(theApp.DPI(24), theApp.DPI(18));

    m_search_edit.SetCueBanner(theApp.m_str_table.LoadText(L"TXT_SEARCH_PROMPT_ALL_LIST").c_str(), TRUE);


    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


void CFindListDlg::OnOK()
{
    //获取选中的项目
    FindListItem list_item = GetSelectedItem();
    if (!list_item.list_data.empty())
    {
        CMusicPlayerCmdHelper helper;
        helper.OnListItemSelected(list_item.list_data, true);
    }

    CTabDlg::OnOK();
}


afx_msg LRESULT CFindListDlg::OnSearchEditBtnClicked(WPARAM wParam, LPARAM lParam)
{
    //点击搜索框中的叉按钮时清除搜索结果
    if (m_searched)
    {
        //清除搜索结果
        m_searched = false;
        m_search_edit.SetWindowText(_T(""));
        ShowList();
    }
    return 0;
}


void CFindListDlg::OnEnChangeSearchEdit()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CTabDlg::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    CString str;
    m_search_edit.GetWindowText(str);
    QuickSearch(wstring(str));
    m_searched = !str.IsEmpty();
    ShowList();
}


void CFindListDlg::OnNMClickSongList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    SetPlaySelectedEnable(pNMItemActivate->iItem != -1);

    *pResult = 0;
}


void CFindListDlg::OnSize(UINT nType, int cx, int cy)
{
    CTabDlg::OnSize(nType, cx, cy);

    if (m_list_ctrl.m_hWnd != NULL && nType != SIZE_MINIMIZED)
    {
        //调整列表中项目的宽度
        CRect rect;
        m_list_ctrl.GetWindowRect(rect);
        int list_width{ rect.Width() - theApp.DPI(20) - 1 };        //列表控件宽度减去留给垂直滚动条的宽度余量
        int width0, width1;
        width1 = theApp.DPI(60);
        width0 = list_width - width1;
        m_list_ctrl.SetColumnWidth(COL_NAME, width0);
        m_list_ctrl.SetColumnWidth(COL_TRACK_NUM, width1);
    }
}


void CFindListDlg::OnNMDblclkSongList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    OnPlayItem();
    *pResult = 0;
}


void CFindListDlg::OnNMRClickSongList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

    //弹出右键菜单
    CMenu* pContextMenu = theApp.m_menu_mgr.GetMenu(MenuMgr::FindListMenu);
    m_list_ctrl.ShowPopupMenu(pContextMenu, pNMItemActivate->iItem, this);
    *pResult = 0;
}


void CFindListDlg::OnPlayItem()
{
    //向父窗口发送IDOK命令
    CWnd* pParent = GetParentWindow();
    ::SendMessage(pParent->GetSafeHwnd(), WM_COMMAND, IDOK, 0);
}


void CFindListDlg::OnCopyText()
{
    FindListItem list_item = GetSelectedItem();
    if (list_item.list_data.empty())
        return;
    if (!CCommon::CopyStringToClipboard(list_item.list_data.GetDisplayName()))
        AfxMessageBox(theApp.m_str_table.LoadText(L"MSG_COPY_CLIPBOARD_FAILED").c_str(), MB_ICONWARNING);
}


void CFindListDlg::OnViewInMediaLib()
{
    FindListItem list_item = GetSelectedItem();
    if (list_item.list_data.empty())
        return;
    CMusicPlayerCmdHelper helper;
    helper.OnViewInMediaLib(list_item.list_data);
}


void CFindListDlg::OnLibRecentPlayedItemProperties()
{
    FindListItem list_item = GetSelectedItem();
    if (list_item.list_data.empty())
        return;
    if (list_item.list_data.type == LT_FOLDER)
    {
        CFolderPropertiesDlg dlg(list_item.list_data);
        dlg.DoModal();
    }
    else if (list_item.list_data.type == LT_PLAYLIST)
    {
        CPlaylistPropertiesDlg dlg(list_item.list_data);
        dlg.DoModal();
    }
    else if (list_item.list_data.type == LT_MEDIA_LIB)
    {
        CMediaLibItemPropertiesDlg dlg(list_item.list_data);
        dlg.DoModal();
    }
}


void CFindListDlg::OnInitMenu(CMenu* pMenu)
{
    CTabDlg::OnInitMenu(pMenu);

    FindListItem list_item = GetSelectedItem();
    bool select_valid = m_list_ctrl.GetCurSel() >= 0;
    pMenu->EnableMenuItem(ID_PLAY_ITEM, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_COPY_TEXT, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_VIEW_IN_MEDIA_LIB, MF_BYCOMMAND | (select_valid && (list_item.item_from == ItemFrom::RECENT_PLAYED || list_item.item_from == ItemFrom::MEDIALIB_ITEM) ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_LIB_RECENT_PLAYED_ITEM_PROPERTIES, MF_BYCOMMAND | (select_valid && list_item.item_from == ItemFrom::RECENT_PLAYED ? MF_ENABLED : MF_GRAYED));
}
