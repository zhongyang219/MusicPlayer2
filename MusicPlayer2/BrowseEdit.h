#pragma once
#include "ColorConvert.h"
#include <afxeditbrowsectrl.h>

#define WM_EDIT_BROWSE_CHANGED (WM_USER+126)        //通过“浏览”按钮选择了一个文件或文件夹后发送此消息
// CBrowseEdit

class CBrowseEdit : public CMFCEditBrowseCtrl
{
	DECLARE_DYNAMIC(CBrowseEdit)

public:
	CBrowseEdit();
	virtual ~CBrowseEdit();

    virtual void OnDrawBrowseButton(CDC* pDC, CRect rect, BOOL bIsButtonPressed, BOOL bIsButtonHot) override;
    virtual void OnChangeLayout() override;
    virtual void OnBrowse() override;
    virtual void OnAfterUpdate() override;

protected:
	DECLARE_MESSAGE_MAP()

private:
    ColorTable& m_theme_color;
    CString m_btn_str;
    CDC* m_pDC{};

    virtual void PreSubclassWindow();
public:
    afx_msg void OnDestroy();
};


