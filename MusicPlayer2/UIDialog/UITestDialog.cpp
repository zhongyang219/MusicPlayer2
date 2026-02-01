#include "stdafx.h"
#include "UITestDialog.h"
#include "../UIElement/Button.h"
#include "../UIElement/Slider.h"

CUITestDialog::CUITestDialog(CWnd* pParent)
    : CUIDialog(IDR_TEST_DIALOG)
{
    UiElement::Button* ok_btn = m_ui.GetCurrentTypeUi()->FindElement<UiElement::Button>("okBtn");
    ok_btn->SetClickedTrigger([&](UiElement::Button* sender) { SendMessage(WM_COMMAND, IDOK); });
    UiElement::Button* cancel_btn = m_ui.GetCurrentTypeUi()->FindElement<UiElement::Button>("cancelBtn");
    cancel_btn->SetClickedTrigger([&](UiElement::Button* sender) { SendMessage(WM_COMMAND, IDCANCEL); });

    UiElement::ComboBox* combobox1 = m_ui.GetCurrentTypeUi()->FindElement<UiElement::ComboBox>("combobox1");
    combobox1->AddString(L"下拉选项1");
    combobox1->AddString(L"下拉选项2");
    combobox1->AddString(L"下拉选项3");

    m_info_text = m_ui.GetCurrentTypeUi()->FindElement<UiElement::Text>("tipInfoText");

    UiElement::Slider* horizentol_slider = m_ui.GetCurrentTypeUi()->FindElement<UiElement::Slider>("horizontalSlider1");
    horizentol_slider->SetPosChangedTrigger([&](UiElement::Slider* sender) {
        std::wstring str = L"滑动条当前值：" + std::to_wstring(sender->GetCurPos());
        m_info_text->SetText(str);
    });

    m_edit1 = m_ui.GetCurrentTypeUi()->FindElement<UiElement::EditControl>("textBlock1");
}

void CUITestDialog::UiTextChanged()
{
}

BOOL CUITestDialog::OnInitDialog()
{
    CUIDialog::OnInitDialog();

    m_edit1->Create(this);

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}
