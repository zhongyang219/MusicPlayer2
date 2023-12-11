#pragma once
#include "SongInfo.h"

#define FORMULAR_TITLE  L"%(Title)"
#define FORMULAR_ARTIST L"%(Artist)"
#define FORMULAR_ALBUM  L"%(Album)"
#define FORMULAR_TRACK  L"%(Track)"
#define FORMULAR_YEAR   L"%(Year)"
#define FORMULAR_GENRE  L"%(Genre)"
#define FORMULAR_COMMENT  L"%(Comment)"
#define FORMULAR_ORIGINAL L"%(Original)"

// CTagModeSelectDlg 对话框

class CTagModeSelectDlg : public CDialog
{
    DECLARE_DYNAMIC(CTagModeSelectDlg)

public:
    CTagModeSelectDlg(wstring title, bool original_str_disable, CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CTagModeSelectDlg();

    // DoModal之前外部设置一个Formular（可选），窗口初始化时会被设为当前选中
    void SetInitInsertFormular(const wstring& formular) { m_init_insert_formular = formular; };
    // DoModal之后获取当前选中的Formular
    wstring GetFormularSelected() const { return m_formular_selected; };

    // DoModal之后可用，使用用户选择的formular从文件名(参数不要输入扩展名)猜测标签
    void GetTagFromFileName(const wstring& file_name, SongInfo& song_info);
    // 根据formular从文件名(参数不要输入扩展名)猜测标签
    static void GetTagFromFileName(const wstring& formular, const wstring& file_name, SongInfo& song_info);
    // DoModal之后可用，使用用户选择的formular从标签生成文件名(不含扩展名)
    wstring FileNameFromTag(const SongInfo& song_info);
    // 根据formular字符串从标签生成文件名(不含扩展名)
    static wstring FileNameFromTag(wstring formular, const SongInfo& song_info);

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_TAG_MODE_SELECT_DIALOG };
#endif

protected:
    void SaveConfig() const;
    void LoadConfig();

    //初始化列表下拉列表，返回列表中添加的第一个项目
    wstring InitComboList();

    void InsertTag(const wchar_t* tag);

    void SetInsertWhenClicked(bool insert);
    bool IsInsertWhenClicked() const;

    bool IsStringContainsFormular(const wstring& str);
    void InsertFormular(const wstring& str_formular);


    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    wstring m_title_str;
    wstring m_init_insert_formular;
    wstring m_formular_selected;
    bool m_original_str_disable{};          // 不显示此窗口的所有%(Original)相关功能
    bool m_insert_when_clicked{};
    vector<wstring> m_default_formular;
    CComboBox m_format_combo;

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
    afx_msg void OnDestroy();
    virtual void OnOK();
};
