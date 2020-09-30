#pragma once
#include "IPropertyTabDlg.h"


// CPropertyAdvancedDlg 对话框

class CPropertyAdvancedDlg : public CTabDlg, public IPropertyTabDlg
{
	DECLARE_DYNAMIC(CPropertyAdvancedDlg)

public:
	CPropertyAdvancedDlg(vector<SongInfo>& all_song_info, int& index, CWnd* pParent = nullptr);   // 标准构造函数
	CPropertyAdvancedDlg(vector<SongInfo>& all_song_info, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPropertyAdvancedDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPERTY_ADVANCED_DIALOG };
#endif

public:
    void AdjustColumnWidth();

protected:
    int& m_index;		//当前显示项目的曲目序号
    vector<SongInfo>& m_all_song_info;
    const bool m_batch_edit;
    int m_no_use{};

    CListCtrlEx m_list_ctrl;
    wstring m_selected_string;
    int m_item_selected{ -1 };

protected:
    virtual int SaveModified() override;
    virtual void PagePrevious() override;
    virtual void PageNext() override;
    virtual void OnTabEntered() override;      //当标签切换到当前窗口时被调用

    const SongInfo& CurrentSong();      //当前查看的歌曲信息
    void ShowInfo();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnCopyText();
    afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnInitMenu(CMenu* pMenu);
    afx_msg void OnCopyAllText();
};
