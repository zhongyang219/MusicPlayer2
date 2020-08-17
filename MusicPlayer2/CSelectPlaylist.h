#pragma once
#include "TabDlg.h"
#include "ListCtrlEx.h"
#include "SearchEditCtrl.h"


// CSelectPlaylist 对话框
#define WM_PLAYLIST_SELECTED (WM_USER+119)

class CSelectPlaylistDlg : public CTabDlg
{
	DECLARE_DYNAMIC(CSelectPlaylistDlg)

public:
	CSelectPlaylistDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSelectPlaylistDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SELECT_PLAYLIST_DIALOG };
#endif

public:
    wstring GetSelPlaylistPath() const;
    int GetTrack() const;
    int GetPosition() const;
    bool IsPlaylistModified() const;
	void SetUpdateFlag();		//设置需要更新标志，当标签切换到此对话框时，会更新一次数据
    void AdjustColumnWidth();       //自动调整列表宽度

private:
    int m_row_selected{};
    //CMenu m_menu;
    bool m_playlist_modified{ false };
    CSearchEditCtrl m_search_edit;
    vector<int> m_search_result;			//储存快速搜索结果的歌曲序号
    bool m_searched{ false };				//是否处理搜索状态
    //CToolTipCtrl m_Mytip;
	bool m_update_flag{ false };

    enum
    {
        SPEC_PLAYLIST_NUM = 2       //特殊播放列表的个数（这里是2，默认播放列表和我喜欢的播放列表）
    };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    void QuickSearch(const wstring& key_words);		//根据关键字执行快速查找m_search_result中
    void SetHighlightItem();
    virtual void OnTabEntered() override;

	DECLARE_MESSAGE_MAP()
public:
    CListCtrlEx m_playlist_ctrl;

    virtual BOOL OnInitDialog();

private:
    void CalculateColumeWidth(vector<int>& width);
    void ShowPathList();
    void SetListRowData(int index, const PlaylistInfo& playlist_info);
    bool SelectValid() const;
    PlaylistInfo GetSelectedPlaylist() const;
    void SetButtonsEnable();
    bool SelectedCanPlay() const;

public:
    afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
    virtual void OnOK();
    afx_msg void OnBnClickedNewPlaylist();
    afx_msg void OnPlayPlaylist();
    afx_msg void OnRenamePlaylist();
    afx_msg void OnDeletePlaylist();
    afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnInitMenu(CMenu* pMenu);
    afx_msg void OnNewPlaylist();
    afx_msg void OnEnChangeSearchEdit();
    //afx_msg void OnBnClickedClearButton();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
    afx_msg LRESULT OnSearchEditBtnClicked(WPARAM wParam, LPARAM lParam);
};
