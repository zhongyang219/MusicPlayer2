// PropertyDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "Player.h"
#include "PropertyDlg.h"


// CPropertyDlg 对话框

IMPLEMENT_DYNAMIC(CPropertyDlg, CBaseDialog)

CPropertyDlg::CPropertyDlg(vector<SongInfo>& all_song_info, int index, bool read_only, int tab_index, bool show_out_album_cover, CWnd* pParent /*=nullptr*/)
    : CBaseDialog(IDD_PROPERTY_PARENT_DIALOG, pParent), m_read_only{ read_only }, m_index{ index }, m_song_num{ static_cast<int>(all_song_info.size()) }, m_tab_index{ tab_index },
    m_property_dlg(all_song_info, m_index, this, read_only),
    m_album_cover_dlg(all_song_info, m_index, show_out_album_cover, read_only, this),
    m_advanced_dlg(all_song_info, m_index),
    m_batch_edit{ false }
{

}

CPropertyDlg::CPropertyDlg(vector<SongInfo>& all_song_info, CWnd* pParent /*= nullptr*/)
    : CBaseDialog(IDD_PROPERTY_PARENT_DIALOG, pParent), m_song_num{ static_cast<int>(all_song_info.size()) },
    m_property_dlg(all_song_info, this),
    m_album_cover_dlg(all_song_info, this),
    m_advanced_dlg(all_song_info),
    m_batch_edit{ true }
{

}

CPropertyDlg::~CPropertyDlg()
{
}

bool CPropertyDlg::GetListRefresh() const
{
    return m_property_dlg.GetListRefresh();
}

bool CPropertyDlg::GetModified() const
{
    return m_modified;
}

void CPropertyDlg::ShowPageNum()
{
    //显示页数
    CString item_info;
    item_info.Format(_T("%d/%d"), m_index + 1, m_song_num);
    SetDlgItemText(IDC_ITEM_STATIC, item_info);

}

CString CPropertyDlg::GetDialogName() const
{
    return _T("PropertyDlg");
}

bool CPropertyDlg::InitializeControls()
{
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TITLE_PROPERTY_PARENT");
    SetWindowTextW(temp.c_str());
    // IDC_TAB1
    temp = theApp.m_str_table.LoadText(L"TXT_PROPERTY_PARENT_PREVIOUS");
    SetDlgItemTextW(IDC_PREVIOUS_BUTTON, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_PROPERTY_PARENT_NEXT");
    SetDlgItemTextW(IDC_NEXT_BUTTON, temp.c_str());
    // IDC_ITEM_STATIC
    temp = theApp.m_str_table.LoadText(L"TXT_PROPERTY_PARENT_SAVE_TO_FILE");
    SetDlgItemTextW(IDC_SAVE_TO_FILE_BUTTON, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_CLOSE");
    SetDlgItemTextW(IDCANCEL, temp.c_str());

    RepositionTextBasedControls({
        { CtrlTextInfo::L2, IDC_PREVIOUS_BUTTON, CtrlTextInfo::W32 },
        { CtrlTextInfo::L1, IDC_NEXT_BUTTON, CtrlTextInfo::W32 },
        { CtrlTextInfo::C0, IDC_ITEM_STATIC },
        { CtrlTextInfo::R1, IDC_SAVE_TO_FILE_BUTTON, CtrlTextInfo::W32 },
        { CtrlTextInfo::R2, IDCANCEL, CtrlTextInfo::W32 }
        });
    return true;
}

void CPropertyDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TAB1, m_tab_ctrl);
    DDX_Control(pDX, IDC_SAVE_TO_FILE_BUTTON, m_save_btn);
    DDX_Control(pDX, IDC_PREVIOUS_BUTTON, m_previous_btn);
    DDX_Control(pDX, IDC_NEXT_BUTTON, m_next_btn);
}


BEGIN_MESSAGE_MAP(CPropertyDlg, CBaseDialog)
    ON_BN_CLICKED(IDC_SAVE_TO_FILE_BUTTON, &CPropertyDlg::OnBnClickedSaveToFileButton)
    ON_BN_CLICKED(IDC_PREVIOUS_BUTTON, &CPropertyDlg::OnBnClickedPreviousButton)
    ON_BN_CLICKED(IDC_NEXT_BUTTON, &CPropertyDlg::OnBnClickedNextButton)
    ON_MESSAGE(WM_PROPERTY_DIALOG_MODIFIED, &CPropertyDlg::OnPropertyDialogModified)
    ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


// CPropertyDlg 消息处理程序


