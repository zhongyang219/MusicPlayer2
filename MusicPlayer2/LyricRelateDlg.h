#pragma once
#include "ListCtrlEx.h"
#include "BaseDialog.h"


// CLyricRelateDlg 对话框

class CLyricRelateDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CLyricRelateDlg)

public:
	CLyricRelateDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CLyricRelateDlg();

    //wstring GetRelatedLyricFile() const { return m_related_lyric; }

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LYRIC_RELATE_DIALOG };
#endif

private:
    CButton m_fuzzy_match_chk;
    CListCtrlEx m_result_list;

    vector<wstring> m_search_result;
    //wstring m_related_lyric;

private:
    void ShowSearchResult();                        //刷新列表
    void AddListRow(const wstring& lyric_path);     //向列表添加一行
    wstring GetListRow(int index);                  //获取列表中一行的歌词路径
    void SearchLyrics();
    void EnableControls(bool enable);

protected:
    virtual CString GetDialogName() const override;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedLocalSearchButton();
    afx_msg void OnBnClickedBrowseButton1();
    afx_msg void OnBnClickedDeleteFileButton();
    afx_msg void OnBnClickedDonotRelateButton();
    virtual void OnOK();
    afx_msg void OnLvnItemchangedSearchResultList(NMHDR *pNMHDR, LRESULT *pResult);
};
