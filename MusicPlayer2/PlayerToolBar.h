#pragma once
#include "CommonData.h"

// CPlayerToolBar
// 自绘的工具栏

class CPlayerToolBar : public CStatic
{
	DECLARE_DYNAMIC(CPlayerToolBar)
public:
	CPlayerToolBar();
	virtual ~CPlayerToolBar();

    // 添加一个按钮，点击后执行一个命令
    // IconRes icon: 图标资源
    // LPCTSTR strText: 图标右侧的文本
    // LPCTSTR strToolTip: 鼠标提示的文本
    // UINT cmdId: 点击后执行的命令ID
    // bool showText: 是否在图标右侧显示文本
    void AddToolButton(IconRes icon, LPCTSTR strText, LPCTSTR strToolTip, UINT cmdId, bool showText = false);

    // 添加一个按钮，点击后弹出菜单
    // IconRes icon: 图标资源
    // LPCTSTR strText: 图标右侧的文本
    // LPCTSTR strToolTip: 鼠标提示的文本
    // CMenu * pMenu: 点击后弹出的菜单
    // bool showText: 是否在图标右侧显示文本
    void AddToolButton(IconRes icon, LPCTSTR strText, LPCTSTR strToolTip, CMenu* pMenu, bool showText = false);

    void ModifyToolButton(int index, IconRes icon, LPCTSTR strText, LPCTSTR strToolTip, UINT cmdId, bool showText = false);
    void ModifyToolButton(int index, IconRes icon, LPCTSTR strText, LPCTSTR strToolTip, CMenu* pMenu, bool showText = false);

    void SetIconSize(int size);
    void SetCmdReciveWindow(CWnd* pWnd);        //设置响应工具按钮命令的窗口，如果不设置，则为工具栏的父窗口

protected:
    struct ToolBtn
    {
        IconRes icon;           //图标
        CString text;           //显示文本
        bool show_text = false; //是否显示文本
        UINT cmd_id;            //点击后发送的命令ID
        CMenu* pMenu = nullptr; //点击后弹出的菜单
        bool is_cmd = true;     //点击后发送命令还是弹出菜单
        CString tooltip_text;   //鼠标提示文本
        CRect rect;				//按钮的矩形区域
        bool hover{ false };	//鼠标是否指向按钮
        bool pressed{ false };	//按钮是否按下
        bool enable{ true };	//按钮是否启用
    };

    void AddToolTips();
    void UpdateToolTipsPosition();  //更新鼠标提示的位置
    CWnd* GetCmdReciveWindow();

protected:
	CToolTipCtrl m_tool_tip;		//文本提示类
    std::vector<ToolBtn> m_buttons;
    const ColorTable& m_theme_color;
    int m_icon_size = 20;
    int m_cur_btn_id = 100;
    bool m_first_draw = true;
    bool m_btn_updated = false;
    bool m_menu_poped_up = false;
    CWnd* m_pCmdReciveWnd = nullptr;

	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void PreSubclassWindow();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnMouseLeave();
    afx_msg void OnMouseHover(UINT nFlags, CPoint point);
protected:
    afx_msg LRESULT OnInitmenu(WPARAM wParam, LPARAM lParam);
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
