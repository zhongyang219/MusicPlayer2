#include "stdafx.h"
#include "Button.h"
#include "Player.h"
#include "UserUI.h"
#include "Helper/UiElementHelper.h"

void UiElement::Button::Draw()
{
    CalculateRect();
    m_btn.enable = IsEnable();
    if (!empty_btn)
    {
        switch (key)
        {
        case CPlayerUIBase::BTN_TRANSLATE:
        {
            static const wstring& btn_str = theApp.m_str_table.LoadText(L"UI_TXT_BTN_TRANSLATE");
            m_btn.enable &= !CPlayer::GetInstance().m_Lyrics.IsEmpty();
            ui->DrawTextButton(rect, m_btn, btn_str.c_str(), theApp.m_lyric_setting_data.show_translate);
        }
        break;
        case CPlayerUIBase::BTN_LRYIC:
        {
            static const wstring& btn_str = theApp.m_str_table.LoadText(L"UI_TXT_BTN_DESKTOP_LYRIC");
            ui->DrawTextButton(rect, m_btn, btn_str.c_str(), theApp.m_lyric_setting_data.show_desktop_lyric);
        }
        break;
        case CPlayerUIBase::BTN_AB_REPEAT:
        {
            CString info;
            CPlayer::ABRepeatMode ab_repeat_mode = CPlayer::GetInstance().GetABRepeatMode();
            if (ab_repeat_mode == CPlayer::AM_A_SELECTED)
                info = _T("A-");
            else
                info = _T("A-B");
            CFont* pOldFont = ui->m_draw.GetFont();
            ui->m_draw.SetFont(&theApp.m_font_set.GetFontBySize(8).GetFont(theApp.m_ui_data.full_screen));      //AB重复使用小一号字体，即播放时间的字体
            m_btn.enable &= (!CPlayer::GetInstance().IsError() && !CPlayer::GetInstance().IsPlaylistEmpty());
            ui->DrawTextButton(rect, m_btn, info, ab_repeat_mode != CPlayer::AM_NONE);
            ui->m_draw.SetFont(pOldFont);
        }
        break;
        case CPlayerUIBase::BTN_KARAOKE:
        {
            m_btn.enable &= !CPlayer::GetInstance().m_Lyrics.IsEmpty();
            //如果是卡拉OK样式显示歌词，则按钮显示为选中状态
            ui->DrawUIButton(rect, CPlayerUIBase::BTN_KARAOKE, m_btn, false, false, 9, theApp.m_lyric_setting_data.lyric_karaoke_disp);
        }
        break;
        default:
        {
            std::wstring text;
            if (show_text)
                text = GetDisplayText();
            ui->DrawUIButton(rect, m_btn, GetBtnIconType(), big_icon, text, font_size, false);

        }
        break;
        }
    }
    else
    {
        m_btn.rect = rect;
    }
    Element::Draw();
}

