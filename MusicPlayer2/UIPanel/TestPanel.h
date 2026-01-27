#pragma once
#include "PlayerUIPanel.h"
#include "UIElement/Button.h"
#include "UIElement/CheckBox.h"
#include "UIElement/Text.h"
#include "UIElement/ToggleButton.h"
#include "UIElement/ListElement.h"
class CTestPanel : public CPlayerUIPanel
{
public:
	CTestPanel(CPlayerUIBase* ui);

private:
	void OnOkClicked(UiElement::Button* sender);
	void OnCancelClicked(UiElement::Button* sender);
	void OnCheckBox1Clicked(UiElement::AbstractToggleButton* sender);
	void OnToggleBtnClicked(UiElement::AbstractToggleButton* sender);
	void OnAddClicked(UiElement::Button* sender);
	void OnDeleteClicked(UiElement::Button* sender);

private:
	UiElement::Button* m_ok_btn{};
	UiElement::Button* m_cancel_btn{};
	UiElement::CheckBox* m_check_box1{};
	UiElement::ToggleButton* m_toggle_btn{};
	UiElement::Text* m_text1{};
	UiElement::Text* m_text2{};

	UiElement::ListElement* m_list1{};
};

