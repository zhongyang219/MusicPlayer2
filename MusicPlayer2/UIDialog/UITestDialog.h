#pragma once
#include "UIDialog.h"
#include "UiElement/Text.h"
#include "UiElement/TextBlock.h"

class CUITestDialog :
    public CUIDialog
{
public:
    CUITestDialog(CWnd* pParent = nullptr);
    virtual void UiTextChanged();

protected:
    UiElement::Text* m_info_text;
    UiElement::TextBlock* m_text_block1;
};

