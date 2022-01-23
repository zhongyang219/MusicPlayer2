#pragma once
#include "ColorConvert.h"


// CMenuEditCtrl
#define WM_MENU_EDIT_BTN_CLICKED (WM_USER+141)

class CMenuEditCtrl : public CMFCEditBrowseCtrl
{
    DECLARE_DYNAMIC(CMenuEditCtrl)

public:
    CMenuEditCtrl();
    virtual ~CMenuEditCtrl();

    virtual void OnBrowse() override;
    virtual void OnDrawBrowseButton(CDC* pDC, CRect rect, BOOL bIsButtonPressed, BOOL bIsButtonHot) override;
    virtual void OnChangeLayout()override;

    CMenu& GetMenu();

protected:
    DECLARE_MESSAGE_MAP()

private:
    ColorTable& m_theme_color;
    CMenu m_menu;
};
