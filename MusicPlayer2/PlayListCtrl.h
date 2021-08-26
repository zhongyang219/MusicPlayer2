//播放列表专用的控件类，从CListCtrlEx派生，用于实现设置CListCtrl控件文字颜色
//以及鼠标提示信息
#pragma once
#include "AudioCommon.h"
#include "ColorConvert.h"
#include "ListCtrlEx.h"
#include "Common.h"

// CPlayListCtrl

class CPlayListCtrl : public CListCtrlEx
{
	DECLARE_DYNAMIC(CPlayListCtrl)

public:
	CPlayListCtrl(const vector<SongInfo>& all_song_info);
	virtual ~CPlayListCtrl();

	static wstring GetDisplayStr(const SongInfo& song_info, DisplayFormat display_format);		//根据display_format指定的显示格式，返回一首曲目显示的字符串
	void ShowPlaylist(DisplayFormat display_format, bool search_result = false);		//显示播放列表
	void QuickSearch(const wstring& key_words);		//根据关键字执行快速查找，查找文件名、歌曲标题、艺术家和唱片集，将找到的曲目的序号保存在m_search_result中
	void GetItemSelectedSearched(vector<int>& item_selected);		//获取处于搜索状态下播放列表选中的项目

	void AdjustColumnWidth();

    virtual bool SetRowHeight(int height) override;

protected:

	CToolTipCtrl m_toolTip;		//文本提示类
	int m_nItem;				//存放行号
	//int m_nSubItem;			//存放列号

	const vector<SongInfo>& m_all_song_info;		//储存播放列表中所有歌曲的信息
	vector<int> m_search_result;					//储存快速搜索结果的歌曲序号
	bool m_searched{ false };
	ListData m_list_data;

protected:
	void CalculateColumeWidth(vector<int>& width);

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};
