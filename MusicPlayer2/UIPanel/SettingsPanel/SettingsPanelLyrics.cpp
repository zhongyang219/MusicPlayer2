#include "stdafx.h"
#include "SettingsPanelLyrics.h"
#include "MusicPlayerDlg.h"
#include "FontDialogEx.h"

#include "UIElement/Text.h"
#include "UIElement/CombinedElement/ToggleSettingGroup.h"
#include "UIElement/Button.h"

CSettingsPanelLyrics::CSettingsPanelLyrics(std::shared_ptr<UiElement::Panel> root_element)
    : CSettingsPanelTab(root_element)
{
}

void CSettingsPanelLyrics::Init()
{
    UiElement::ToggleSettingGroup* lyric_karaoke_style_btn = m_root_element->FindElement<UiElement::ToggleSettingGroup>("lyricKaraokeStyle");
    lyric_karaoke_style_btn->GetToggleBtn()->BindBool(&theApp.m_lyric_setting_data.lyric_karaoke_disp);
    lyric_font_sub_text = m_root_element->FindElement<UiElement::Text>("lyricFontSettingSubText");
    UiElement::Button* lyric_font_btn = m_root_element->FindElement<UiElement::Button>("lyricFontSettingBtn");
    lyric_font_btn->SetClickedTrigger([&](UiElement::Button* sender) {
        UpdateSettingsData();
        OnBnClickedSetFontButton();
        OnSettingsChanged();
    });

    UiElement::ToggleSettingGroup* show_desktop_lyric_btn = m_root_element->FindElement<UiElement::ToggleSettingGroup>("showDesktopLyric");
    show_desktop_lyric_btn->GetToggleBtn()->BindBool(&theApp.m_lyric_setting_data.show_desktop_lyric);

}

void CSettingsPanelLyrics::UpdateSettingsData()
{
    m_data = theApp.m_lyric_setting_data;
}

void CSettingsPanelLyrics::SettingDataToUi()
{
    lyric_font_sub_text->SetText(m_data.lyric_font.GetFontInfoString());
}

void CSettingsPanelLyrics::OnSettingsChanged()
{
    CMusicPlayerDlg::GetInstance()->ApplyLyricsSettings(m_data, m_lyric_font_changed);
    CMusicPlayerDlg::GetInstance()->SettingsChanged();
}

void CSettingsPanelLyrics::OnBnClickedSetFontButton()
{
    LOGFONT lf{};             //LOGFONT变量
    theApp.m_font_set.lyric.GetFont().GetLogFont(&lf);
    CCommon::NormalizeFont(lf);
    CFontDialogEx fontDlg(&lf, false);  //构造字体对话框，初始选择字体为之前字体
    fontDlg.m_cf.Flags |= CF_NOVERTFONTS;   //仅列出水平方向的字体
    if (IDOK == fontDlg.DoModal())     // 显示字体对话框
    {
        //获取字体信息
        m_data.lyric_font.name = fontDlg.GetFaceName();
        m_data.lyric_font.size = fontDlg.GetSize() / 10;
        m_data.lyric_font.style.bold = (fontDlg.IsBold() != FALSE);
        m_data.lyric_font.style.italic = (fontDlg.IsItalic() != FALSE);
        m_data.lyric_font.style.underline = (fontDlg.IsUnderline() != FALSE);
        m_data.lyric_font.style.strike_out = (fontDlg.IsStrikeOut() != FALSE);
        //将字体已更改flag置为true
        m_lyric_font_changed = true;
        lyric_font_sub_text->SetText(m_data.lyric_font.GetFontInfoString());
    }
}
