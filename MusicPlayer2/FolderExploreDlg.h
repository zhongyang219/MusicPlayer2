#pragma once
#include "ListCtrlEx.h"
#include "SearchEditCtrl.h"
#include "TreeCtrlEx.h"


// CFolderExploreDlg 对话框

class CFolderExploreDlg : public CTabDlg
{
	DECLARE_DYNAMIC(CFolderExploreDlg)

public:
	CFolderExploreDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CFolderExploreDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FOLDER_EXPLORE_DIALOG };
#endif

protected:
    enum SongColumeIndex
    {
        COL_FILE_NAME = 0,
        COL_TITLE,
        COL_ARTIST,
        COL_ALBUM,
        COL_PATH,
    };

protected:
    CSearchEditCtrl m_search_edit;
    CTreeCtrlEx m_folder_explore_tree;
    CListCtrlEx m_song_list_ctrl;

    CString m_folder_path_selected;

protected:
    void ShowFolderTree();
    void ShowSongList(bool size_changed = true);
    void FolderTreeClicked(HTREEITEM hItem);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnNMRClickFolderExploreTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMClickFolderExploreTree(NMHDR *pNMHDR, LRESULT *pResult);
};
