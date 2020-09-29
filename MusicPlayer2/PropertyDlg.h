#pragma once
#include "PropertyTabDlg.h"
#include "CTabCtrlEx.h"
#include "BaseDialog.h"
#include "PropertyAlbumCoverDlg.h"
#include "PropertyAdvancedDlg.h"


// CPropertyDlg 对话框

class CPropertyDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CPropertyDlg)

public:
	CPropertyDlg(vector<SongInfo>& all_song_info, int index, bool read_only, int tab_index = 0, bool show_out_album_cover = false, CWnd* pParent = nullptr);   // 标准构造函数
	CPropertyDlg(vector<SongInfo>& all_song_info, CWnd* pParent = nullptr);   // 批量编辑
	virtual ~CPropertyDlg();

    bool GetListRefresh() const;
    bool GetModified() const;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPERTY_PARENT_DIALOG };
#endif

protected:
    CTabCtrlEx m_tab_ctrl;
    CPropertyTabDlg m_property_dlg;
    CPropertyAlbumCoverDlg m_album_cover_dlg;
    CPropertyAdvancedDlg m_advanced_dlg;
    CButton m_save_btn;
    CButton m_previous_btn;
    CButton m_next_btn;

    int m_index{};		//当前显示项目的曲目序号
    bool m_read_only{};
    int m_song_num;
    int m_tab_index{};
    const int m_batch_edit;
    bool m_modified{};      //如果点击过“保存到文件”，则为true

protected:
    void ShowPageNum();

    virtual CString GetDialogName() const override;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedSaveToFileButton();
    afx_msg void OnBnClickedPreviousButton();
    afx_msg void OnBnClickedNextButton();
protected:
    afx_msg LRESULT OnPropertyDialogModified(WPARAM wParam, LPARAM lParam);
public:
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
};
