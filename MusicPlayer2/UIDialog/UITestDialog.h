#pragma once
#include "UIDialog.h"
#include "UiElement/Text.h"
#include "UiElement/EditControl.h"

class CUITestDialog :
    public CUIDialog
{
public:
    CUITestDialog(CWnd* pParent = nullptr);
    virtual void UiTextChanged();

protected:
    UiElement::Text* m_info_text;
    UiElement::EditControl* m_edit1;
public:
    virtual BOOL OnInitDialog();
};

