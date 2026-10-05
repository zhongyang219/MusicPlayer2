#pragma once
#include "PlayerUIPanel.h"
#include "UIElement/Button.h"
#include "UIElement/CheckBox.h"
#include "UIElement/RadioButton.h"
#include "UIElement/Text.h"
#include "UIElement/ToggleButton.h"
#include "UIElement/ListElement.h"
#include "UIElement/ComboBox.h"
#include "UIElement/CombinedElement/ToggleSettingGroup.h"

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
    void OnComboboxSelChanged(UiElement::ComboBox* sender);

private:
    UiElement::Button* m_ok_btn{};
    UiElement::Button* m_cancel_btn{};
    UiElement::ToggleButton* m_toggle_btn{};
    UiElement::CheckBox* m_check_box1{};
    UiElement::RadioButton* m_radio1{};
    UiElement::RadioButton* m_radio2{};
    UiElement::ToggleSettingGroup* m_toggle_group2{};
    UiElement::Text* m_tip_info_text{};
    UiElement::ComboBox* m_combobox1{};

    UiElement::ListElement* m_list1{};
};

