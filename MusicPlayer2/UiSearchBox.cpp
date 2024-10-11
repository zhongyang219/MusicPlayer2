// UiSearchBox.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "afxdialogex.h"
#include "UiSearchBox.h"
#include "UIElement.h"
#include "MusicPlayerDlg.h"


// CUiSearchBox 对话框

IMPLEMENT_DYNAMIC(CUiSearchBox, CDialog)

CUiSearchBox::CUiSearchBox(CWnd* pParent)
	: CDialog(IDD_UI_SEARCH_BOX_DIALOG, pParent)
{

}

void CUiSearchBox::Create()
{
    CDialog::Create(IDD_UI_SEARCH_BOX_DIALOG, m_pParentWnd);
}

CUiSearchBox::~CUiSearchBox()
{
}

void CUiSearchBox::Show(UiElement::SearchBox* ui_search_box, bool big_icon)
{
    m_ui_search_box = ui_search_box;
    ASSERT(m_pParentWnd != nullptr);
    if (m_pParentWnd != nullptr)
    {
        CFont& font{ theApp.m_font_set.dlg.GetFont(big_icon) };
        CCommon::SetDialogFont(this, &font);
        ShowWindow(SW_SHOW);
        CRect rect = ui_search_box->GetRect();
        m_pParentWnd->ClientToScreen(&rect);
        MoveWindow(rect);
        m_search_box.OnChangeLayout();
        m_search_box.SetBigIcon(big_icon);
    }
}

void CUiSearchBox::Clear()
{
    m_search_box.SetWindowText(_T(""));
}

void CUiSearchBox::UpdatePos()
{
    ASSERT(m_pParentWnd != nullptr);
    if (m_pParentWnd != nullptr && m_ui_search_box != nullptr && IsWindow(this->GetSafeHwnd()) && !m_pParentWnd->IsZoomed() && !m_pParentWnd->IsIconic())
    {
        CRect rect;
        m_pParentWnd->GetClientRect(&rect);
        m_pParentWnd->ClientToScreen(&rect);
        if (!rect.IsRectEmpty())
        {
            CPoint obsolute_pos = rect.TopLeft() + m_ui_search_box->GetRect().TopLeft();
            SetWindowPos(nullptr, obsolute_pos.x, obsolute_pos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        }
    }
}

void CUiSearchBox::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_UI_SEARCH_BOX_EDIT, m_search_box);
}


BEGIN_MESSAGE_MAP(CUiSearchBox, CDialog)
    ON_WM_SIZE()
    ON_EN_KILLFOCUS(IDC_UI_SEARCH_BOX_EDIT, &CUiSearchBox::OnEnKillfocusUiSearchBoxEdit)
    ON_MESSAGE(WM_SEARCH_EDIT_BTN_CLICKED, &CUiSearchBox::OnSearchEditBtnClicked)
    ON_EN_CHANGE(IDC_UI_SEARCH_BOX_EDIT, &CUiSearchBox::OnEnChangeUiSearchBoxEdit)
END_MESSAGE_MAP()


// CUiSearchBox 消息处理程序


BOOL CUiSearchBox::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    CCommon::SetDialogFont(this, &theApp.m_font_set.dlg.GetFont());

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


void CUiSearchBox::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    // TODO: 在此处添加消息处理程序代码
    if (nType != SIZE_MINIMIZED && IsWindow(m_search_box.GetSafeHwnd()))
    {
        m_search_box.MoveWindow(0, 0, cx, cy);
    }
}


void CUiSearchBox::OnCancel()
{
    // TODO: 在此添加专用代码和/或调用基类
    DestroyWindow();
    //CDialog::OnCancel();
}


void CUiSearchBox::OnEnKillfocusUiSearchBoxEdit()
{
    // TODO: 在此添加控件通知处理程序代码
    ShowWindow(SW_HIDE);        //失去焦点时隐藏
}


afx_msg LRESULT CUiSearchBox::OnSearchEditBtnClicked(WPARAM wParam, LPARAM lParam)
{
    CSearchEditCtrl* pEdit = (CSearchEditCtrl*)wParam;
    if (pEdit == &m_search_box)
    {
        Clear();
    }
    return 0;
}


void CUiSearchBox::OnEnChangeUiSearchBoxEdit()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CDialog::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    if (m_ui_search_box != nullptr)
    {
        //搜索框文本改变时同步更新对UiElement中的搜索框
        CString str;
        m_search_box.GetWindowText(str);
        m_ui_search_box->key_word = str.GetString();
        m_ui_search_box->OnKeyWordsChanged();
        //通知线程强制重绘
        CMusicPlayerDlg* pMainWindow = CMusicPlayerDlg::GetInstance();
        if (pMainWindow != nullptr)
            pMainWindow->UiForceRefresh();
    }
}
