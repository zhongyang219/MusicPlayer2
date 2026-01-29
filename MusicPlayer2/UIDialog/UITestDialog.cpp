#include "stdafx.h"
#include "UITestDialog.h"
#include "../UIElement/Button.h"

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
}
