#pragma once
#include "TabDlg.h"
#include "ListCtrlEx.h"
#include "MyComboBox.h"
#include "SpinEdit.h"
#include "BrowseEdit.h"

// CPlaySettingsDlg 对话框

class CPlaySettingsDlg : public CTabDlg
{
    DECLARE_DYNAMIC(CPlaySettingsDlg)

public:
    CPlaySettingsDlg(CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CPlaySettingsDlg();

    PlaySettingData m_data;

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_PLAY_SETTING_DIALOG };
#endif
protected:
    //控件变量
    CMyComboBox m_output_device_combo;
    CListCtrlEx m_device_info_list;
    CSpinEdit m_ffmpeg_cache_length;
    CSpinEdit m_ffmpeg_max_retry_count;
    CSpinEdit m_ffmpeg_url_retry_interval;
    CToolTipCtrl m_toolTip;
    CSpinEdit m_ffmpeg_max_wait_time;
    CBrowseEdit m_sf2_path_edit;

protected:
    void ShowDeviceInfo();
    void EnableControl();

    virtual void GetDataFromUi() override;
    virtual void ApplyDataToUi() override;
    virtual bool InitializeControls() override;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnCbnSelchangeOutputDeviceCombo();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnNMClickFfmpegDownSyslink(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnBnClickedFfmpegEnableWasapi();
    afx_msg void OnBnClickedMidiUseInnerLyricCheck();
    afx_msg void OnEnChangeSf2PathEdit();
protected:
    afx_msg LRESULT OnEditBrowseChanged(WPARAM wParam, LPARAM lParam);
};
