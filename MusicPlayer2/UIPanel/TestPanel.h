#pragma once
#include "PlayerUIPanel.h"
#include "UIElement/Button.h"
class CTestPanel : public CPlayerUIPanel
{
public:
	CTestPanel(CPlayerUIBase* ui);

private:
	void OnOkClicked(UiElement::Element* sender);
	void OnCancelClicked(UiElement::Element* sender);

private:
	UiElement::Button* m_ok_btn{};
	UiElement::Button* m_cancel_btn{};
};