void UiElement::Button::FromString(const std::string& key_type)
{
    if (key_type == "menu")
        key = CPlayerUIBase::BTN_MENU;
    else if (key_type == "miniMode")
        key = CPlayerUIBase::BTN_MINI;
    else if (key_type == "miniModeClose")
        key = CPlayerUIBase::BTN_CLOSE;
    else if (key_type == "fullScreen")
        key = CPlayerUIBase::BTN_FULL_SCREEN;
    else if (key_type == "repeatMode")
        key = CPlayerUIBase::BTN_REPETEMODE;
    else if (key_type == "settings")
        key = CPlayerUIBase::BTN_SETTING;
    else if (key_type == "equalizer")
        key = CPlayerUIBase::BTN_EQ;
    else if (key_type == "skin")
        key = CPlayerUIBase::BTN_SKIN;
    else if (key_type == "info")
        key = CPlayerUIBase::BTN_INFO;
    else if (key_type == "find")
        key = CPlayerUIBase::BTN_FIND;
    else if (key_type == "abRepeat")
        key = CPlayerUIBase::BTN_AB_REPEAT;
    else if (key_type == "desktopLyric")
        key = CPlayerUIBase::BTN_LRYIC;
    else if (key_type == "lyricTranslate")
        key = CPlayerUIBase::BTN_TRANSLATE;
    else if (key_type == "stop")
        key = CPlayerUIBase::BTN_STOP;
    else if (key_type == "previous")
        key = CPlayerUIBase::BTN_PREVIOUS;
    else if (key_type == "next")
        key = CPlayerUIBase::BTN_NEXT;
    else if (key_type == "playPause")
        key = CPlayerUIBase::BTN_PLAY_PAUSE;
    else if (key_type == "favorite")
        key = CPlayerUIBase::BTN_FAVOURITE;
    else if (key_type == "mediaLib")
        key = CPlayerUIBase::BTN_MEDIA_LIB;
    else if (key_type == "showPlaylist")
        key = CPlayerUIBase::BTN_SHOW_PLAYLIST;
    else if (key_type == "addToPlaylist")
        key = CPlayerUIBase::BTN_ADD_TO_PLAYLIST;
    else if (key_type == "switchDisplay")
        key = CPlayerUIBase::BTN_SWITCH_DISPLAY;
    else if (key_type == "darkLightMode")
        key = CPlayerUIBase::BTN_DARK_LIGHT;
    else if (key_type == "locateTrack")
        key = CPlayerUIBase::BTN_LOCATE_TO_CURRENT;
    else if (key_type == "openFolder")
        key = CPlayerUIBase::BTN_OPEN_FOLDER;
    else if (key_type == "newPlaylist")
        key = CPlayerUIBase::BTN_NEW_PLAYLIST;
    else if (key_type == "playMyFavourite")
        key = CPlayerUIBase::BTN_PLAY_MY_FAVOURITE;
    else if (key_type == "playAllTracks")
        key = CPlayerUIBase::BTN_PLAY_ALL_TRACKS;
    else if (key_type == "medialibFolderSort")
        key = CPlayerUIBase::BTN_MEDIALIB_FOLDER_SORT;
    else if (key_type == "medialibPlaylistSort")
        key = CPlayerUIBase::BTN_MEDIALIB_PLAYLIST_SORT;
    else if (key_type == "karaoke")
        key = CPlayerUIBase::BTN_KARAOKE;
    else if (key_type == "showPlayQueue")
        key = CPlayerUIBase::BTN_SHOW_PLAY_QUEUE;
    else if (key_type == "closePanel")
        key = CPlayerUIBase::BTN_CLOSE_PANEL;
    else if (key_type == "showPanel")
        key = CPlayerUIBase::BTN_SHOW_PANEL;
    else if (key_type == "showHideElement")
        key = CPlayerUIBase::BTN_SHOW_HIDE_ELEMENT;
    else if (key_type == "showSettingsPanel")
        key = CPlayerUIBase::BTN_SHOW_SETTINGS_PANEL;
    else
        key = CPlayerUIBase::BTN_INVALID;
}

void UiElement::Button::IconTypeFromString(const std::string& icon_name)
{
    icon_type = UiElementHelper::NameToIconType(icon_name);
}

int UiElement::Button::GetMaxWidth(CRect parent_rect) const
{
    //显示文本，并且没有指定宽度时时跟随文本宽度
    if (show_text && !IsWidthValid())
    {
        std::wstring text = GetDisplayText();
        //第一次执行到这里时，由于rect还没有从layout元素中计算出来，因此这里做一下判断，如果高度为0，则直接获取height的值
        int btn_height = rect.Height();
        if (btn_height == 0)
            btn_height = Element::height.GetValue(parent_rect);
        int right_space = (btn_height - ui->DPI(16)) / 2;

        //计算文本宽度前先设置一下字体
        UiFontGuard set_font(ui, font_size);

        int width_text{ ui->m_draw.GetTextExtent(text.c_str()).cx + right_space + btn_height };

        int width_max{ max_width.IsValid() ? max_width.GetValue(parent_rect) : INT_MAX };
        return min(width_text, width_max);
    }
    else
    {
        return Element::GetMaxWidth(parent_rect);
    }
}

void UiElement::Button::ClearRect()
{
    Element::ClearRect();
    m_btn.rect = CRect();
}

