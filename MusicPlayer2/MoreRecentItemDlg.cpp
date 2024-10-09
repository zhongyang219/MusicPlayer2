// AddToPlaylistDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "MoreRecentItemDlg.h"
#include "FilePathHelper.h"
#include "MusicPlayerCmdHelper.h"
#include "FolderPropertiesDlg.h"
#include "PlaylistPropertiesDlg.h"
#include "MediaLibItemPropertiesDlg.h"
#include "CRecentList.h"

// CMoreRecentItemDlg 对话框

IMPLEMENT_DYNAMIC(CMoreRecentItemDlg, CBaseDialog)

CMoreRecentItemDlg::CMoreRecentItemDlg(CWnd* pParent /*=nullptr*/)
    : CBaseDialog(IDD_ADD_TO_PLAYLIST_DIALOG, pParent)
    , m_list_cache(LT_RECENT)
{

}

CMoreRecentItemDlg::~CMoreRecentItemDlg()
{
}

ListItem CMoreRecentItemDlg::GetSelectedItem() const
{
    if (m_selected_item >= 0 && m_selected_item < static_cast<int>(m_search_result.size()))
        return m_list_cache.at(m_search_result[m_selected_item]);
    return ListItem();
}

CString CMoreRecentItemDlg::GetDialogName() const
{
    return _T("MoreRecentItemDlg");
}

bool CMoreRecentItemDlg::InitializeControls()
{
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TITLE_MORE_RECENT_ITEM");
    SetWindowTextW(temp.c_str());

    SetDlgControlText(IDOK, L"TXT_MORE_RECENT_ITEM_PLAY_SEL");
    SetButtonIcon(IDOK, IconMgr::IconType::IT_Play);

    RepositionTextBasedControls({
        { CtrlTextInfo::R1, IDOK, CtrlTextInfo::W32 },
        { CtrlTextInfo::R2, IDCANCEL, CtrlTextInfo::W32 }
        });
    return true;
}

void CMoreRecentItemDlg::ShowList()
{
    m_list_cache.reload();
    m_list_ctrl.DeleteAllItems();
    m_search_result.clear();
    for (size_t i{}; i < m_list_cache.size(); ++i)
    {
        if (m_search_str.empty() || theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(m_search_str, m_list_cache.at(i).GetDisplayName()))
        {
            m_search_result.push_back(i);
            m_list_ctrl.AddString(m_list_cache.at(i).GetDisplayName().c_str());
            m_list_ctrl.SetItemIcon(m_search_result.size() - 1, theApp.m_icon_mgr.GetHICON(m_list_cache.at(i).GetTypeIcon(), IconMgr::IS_OutlinedDark, IconMgr::IconSize::IS_DPI_16));
        }
    }
}

void CMoreRecentItemDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
    DDX_Control(pDX, IDC_SEARCH_EDIT, m_search_edit);
}


BEGIN_MESSAGE_MAP(CMoreRecentItemDlg, CBaseDialog)
    ON_WM_GETMINMAXINFO()
    ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CMoreRecentItemDlg::OnNMDblclkList1)
    ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CMoreRecentItemDlg::OnNMRClickList1)
    ON_EN_CHANGE(IDC_SEARCH_EDIT, &CMoreRecentItemDlg::OnEnChangeSearchEdit)
    ON_MESSAGE(WM_SEARCH_EDIT_BTN_CLICKED, &CMoreRecentItemDlg::OnSearchEditBtnClicked)
    ON_MESSAGE(WM_LISTBOX_SEL_CHANGED, &CMoreRecentItemDlg::OnListboxSelChanged)
    ON_COMMAND(ID_PLAY_ITEM, &CMoreRecentItemDlg::OnPlayItem)
    ON_COMMAND(ID_RECENT_PLAYED_REMOVE, &CMoreRecentItemDlg::OnRecentPlayedRemove)
    ON_COMMAND(ID_COPY_TEXT, &CMoreRecentItemDlg::OnCopyText)
    ON_COMMAND(ID_VIEW_IN_MEDIA_LIB, &CMoreRecentItemDlg::OnViewInMediaLib)
    ON_COMMAND(ID_LIB_RECENT_PLAYED_ITEM_PROPERTIES, &CMoreRecentItemDlg::OnLibRecentPlayedItemProperties)
    ON_WM_INITMENU()
END_MESSAGE_MAP()


// CMoreRecentItemDlg 消息处理程序


BOOL CMoreRecentItemDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    SetIcon(IconMgr::IconType::IT_Media_Lib, FALSE);     // 设置小图标
    m_search_edit.SetCueBanner(theApp.m_str_table.LoadText(L"TXT_SEARCH_PROMPT").c_str(), TRUE);
    EnableDlgCtrl(IDOK, false);

    //初始化列表
    m_list_ctrl.SetRowHeight(theApp.DPI(24), theApp.DPI(18));
    ShowList();

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


void CMoreRecentItemDlg::OnOK()
{
    m_selected_item = m_list_ctrl.GetCurSel();

    CBaseDialog::OnOK();
}


void CMoreRecentItemDlg::OnNMDblclkList1(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    m_selected_item = pNMItemActivate->iItem;

    CBaseDialog::OnOK();

    *pResult = 0;
}

