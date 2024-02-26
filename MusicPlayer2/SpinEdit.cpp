// SpinEdit.cpp: 实现文件
//

#include "stdafx.h"
#include "SpinEdit.h"


// CSpinEdit

IMPLEMENT_DYNAMIC(CSpinEdit, CEdit)

CSpinEdit::CSpinEdit()
{

}

CSpinEdit::~CSpinEdit()
{
}

void CSpinEdit::SetRange(short lower, short upper)
{
    if (m_spin.GetSafeHwnd() != NULL)
        m_spin.SetRange(lower, upper);
}

void CSpinEdit::SetValue(int value)
{
    if (m_spin.GetSafeHwnd() != NULL)
        m_spin.SetPos(value);
}

int CSpinEdit::GetValue()
{
    if (m_spin.GetSafeHwnd() != NULL)
        return m_spin.GetPos();
    else
        return 0;
}


void CSpinEdit::SetMouseWheelEnable(bool enable)
{
    m_mouse_wheel_enable = enable;
}


BEGIN_MESSAGE_MAP(CSpinEdit, CEdit)
    ON_MESSAGE(WM_TABLET_QUERYSYSTEMGESTURESTATUS, &CSpinEdit::OnTabletQuerysystemgesturestatus)
    ON_WM_SIZE()
END_MESSAGE_MAP()



// CSpinEdit 消息处理程序

void CSpinEdit::PreSubclassWindow()
{
    // TODO: 在此添加专用代码和/或调用基类

    // 要求文本左对齐，右侧会被spin控件覆盖，只能输入数字
    // WS_CLIPCHILDREN在绘制中排除子窗口区域，避免重叠的控件绘制闪烁
    ModifyStyle(ES_CENTER | ES_RIGHT, ES_LEFT | ES_NUMBER | WS_CLIPCHILDREN);
    // 将spin创建为edit的子控件，因为CTabDlg不一定能正常滚动动态创建的子控件（Spin）
    // 准确的说是CWnd::ScrollWindow在窗口可见/不可见时对动态创建的子控件移动行为不一致（多半是这样）（具体参考wincore.cpp的实现）（在CTabDlg::OnTabEntered随意滚一下再滚回来窗口就能正常）
    // 我试着在这里保持与edit为兄弟关系的spin始终跟随edit但没有成功，只好将spin作为edit的子控件（子窗口受限于父窗口的ClientRect所以不能使用UDS_ALIGNRIGHT）
    m_spin.Create(WS_CHILD | WS_VISIBLE | UDS_ARROWKEYS | UDS_HOTTRACK | UDS_SETBUDDYINT, CRect(), this, SPIN_ID);
    // 获取spin控件宽度
    CRect rect;
    m_spin.GetWindowRect(&rect);
    m_spin_width = rect.Width();
    // 设置spin控件区域（作为CEdit的子控件，ClientRect区域外的部分会被裁剪）
    GetClientRect(&rect);
    rect.left = rect.right - m_spin_width;
    m_spin.MoveWindow(&rect);
    // 设置Edit控件为关联控件
    m_spin.SetBuddy(this);
    // 设置默认范围
    m_spin.SetRange(0, 999);
    CEdit::PreSubclassWindow();
}


BOOL CSpinEdit::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类

    //如果m_mouse_wheel_enable为false，则不响应鼠标滚轮消息
    if (pMsg->message == WM_MOUSEWHEEL && !m_mouse_wheel_enable)
    {
        //将鼠标滚轮消息发送给父窗口
        CWnd* pParent = GetParent();
        pParent->SendMessage(WM_MOUSEWHEEL, pMsg->wParam, pMsg->lParam);
        return true;
    }

    return CEdit::PreTranslateMessage(pMsg);
}


afx_msg LRESULT CSpinEdit::OnTabletQuerysystemgesturestatus(WPARAM wParam, LPARAM lParam)
{
    return 0;
}


void CSpinEdit::OnSize(UINT nType, int cx, int cy)
{
    CEdit::OnSize(nType, cx, cy);
    CRect rect;
    GetClientRect(&rect);
    rect.left = rect.right - m_spin_width;
    m_spin.MoveWindow(&rect);
}
