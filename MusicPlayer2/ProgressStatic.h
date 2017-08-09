//一个从CStatic类派生的实现进度条功能的类
#pragma once
#include "Time.h"

// CProgressStatic

class CProgressStatic : public CStatic
{
	DECLARE_DYNAMIC(CProgressStatic)

public:
	CProgressStatic();
	virtual ~CProgressStatic();

	void SetRange(int range);
	void SetPos(int pos);
	int GetPos() const { return m_pos; }
	void SetColor(COLORREF color);
	void SetBackColor(COLORREF back_color);
	void SetProgressBarHeight(int height);
	void SetSongLength(int song_length) { m_song_length = song_length; }

protected:
	int m_range{ 1 };			//进度条的总长度
	int m_pos{};				//进度条当前的位置
	COLORREF m_progress_color{ RGB(124,248,128) };		//进度条的颜色
	COLORREF m_back_color{ RGB(255,255,255) };			//进度条背景色
	int m_progress_height{};			//进度条的高度
	int m_last_progress_length{ -1 };	//储存上一次进度条的长度
	int m_song_length;			//歌曲长度的毫秒数（用于在鼠标指向进度时计算定位到几分几秒）

	CToolTipCtrl m_toolTip;		//文本提示类

	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();
//	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void PreSubclassWindow();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


