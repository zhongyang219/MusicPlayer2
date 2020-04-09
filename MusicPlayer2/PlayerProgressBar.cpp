// PlayerProgressBar.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "PlayerProgressBar.h"
#include "DrawCommon.h"


// CPlayerProgressBar

IMPLEMENT_DYNAMIC(CPlayerProgressBar, CStatic)

CPlayerProgressBar::CPlayerProgressBar()
    : m_theme_color(theApp.m_app_setting_data.theme_color)
{
}

CPlayerProgressBar::~CPlayerProgressBar()
{
}


void CPlayerProgressBar::SetProgress(int progress)
{
    m_progress = progress;
    if (m_progress < 0)
        m_progress = 0;
    if (m_progress > 100)
        m_progress = 100;
    Invalidate();
}

void CPlayerProgressBar::SetBackgroundColor(COLORREF back_color)
{
    m_back_color = back_color;
}

void CPlayerProgressBar::SetBarCount(int bar_cnt)
{
    m_bar_count = bar_cnt;
}

BEGIN_MESSAGE_MAP(CPlayerProgressBar, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CPlayerProgressBar 消息处理程序



void CPlayerProgressBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CStatic::OnPaint()
    CRect rect;
    GetClientRect(rect);

    //双缓冲绘图
    CDrawDoubleBuffer drawDoubleBuffer(&dc, rect);
    CDrawCommon drawer;
    drawer.Create(drawDoubleBuffer.GetMemDC(), this);

    //开始绘图
    int gap_width = rect.Width() / m_bar_count / 4;
    if (gap_width < 1)
        gap_width = 1;
    drawer.FillRect(rect, m_back_color);
    CRect rc_tmp{ rect };
    rc_tmp.DeflateRect(theApp.DPI(1), theApp.DPI(1));
    //绘制进度条
    int bar_width = (rc_tmp.Width() - theApp.DPI(2) * 2) / m_bar_count;     //每一格的宽度
    if (bar_width < gap_width + 1)
        bar_width = gap_width + 1;
    int progress_width = bar_width * m_bar_count + theApp.DPI(2) * 2;
    rc_tmp.right = rect.left + progress_width;
    drawer.FillRect(rc_tmp, RGB(255, 255, 255));
    drawer.DrawRectOutLine(rc_tmp, m_theme_color.dark1, theApp.DPI(1), false);
    int bar_cnt = m_progress / (100 / m_bar_count) + 1;        //格子数
    int last_bar_percent = m_progress % (100 / m_bar_count);
    CRect rc_bar{ rc_tmp };
    rc_bar.DeflateRect(theApp.DPI(2), theApp.DPI(2));
    rc_bar.right = rc_bar.left + bar_width - gap_width;
    int start_x_pos = rc_bar.left;
    for (int i = 0; i < bar_cnt; i++)
    {
        rc_bar.MoveToX(start_x_pos + i * bar_width);
        if (i != bar_cnt - 1)
        {
            drawer.FillRect(rc_bar, m_theme_color.dark1, true);
        }
        else
        {
            BYTE alpha = ALPHA_CHG(last_bar_percent * m_bar_count);
            drawer.FillAlphaRect(rc_bar, m_theme_color.dark1, alpha, true);
        }
    }
}
