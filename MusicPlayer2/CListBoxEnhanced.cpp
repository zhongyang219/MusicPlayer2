#include "stdafx.h"
#include "CListBoxEnhanced.h"
#include "MusicPlayer2.h"


CListBoxEnhanced::CListBoxEnhanced()
	: CListCtrlEx()
{
}


CListBoxEnhanced::~CListBoxEnhanced()
{
}

void CListBoxEnhanced::AddString(LPCTSTR str)
{
	int cnt = GetItemCount();
	InsertItem(cnt, str);
}

CString CListBoxEnhanced::GetItemText(int index)
{
    return CListCtrlEx::GetItemText(index, 0);
}

bool CListBoxEnhanced::DeleteItem(int nItem)
{
    bool selected_item_deleted = (nItem == GetCurSel());
    bool rtn = CListCtrlEx::DeleteItem(nItem);
    //如果删除的是选中的条目，则更新选中状态并通知父窗口
    if (selected_item_deleted)
    {
        SetCurSel(-1);
        CWnd* pParent{ GetParent() };
        if (pParent != nullptr)
            pParent->SendMessage(WM_LISTBOX_SEL_CHANGED, (WPARAM)this, (LPARAM)-1);
    }
    return rtn;
}

void CListBoxEnhanced::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
    SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP);
	ModifyStyle(0, LVS_NOCOLUMNHEADER);

	InsertColumn(0, _T("text"), LVCFMT_LEFT, CalculateColumnWidth());

	CListCtrlEx::PreSubclassWindow();
}

int CListBoxEnhanced::CalculateColumnWidth()
{
    CRect rect;
    GetWindowRect(rect);
    return rect.Width() - theApp.DPI(20);
}

BEGIN_MESSAGE_MAP(CListBoxEnhanced, CListCtrlEx)
	ON_NOTIFY_REFLECT(NM_CLICK, &CListBoxEnhanced::OnNMClick)
    ON_NOTIFY_REFLECT_EX(NM_RCLICK, &CListBoxEnhanced::OnNMRClick)
    ON_WM_SIZE()
END_MESSAGE_MAP()


void CListBoxEnhanced::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
    CWnd* pParent{ GetParent() };
    if (pParent != nullptr)
    {
        CRect check_rect;
        GetItemRect(pNMItemActivate->iItem, check_rect, LVIR_LABEL);		//获取选中项目文本的矩形区域（以列表控件左上角为原点）
        if (check_rect.PtInRect(pNMItemActivate->ptAction))                 //如果点击的
            pParent->SendMessage(WM_LISTBOX_SEL_CHANGED, (WPARAM)this, (LPARAM)pNMItemActivate->iItem);
        else
            pParent->SendMessage(WM_LISTBOX_SEL_CHANGED, (WPARAM)this, (LPARAM)-1);
    }

	*pResult = 0;
}


BOOL CListBoxEnhanced::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CWnd* pParent{ GetParent() };
	if (pParent != nullptr)
		pParent->SendMessage(WM_LISTBOX_SEL_CHANGED, (WPARAM)this, (LPARAM)pNMItemActivate->iItem);

	*pResult = 0;
    return FALSE;
}


void CListBoxEnhanced::OnSize(UINT nType, int cx, int cy)
{
    CListCtrlEx::OnSize(nType, cx, cy);

    // TODO: 在此处添加消息处理程序代码
    SetColumnWidth(0, CalculateColumnWidth());
    ShowScrollBar(SB_HORZ, FALSE);
}
