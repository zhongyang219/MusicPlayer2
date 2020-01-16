#pragma once
#include "TabDlg.h"
#include "ListCtrlEx.h"
#include "SearchEditCtrl.h"


// CAllMediaDlg 对话框

class CAllMediaDlg : public CTabDlg
{
	DECLARE_DYNAMIC(CAllMediaDlg)

public:
	CAllMediaDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CAllMediaDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ALL_MEDIA_DIALOG };
#endif

protected:
	enum SongColumeIndex
	{
		COL_INDEX = 0,
		COL_TITLE,
		COL_ARTIST,
		COL_ALBUM,
		COL_TRACK,
		COL_GENRE,
		COL_YEAR,
		COL_PATH,
	};

protected:
	CListCtrlEx m_song_list_ctrl;
	CSearchEditCtrl m_search_edit;

	CListCtrlEx::ListData m_list_data;
	CListCtrlEx::ListData m_list_data_searched;

	bool m_initialized{ false };

protected:
	virtual void OnTabEntered() override;
	void InitListData();
	void ShowSongList();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
