// CTabCtrlEx.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "CTabCtrlEx.h"
#include "TabDlg.h"


// CTabCtrlEx

IMPLEMENT_DYNAMIC(CTabCtrlEx, CTabCtrl)

CTabCtrlEx::CTabCtrlEx()
{

}

CTabCtrlEx::~CTabCtrlEx()
{
}

void CTabCtrlEx::AddWindow(CWnd* pWnd, LPCTSTR lable_text, IconMgr::IconType icon_type)
{
	if (pWnd == nullptr || pWnd->GetSafeHwnd() == NULL)
		return;

    InsertItem(m_tab_list.size(), lable_text, m_tab_list.size());

	pWnd->SetParent(this);
	pWnd->MoveWindow(m_tab_rect);

	m_tab_list.push_back(pWnd);
    if (icon_type != IconMgr::IconType::IT_NO_ICON)
        m_icon_list.push_back(icon_type);
}

void CTabCtrlEx::SetCurTab(int index)
{
    if (index < 0 || index >= static_cast<int>(m_tab_list.size()))
        index = 0;
	SetCurSel(index);

	int tab_size = m_tab_list.size();
	for (int i = 0; i < tab_size; i++)
	{
		if (i == index)
		{
			m_tab_list[i]->ShowWindow(SW_SHOW);
			m_tab_list[i]->SetFocus();
		}
		else
		{
			m_tab_list[i]->ShowWindow(SW_HIDE);
		}
	}

    CTabDlg* pTabWnd = dynamic_cast<CTabDlg*>(m_tab_list[index]);
    if (pTabWnd != nullptr)
        pTabWnd->OnTabEntered();

    if (m_last_tab_index != index && m_last_tab_index >= 0 && m_last_tab_index < static_cast<int>(m_tab_list.size()))
    {
        CTabDlg* pLastTabWnd = dynamic_cast<CTabDlg*>(m_tab_list[m_last_tab_index]);
        if (pLastTabWnd != nullptr)
            pLastTabWnd->OnTabExited();
    }

    m_last_tab_index = index;
}

CWnd* CTabCtrlEx::GetCurrentTab()
{
    size_t cur_tab_index = GetCurSel();
    if (cur_tab_index >= 0 && cur_tab_index < m_tab_list.size())
    {
        return m_tab_list[cur_tab_index];
    }
    return nullptr;
}

void CTabCtrlEx::AdjustTabWindowSize()
{
    CalSubWindowSize();
    for (size_t i{}; i < m_tab_list.size(); i++)
    {
        m_tab_list[i]->MoveWindow(m_tab_rect);
    }
    //为每个标签添加图标
    if (m_icon_list.empty())
        return;
    CSize icon_size = IconMgr::GetIconSize(IconMgr::IconSize::IS_DPI_16);
    CImageList ImageList;
    ImageList.Create(icon_size.cx, icon_size.cy, ILC_COLOR32 | ILC_MASK, 2, 2);
    for (auto icon_type : m_icon_list)
    {
        HICON hIcon = theApp.m_icon_mgr.GetHICON(icon_type, IconMgr::IconStyle::IS_OutlinedDark, IconMgr::IconSize::IS_DPI_16);
        ImageList.Add(hIcon);
    }
    SetImageList(&ImageList);
    ImageList.Detach();
}

void CTabCtrlEx::CalSubWindowSize()
{
    GetClientRect(m_tab_rect);
    CRect rc_temp = m_tab_rect;
    AdjustRect(FALSE, rc_temp);
    int margin = rc_temp.left - m_tab_rect.left;
    CRect rcTabItem;
    GetItemRect(0, rcTabItem);
    m_tab_rect.top += rcTabItem.Height() + margin;
    m_tab_rect.left += margin;
    m_tab_rect.bottom -= margin;
    m_tab_rect.right -= margin;
}


BEGIN_MESSAGE_MAP(CTabCtrlEx, CTabCtrl)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, &CTabCtrlEx::OnTcnSelchange)
    ON_WM_SIZE()
END_MESSAGE_MAP()



// CTabCtrlEx 消息处理程序




void CTabCtrlEx::OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	int tab_selected = GetCurSel();
	SetCurTab(tab_selected);

	*pResult = 0;
}


void CTabCtrlEx::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类

	//计算子窗口的位置
    CalSubWindowSize();

	CTabCtrl::PreSubclassWindow();
}


void CTabCtrlEx::OnSize(UINT nType, int cx, int cy)
{
    CTabCtrl::OnSize(nType, cx, cy);

    // TODO: 在此处添加消息处理程序代码
    AdjustTabWindowSize();
}
