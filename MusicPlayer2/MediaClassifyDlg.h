#pragma once
#include "MediaLibHelper.h"
#include "ListCtrlEx.h"
#include "SearchEditCtrl.h"
#include "MediaLibTabDlg.h"


// CMediaClassifyDlg 对话框

class CMediaClassifyDlg : public CMediaLibTabDlg
{
	DECLARE_DYNAMIC(CMediaClassifyDlg)

public:
	CMediaClassifyDlg(CMediaClassifier::ClassificationType type, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CMediaClassifyDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MEDIA_CLASSIFY_DIALOG };
#endif

public:
	virtual void GetSongsSelected(std::vector<wstring>& song_list) const override;
	void RefreshData();
    bool SetLeftListSel(const wstring& item);

protected:
    enum SongColumeIndex
    {
        COL_TITLE = 0,
        COL_ARTIST,
        COL_ALBUM,
        COL_TRACK,
        COL_GENRE,
        COL_BITRATE,
        COL_PATH,
    };

protected:
    CListCtrlEx m_classify_list_ctrl;
    CListCtrlEx m_song_list_ctrl;
    CSearchEditCtrl m_search_edit;

    CMediaClassifier::ClassificationType m_type;
    CMediaClassifier m_classifer;
    CString m_classify_selected;        //左侧列表选中项的文本
    CString m_default_str;              //“艺术家”或“唱片集”为空的字符串
    bool m_searched{ false };           //是否处于搜索状态
    CMediaClassifier::MediaList m_search_result;  //保存搜索结果
	CListCtrlEx::ListData m_list_data_left;	//左侧列表数据
	CListCtrlEx::ListData m_list_data;	//右侧列表数据

    int last_selected_index{ -2 };
    std::vector<int> m_left_selected_items;   //左侧列表选中的项目的序号
    std::vector<int> m_right_selected_items;   //右侧列表选中的项目的序号
    int m_right_selected_item{ -1 };
    CString m_selected_string;
    bool m_left_selected{};                   //最后一次选中的是左侧还是右侧

    bool m_initialized{ false };

protected:
    virtual void RefreshSongList() override;
    void ShowClassifyList();
    void ShowSongList();
    CString GetClassifyListSelectedString(int index) const;
    void ClassifyListClicked(int index);
    void SongListClicked(int index);
    bool IsItemMatchKeyWord(const SongInfo& song, const wstring& key_word);
    bool IsItemMatchKeyWord(const wstring& str, const wstring& key_word);
    void QuickSearch(const wstring& key_word);		//根据关键字执行快速查找，将结果保存在m_search_result中
    void SetButtonsEnable();
    void SetButtonsEnable(bool enable);
    virtual void OnTabEntered() override;
	virtual bool _OnAddToNewPlaylist(std::wstring& playlist_path) override;       //执行添加到新建播放列表命令，成功返回true，playlist_path用于接收新播放列表的路径

    void CalculateClassifyListColumeWidth(std::vector<int>& width);

	virtual const CListCtrlEx& GetSongListCtrl() const override;
	virtual int GetItemSelected() const override;
	virtual const vector<int>& GetItemsSelected() const override;
	virtual void AfterDeleteFromDisk(const std::vector<SongInfo>& files) override;
	virtual int GetPathColIndex() const override;
	virtual wstring GetSelectedString() const override;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnNMClickClassifyList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMRClickClassifyList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnEnChangeMfceditbrowse1();
protected:
    afx_msg LRESULT OnSearchEditBtnClicked(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnNMClickSongList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMRClickSongList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMDblclkClassifyList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMDblclkSongList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnHdnItemclickSongList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDestroy();
};
