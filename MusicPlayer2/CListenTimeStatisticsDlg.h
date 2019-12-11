#pragma once
#include "ListCtrlEx.h"


// CListenTimeStatisticsDlg 对话框

class CListenTimeStatisticsDlg : public CDialog
{
	DECLARE_DYNAMIC(CListenTimeStatisticsDlg)

public:
	CListenTimeStatisticsDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CListenTimeStatisticsDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LISTEN_TIME_STATISTICS_DLG };
#endif

protected:
    enum ColumeIndex
    {
        COL_INDEX = 0,
        COL_TRACK,
        COL_PATH,
        COL_TOTAL_TIME,
        COL_LENGTH,
        COL_TIMES,
    };

    struct ListItem
    {
        wstring name;
        wstring path;
        Time total_time;
        Time length;
        double times;
    };

protected:
	CListCtrlEx m_list_ctrl;

	CSize m_min_size;
    vector<ListItem> m_data_list;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	void ShowData(bool size_changed = true);
    ListItem SongInfoToListItem(const SongInfo& song);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedExportButton();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg void OnBnClickedClearButton();
    afx_msg void OnHdnItemclickList1(NMHDR *pNMHDR, LRESULT *pResult);
};
