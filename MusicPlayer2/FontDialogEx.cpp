// FontDialogEx.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "FontDialogEx.h"

// CFontDialogEx

IMPLEMENT_DYNAMIC(CFontDialogEx, CFontDialog)

CFontDialogEx::CFontDialogEx(LPLOGFONT lplfInitial, bool show_color, DWORD dwFlags, CDC* pdcPrinter, CWnd* pParentWnd) :
	CFontDialog(lplfInitial, dwFlags, pdcPrinter, pParentWnd),
    m_show_color(show_color)
{

}

CFontDialogEx::~CFontDialogEx()
{
}


BEGIN_MESSAGE_MAP(CFontDialogEx, CFontDialog)
END_MESSAGE_MAP()



// CFontDialogEx 消息处理程序



void CFontDialogEx::ShowDlgCtrl(int id, bool show)
{
    CWnd* ctrl = GetDlgItem(id);
    if (ctrl != nullptr)
        ctrl->ShowWindow(show ? SW_SHOW : SW_HIDE);
}

BOOL CFontDialogEx::OnInitDialog()
{
    CFontDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化

    //隐藏颜色
    if (!m_show_color)
    {
        ShowDlgCtrl(0x473, false); //颜色COMBO
        ShowDlgCtrl(0x443, false); //颜色Static
    }

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}
