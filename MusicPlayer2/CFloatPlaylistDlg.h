#pragma once
#include "BaseDialog.h"
#include "PlayListCtrl.h"
#include "StaticEx.h"
#include "CPlayerUIBase.h"
#include "PlayerToolBar.h"
#include "SearchEditCtrl.h"
#include "MenuEditCtrl.h"

#define WM_FLOAT_PLAYLIST_CLOSED (WM_USER+118)

// CFloatPlaylistDlg 对话框

class CFloatPlaylistDlg : public CBaseDialog
{
    DECLARE_DYNAMIC(CFloatPlaylistDlg)

public:
    CFloatPlaylistDlg(int& item_selected, vector<int>& items_selected, CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CFloatPlaylistDlg();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_MUSICPLAYER2_DIALOG };
#endif

    void RefreshData();			//刷新数据
    void ReSizeControl(int cx, int cy);		//调整控件的大小和位置
    void RefreshState(bool highlight_visible = true);		//刷新播放列表的状态
    CPlayListCtrl& GetListCtrl();
    CStaticEx& GetPathStatic();
    CMenuEditCtrl& GetPathEdit();
    CSearchEditCtrl& GetSearchBox();
    void GetPlaylistItemSelected();
    void SetDragEnable();
    void EnableControl(bool enable);
    void UpdateStyles();
    void SetInitPoint(CPoint point);

private:
    CPlayListCtrl m_playlist_ctrl;
    CStaticEx m_path_static;
    CMenuEditCtrl m_path_edit;
    CButton m_media_lib_button;
    CSearchEditCtrl m_search_edit;
    CPlayerToolBar m_playlist_toolbar;

    SLayoutData m_layout;		//窗口布局的固定数据
    int m_medialib_btn_width{ theApp.DPI(64) };             // 这里的值是最小宽度，窗口init时会根据文字变大

    bool m_searched{ false };		//播放列表是否处于搜索状态
    int& m_item_selected;		//播放列表中鼠标选中的项目
    vector<int>& m_items_selected;

    CPoint m_init_point{ INT_MAX, INT_MAX };

private:
    bool Initilized() const;

protected:
    virtual CString GetDialogName() const override;
    virtual bool InitializeControls() override;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnNMRClickPlaylistList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMDblclkPlaylistList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnEnChangeSearchEdit();
    virtual void OnCancel();
    afx_msg void OnClose();
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    afx_msg void OnNMClickPlaylistList(NMHDR* pNMHDR, LRESULT* pResult);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
    afx_msg LRESULT OnInitmenu(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnListItemDragged(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSearchEditBtnClicked(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnLocateToCurrent();
protected:
    afx_msg LRESULT OnMainWindowActivated(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnDropFiles(HDROP hDropInfo);
    afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
};
