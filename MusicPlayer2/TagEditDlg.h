#pragma once
#include "BaseDialog.h"

// CTagEditDlg 对话框

class CTagEditDlg : public CBaseDialog
{
    DECLARE_DYNAMIC(CTagEditDlg)

public:
    CTagEditDlg(vector<SongInfo>& file_list, int index, CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CTagEditDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_TAG_EDIT_DIALOG };
#endif

protected:
    vector<SongInfo>& m_file_list;
    int m_index;
    CComboBox m_genre_combo;

    void ShowInfo();

    virtual CString GetDialogName() const override;
    virtual bool InitializeControls() override;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedPreviousButton();
    afx_msg void OnBnClickedNextButton();
    afx_msg void OnBnClickedSaveButton();
};
