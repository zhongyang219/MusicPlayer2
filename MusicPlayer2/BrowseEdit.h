#pragma once
#include "ColorConvert.h"
#include <afxeditbrowsectrl.h>
#include "CommonData.h"

#define WM_EDIT_BROWSE_CHANGED (WM_USER+126)        //通过“浏览”按钮选择了一个文件或文件夹后发送此消息
// CBrowseEdit

class CBrowseEdit : public CMFCEditBrowseCtrl
{
	DECLARE_DYNAMIC(CBrowseEdit)

public:
	CBrowseEdit();
	virtual ~CBrowseEdit();

    //自定义的编辑模式
    enum class EditBrowseMode
    {
        NONE,
        RENAME,     //重命名模式
        LIST        //编辑列表模式
    };

    virtual void OnDrawBrowseButton(CDC* pDC, CRect rect, BOOL bIsButtonPressed, BOOL bIsButtonHot) override;
    virtual void OnChangeLayout() override;
    virtual void OnBrowse() override;
    virtual void OnAfterUpdate() override;
    void SetEditBrowseMode(EditBrowseMode browse_mode);
    void SetPopupDlgTitle(const CString& popup_dlg_title);

protected:
	DECLARE_MESSAGE_MAP()

    IconRes& GetIcon();

private:
    ColorTable& m_theme_color;
    CString m_btn_str;
    CDC* m_pDC{};
    EditBrowseMode m_browse_mode;       //自定义的编辑模式，当基类的m_Mode设置为BrowseMode_Default时使用
    CString m_poopup_dlg_title;         //弹出对话框的标题

    virtual void PreSubclassWindow();
public:
    afx_msg void OnDestroy();
};
