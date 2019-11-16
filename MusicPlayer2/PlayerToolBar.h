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

    void AddToolButton(IconRes icon, LPCTSTR strText, LPCTSTR strToolTip, UINT cmdId, bool showTest = false);        //添加一个按钮，点击后发送cmdId命令
    void AddToolButton(IconRes icon, LPCTSTR strText, LPCTSTR strToolTip, CMenu* pMenu, bool showTest = false);      //添加一个按钮，点击后弹出菜单
    void SetIconSize(int size);

	CToolTipCtrl* GetToolTip() { return &m_tool_tip; }

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

protected:
	CToolTipCtrl m_tool_tip;		//文本提示类
    std::vector<ToolBtn> m_buttons;
    ColorTable& m_theme_color;
    int m_icon_size = 20;
    int m_cur_btn_id = 100;
    bool m_first_draw = true;

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
};


