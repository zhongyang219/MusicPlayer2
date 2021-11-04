// PropertyDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "PropertyDlg.h"
#include "afxdialogex.h"


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

    SetIcon(theApp.m_icon_set.info.GetIcon(true), FALSE);

    m_previous_btn.SetIcon(theApp.m_icon_set.lyric_delay.GetIcon(true));
    m_next_btn.SetIcon(theApp.m_icon_set.lyric_forward.GetIcon(true));
    SetButtonIcon(IDC_SAVE_TO_FILE_BUTTON, theApp.m_icon_set.save_new);

    //创建子对话框
    m_property_dlg.Create(IDD_PROPERTY_DIALOG);
    m_album_cover_dlg.Create(IDD_PROPERTY_ALBUM_COVER_DIALOG);
    if (!m_batch_edit)      //批量编辑时不添加“高级标签信息”标签
        m_advanced_dlg.Create(IDD_PROPERTY_ADVANCED_DIALOG);

    //添加对话框
    m_tab_ctrl.AddWindow(&m_property_dlg, CCommon::LoadText(IDS_FILE_PROPERTY));
    m_tab_ctrl.AddWindow(&m_album_cover_dlg, CCommon::LoadText(IDS_ALBUM_COVER));
    if (!m_batch_edit)
        m_tab_ctrl.AddWindow(&m_advanced_dlg, CCommon::LoadText(IDS_ADVANCED_PROPERTY));

    //为每个标签添加图标
    CImageList ImageList;
    ImageList.Create(theApp.DPI(16), theApp.DPI(16), ILC_COLOR32 | ILC_MASK, 2, 2);
    ImageList.Add(theApp.m_icon_set.file_relate);
    ImageList.Add(theApp.m_icon_set.album_cover);
    if (!m_batch_edit)
        ImageList.Add(theApp.m_icon_set.tag);
    m_tab_ctrl.SetImageList(&ImageList);
    ImageList.Detach();

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
        ShowDlgCtrl(IDC_PREVIOUS_BUTTON, false);
        ShowDlgCtrl(IDC_NEXT_BUTTON, false);
        ShowDlgCtrl(IDC_ITEM_STATIC, false);

        CString str_title;
        GetWindowText(str_title);
        str_title += CCommon::LoadTextFormat(IDS_PROPERTY_TITLE_INFO, { m_song_num });
        SetWindowText(str_title);
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
        int saved_num = cur_tab->SaveModified();
        m_modified = true;
        if (m_batch_edit)
        {
            CString info = CCommon::LoadTextFormat(IDS_TAG_BATCH_EDIT_INFO, { saved_num });
            MessageBox(info, NULL, MB_ICONINFORMATION | MB_OK);
        }
        else
        {
            if (saved_num == 0)
                MessageBox(CCommon::LoadText(IDS_CANNOT_WRITE_TO_FILE), NULL, MB_ICONWARNING | MB_OK);
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
