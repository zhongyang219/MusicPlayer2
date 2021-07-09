#pragma once


// CTagFromFileNameDlg 对话框

class CTagFromFileNameDlg : public CDialog
{
	DECLARE_DYNAMIC(CTagFromFileNameDlg)

public:
	CTagFromFileNameDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CTagFromFileNameDlg();

    void SetDialogTitle(LPCTSTR str_title);
    void SetInitInsertFormular(const wstring& str_formular);
    wstring GetFormularSelected() const;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAG_FROM_FILE_NAME_DIALOG };
#endif

protected:
    void SaveConfig() const;
    void LoadConfig();

    void InitComboList();

    void InsertTag(const wchar_t* tag);

    void SetInsertWhenClicked(bool insert);
    bool IsInsertWhenClicked() const;

    void InsertFormular(const wstring& str_formular);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

protected:
    bool m_insert_when_clicked{};
    vector<wstring> m_default_formular;
    wstring m_formular_selected;

    CString m_dlg_title;
    wstring m_init_insert_formular;

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedTitleButton();
    afx_msg void OnBnClickedArtistButton();
    afx_msg void OnBnClickedAlbumButton();
    afx_msg void OnBnClickedTrackButton();
    afx_msg void OnBnClickedYearButton();
    afx_msg void OnBnClickedGenreButton();
    CComboBox m_format_combo;
    afx_msg void OnDestroy();
    afx_msg void OnCbnSelchangeCombo1();
    virtual void OnOK();
    afx_msg void OnBnClickedCommentButton();
    afx_msg void OnBnClickedOriginalButton();
};
