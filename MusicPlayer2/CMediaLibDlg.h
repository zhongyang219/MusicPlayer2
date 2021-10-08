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
    CMediaClassifyDlg m_rating_dlg{ CMediaClassifier::CT_RATING };
    CAllMediaDlg m_all_media_dlg{ CAllMediaDlg::DT_ALL_MEDIA };
    CAllMediaDlg m_recent_media_dlg{ CAllMediaDlg::DT_RECENT_MEDIA };
    CFolderExploreDlg m_folder_explore_dlg;

    void SetCurTab(int tab);

    //设置要强制显示出来的对话框（即使媒体库设置中为显示此标签）
    //tabs: 要显示出来的标签，它不是标签的序号，而是多个bit的组合，用于表示多个标签，每个bit的含义为MediaLibDisplayItem枚举的常量
    void SetTabForceShow(int tabs) { m_tab_show_force = tabs; }

    bool NavigateToItem(const wstring& item);       //跳转到对应项目，例如当前显示的是艺术家标签，则跳转到对应艺术家

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
    int m_tab_show_force{};     //要强制显示出来的标签

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
    afx_msg void OnBnClickedStatisticsInfoButton();
};
