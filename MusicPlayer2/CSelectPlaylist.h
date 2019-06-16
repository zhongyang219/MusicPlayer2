#pragma once
#include "TabDlg.h"
#include "ListCtrlEx.h"


// CSelectPlaylist 对话框
#define WM_PLAYLIST_SELECTED (WM_USER+119)

class CSelectPlaylist : public CTabDlg
{
	DECLARE_DYNAMIC(CSelectPlaylist)

public:
	CSelectPlaylist(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSelectPlaylist();

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
};
