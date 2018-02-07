#include "stdafx.h"
#include "StaticEx.h"


CStaticEx::CStaticEx()
{
}

CStaticEx::~CStaticEx()
{
}

void CStaticEx::DrawWindowText(LPCTSTR lpszString)
{
	CDC* pDC = GetDC();
	pDC->SetBkMode(TRANSPARENT);
	pDC->SelectObject(this->GetFont());
	CRect rect;
	this->GetClientRect(&rect);
	DrawThemeParentBackground(m_hWnd, pDC->GetSafeHdc(), &rect);	//重绘控件区域以解决文字重叠的问题
	pDC->SetTextColor(m_TextColor);
	pDC->DrawText(lpszString, rect, (m_center ? DT_CENTER : 0) | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	ReleaseDC(pDC);
	m_text = lpszString;
}

void CStaticEx::DrawWindowText(LPCTSTR lpszString, int split)
{
	if (split < 0) split = 0;
	if (split > 1000) split = 1000;
	CDC* pDC = GetDC();
	pDC->SetBkMode(TRANSPARENT);
	pDC->SelectObject(this->GetFont());
	CRect rect;		//整个控件的矩形
	CSize text_size;	//文本的大小
	int text_top, text_left;		//输出文本的top和left位置
	this->GetClientRect(&rect);
	//设置绘图的剪辑区域，防止文字输出超出控件区域
	CRgn rgn;
	rgn.CreateRectRgnIndirect(rect);
	pDC->SelectClipRgn(&rgn);
	//获取文字的宽度和高度
	text_size = pDC->GetTextExtent(lpszString);
	//计算文字的起始坐标
	text_top = rect.top + (rect.Height() - text_size.cy) / 2;
	if (m_center)
		text_left = rect.left + (rect.Width() - text_size.cx) / 2;
	else
		text_left = 0;
	//计算背景文字和覆盖文字的矩形区域
	CRect text_rect{ CPoint{text_left, text_top}, text_size};		//背景文字的区域
	CRect text_f_rect{ CPoint{ text_left, text_top }, CSize{ text_size.cx * split / 1000, text_size.cy} };		//覆盖文字的区域
	//如果文本宽度大于控件宽度，就要根据分割的位置滚动文本
	if (text_size.cx > rect.Width())
	{
		//如果分割的位置（歌词进度）剩下的宽度已经小于控件宽度的一半，此时使文本右侧和控件右侧对齐
		if (text_rect.Width() - text_f_rect.Width() < rect.Width() / 2)
		{
			text_rect.MoveToX(rect.left-(text_rect.Width()-rect.Width()));
			text_f_rect.MoveToX(text_rect.left);
		}
		//分割位置剩下的宽度还没有到小于控件宽度的一半，但是分割位置的宽度已经大于控件宽度的一半时，需要移动文本使分割位置正好在控件的中间
		else if (text_f_rect.Width() > rect.Width() / 2)
		{
			text_rect.MoveToX(rect.left - (text_f_rect.Width() - rect.Width() / 2));
			text_f_rect.MoveToX(text_rect.left);
		}
		//分割位置还不到控件宽度的一半时，使文本左侧和控件左侧对齐
		else
		{
			text_rect.MoveToX(0);
			text_f_rect.MoveToX(0);
		}
	}

	DrawThemeParentBackground(m_hWnd, pDC->GetSafeHdc(), &rect);	//重绘控件区域以解决文字重叠的问题
	pDC->SetTextColor(m_TextBackColor);
	pDC->DrawText(lpszString, text_rect, DT_SINGLELINE | DT_NOPREFIX);		//绘制背景文字
	pDC->SetTextColor(m_TextColor);
	pDC->DrawText(lpszString, text_f_rect, DT_SINGLELINE | DT_NOPREFIX);		//绘制覆盖文字

	ReleaseDC(pDC);
	m_text = lpszString;
}

void CStaticEx::DrawScrollText(LPCTSTR lpszString, int pixel, bool reset)
{
	static int shift_cnt;		//移动的次数
	static bool shift_dir;		//移动的方向，右移为false，左移为true
	static int freez;			//当该变量大于0时，文本不滚动，直到小于等于0为止
	static bool dir_changed{ false };	//如果方向发生了变化，则为true
	if (reset)
	{
		shift_cnt = 0;
		shift_dir = false;
		freez = 20;
		dir_changed = false;
	}
	CDC* pDC = GetDC();
	pDC->SetTextColor(m_TextColor);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SelectObject(this->GetFont());
	CRect rect;		//整个控件的矩形
	CSize text_size;	//文本的大小
	int text_top, text_left;		//输出文本的top和left位置
	this->GetClientRect(&rect);
	//设置绘图的剪辑区域，防止文字输出超出控件区域
	CRgn rgn;
	rgn.CreateRectRgnIndirect(rect);
	pDC->SelectClipRgn(&rgn);
	//获取文字的宽度和高度
	text_size = pDC->GetTextExtent(lpszString);
	//计算文字的起始坐标
	text_top = rect.top + (rect.Height() - text_size.cy) / 2;
	if (m_center)
		text_left = rect.left + (rect.Width() - text_size.cx) / 2;
	else
		text_left = 0;
	//计算文字的矩形区域
	CRect text_rect{ CPoint{ text_left, text_top }, text_size };
	//如果文本宽度大于控件宽度，就滚动文本
	if (text_size.cx > rect.Width())
	{
		text_rect.MoveToX(rect.left - shift_cnt*pixel);
		if ((text_rect.right < rect.right || text_rect.left > rect.left))		//移动到边界时换方向
		{
			if (!dir_changed)
			{
				shift_dir = !shift_dir;
				freez = 20;		//变换方向时稍微暂停滚动一段时间
			}
			dir_changed = true;
		}
		else
		{
			dir_changed = false;
		}
	}
	DrawThemeParentBackground(m_hWnd, pDC->GetSafeHdc(), &rect);	//重绘控件区域以解决文字重叠的问题
	pDC->DrawText(lpszString, text_rect, DT_SINGLELINE | DT_NOPREFIX);
	ReleaseDC(pDC);
	if (freez <= 0)		//当freez为0的时候才滚动
	{
		if (shift_dir)
			shift_cnt--;
		else
			shift_cnt++;
	}
	else
	{
		freez--;
	}
	m_text = lpszString;
}

void CStaticEx::SetTextColor(COLORREF textColor)
{
	m_TextColor = textColor;
	//DrawWindowText(m_text);
}

void CStaticEx::SetText2Color(COLORREF textColor)
{
	m_TextBackColor = textColor;
}

CString CStaticEx::GetString() const
{
	return m_text;
}

void CStaticEx::SetFillColor(COLORREF fill_color)
{
	m_fill_color = fill_color;
	m_fill_color_enable = true;
	Invalidate();
}


LRESULT CStaticEx::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (message == WM_SETTEXT)
	{
		CRect rect;
		CDC* pDC = GetDC();
		GetClientRect(rect);
		DrawThemeParentBackground(m_hWnd, pDC->GetSafeHdc(), &rect);
		ReleaseDC(pDC);
	}
	return CStatic::DefWindowProc(message, wParam, lParam);
}
BEGIN_MESSAGE_MAP(CStaticEx, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()



void CStaticEx::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	GetWindowText(m_text);

	CStatic::PreSubclassWindow();
}


void CStaticEx::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CStatic::OnPaint()
	if (m_fill_color_enable)
	{
		CRect rect;
		GetClientRect(rect);
		dc.FillSolidRect(rect, m_fill_color);
	}
}
