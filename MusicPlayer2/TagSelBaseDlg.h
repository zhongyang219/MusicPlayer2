#pragma once
#include "BaseDialog.h"

// CTagSelBaseDlg 对话框

class CTagSelBaseDlg : public CBaseDialog
{
    DECLARE_DYNAMIC(CTagSelBaseDlg)

public:
    CTagSelBaseDlg(bool original_str_disable, CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CTagSelBaseDlg();

    // DoModal之前外部设置一个Formular（可选），窗口初始化时会被设为当前选中
    void SetInitInsertFormular(const wstring& formular) { m_init_insert_formular = formular; };
    // DoModal之后获取当前选中的Formular
    wstring GetFormularSelected() const { return m_formular_selected; };

    // 根据formular从文件名(参数不要输入扩展名)猜测标签
    static void GetTagFromFileName(const wstring& formular, const wstring& file_name, SongInfo& song_info);
    // 根据formular字符串从标签生成文件名(不含扩展名)
    static wstring FileNameFromTag(wstring formular, const SongInfo& song_info);

    static const wstring FORMULAR_TITLE;
    static const wstring FORMULAR_ARTIST;
    static const wstring FORMULAR_ALBUM;
    static const wstring FORMULAR_TRACK;
    static const wstring FORMULAR_YEAR;
    static const wstring FORMULAR_GENRE;
    static const wstring FORMULAR_COMMENT;
    static const wstring FORMULAR_ORIGINAL;
    static const vector<wstring> default_formular;

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_TAG_MODE_SELECT_DIALOG };
#endif

private:
    void SaveConfig() const;
    void LoadConfig();

    // 使用m_default_formular初始化列表下拉列表，并选中第一项
    void InitComboList();
    bool IsStringContainsFormular(const wstring& str) const;
    void InsertFormular(const wstring& str_formular);
    void OnTagBtnClicked(const wstring& tag);

    wstring m_formular_selected;
    bool m_original_str_disable{};          // 不显示此窗口的所有%(Original)相关功能

    bool m_insert_when_clicked{};
    vector<wstring> m_default_formular;
protected:
    wstring m_init_insert_formular;
    CComboBox m_format_combo;

    // 从CBaseDialog继承的虚方法
    // virtual CString GetDialogName() const = 0;
    virtual bool IsRememberDialogSizeEnable() const override final { return false; };
    // 派生类需要覆写InitializeControls设置标题和IDC_INFO_STATIC控件文本后调用基类
    virtual bool InitializeControls() override;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedTitleButton();
    afx_msg void OnBnClickedArtistButton();
    afx_msg void OnBnClickedAlbumButton();
    afx_msg void OnBnClickedTrackButton();
    afx_msg void OnBnClickedYearButton();
    afx_msg void OnBnClickedGenreButton();
    afx_msg void OnBnClickedCommentButton();
    afx_msg void OnBnClickedOriginalButton();
    afx_msg void OnCbnSelchangeCombo1();
    afx_msg void OnBnClickedInsertRadio();
    afx_msg void OnBnClickedCopyRadio();
    afx_msg void OnDestroy();
    virtual void OnOK();
};
