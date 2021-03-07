#pragma once
#include "afxcmn.h"
#include "ColorConvert.h"

//鼠标拖动完成向父窗口发送此消息，通过wPara传递拖放结束的位置
//注意，执行拖动操作后列表项目的位置不会改变，需要在响应此消息后手动刷新列表
#define WM_LIST_ITEM_DRAGGED (WM_USER+121)

class CListCtrlEx :
	public CListCtrl
{
	DECLARE_DYNAMIC(CListCtrlEx)
public:
	CListCtrlEx();
	~CListCtrlEx();

	//void SetColor(const ColorTable& colors);
	void GetItemSelected(vector<int>& item_selected) const;
	int GetCurSel() const;		//获取当前选中的项目
	void SetCurSel(int select);
    void SetCurSel(int first, int last);
	void SelectAll();
	void SelectNone();
	void SelectReverse();
	virtual bool SetRowHeight(int height);		//设置表格行高
	void SetHightItem(int item) { m_highlight_item = item; }			//设置高亮的项目（播放列表中正在播放的项目）
    void SetDragEnable(bool enable = true) { m_drag_enable = enable; }      //是否允许鼠标拖动
    void ShowPopupMenu(CMenu* pMenu, int item_index, CWnd* pWnd);
    void FillLeftSpaceAfterPaint(bool fill);        //如果为true，则在每行绘制之后填充左侧空白，否则在绘制之前填充（如果表格没有图标或复选框，则应设置为true，否则设置为false）

    typedef map<int, wstring> RowData;      //列表数据中每一行的数据，map的key为列序号，value为显示的文本
    typedef vector<RowData> ListData;       //列表数据，其中vector为每一行的数据

    //设置列表数据
	//（注：使用此函数设置列表数据使用了虚拟列表的方式，可显著提升在数据量很多的情况下的加载速度，
	//使用此函数时必须确保列表具有 LVS_OWNERDATA 样式）
    void SetListData(ListData* pListData);

	//设置列表数据
	//（使用传统的方式）
	void SetListData(const ListData& list_data);

	void SetCtrlAEnable(bool enable) { m_enable_ctrl_a = enable; }		//是否允许Ctrl + A全选

    wstring GetAllText(const wchar_t* sperator = L"\t");

protected:
	const ColorTable& m_theme_color;
	COLORREF m_background_color{ GRAY(255) };	//列表控件的背景色
	int m_highlight_item{ -1 };			//高亮的项目
    bool m_drag_enable{ false };
    bool m_dragging{ false };
    HCURSOR m_drag_cursor{};
    bool m_fill_left_space_after_paint{ true };
	ListData* m_pListData{};
	bool m_enable_ctrl_a{ false };	//是否允许Ctrl+A全选

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void PreSubclassWindow();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);
};

