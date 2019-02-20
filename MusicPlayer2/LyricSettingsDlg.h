#pragma once
#include "afxwin.h"
#include "FolderBrowserDlg.h"
#include "TabDlg.h"

// CLyricSettingsDlg 对话框

class CLyricSettingsDlg : public CTabDlg
{
	DECLARE_DYNAMIC(CLyricSettingsDlg)

public:
	CLyricSettingsDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLyricSettingsDlg();

	bool FontChanged() const { return m_font_changed; }

	//选项设置的数据
	LyricSettingData m_data;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LYRIC_SETTING_DIALOG };
#endif

protected:
	CButton m_karaoke_disp_check;
	CButton m_lyric_fuzzy_match_check;
	CButton m_lyric_double_line_chk;
	CButton m_show_album_cover_in_cortana_check;
	CButton m_cortana_icon_beat_check;
	CComboBox m_cortana_color_combo;
	CButton m_lyric_compatible_mode_chk;
	CButton m_keep_display_chk;

	CToolTipCtrl m_tool_tip;
	bool m_font_changed{ false };

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	//void SetCortanaControlEnable(bool enable);		//启用或禁用Cortana歌词显示相关的控件
	void EnableControl();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStopWhenError();
	afx_msg void OnBnClickedKaraokeDisp();
	afx_msg void OnBnClickedExploreLyricButton();
	afx_msg void OnBnClickedShowTaskbarProgress();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnBnClickedLyricFuzzyMatch();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CButton m_show_lyric_in_cortana_check;
	afx_msg void OnBnClickedShowLyricInCortana();
	//afx_msg void OnBnClickedSaveInOffsetTag();
	//afx_msg void OnBnClickedSaveInTimeTag();
	afx_msg void OnBnClickedLyricDoubleLineCheck();
	afx_msg void OnCbnSelchangeCortanaColorCombo();
	afx_msg void OnBnClickedShowAlbumCoverInCortana();
	afx_msg void OnBnClickedCortanaIconDeatCheck();
	afx_msg void OnBnClickedLyricCompatibleMode();
	afx_msg void OnBnClickedSetFont();
	afx_msg void OnBnClickedKeepDisplayCheck();
};
