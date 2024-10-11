#pragma once
#include "afxdialogex.h"
#include "SearchEditCtrl.h"

// CUiSearchBox 对话框

namespace UiElement
{
    class SearchBox;
}

class CUiSearchBox : public CDialog
{
	DECLARE_DYNAMIC(CUiSearchBox)

public:
	CUiSearchBox(CWnd* pParent);   // 标准构造函数
    void Create();
	virtual ~CUiSearchBox();
    void Show(UiElement::SearchBox* ui_search_box, bool big_icon);     //显示搜索框。ui_search_box：对应的UI中的搜索框
    void Clear();           //清除搜索状态
    void UpdatePos();       //当父窗口大小或位置变化时调用此函数以同步搜索框的位置（暂时未使用）

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UI_SEARCH_BOX_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    CSearchEditCtrl m_search_box;
    virtual void OnCancel();

private:
    UiElement::SearchBox* m_ui_search_box{};

public:
    afx_msg void OnEnKillfocusUiSearchBoxEdit();
protected:
    afx_msg LRESULT OnSearchEditBtnClicked(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnEnChangeUiSearchBoxEdit();
};
