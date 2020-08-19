#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "Common.h"
#include "DrawCommon.h"
#include "ColorStatic.h"
#include "TabDlg.h"
#include "SpinEdit.h"
#include "BrowseEdit.h"

// CAppearanceSettingDlg 对话框

class CAppearanceSettingDlg : public CTabDlg
{
	DECLARE_DYNAMIC(CAppearanceSettingDlg)

public:
	HWND m_hMainWnd;		//主窗口的句柄，用于实时更改窗口不透明度

	ApperanceSettingData m_data;

	CAppearanceSettingDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAppearanceSettingDlg();

	void DrawColor();

    const int MIM_LINE_SPACE = 0;		//歌词行间距设定的最小值
    const int MAX_LINE_SPACE = 40;		//歌词行间距设定的最大值

    const int PREVIEW_WIDTH = theApp.DPI(200);		//预览图的宽高
    const int PREVIEW_HEIGHT = theApp.DPI(40);

    const int ICON_X = theApp.DPI(46);		//预览图中图标的位置
    const int ICON_Y = theApp.DPI(12);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_APPEREANCE_SETTING_DLG };
#endif

protected:
	bool m_font_changed{ false };
	CSliderCtrl m_transparency_slid;
	CColorStatic m_color_static;
	CColorStatic m_color_static1;
	CColorStatic m_color_static2;
	CColorStatic m_color_static3;
	CColorStatic m_color_static4;
	CColorStatic m_color_static5;
	CColorStatic m_color_static6;

	const COLORREF m_color1{ RGB(123,189,255) };		//天蓝色
	const COLORREF m_color2{ RGB(115,210,45) };			//绿色
	const COLORREF m_color3{ RGB(255,167,87) };			//橙色
	const COLORREF m_color4{ RGB(33,147,167) };			//青绿色
	const COLORREF m_color5{ RGB(255,162,208) };		//浅红色
	const COLORREF m_color6{ RGB(168,152,222) };		//淡紫色

	CToolTipCtrl m_toolTip;

	CButton m_follow_system_color_check;
	CSliderCtrl m_spectrum_height_slid;
	CButton m_show_album_cover_chk;
	CComboBox m_album_cover_fit_combo;
	CButton m_album_cover_as_background_chk;
	CButton m_show_spectrum_chk;
	CSliderCtrl m_back_transparency_slid;
	CButton m_use_out_image_chk;
	CButton m_background_gauss_blur_chk;
	CSliderCtrl m_gauss_blur_radius_sld;
	CButton m_lyric_background_chk;
	CButton m_dark_mode_chk;
	CButton m_use_inner_image_first_chk;
    CButton m_enable_background_chk;
    CButton m_low_freq_in_center_chk;
    CComboBox m_alignment_combo;
    CButton m_default_cover_hq_chk;
    CSpinEdit m_lyric_line_space_edit;
    CSpinEdit m_ui_refresh_interval_edit;
    CComboBox m_icon_select_combo;
    CButton m_notify_icon_auto_adapt_chk;
    CButton m_btn_round_corners_chk;

    CRect m_notify_icon_preview;
    CBitmap m_preview_dark;
    CBitmap m_preview_light;

    CBrowseEdit m_default_background_edit;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	void SetTransparency();
	void ClickColor();
	static int SpectrumHeightChg(int value);		//使用二次函数将10~300范围内的值映射到0~100范围内
	static int SpectrumHeightRChg(int value);		//使用二次函数将0~100范围内的值映射到10~300范围内

	void SetControlEnable();
    void CalculateNotifyIconPreviewRect();

	DECLARE_MESSAGE_MAP()
public:
	bool FontChanged()const { return m_font_changed; }

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSetFontButton();
//	afx_msg void OnNMReleasedcaptureTransparentSlider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedSetThemeButton();
	afx_msg void OnStnClickedColorStatic2();
	afx_msg void OnStnClickedColorStatic3();
	afx_msg void OnStnClickedColorStatic4();
	afx_msg void OnStnClickedColorStatic5();
	afx_msg void OnStnClickedColorStatic6();
	afx_msg void OnStnClickedColorStatic7();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedFollowSystemColorCheck();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedShowAlbumCoverCheck();
	afx_msg void OnCbnSelchangeAlbumFitCombo();
	afx_msg void OnBnClickedAlbumCoverBackgroundCheck();
	afx_msg void OnBnClickedShowSpectrumCheck();
	afx_msg void OnBnClickedUseOutImageCheck();
	afx_msg void OnEnChangeDefaultCoverNameEdit();
	afx_msg void OnBnClickedBackgroundGaussBlurCheck();
	afx_msg void OnBnClickedLyricBackgroundCheck();
	afx_msg void OnBnClickedDarkModeCheck();
	afx_msg void OnBnClickedUseInnerImageFirstCheck();
    afx_msg void OnBnClickedEnableBackgroundCheck();
    afx_msg void OnBnClickedLowFreqInCenterCheck();
    afx_msg void OnCbnSelchangeAlignmentCombo();
    afx_msg void OnBnClickedDefaultAlbumCoverHq();
    afx_msg void OnBnClickedRestoreDefaultButton();
    afx_msg void OnEnKillfocusUiIntervalEdit();
    afx_msg void OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnCbnSelchangeCombo1();
    afx_msg void OnPaint();
    afx_msg void OnBnClickedNotifyIconAutoAdaptCheck();
    afx_msg void OnBnClickedBtnRoundCornersCheck();
protected:
    afx_msg LRESULT OnEditBrowseChanged(WPARAM wParam, LPARAM lParam);
};
