#include "stdafx.h"
#include "TestPanel.h"
#include "UserUi.h"

CTestPanel::CTestPanel(CPlayerUIBase* ui)
	: CPlayerUIPanel(ui, IDR_TEST_PANEL)
{
	m_ok_btn = m_root_element->FindElement<UiElement::Button>("okBtn");
	if (m_ok_btn != nullptr)
		m_ok_btn->SetClickedTrigger([&](UiElement::Button* sender) { OnOkClicked(sender); });

	m_cancel_btn = m_root_element->FindElement<UiElement::Button>("cancelBtn");
	if (m_cancel_btn != nullptr)
		m_cancel_btn->SetClickedTrigger([&](UiElement::Button* sender) { OnCancelClicked(sender); });

	m_check_box1 = m_root_element->FindElement<UiElement::CheckBox>("checkbox1");
	if (m_check_box1 != nullptr)
		m_check_box1->SetClickedTrigger([&](UiElement::CheckBox* sender) { OnCheckBox1Clicked(sender); });

	m_toggle_btn = m_root_element->FindElement<UiElement::ToggleButton>("toggleBtn1");
	if (m_toggle_btn != nullptr)
		m_toggle_btn->SetClickedTrigger([&](UiElement::ToggleButton* sender) { OnToggleBtnClicked(sender); });

	m_text1 = m_root_element->FindElement<UiElement::Text>("text1");
	m_text2 = m_root_element->FindElement<UiElement::Text>("text2");
	m_list1 = m_root_element->FindElement<UiElement::ListElement>("list1");
	m_list1->SetColumnCount(2);
	m_list1->SetColumnWidth(0, ui->DPI(80));

	auto* add_btn = m_root_element->FindElement<UiElement::Button>("addBtn");
	add_btn->SetClickedTrigger([&](UiElement::Button* sender) { OnAddClicked(sender); });
	auto* del_btn = m_root_element->FindElement<UiElement::Button>("deleteBtn");
	del_btn->SetClickedTrigger([&](UiElement::Button* sender) { OnDeleteClicked(sender); });
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

void CTestPanel::OnToggleBtnClicked(UiElement::ToggleButton* sender)
{
	bool checked = sender->Checked();
	if (checked)
		m_text2->text = _T("开关已打开");
	else
		m_text2->text = _T("开关已关闭");
}

void CTestPanel::OnAddClicked(UiElement::Button* sender)
{
	std::map<int, std::wstring> row_data;
	int row = m_list1->GetRowCount();
	row_data[0] = std::to_wstring(row + 1);
	row_data[1] = L"第" + std::to_wstring(row + 1) + L"行";
	m_list1->AddRow(row_data);
}

void CTestPanel::OnDeleteClicked(UiElement::Button* sender)
{
	int row = m_list1->GetItemSelected();
	if (row >= 0 && row < m_list1->GetRowCount())
		m_list1->DeleteRow(row);
}

