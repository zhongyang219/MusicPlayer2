#pragma once
#include "PlayerUIPanel.h"
#include "UIElement/Button.h"
#include "UIElement/CheckBox.h"
#include "UIElement/Text.h"
class CTestPanel : public CPlayerUIPanel
{
public:
	CTestPanel(CPlayerUIBase* ui);

private:
	void OnOkClicked(UiElement::Button* sender);
	void OnCancelClicked(UiElement::Button* sender);
	void OnCheckBox1Clicked(UiElement::CheckBox* sender);

private:
	UiElement::Button* m_ok_btn{};
	UiElement::Button* m_cancel_btn{};
	UiElement::CheckBox* m_check_box1{};
	UiElement::Text* m_text1{};
};

