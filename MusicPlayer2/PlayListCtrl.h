//播放列表专用的控件类，从CListCtrlEx派生，用于实现设置CListCtrl控件文字颜色
//以及鼠标提示信息
#pragma once
#include "AudioCommon.h"
#include "ColorConvert.h"
#include "ListCtrlEx.h"

// CPlayListCtrl

class CPlayListCtrl : public CListCtrlEx
{
	DECLARE_DYNAMIC(CPlayListCtrl)

public:
	CPlayListCtrl(const vector<SongInfo>& all_song_info);
	virtual ~CPlayListCtrl();

	void EnableTip(bool enable = true) { m_bEnableTips = enable; }		//设置是否开启提示

	static wstring GetDisplayStr(const SongInfo& song_info, DisplayFormat display_format);		//根据display_format指定的显示格式，返回一首曲目显示的字符串
	void ShowPlaylist(DisplayFormat display_format);		//显示播放列表

protected:

	CToolTipCtrl m_toolTip;		//文本提示类
	int m_nItem;				//存放行号
	//int m_nSubItem;			//存放列号
	bool m_bEnableTips{ false };	//是否开启文本提示

	const vector<SongInfo>& m_all_song_info;		//储存播放列表中所有歌曲的信息

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
};


