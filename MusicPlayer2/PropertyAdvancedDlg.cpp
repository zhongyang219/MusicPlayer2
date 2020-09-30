// PropertyAdvancedDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "PropertyAdvancedDlg.h"
#include "afxdialogex.h"


// CPropertyAdvancedDlg 对话框

IMPLEMENT_DYNAMIC(CPropertyAdvancedDlg, CTabDlg)

CPropertyAdvancedDlg::CPropertyAdvancedDlg(vector<SongInfo>& all_song_info, int& index, CWnd* pParent /*= nullptr*/)
    : CTabDlg(IDD_PROPERTY_ADVANCED_DIALOG, pParent), m_all_song_info{ all_song_info }, m_index{ index }, m_batch_edit{ false }
{

}

CPropertyAdvancedDlg::CPropertyAdvancedDlg(vector<SongInfo>& all_song_info, CWnd* pParent /*= nullptr*/)
    : CTabDlg(IDD_PROPERTY_ADVANCED_DIALOG, pParent), m_all_song_info{ all_song_info }, m_index{ m_no_use }, m_batch_edit{ true }
{

}

CPropertyAdvancedDlg::~CPropertyAdvancedDlg()
{
}

void CPropertyAdvancedDlg::AdjustColumnWidth()
{
    CRect rect;
    m_list_ctrl.GetWindowRect(rect);
    int width0 = theApp.DPI(160);
    int width1 = rect.Width() - width0 - theApp.DPI(20) - 1;
    m_list_ctrl.SetColumnWidth(0, width0);
    m_list_ctrl.SetColumnWidth(1, width1);
}

int CPropertyAdvancedDlg::SaveModified()
{
    return 0;
}

void CPropertyAdvancedDlg::PagePrevious()
{
    m_index--;
    if (m_index < 0) m_index = static_cast<int>(m_all_song_info.size()) - 1;
    if (m_index < 0) m_index = 0;
    ShowInfo();
}

void CPropertyAdvancedDlg::PageNext()
{
    m_index++;
    if (m_index >= static_cast<int>(m_all_song_info.size())) m_index = 0;
    ShowInfo();
}

void CPropertyAdvancedDlg::OnTabEntered()
{
    ShowInfo();
    CWnd* pParent = GetParentWindow();
    if (pParent != nullptr)
        pParent->SendMessage(WM_PROPERTY_DIALOG_MODIFIED, false);       //设置“保存到文件”按钮不可用
}

const SongInfo& CPropertyAdvancedDlg::CurrentSong()
{
    if (m_index >= 0 && m_index < static_cast<int>(m_all_song_info.size()) && !m_batch_edit)
    {
        return m_all_song_info[m_index];
    }
    else
    {
        static SongInfo song;
        return song;
    }
}

void CPropertyAdvancedDlg::ShowInfo()
{
    EnableWindow(!m_batch_edit);
    m_list_ctrl.DeleteAllItems();
    if (!m_batch_edit)      //批量编辑（多选）模式下不支持显示高级标签信息
    {
        SongInfo cur_song = CurrentSong();
        if (!cur_song.is_cue)
        {
            CAudioTag audio_tag(cur_song);
            std::map<wstring, wstring> property_map;
            audio_tag.GetAudioTagPropertyMap(property_map);

            int index{};
            for (const auto& prop : property_map)
            {
                m_list_ctrl.InsertItem(index, prop.first.c_str());
                m_list_ctrl.SetItemText(index, 1, prop.second.c_str());
                index++;
            }
        }
    }
}

void CPropertyAdvancedDlg::DoDataExchange(CDataExchange* pDX)
{
    CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
}


BEGIN_MESSAGE_MAP(CPropertyAdvancedDlg, CTabDlg)
    ON_COMMAND(ID_COPY_TEXT, &CPropertyAdvancedDlg::OnCopyText)
    ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CPropertyAdvancedDlg::OnNMRClickList1)
    ON_WM_INITMENU()
    ON_COMMAND(ID_COPY_ALL_TEXT, &CPropertyAdvancedDlg::OnCopyAllText)
END_MESSAGE_MAP()


// CPropertyAdvancedDlg 消息处理程序


BOOL CPropertyAdvancedDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    CCommon::SetDialogFont(this, theApp.m_pMainWnd->GetFont());     //由于此对话框资源由不同语言共用，所以这里要设置一下字体

    //初始化列表
    m_list_ctrl.SetExtendedStyle(m_list_ctrl.GetExtendedStyle() | LVS_EX_GRIDLINES);
    m_list_ctrl.InsertColumn(0, CCommon::LoadText(IDS_ITEM), LVCFMT_LEFT, theApp.DPI(100));
    m_list_ctrl.InsertColumn(1, CCommon::LoadText(IDS_VLAUE), LVCFMT_LEFT, theApp.DPI(200));

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CPropertyAdvancedDlg::OnCopyText()
{
    // TODO: 在此添加命令处理程序代码
    if (!CCommon::CopyStringToClipboard(m_selected_string))
        MessageBox(CCommon::LoadText(IDS_COPY_CLIPBOARD_FAILED), NULL, MB_ICONWARNING);
}


void CPropertyAdvancedDlg::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    m_selected_string = m_list_ctrl.GetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem);
    m_item_selected = pNMItemActivate->iItem;

    //弹出右键菜单
    CMenu* pMenu = theApp.m_menu_set.m_property_menu.GetSubMenu(0);
    ASSERT(pMenu != nullptr);
    if (pMenu != nullptr)
    {
        m_list_ctrl.ShowPopupMenu(pMenu, pNMItemActivate->iItem, this);
    }

    *pResult = 0;
}


void CPropertyAdvancedDlg::OnInitMenu(CMenu* pMenu)
{
    CTabDlg::OnInitMenu(pMenu);

    // TODO: 在此处添加消息处理程序代码
    bool select_valid{ m_item_selected >= 0 && m_item_selected < m_list_ctrl.GetItemCount() };
    pMenu->EnableMenuItem(ID_COPY_TEXT, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
}


void CPropertyAdvancedDlg::OnCopyAllText()
{
    // TODO: 在此添加命令处理程序代码
    CCommon::CopyStringToClipboard(m_list_ctrl.GetAllText());
}
