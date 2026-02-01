#pragma once
#include "SettingsPanelTab.h"
#include "CommonData.h"
namespace UiElement
{
    class Text;
}

class CSettingsPanelLyrics : public CSettingsPanelTab
{
public:
    CSettingsPanelLyrics(std::shared_ptr<UiElement::Panel> root_element);

    // 通过 CSettingsPanelTab 继承
    virtual void Init() override;
    virtual void UpdateSettingsData() override;
    virtual void SettingDataToUi() override;
    virtual void OnSettingsChanged() override;

private:
    void OnBnClickedSetFontButton();

private:
    LyricSettingData m_data;
    bool m_lyric_font_changed{};

    UiElement::Text* lyric_font_sub_text{};

};

