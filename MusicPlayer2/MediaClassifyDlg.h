#pragma once
#include "MediaLibHelper.h"
#include "ListCtrlEx.h"
#include "SearchEditCtrl.h"


// CMediaClassifyDlg 对话框

class CMediaClassifyDlg : public CTabDlg
{
	DECLARE_DYNAMIC(CMediaClassifyDlg)

public:
	CMediaClassifyDlg(CMediaClassifier::ClassificationType type, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CMediaClassifyDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MEDIA_CLASSIFY_DIALOG };
#endif

protected:
    CListCtrlEx m_classify_list_ctrl;
    CListCtrlEx m_song_list_ctrl;
    CSearchEditCtrl m_search_edit;

    CMediaClassifier::ClassificationType m_type;
    const CMediaClassifier& m_classifer;
    CString m_classify_selected;        //右侧列表选中项的文本
    CString m_default_str;              //“艺术家”或“唱片集”为空的字符串
    bool m_searched{ false };           //是否处于搜索状态
    std::map<std::wstring, std::vector<SongInfo>> m_search_result;  //保存搜索结果

    int last_selected_index{ -2 };

protected:
    void ShowClassifyList();
    void ShowSongList();
    void ClassifyListClicked(int index);
    bool IsItemMatchKeyWord(const SongInfo& song, const wstring& key_word);
    bool IsItemMatchKeyWord(const wstring& str, const wstring& key_word);
    void QuickSearch(const wstring& key_word);		//根据关键字执行快速查找，将结果保存在m_search_result中

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnNMClickClassifyList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMRClickClassifyList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnEnChangeMfceditbrowse1();
protected:
    afx_msg LRESULT OnSearchEditBtnClicked(WPARAM wParam, LPARAM lParam);
};