BOOL CPropertyDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化

    SetIcon(IconMgr::IconType::IT_Info, FALSE);
    SetButtonIcon(IDC_PREVIOUS_BUTTON, IconMgr::IconType::IT_Triangle_Left);
    SetButtonIcon(IDC_NEXT_BUTTON, IconMgr::IconType::IT_Triangle_Right);
    SetButtonIcon(IDC_SAVE_TO_FILE_BUTTON, IconMgr::IconType::IT_Save);

    //创建子对话框
    m_property_dlg.Create(IDD_PROPERTY_DIALOG);
    m_album_cover_dlg.Create(IDD_PROPERTY_ALBUM_COVER_DIALOG);
    if (!m_batch_edit)      //批量编辑时不添加“高级标签信息”标签
        m_advanced_dlg.Create(IDD_PROPERTY_ADVANCED_DIALOG);

    //添加对话框
    m_tab_ctrl.AddWindow(&m_property_dlg, theApp.m_str_table.LoadText(L"TITLE_PROPERTY_DLG").c_str(), IconMgr::IconType::IT_File_Relate);
    m_tab_ctrl.AddWindow(&m_album_cover_dlg, theApp.m_str_table.LoadText(L"TITLE_COVER_PROPERTY").c_str(), IconMgr::IconType::IT_Album_Cover);
    if (!m_batch_edit)
        m_tab_ctrl.AddWindow(&m_advanced_dlg, theApp.m_str_table.LoadText(L"TITLE_ADVANCED_PROPERTY").c_str(), IconMgr::IconType::IT_Tag);

    m_tab_ctrl.SetItemSize(CSize(theApp.DPI(60), theApp.DPI(24)));
    m_tab_ctrl.AdjustTabWindowSize();

    m_album_cover_dlg.AdjustColumnWidth();
    if (!m_batch_edit)
        m_advanced_dlg.AdjustColumnWidth();

    m_tab_ctrl.SetCurTab(m_tab_index);

    ShowPageNum();

    if (m_read_only)
    {
        CWnd* pBtn = GetDlgItem(IDC_SAVE_TO_FILE_BUTTON);
        if (pBtn != nullptr)
            pBtn->ShowWindow(SW_HIDE);
    }

    if (m_batch_edit)
    {
        SetWindowText(theApp.m_str_table.LoadTextFormat(L"TITLE_PROPERTY_PARENT_BATCH", { m_song_num }).c_str());
        ShowDlgCtrl(IDC_PREVIOUS_BUTTON, false);
        ShowDlgCtrl(IDC_NEXT_BUTTON, false);
        ShowDlgCtrl(IDC_ITEM_STATIC, false);
    }

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CPropertyDlg::OnBnClickedSaveToFileButton()
{
    // TODO: 在此添加控件通知处理程序代码
    IPropertyTabDlg* cur_tab = dynamic_cast<IPropertyTabDlg*>(m_tab_ctrl.GetCurrentTab());
    if (cur_tab != nullptr)
    {
        CPlayer::ReOpen reopen(true);   // ReOpen需要在IsLockSuccess失败时撤销操作所以改在这里，但这里判断是否为修改当前播放有点困难所以总是先关闭
        if (reopen.IsLockSuccess())
        {
            int saved_num = cur_tab->SaveModified();
            m_modified = true;
            if (m_batch_edit)
            {
                wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_PROPERTY_PARENT_TAG_BATCH_EDIT_SAVE_INFO", { saved_num });
                MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
            }
            else
            {
                if (saved_num == 0)
                {
                    const wstring& info = theApp.m_str_table.LoadText(L"MSG_FILE_WRITE_FAILED");
                    MessageBox(info.c_str(), NULL, MB_ICONWARNING | MB_OK);
                }
            }
        }
        else
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
            MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
        }
    }
}


void CPropertyDlg::OnBnClickedPreviousButton()
{
    // TODO: 在此添加控件通知处理程序代码
    if (!m_batch_edit)
    {
        IPropertyTabDlg* cur_tab = dynamic_cast<IPropertyTabDlg*>(m_tab_ctrl.GetCurrentTab());
        if (cur_tab != nullptr)
        {
            cur_tab->PagePrevious();
            ShowPageNum();
        }
    }
}


void CPropertyDlg::OnBnClickedNextButton()
{
    // TODO: 在此添加控件通知处理程序代码
    if (!m_batch_edit)
    {
        IPropertyTabDlg* cur_tab = dynamic_cast<IPropertyTabDlg*>(m_tab_ctrl.GetCurrentTab());
        if (cur_tab != nullptr)
        {
            cur_tab->PageNext();
            ShowPageNum();
        }
    }
}


afx_msg LRESULT CPropertyDlg::OnPropertyDialogModified(WPARAM wParam, LPARAM lParam)
{
    CWnd* pBtn = GetDlgItem(IDC_SAVE_TO_FILE_BUTTON);
    if (pBtn != nullptr)
        pBtn->EnableWindow((BOOL)wParam);
    return 0;
}


BOOL CPropertyDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (zDelta > 0)
    {
        OnBnClickedPreviousButton();
    }
    if (zDelta < 0)
    {
        OnBnClickedNextButton();
    }

    return CBaseDialog::OnMouseWheel(nFlags, zDelta, pt);
}


BOOL CPropertyDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_UP)
        {
            OnBnClickedPreviousButton();
            return TRUE;
        }
        if (pMsg->wParam == VK_DOWN)
        {
            OnBnClickedNextButton();
            return TRUE;
        }
    }

    return CBaseDialog::PreTranslateMessage(pMsg);
}
