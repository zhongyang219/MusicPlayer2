#pragma once
#include "TabDlg.h"
#include "ListCtrlEx.h"
#include "SearchEditCtrl.h"
#include "MediaLibTabDlg.h"


// CAllMediaDlg 对话框

class CAllMediaDlg : public CMediaLibTabDlg
{
	DECLARE_DYNAMIC(CAllMediaDlg)

public:
	enum DialogType
	{
		DT_ALL_MEDIA,			//所有曲目
		DT_RECENT_MEDIA			//最近播放的曲目
	};

public:
	CAllMediaDlg(CAllMediaDlg::DialogType type, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CAllMediaDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ALL_MEDIA_DIALOG };
#endif

public:
	void RefreshData();

protected:
	enum SongColumeIndex
	{
		COL_INDEX = 0,
		COL_TITLE,
		COL_ARTIST,
		COL_ALBUM,
		COL_TRACK,
		COL_GENRE,
		COL_BITRATE,
		COL_YEAR,
		COL_PATH,
		COL_LAST_PLAYED_TIME,
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

	DialogType m_type{};

protected:
    virtual void RefreshSongList() override;        //刷新选中行的信息
    virtual void OnTabEntered() override;
	void InitListData();
    void SetRowData(CListCtrlEx::RowData& row_data, const SongInfo& song);
	void UpdateListIndex();
	void ShowSongList();
	void QuickSearch(const wstring& key_word);		//根据关键字执行快速查找，将结果保存在m_list_data_searched中
	void SongListClicked(int index);
	void SetButtonsEnable(bool enable);

	virtual const CListCtrlEx& GetSongListCtrl() const override;
	virtual int GetItemSelected() const override;
	virtual const vector<int>& GetItemsSelected() const override;
	virtual void AfterDeleteFromDisk(const std::vector<SongInfo>& files) override;
	virtual int GetPathColIndex() const override;
	virtual wstring GetSelectedString() const override;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnOK() override;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHdnItemclickSongList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeSearchEdit();
	afx_msg void OnNMClickSongList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickSongList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkSongList(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	afx_msg LRESULT OnSearchEditBtnClicked(WPARAM wParam, LPARAM lParam);
public:
};
