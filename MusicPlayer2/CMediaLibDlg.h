#pragma once
#include "CTabCtrlEx.h"
#include "CSelectPlaylist.h"
#include "MediaClassifyDlg.h"
#include "FolderExploreDlg.h"
#include "AllMediaDlg.h"

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
    CMediaClassifyDlg m_type_dlg{ CMediaClassifier::CT_TYPE };
    CMediaClassifyDlg m_bitrate_dlg{ CMediaClassifier::CT_BITRATE };
	CAllMediaDlg m_all_media_dlg{ CAllMediaDlg::DT_ALL_MEDIA };
	CAllMediaDlg m_recent_media_dlg{ CAllMediaDlg::DT_RECENT_MEDIA };
    CFolderExploreDlg m_folder_explore_dlg;

protected:
    void SaveConfig() const;
    void LoadConfig();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
    CTabCtrlEx m_tab_ctrl;
    CSize m_min_size{};
    int m_init_tab{};
    CSize m_window_size{};

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedPlaySelected();
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    virtual void OnCancel();
protected:
    afx_msg LRESULT OnPlaySelectedBtnEnable(WPARAM wParam, LPARAM lParam);
public:
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    virtual void OnOK();
    afx_msg void OnBnClickedMediaLibSettingsBtn();
    afx_msg void OnSize(UINT nType, int cx, int cy);
};
