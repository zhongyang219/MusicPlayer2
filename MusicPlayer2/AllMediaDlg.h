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

public:
	void GetSongsSelected(std::vector<wstring>& song_list) const;
	void GetSongsSelected(std::vector<SongInfo>& song_list) const;
	void GetCurrentSongList(std::vector<SongInfo>& song_list) const;

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
		COL_MAX,
	};

protected:
	CListCtrlEx m_song_list_ctrl;
	CSearchEditCtrl m_search_edit;

	CListCtrlEx::ListData m_list_data;
	CListCtrlEx::ListData m_list_data_searched;

	bool m_initialized{ false };
	bool m_searched{ false };           //是否处于搜索状态
	std::vector<int> m_selected_items;   //列表选中的项目的序号
	int m_selected_item{ -1 };
	CString m_selected_string;

protected:
	virtual void OnTabEntered() override;
	void InitListData();
	void UpdateListIndex();
	void ShowSongList();
	void QuickSearch(const wstring& key_word);		//根据关键字执行快速查找，将结果保存在m_list_data_searched中
	void SongListClicked(int index);
	void SetButtonsEnable(bool enable);
	bool _OnAddToNewPlaylist(std::wstring& playlist_path);       //执行添加到新建播放列表命令，成功返回true，playlist_path用于接收新播放列表的路径

	static UINT ViewOnlineThreadFunc(LPVOID lpParam);	//执行在线查看的线程函数

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHdnItemclickSongList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeSearchEdit();
	afx_msg void OnNMClickSongList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickSongList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkSongList(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void OnOK();
protected:
	afx_msg LRESULT OnSearchEditBtnClicked(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg void OnPlayItem();
	afx_msg void OnPlayItemInFolderMode();
	afx_msg void OnAddToNewPlaylist();
	afx_msg void OnAddToNewPalylistAndPlay();
	afx_msg void OnExploreOnline();
	afx_msg void OnExploreTrack();
	afx_msg void OnFormatConvert();
	afx_msg void OnDeleteFromDisk();
	afx_msg void OnItemProperty();
	afx_msg void OnCopyText();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnCancel();
};