void CMoreRecentItemDlg::OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    m_selected_item = pNMItemActivate->iItem;
    
    //弹出右键菜单
    CMenu* pContextMenu = theApp.m_menu_mgr.GetMenu(MenuMgr::UiRecentPlayedMenu);
    m_list_ctrl.ShowPopupMenu(pContextMenu, pNMItemActivate->iItem, this);
    
    *pResult = 0;
}


void CMoreRecentItemDlg::OnEnChangeSearchEdit()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CBaseDialog::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    CString str;
    m_search_edit.GetWindowText(str);
    m_search_str = str;
    ShowList();
}


afx_msg LRESULT CMoreRecentItemDlg::OnSearchEditBtnClicked(WPARAM wParam, LPARAM lParam)
{
    CSearchEditCtrl* pEdit = (CSearchEditCtrl*)wParam;
    if (pEdit == &m_search_edit)
    {
        if (!m_search_str.empty())
        {
            //清除搜索结果
            m_search_str.clear();
            m_search_edit.SetWindowText(_T(""));
            ShowList();
        }
    }
    return 0;
}


afx_msg LRESULT CMoreRecentItemDlg::OnListboxSelChanged(WPARAM wParam, LPARAM lParam)
{
    CListBoxEnhanced* pCtrl = (CListBoxEnhanced*)wParam;
    if (pCtrl == &m_list_ctrl)
    {
        m_selected_item = lParam;
        //选中项是否有效
        ListItem list_item = GetSelectedItem();
        EnableDlgCtrl(IDOK, !list_item.empty());
    }
    return 0;
}


void CMoreRecentItemDlg::OnPlayItem()
{
    OnOK();
}


void CMoreRecentItemDlg::OnRecentPlayedRemove()
{
    ListItem list_item = GetSelectedItem();
    if (list_item.empty())
        return;
    wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_DELETE_RECENTPLAYED_ITEM_INQUIRY", { list_item.GetTypeDisplayName(), list_item.GetDisplayName() });
    if (MessageBox(info.c_str(), nullptr, MB_ICONQUESTION | MB_YESNO) != IDYES)
        return;
    if (CRecentList::Instance().ResetLastPlayedTime(list_item))
        ShowList();
}


void CMoreRecentItemDlg::OnCopyText()
{
    ListItem list_item = GetSelectedItem();
    if (list_item.empty())
        return;
    if (!CCommon::CopyStringToClipboard(list_item.GetDisplayName()))
        AfxMessageBox(theApp.m_str_table.LoadText(L"MSG_COPY_CLIPBOARD_FAILED").c_str(), MB_ICONWARNING);
}


void CMoreRecentItemDlg::OnViewInMediaLib()
{
    ListItem list_item = GetSelectedItem();
    if (list_item.empty())
        return;
    CMusicPlayerCmdHelper helper;
    CMusicPlayerCmdHelper::eMediaLibTab tab{};
    switch (list_item.type)
    {
    case LT_FOLDER: tab = CMusicPlayerCmdHelper::ML_FOLDER; break;
    case LT_PLAYLIST: tab = CMusicPlayerCmdHelper::ML_PLAYLIST; break;
    case LT_MEDIA_LIB:
        switch (list_item.medialib_type)
        {
        case CMediaClassifier::CT_ARTIST: tab = CMusicPlayerCmdHelper::ML_ARTIST; break;
        case CMediaClassifier::CT_ALBUM: tab = CMusicPlayerCmdHelper::ML_ALBUM; break;
        case CMediaClassifier::CT_GENRE: tab = CMusicPlayerCmdHelper::ML_GENRE; break;
        case CMediaClassifier::CT_YEAR: tab = CMusicPlayerCmdHelper::ML_YEAR; break;
        case CMediaClassifier::CT_TYPE: tab = CMusicPlayerCmdHelper::ML_FILE_TYPE; break;
        case CMediaClassifier::CT_BITRATE: tab = CMusicPlayerCmdHelper::ML_BITRATE; break;
        case CMediaClassifier::CT_RATING: tab = CMusicPlayerCmdHelper::ML_RATING; break;
        case CMediaClassifier::CT_NONE: tab = CMusicPlayerCmdHelper::ML_ALL; break;
        }
    }
    helper.OnViewInMediaLib(tab, list_item.path);
}


void CMoreRecentItemDlg::OnLibRecentPlayedItemProperties()
{
    ListItem list_item = GetSelectedItem();
    if (list_item.empty())
        return;
    if (list_item.type == LT_FOLDER)
    {
        CFolderPropertiesDlg dlg(list_item);
        dlg.DoModal();
    }
    else if (list_item.type == LT_PLAYLIST)
    {
        CPlaylistPropertiesDlg dlg(list_item);
        dlg.DoModal();
    }
    else if (list_item.type == LT_MEDIA_LIB)
    {
        CMediaLibItemPropertiesDlg dlg(list_item);
        dlg.DoModal();
    }
}


void CMoreRecentItemDlg::OnInitMenu(CMenu* pMenu)
{
    CBaseDialog::OnInitMenu(pMenu);

    ListItem list_item = GetSelectedItem();
    bool delete_enable{ list_item != m_list_cache.at(0) };  // 禁止删除当前播放
    bool select_valid{ !list_item.empty() };

    pMenu->EnableMenuItem(ID_PLAY_ITEM, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_RECENT_PLAYED_REMOVE, MF_BYCOMMAND | (delete_enable ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_COPY_TEXT, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_VIEW_IN_MEDIA_LIB, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_LIB_RECENT_PLAYED_ITEM_PROPERTIES, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
}
