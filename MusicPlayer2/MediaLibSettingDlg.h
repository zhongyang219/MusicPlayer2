#pragma once
#include "TabDlg.h"
#include "CListBoxEnhanced.h"
#include "MyComboBox.h"
#include "SpinEdit.h"
#include "BrowseEdit.h"

// CMediaLibSettingDlg 对话框

class CMediaLibSettingDlg : public CTabDlg
{
    DECLARE_DYNAMIC(CMediaLibSettingDlg)

public:
    CMediaLibSettingDlg(CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CMediaLibSettingDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_MEDIA_LIB_SETTING_DIALOG };
#endif

    MediaLibSettingData m_data;

private:
    CListBoxEnhanced m_dir_list_ctrl;
    CMyComboBox m_playlist_display_mode_combo;
    CMyComboBox m_recent_played_range_combo;
    CSpinEdit m_file_too_short_sec_edit;
    CMyComboBox m_id3v2_type_combo;
    CButton m_enable_lastfm;
    CSpinEdit m_playlist_item_height_edit;

    CToolTipCtrl m_toolTip;
    size_t m_data_size{};                   // 数据文件的大小
    CStatic m_lastfm_status;
    CButton m_lastfm_login;
    CSpinEdit m_lastfm_least_perdur;
    CSpinEdit m_lastfm_least_dur;
    CButton m_lastfm_auto_scrobble;
    CSpinEdit m_lastfm_auto_scrobble_min;
    CStatic m_lastfm_cache_status;
    CButton m_lastfm_upload_cache;
    CButton m_lastfm_enable_https;
    CButton m_lastfm_enable_nowplaying;
    CBrowseEdit m_artist_split_ext_edit;

protected:
    void ShowDataSizeInfo();

    virtual bool InitializeControls() override;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    // 在点击选项设置对话框的“应用”或“确定”时CMusicPlayerDlg::ApplySettings()将所有m_data写入theApp前被调用
    // 这里是设置应用前最后的更新ui到m_data的机会，不需要实时更新到m_data的ui也可以只在这里获取
    virtual void GetDataFromUi() override;
    // 在点击选项设置对话框的“应用”时，时CMusicPlayerDlg::ApplySettings()将所有m_data写入theApp后被调用
    // 进行一些只有在CMusicPlayerDlg::ApplySettings()后才能做的ui更新
    // virtual void ApplyDataToUi() override;
    void UpdateLastFMStatus();
    void UpdateLastFMCacheStatus();
    afx_msg void OnTimer(UINT_PTR nIDEvent);

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedAddButton();
    afx_msg void OnBnClickedDeleteButton();
    afx_msg void OnBnClickedCleanDataFileButton();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnBnClickedClearRecentPlayedListBtn();
    virtual void OnOK();
    afx_msg void OnBnClickedRefreshMediaLibButton();
    afx_msg void OnBnClickedEnableLastfm();
    afx_msg void OnBnClickedLastfmLogin();
    afx_msg void OnBnClickedLastfmAutoScrobble();
    afx_msg void OnBnClickedLastfmUploadCache();
    afx_msg void OnBnClickedLastfmEnableHttps();
    afx_msg void OnBnClickedLastfmEnableNowplaying();
protected:
    afx_msg LRESULT OnEditBrowseChanged(WPARAM wParam, LPARAM lParam);
};
