//CEdit的派生类，用于支持按Ctrl+A全选
#pragma once


// CEditEx

class CEditEx : public CEdit
{
	DECLARE_DYNAMIC(CEditEx)

public:
	CEditEx();
	virtual ~CEditEx();

    void ResetModified();
    CString GetText() const;
    void SetColorWhenModified(bool val);    //是否要在有修改时将文本颜色改为主题色

protected:
    bool m_show_color_when_modified{ true };

protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
    afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
};


