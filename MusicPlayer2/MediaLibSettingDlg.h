#pragma once
#include "TabDlg.h"
#include "CListBoxEnhanced.h"
#include "afxwin.h"
#include "MyComboBox.h"


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
    CButton m_classify_other_chk;
    //CButton m_show_tree_tool_tips_chk;
    CListBoxEnhanced m_dir_list_ctrl;
    CButton m_update_media_lib_chk;
	CButton m_disable_drag_sort_chk;
	CMyComboBox m_playlist_display_mode_combo;
	CMyComboBox m_recent_played_range_combo;
    CButton m_ignore_exist_chk;
    CMyComboBox m_id3v2_type_combo;

    CToolTipCtrl m_toolTip;
    size_t m_data_size;		//数据文件的大小

protected:
    void ShowDataSizeInfo();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedClassifyOtherCheck();
    //afx_msg void OnBnClickedShowTreeToolTipsCheck();
    afx_msg void OnBnClickedAddButton();
    afx_msg void OnBnClickedDeleteButton();
    afx_msg void OnBnClickedUpdateMediaLibChk();
    afx_msg void OnBnClickedCleanDataFileButton();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedClearRecentPlayedListBtn();
	afx_msg void OnBnClickedDisableDrageSortCheck();
	afx_msg void OnCbnSelchangePlaylistDisplayModeOmbo();
	afx_msg void OnCbnSelchangeRecentPlayedRangeOmbo();
    afx_msg void OnBnClickedIgnoreExistCheck();
    virtual void OnOK();
    afx_msg void OnCbnSelchangeId3v2TypeCombo();
    afx_msg void OnBnClickedRefreshMediaLibButton();
    afx_msg void OnBnClickedDisableDeleteFromDiskCheck();
    afx_msg void OnBnClickedShowPlaylistTooltipCheck();
};
