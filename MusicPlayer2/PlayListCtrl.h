//播放列表专用的控件类，从CListCtrl派生，用于实现设置CListCtrl控件文字颜色
//以及鼠标提示信息
#pragma once
#include "AudioCommon.h"

// CPlayListCtrl

class CPlayListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CPlayListCtrl)

public:
	CPlayListCtrl(const vector<SongInfo>& all_song_info);
	virtual ~CPlayListCtrl();

	void SetHightItem(int item) { m_hight_item = item; }					//设置高亮的项目，即播放列表中正在播放的项目
	void SetColor(COLORREF TextColor, COLORREF TextBkColor, COLORREF selected_color, COLORREF other_text_color);	//设置播放列表的颜色

	void EnableTip(bool enable = true) { m_bEnableTips = enable; }		//设置是否开启提示

	static wstring GetDisplayStr(const SongInfo& song_info, DisplayFormat display_format);		//根据display_format指定的显示格式，返回一首曲目显示的字符串
	void ShowPlaylist(DisplayFormat display_format);		//显示播放列表

protected:
	COLORREF m_background_color{ RGB(255,255,255) };	//列表控件的背景色
	COLORREF m_text_color;		//高亮项目的文本颜色
	COLORREF m_back_color;		//高亮项目的背景颜色
	COLORREF m_selected_color;	//选中项目的颜色
	COLORREF m_other_text_color;	//非高亮项目的文本颜色
	int m_hight_item;			//高亮的项目

	CToolTipCtrl m_toolTip;		//文本提示类
	int m_nItem;				//存放行号
	//int m_nSubItem;			//存放列号
	bool m_bEnableTips{ false };	//是否开启文本提示

	const vector<SongInfo>& m_all_song_info;		//储存播放列表中所有歌曲的信息

	afx_msg void CPlayListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};