bool UiElement::Button::LButtonUp(CPoint point)
{
    bool pressed = m_btn.pressed;
    m_btn.pressed = false;

    if (pressed && m_btn.rect.PtInRect(point) && m_btn.enable && IsShown(ParentRect()))
    {
        //如果设置了点击触发函数，则调用设置的点击触发函数
        if (m_clicked_trigger)
        {
            m_clicked_trigger(this);
        }
        //显示面板
        else if (key == CPlayerUIBase::BTN_SHOW_PANEL)
        {
            CUserUi* user_ui = dynamic_cast<CUserUi*>(ui);
            if (user_ui != nullptr)
            {
                if (!panel_id.empty())
                    user_ui->ShowPanelById(panel_id);
                else if (!panel_file_name.empty())
                    user_ui->ShowPanelByFileName(panel_file_name);
            }
        }
        //显示/隐藏元素
        if (key == CPlayerUIBase::BTN_SHOW_HIDE_ELEMENT)
        {
            CUserUi* user_ui = dynamic_cast<CUserUi*>(ui);
            if (user_ui != nullptr)
            {
                Element* root_element = user_ui->GetMouseEventResponseElement().get();
                if (root_element != nullptr)
                {
                    Element* element = root_element->FindElement(related_element_id);
                    if (element != nullptr)
                    {
                        element->SetVisible(!element->IsVisible());
                    }
                }
            }
        }
        else
        {
            ui->ButtonClicked(key, m_btn);
        }
        return true;
    }
    return false;
}

bool UiElement::Button::LButtonDown(CPoint point)
{
    if (m_btn.enable && m_btn.rect.PtInRect(point))
    {
        m_btn.pressed = true;
        return true;
    }
    return false;
}

bool UiElement::Button::MouseMove(CPoint point)
{
    if (m_btn.enable)
    {
        m_btn.hover = (m_btn.rect.PtInRect(point));
        //鼠标进入按钮区域时
        if (!last_hover && m_btn.hover)
        {
            std::wstring tooltip_text;
            if (show_text)
            {
                tooltip_text = ui->GetItemTooltip(key);
                if (tooltip_text.empty())
                    tooltip_text = text;
            }
            else
            {
                tooltip_text = text;
                if (tooltip_text.empty())
                    tooltip_text = ui->GetItemTooltip(key);
            }
            if (tooltip_text.empty())
                tooltip_text = ui->GetButtonText(key);
            //指定了按钮文本，且不显示文本或显示文本和鼠标提示不同时，显示鼠标提示
            if (!tooltip_text.empty() && (!show_text || tooltip_text != GetDisplayText()))
                ui->UpdateMouseToolTip(key, tooltip_text.c_str());
            else
                ui->UpdateMouseToolTip(key, _T(""));
        }

        if (m_btn.hover)
        {
            ui->UpdateMouseToolTipPosition(key, m_btn.rect);
        }
        last_hover = m_btn.hover;
        return true;
    }
    return false;
}

bool UiElement::Button::RButtonUp(CPoint point)
{
    if (m_btn.rect.PtInRect(point))
    {
        ui->ButtonRClicked(key, m_btn);
        return true;    //在按钮区域内点击了鼠标右键时总是返回true，不弹出主窗口右键菜单
    }

    return false;
}

bool UiElement::Button::MouseLeave()
{
    m_btn.hover = false;
    m_btn.pressed = false;
    return true;
}

void UiElement::Button::HideTooltip()
{
    ui->UpdateMouseToolTipPosition(key, CRect());
}

bool UiElement::Button::SetCursor()
{
    if (m_btn.hover && hand_cursor)
    {
        ::SetCursor(::LoadCursor(NULL, IDC_HAND));
        return true;
    }
    return false;
}

void UiElement::Button::SetClickedTrigger(std::function<void(Button*)> func)
{
    m_clicked_trigger = func;
}

std::wstring UiElement::Button::GetDisplayText() const
{
    if (!this->text.empty())
        return this->text;
    else
        return ui->GetButtonText(key);
}

IconMgr::IconType UiElement::Button::GetBtnIconType() const
{
    if (icon_type != IconMgr::IT_NO_ICON)
        return icon_type;
    else
        return ui->GetBtnIconType(key);
}
