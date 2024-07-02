#pragma once
#include "BaseDialog.h"
#include "CListBoxEnhanced.h"


// CCleanupRangeDlg 对话框

class CCleanupRangeDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CCleanupRangeDlg)

public:
	CCleanupRangeDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CCleanupRangeDlg();

    bool IsCleanFileNotExist() const { return m_clean_file_not_exist; }
    bool IsCleanFileNotInMediaLibDir() const { return m_clean_file_not_in_media_lib_dir; }
    bool IsCleanFileWrong() const { return m_clean_file_wrong; }
    bool IsCleanFileTooShort() const { return m_clean_file_too_short; }
    bool IsCleanFileNonMainInOsu() const { return m_clean_file_non_main_in_osu; };
    void SetCleanFileNonMainInOsuEnable(bool enable) { m_clean_file_non_main_in_osu_enable = enable; };

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLEAN_UP_RANGE_DIALOG };
#endif

protected:
    CListBoxEnhanced m_list_ctrl;
    bool m_clean_file_not_exist{};
    bool m_clean_file_not_in_media_lib_dir{};
    bool m_clean_file_wrong{};
    bool m_clean_file_too_short{};
    bool m_clean_file_non_main_in_osu{};
    bool m_clean_file_non_main_in_osu_enable{};

protected:
    virtual CString GetDialogName() const override;
    virtual bool InitializeControls() override;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
protected:
    afx_msg LRESULT OnListboxSelChanged(WPARAM wParam, LPARAM lParam);
};
