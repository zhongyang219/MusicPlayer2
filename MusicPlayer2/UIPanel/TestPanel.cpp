#include "stdafx.h"
#include "TestPanel.h"
#include "UserUi.h"

CTestPanel::CTestPanel(CPlayerUIBase* ui)
	: CPlayerUIPanel(ui, IDR_TEST_PANEL)
{
	m_ok_btn = dynamic_cast<UiElement::Button*>(m_root_element->FindElement("okBtn"));
	if (m_ok_btn != nullptr)
		m_ok_btn->SetClickedTrigger([&](UiElement::Button* sender) { OnOkClicked(sender); });

	m_cancel_btn = dynamic_cast<UiElement::Button*>(m_root_element->FindElement("cancelBtn"));
	if (m_cancel_btn != nullptr)
		m_cancel_btn->SetClickedTrigger([&](UiElement::Button* sender) { OnCancelClicked(sender); });

	m_check_box1 = dynamic_cast<UiElement::CheckBox*>(m_root_element->FindElement("checkbox1"));
	if (m_check_box1 != nullptr)
		m_check_box1->SetClickedTrigger([&](UiElement::CheckBox* sender) { OnCheckBox1Clicked(sender); });

	m_text1 = dynamic_cast<UiElement::Text*>(m_root_element->FindElement("text1"));
}

void CTestPanel::OnOkClicked(UiElement::Button* sender)
{
	m_ui->ShowUiTipInfo(L"点击了应用按钮。");
}

void CTestPanel::OnCancelClicked(UiElement::Button* sender)
{
	CUserUi* user_ui = dynamic_cast<CUserUi*>(m_ui);
	user_ui->ClosePanel();
}

void CTestPanel::OnCheckBox1Clicked(UiElement::CheckBox* sender)
{
	bool checked = m_check_box1->Checked();
	if (checked)
		m_text1->text = _T("复选框已勾选。");
	else
		m_text1->text = _T("复选框取消勾选。");
}

