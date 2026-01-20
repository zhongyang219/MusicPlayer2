#include "stdafx.h"
#include "TestPanel.h"
#include "UserUi.h"

CTestPanel::CTestPanel(CPlayerUIBase* ui)
	: CPlayerUIPanel(ui, IDR_TEST_PANEL)
{
	m_ok_btn = dynamic_cast<UiElement::Button*>(m_root_element->FindElement("okBtn"));
	if (m_ok_btn != nullptr)
		m_ok_btn->SetClickedTrigger([&](UiElement::Element* sender) { OnOkClicked(sender); });

	m_cancel_btn = dynamic_cast<UiElement::Button*>(m_root_element->FindElement("cancelBtn"));
	if (m_cancel_btn != nullptr)
		m_cancel_btn->SetClickedTrigger([&](UiElement::Element* sender) { OnCancelClicked(sender); });
}

void CTestPanel::OnOkClicked(UiElement::Element* sender)
{
	m_ui->ShowUiTipInfo(L"点击了应用按钮。");
}

void CTestPanel::OnCancelClicked(UiElement::Element* sender)
{
	CUserUi* user_ui = dynamic_cast<CUserUi*>(m_ui);
	user_ui->ClosePanel();
}

