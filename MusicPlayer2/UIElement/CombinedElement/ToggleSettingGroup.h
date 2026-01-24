#pragma once
#include "CombinedElement.h"
#include "../ToggleButton.h"
namespace UiElement
{
    class ToggleSettingGroup : public CombinedElement
    {
    public:
        ToggleSettingGroup();
        ToggleButton* GetToggleBtn() const;

    protected:
        virtual void InitComplete() override;

    public:
        std::string icon_name;
        std::wstring main_text;
        std::wstring sub_text;

    public:
        ToggleButton* m_toggle_btn{};
    };
}

