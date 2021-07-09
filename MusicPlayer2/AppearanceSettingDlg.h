#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "Common.h"
#include "DrawCommon.h"
#include "ColorStatic.h"
#include "TabDlg.h"
#include "SpinEdit.h"
#include "BrowseEdit.h"
#include "SliderCtrlEx.h"
#include "MyComboBox.h"

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

    const int PREVIEW_WIDTH = theApp.DPI(200);		//预览图的宽高
    const int PREVIEW_HEIGHT = theApp.DPI(40);

    const int ICON_X = theApp.DPI(46);		//预览图中图标的位置
    const int ICON_Y = theApp.DPI(12);

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_APPEREANCE_SETTING_DLG };
#endif

protected:
    CSliderCtrlEx m_transparency_slid;
    CColorStatic m_color_static;
    CColorStatic m_color_static1;
    CColorStatic m_color_static2;
    CColorStatic m_color_static3;
    CColorStatic m_color_static4;
    CColorStatic m_color_static5;
    CColorStatic m_color_static6;

    const COLORREF m_color1{ RGB(134,186,249) };		//天蓝色
    const COLORREF m_color2{ RGB(115,210,45) };			//绿色
    const COLORREF m_color3{ RGB(255,164,16) };			//橙色
    const COLORREF m_color4{ RGB(33,147,167) };			//青绿色
    const COLORREF m_color5{ RGB(249,153,197) };		//浅红色
    const COLORREF m_color6{ RGB(162,161,216) };		//淡紫色

    CToolTipCtrl m_toolTip;

    CButton m_follow_system_color_check;
    CSliderCtrlEx m_spectrum_height_slid;
    CButton m_show_album_cover_chk;
    CMyComboBox m_album_cover_fit_combo;
    CButton m_album_cover_as_background_chk;
    CButton m_show_spectrum_chk;
    CSliderCtrlEx m_back_transparency_slid;
    CButton m_use_out_image_chk;
    CButton m_background_gauss_blur_chk;
    CSliderCtrlEx m_gauss_blur_radius_sld;
    CButton m_lyric_background_chk;
    CButton m_dark_mode_chk;
    CButton m_use_inner_image_first_chk;
    CButton m_enable_background_chk;
    CButton m_low_freq_in_center_chk;
    CButton m_default_cover_hq_chk;
    CSpinEdit m_ui_refresh_interval_edit;
    CMyComboBox m_icon_select_combo;
    CButton m_notify_icon_auto_adapt_chk;
    CButton m_btn_round_corners_chk;

    CRect m_notify_icon_preview;
    CBitmap m_preview_dark;
    CBitmap m_preview_light;

    CBrowseEdit m_default_background_edit;
    CBrowseEdit m_album_cover_name_edit;

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    void SetTransparency();
    void ClickColor();
    static int SpectrumHeightChg(int value);		//使用二次函数将10~300范围内的值映射到0~100范围内
    static int SpectrumHeightRChg(int value);		//使用二次函数将0~100范围内的值映射到10~300范围内

    void SetControlEnable();
    void CalculateNotifyIconPreviewRect();

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
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
    afx_msg void OnBnClickedDefaultAlbumCoverHq();
    afx_msg void OnBnClickedRestoreDefaultButton();
    afx_msg void OnEnKillfocusUiIntervalEdit();
    afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnCbnSelchangeCombo1();
    afx_msg void OnPaint();
    afx_msg void OnBnClickedNotifyIconAutoAdaptCheck();
    afx_msg void OnBnClickedBtnRoundCornersCheck();
protected:
    afx_msg LRESULT OnEditBrowseChanged(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnBnClickedUseDesktopBackgroundCheck();
    afx_msg void OnBnClickedShowNextCheck();
    afx_msg void OnBnClickedShowFpsCheck();
};
