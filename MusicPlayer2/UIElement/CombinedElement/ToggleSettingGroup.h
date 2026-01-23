#pragma once
#include "CombinedElement.h"
#include "../ToggleButton.h"
namespace UiElement
{
    class ToggleSettingGroup : public CombinedElement
    {
    public:
        ToggleSettingGroup();
    protected:
        virtual void InitComplete() override;

        void SetToggleBtnTrigger(std::function<void(ToggleButton*)> func);

    public:
        std::string icon_name;
        std::wstring main_text;
        std::wstring sub_text;

    public:
        ToggleButton* m_toggle_btn{};
    };
}

