// ProgressStatic.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "ProgressStatic.h"


// CProgressStatic

IMPLEMENT_DYNAMIC(CProgressStatic, CStatic)

CProgressStatic::CProgressStatic()
{
	m_toolTip.Create(this, TTS_ALWAYSTIP);
}

CProgressStatic::~CProgressStatic()
{
}


BEGIN_MESSAGE_MAP(CProgressStatic, CStatic)
	ON_WM_PAINT()
//	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()



// CProgressStatic 消息处理程序




void CProgressStatic::SetRange(int range)
{
	m_range = range;
}

void CProgressStatic::SetPos(int pos)
{
	m_pos = pos;
	CRect rect;
	this->GetClientRect(&rect);
	int length;
	length = rect.Width() * m_pos / m_range;	//获取进度条宽度的像素值
	//只有当进度条的进度需要改变时或进度条控件长度改变时才重新绘制，避免刷新过频繁
	if (length != m_last_progress_length)
	{
		m_last_progress_length = length;
		Invalidate();
	}
}

void CProgressStatic::SetColor(COLORREF color)
{
	m_progress_color = color;
	Invalidate();
}

void CProgressStatic::SetBackColor(COLORREF back_color)
{
	m_back_color = back_color;
	Invalidate();
}

void CProgressStatic::SetProgressBarHeight(int height)
{
	m_progress_height = height;
	Invalidate();
}

void CProgressStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CStatic::OnPaint()
	CRect rect;
	this->GetClientRect(&rect);
	int max_length = rect.Width();

	////由于非windows10时为主窗口设置了WS_CLIPCHILDREN属性，所以这里需要手动重绘控件区域
	//if (!CWinVersionHelper::IsWindows10OrLater())
	//	DrawThemeParentBackground(m_hWnd, dc.GetSafeHdc(), &rect);	//重绘控件区域

	//将矩形的高度更改为进度条显示的高度
	if (m_progress_height < rect.Height() && m_progress_height > 0)
	{
		rect.top = (rect.Height() - m_progress_height) / 2;
		rect.bottom = rect.top + m_progress_height;
	}
	//画背景颜色
	CBrush BGBrush, *pOldBrush;
	BGBrush.CreateSolidBrush(m_back_color);
	pOldBrush = dc.SelectObject(&BGBrush);
	dc.FillRect(&rect, &BGBrush);
	dc.SelectObject(pOldBrush);
	BGBrush.DeleteObject();
	//获取进度条的长度
	int length;
	length = rect.Width() * m_pos / m_range;
	if (length > max_length) length = max_length;
	rect.right = rect.left + length;
	//画进度条颜色
	if (!rect.IsRectEmpty())
	{
		CBrush GBrush;
		GBrush.CreateSolidBrush(m_progress_color);
		pOldBrush = dc.SelectObject(&GBrush);
		dc.FillRect(&rect, &GBrush);
		dc.SelectObject(pOldBrush);
		GBrush.DeleteObject();
	}
}


//void CProgressStatic::OnLButtonUp(UINT nFlags, CPoint point)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	CRect rect;
//	this->GetClientRect(&rect);
//	m_pos = point.x*m_range / rect.Width();		//鼠标点击后将进度条设置到鼠标点击的位置
//	Invalidate();
//	CStatic::OnLButtonUp(nFlags, point);
//}


void CProgressStatic::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	//为控件设置SS_NOTIFY属性
	DWORD dwStyle = GetStyle();
	::SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);

	m_toolTip.AddTool(this, CCommon::LoadText(IDS_SEEK_TO));

	CStatic::PreSubclassWindow();
}


void CProgressStatic::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rect;
	this->GetClientRect(&rect);
	m_pos = point.x*m_range / rect.Width();		//鼠标点击后将进度条设置到鼠标点击的位置
	Invalidate();

	CStatic::OnLButtonDown(nFlags, point);
}


BOOL CProgressStatic::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(32649)));	//将鼠标指向进度条的指针设置为手型图标
	return TRUE;
	//return CStatic::OnSetCursor(pWnd, nHitTest, message);
}


void CProgressStatic::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//鼠标指向进度条时显示定位到几分几秒
	CRect rect;
	this->GetClientRect(&rect);
	__int64 song_pos;
	song_pos = static_cast<__int64>(point.x) * m_song_length / rect.Width();
	Time song_pos_time;
	song_pos_time.int2time(static_cast<int>(song_pos));
	CString str;
	static int last_sec{};
	if (last_sec != song_pos_time.sec)		//只有鼠标指向位置对应的秒数变化了才更新鼠标提示
	{
		str.Format(CCommon::LoadText(IDS_SEEK_TO_MINUTE_SECOND), song_pos_time.min, song_pos_time.sec);
		m_toolTip.UpdateTipText(str, this);
		last_sec = song_pos_time.sec;
	}

	CStatic::OnMouseMove(nFlags, point);
}


BOOL CProgressStatic::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (m_toolTip.GetSafeHwnd() && pMsg->message == WM_MOUSEMOVE)
	{
		m_toolTip.RelayEvent(pMsg);
	}


	return CStatic::PreTranslateMessage(pMsg);
}
