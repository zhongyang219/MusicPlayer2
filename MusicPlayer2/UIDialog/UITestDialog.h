#pragma once
#include "UIDialog.h"
#include "UiElement/Text.h"

class CUITestDialog :
    public CUIDialog
{
public:
    CUITestDialog(CWnd* pParent = nullptr);

protected:
    UiElement::Text* m_info_text;
};

