#pragma once
#include "TabDlg.h"
#include "ListCtrlEx.h"


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

private:
    int m_row_selected{};
    CMenu m_menu;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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

public:
    afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
    virtual void OnOK();
    afx_msg void OnBnClickedNewPlaylist();
    afx_msg void OnPlayPlaylist();
    afx_msg void OnRenamePlaylist();
    afx_msg void OnDeletePlaylist();
    afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
};
