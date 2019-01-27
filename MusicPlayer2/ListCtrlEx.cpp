#include "stdafx.h"
#include "ListCtrlEx.h"
#include "MusicPlayer2.h"

IMPLEMENT_DYNAMIC(CListCtrlEx, CListCtrl)

CListCtrlEx::CListCtrlEx()
	: m_theme_color(theApp.m_app_setting_data.theme_color)
{
	
	//初始化颜色
	//m_theme_color.original_color = GRAY(180);
	//CColorConvert::ConvertColor(m_theme_color);
}


CListCtrlEx::~CListCtrlEx()
{
}


//void CListCtrlEx::SetColor(const ColorTable & colors)
//{
//	m_theme_color = colors;
//	if (m_hWnd != NULL)
//		Invalidate();
//}

void CListCtrlEx::GetItemSelected(vector<int>& item_selected) const
{
	item_selected.clear();
	POSITION pos = GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		while (pos)
		{
			int nItem = GetNextSelectedItem(pos);
			item_selected.push_back(nItem);
		}
	}
}

int CListCtrlEx::GetCurSel() const
{
	vector<int> item_selected;
	GetItemSelected(item_selected);
	if (!item_selected.empty())
		return item_selected[0];
	else
		return -1;
}

void CListCtrlEx::SetCurSel(int select)
{
	int size = GetItemCount();
	if (select >= 0 && select < size)
	{
		SetItemState(select, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);	//选中行
		EnsureVisible(select, FALSE);		//使选中行保持可见
	}
	else
	{
		//取消所有行的选中
		for(int i{}; i<size; i++)
			SetItemState(i, 0, LVIS_SELECTED);
	}
}

bool CListCtrlEx::SetRowHeight(int height)
{
	if (height > 0 && height <= 512)
	{
		CImageList imgList;		//为ClistCtrl设置一个图像列表，以设置行高
		BOOL rtn = imgList.Create(1, height, ILC_COLOR, 1, 1);
		if (rtn != FALSE)
		{
			SetImageList(&imgList, LVSIL_SMALL);
			return true;
		}
	}
	return false;
}


BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CListCtrlEx::OnNMCustomdraw)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()


void CListCtrlEx::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = CDRF_DODEFAULT;
	LPNMLVCUSTOMDRAW lplvdr = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	NMCUSTOMDRAW& nmcd = lplvdr->nmcd;
	static bool this_item_select = false;
	switch (lplvdr->nmcd.dwDrawStage)	//判断状态   
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT:			//如果为画ITEM之前就要进行颜色的改变
		this_item_select = false;
		if (IsWindowEnabled())
		{
			//设置选中行的颜色
			if (GetItemState(nmcd.dwItemSpec, LVIS_SELECTED) == LVIS_SELECTED)
			{
				this_item_select = true;
				SetItemState(nmcd.dwItemSpec, 0, LVIS_SELECTED);
				lplvdr->clrText = m_theme_color.dark3;
				lplvdr->clrTextBk = m_theme_color.light2;
			}
			//设置偶数行的颜色
			else if (nmcd.dwItemSpec % 2 == 0)
			{
				lplvdr->clrText = CColorConvert::m_gray_color.dark3;
				lplvdr->clrTextBk = CColorConvert::m_gray_color.light3;
			}
			//设置奇数行的颜色
			else
			{
				lplvdr->clrText = CColorConvert::m_gray_color.dark3;
				lplvdr->clrTextBk = CColorConvert::m_gray_color.light4;
			}

			//用背景色填充单元格，以去掉每行前面的空白
			CRect rect = nmcd.rc;
			CDC* pDC = CDC::FromHandle(nmcd.hdc);		//获取绘图DC
			pDC->FillSolidRect(rect, lplvdr->clrTextBk);
		}
		else		//当控件被禁用时，显示文本设为灰色
		{
			lplvdr->clrText = GRAY(140);
			lplvdr->clrTextBk = GRAY(240);
		}
		*pResult = CDRF_NOTIFYPOSTPAINT;
		break;
	case CDDS_ITEMPOSTPAINT:
		if (this_item_select)
			SetItemState(nmcd.dwItemSpec, 0xFF, LVIS_SELECTED);
		//*pResult = CDRF_DODEFAULT;
		break;
	}
}


void CListCtrlEx::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	SetBkColor(m_background_color);
	//SetHightItem(-1);

	CListCtrl::PreSubclassWindow();
}


void CListCtrlEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	this->SetFocus();
	CListCtrl::OnLButtonDown(nFlags, point);
}


void CListCtrlEx::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	this->SetFocus();
	CListCtrl::OnRButtonDown(nFlags, point);
}
