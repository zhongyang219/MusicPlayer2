#pragma once
#include "CTabCtrlEx.h"
#include "CSelectPlaylist.h"
#include "MediaClassifyDlg.h"

// CMediaLibDlg 对话框

class CMediaLibDlg : public CDialog
{
	DECLARE_DYNAMIC(CMediaLibDlg)

public:
	CMediaLibDlg(int cur_tab = 0, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CMediaLibDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MEDIA_LIB_DIALOG };
#endif

public:
    CSetPathDlg m_path_dlg;
    CSelectPlaylistDlg m_playlist_dlg;
    CMediaClassifyDlg m_artist_dlg{ CMediaClassifier::CT_ARTIST };
    CMediaClassifyDlg m_album_dlg{ CMediaClassifier::CT_ALBUM };
    CMediaClassifyDlg m_genre_dlg{ CMediaClassifier::CT_GENRE };
    CMediaClassifyDlg m_year_dlg{ CMediaClassifier::CT_YEAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
    CTabCtrlEx m_tab_ctrl;
    CSize m_min_size{};
    int m_init_tab{};

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedPlaySelected();
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    virtual void OnCancel();
protected:
    afx_msg LRESULT OnPlaySelectedBtnEnable(WPARAM wParam, LPARAM lParam);
};
