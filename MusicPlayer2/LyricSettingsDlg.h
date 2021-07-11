#pragma once
#include "afxwin.h"
#include "FolderBrowserDlg.h"
#include "TabDlg.h"
#include "ColorStaticEx.h"
#include "DesktopLyric.h"
#include "BrowseEdit.h"
#include "MyComboBox.h"
#include "SliderCtrlEx.h"
#include "SpinEdit.h"

// CLyricSettingsDlg 对话框

class CLyricSettingsDlg : public CTabDlg
{
    DECLARE_DYNAMIC(CLyricSettingsDlg)

public:
    CLyricSettingsDlg(CWnd* pParent = NULL);   // 标准构造函数
    virtual ~CLyricSettingsDlg();

    bool SearchBoxFontChanged() const { return m_search_box_font_changed; }
    bool FontChanged()const { return m_font_changed; }

    LyricSettingData m_data;	//选项设置的数据
    CDesktopLyric* m_pDesktopLyric = nullptr;

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_LYRIC_SETTING_DIALOG };
#endif

    const int MIM_LINE_SPACE = 0;		//歌词行间距设定的最小值
    const int MAX_LINE_SPACE = 40;		//歌词行间距设定的最大值

protected:
    CButton m_karaoke_disp_check;
    CButton m_lyric_fuzzy_match_check;
    CButton m_use_inner_lyric_chk;
    CButton m_lyric_double_line_chk;
    CButton m_show_album_cover_in_cortana_check;
    CButton m_cortana_icon_beat_check;
    CMyComboBox m_cortana_color_combo;
    CButton m_lyric_compatible_mode_chk;
    CButton m_keep_display_chk;
    CButton m_show_spectrum_chk;
    CButton m_show_lyric_in_cortana_chk;
    CButton m_show_desktop_lyric_chk;
    CBrowseEdit m_lyric_dir_edit;
    CMyComboBox m_lyric_save_policy_combo;
    CButton m_cortana_info_enable_check;
    //CButton m_search_box_opaque_chk;

    CButton m_desktop_lyric_double_line_chk;
    CColorStaticEx m_text_color1_static;
    CColorStaticEx m_text_color2_static;
    CMyComboBox m_text_gradient_combo;
    CColorStaticEx m_highlight_color1_static;
    CColorStaticEx m_highlight_color2_static;
    CMyComboBox m_highlight_gradient_combo;
    CSliderCtrlEx m_desktop_lyric_opacity_sld;
    CButton m_lock_desktop_lyric_chk;
    CButton m_hide_lyric_without_lyric_chk;
    CButton m_hide_lyric_paused_chk;
    CButton m_lyric_background_penetrate_chk;
    CMyComboBox m_alignment_combo;
    CButton m_show_unlock_when_locked_chk;
    CButton m_show_default_album_icon_chk;
    CMyComboBox m_desktop_lyric_alignment_combo;

    CSpinEdit m_lyric_line_space_edit;
    CMyComboBox m_lyric_alignment_combo;

    CToolTipCtrl m_tool_tip;
    bool m_search_box_font_changed{ false };
    bool m_font_changed{ false };

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual BOOL OnInitDialog();
    void EnableControl();
    void EnableControlForDesktopLyric();
    void ApplyDefaultLyricStyle(const LyricStyleDefaultData& style);

    void SetSearchBoxTransparentInWhiteTheme(bool transparent);
    bool IsSearchBoxTransparentInWhiteTheme() const;

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedKaraokeDisp();
    //afx_msg void OnBnClickedExploreLyricButton();
    virtual void OnCancel();
    virtual void OnOK();
    afx_msg void OnBnClickedLyricFuzzyMatch();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnBnClickedShowLyricInCortana();
    afx_msg void OnBnClickedLyricDoubleLineCheck();
    afx_msg void OnCbnSelchangeCortanaColorCombo();
    afx_msg void OnBnClickedShowAlbumCoverInCortana();
    afx_msg void OnBnClickedCortanaIconDeatCheck();
    afx_msg void OnBnClickedLyricCompatibleMode();
    afx_msg void OnBnClickedSetFont();
    afx_msg void OnBnClickedKeepDisplayCheck();
    afx_msg void OnBnClickedShowSpectrumInCortana();
    afx_msg void OnBnClickedShowLyricInCortana2();
    //afx_msg void OnBnClickedSearchBoxOpaqueCheck();
    afx_msg void OnBnClickedShowDesktopLyric();
    afx_msg void OnBnClickedSetFont2();
    afx_msg void OnCbnSelchangeTextGradientCombo();
    afx_msg void OnCbnSelchangeHighlightGradientCombo();
    afx_msg void OnBnClickedLockDesktopLyricCheck();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnBnClickedHideLyricWithoutLyricCheck();
    afx_msg void OnBnClickedHideLyricPauseCheck();
    afx_msg void OnBnClickedLyricDoubleLineCheck2();
    afx_msg void OnBnClickedDefaultStyle();
    afx_msg void OnLyricDefaultStyle1();
    afx_msg void OnLyricDefaultStyle2();
    afx_msg void OnLyricDefaultStyle3();
    afx_msg void OnBnClickedHideLyricWithoutLyricCheck3();
protected:
    afx_msg LRESULT OnColorSelected(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnLyricDefaultStyle1Modify();
    afx_msg void OnLyricDefaultStyle2Modify();
    afx_msg void OnLyricDefaultStyle3Modify();
    afx_msg void OnRestoreDefaultStyle();
    //    afx_msg void OnEnChangeLyricPathEdit();
protected:
    afx_msg LRESULT OnEditBrowseChanged(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnCbnSelchangeAlignmentCombo();
    afx_msg void OnCbnSelchangeLyricSaveCombo();
    afx_msg void OnBnClickedUseInnerLyricCheck();
    afx_msg void OnBnClickedShowUnlockWhenLocked();
    afx_msg void OnBnClickedShowDefaultAlbumIconChk();
    afx_msg void OnBnClickedSearchBoxTransparentInWhiteMode();
    afx_msg void OnBnClickedSetFontButton();
    afx_msg void OnCbnSelchangeAlignmentCombo2();
    afx_msg void OnCbnSelchangeDesktopLyricAlignmentCombo();
};
