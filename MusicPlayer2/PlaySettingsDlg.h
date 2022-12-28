#pragma once
#include "ListCtrlEx.h"
#include "TabDlg.h"
#include "MyComboBox.h"
#include "SpinEdit.h"

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
	CButton m_stop_when_error_check;
	CButton m_auto_play_when_start_chk;
	CButton m_show_taskbar_progress_check;
	CButton m_show_play_state_icon_chk;
    CButton m_sound_fade_chk;
    CButton m_continue_when_switch_playlist_check;
    CMyComboBox m_output_device_combo;
	CListCtrlEx m_device_info_list;
    CButton m_bass_radio;
    CButton m_mci_radio;
    CButton m_ffmpeg_radio;
    CSpinEdit m_ffmpeg_cache_length;
    CSpinEdit m_ffmpeg_max_retry_count;
    CSpinEdit m_ffmpeg_url_retry_interval;
    CToolTipCtrl m_toolTip;
    CButton m_ffmpeg_enable_wasapi;
    CButton m_ffmpeg_enable_wasapi_exclusive;
    CSpinEdit m_ffmpeg_max_wait_time;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	void ShowDeviceInfo();
    void EnableControl();
    virtual void GetDataFromUi() override;
    virtual void ApplyDataToUi() override;

	DECLARE_MESSAGE_MAP()
public:
    virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedStopWhenError();
	afx_msg void OnBnClickedShowTaskbarProgress();
	afx_msg void OnCbnSelchangeOutputDeviceCombo();
	afx_msg void OnBnClickedAutoPlayWhenStartCheck();
	afx_msg void OnBnClickedShowPlayStateIconCheck();
    afx_msg void OnBnClickedSoundFadeCheck();
    afx_msg void OnBnClickedContinueWhenSwitchPlaylistCheck();
    afx_msg void OnBnClickedBassRadio();
    afx_msg void OnBnClickedMciRadio();
    afx_msg void OnBnClickedFfmpegRadio();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnNMClickFfmpegDownSyslink(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnBnClickedFfmpegEnableWasapi();
    afx_msg void OnBnClickedFfmpegEnableWasapiExclusive();
};
