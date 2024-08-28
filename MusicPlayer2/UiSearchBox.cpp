// UiSearchBox.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "afxdialogex.h"
#include "UiSearchBox.h"


// CUiSearchBox 对话框

IMPLEMENT_DYNAMIC(CUiSearchBox, CDialog)

CUiSearchBox::CUiSearchBox(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_UI_SEARCH_BOX_DIALOG, pParent)
{

}

CUiSearchBox::~CUiSearchBox()
{
}

void CUiSearchBox::MainWindowMoved(CWnd* pWnd)
{
    if (pWnd != nullptr && IsWindow(this->GetSafeHwnd()) && !pWnd->IsZoomed() && !pWnd->IsIconic())
    {
        CRect rect;
        pWnd->GetClientRect(&rect);
        pWnd->ClientToScreen(&rect);
        if (!rect.IsRectEmpty())
        {
            CPoint obsolute_pos = rect.TopLeft() + m_relative_pos;
            SetWindowPos(nullptr, obsolute_pos.x, obsolute_pos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        }
    }
}

void CUiSearchBox::SetRelativePos(CPoint pos)
{
    m_relative_pos = pos;
}

void CUiSearchBox::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_UI_SEARCH_BOX_EDIT, m_search_box);
}


BEGIN_MESSAGE_MAP(CUiSearchBox, CDialog)
    ON_WM_SIZE()
    ON_EN_KILLFOCUS(IDC_UI_SEARCH_BOX_EDIT, &CUiSearchBox::OnEnKillfocusUiSearchBoxEdit)
END_MESSAGE_MAP()


// CUiSearchBox 消息处理程序


BOOL CUiSearchBox::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化

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
