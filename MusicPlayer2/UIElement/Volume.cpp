#include "stdafx.h"
#include "Volume.h"
#include "TinyXml2Helper.h"
#include "Player.h"
#include "MusicPlayerDlg.h"

void UiElement::Volume::Draw()
{
    CalculateRect();

    if (volumn_btn.pressed)
        rect.MoveToXY(rect.left + theApp.DPI(1), rect.top + theApp.DPI(1));

    DrawAreaGuard guard(&ui->GetDrawer(), rect);

    //绘制图标
    CRect rect_icon{ rect };
    rect_icon.right = rect_icon.left + rect_icon.Height();
    ui->DrawUiIcon(rect_icon, ui->GetBtnIconType(CPlayerUIBase::BTN_VOLUME));

    //绘制文本
    if (show_text && rect_icon.right < rect.right)
    {
        CRect rect_text{ rect };
        rect_text.left = rect_icon.right;
        CString str;
        if (CPlayer::GetInstance().GetVolume() <= 0)
            str = theApp.m_str_table.LoadText(L"UI_TXT_VOLUME_MUTE").c_str();
        else
            str.Format(_T("%d%%"), CPlayer::GetInstance().GetVolume());
        if (volumn_btn.hover)        //鼠标指向音量区域时，以另外一种颜色显示
            ui->GetDrawer().DrawWindowText(rect_text, str, ui->GetUIColors().color_text_heighlight);
        else
            ui->GetDrawer().DrawWindowText(rect_text, str, ui->GetUIColors().color_text);
    }
    //设置音量调整按钮的位置
    CRect rc_tmp = rect;
    rc_tmp.bottom = rc_tmp.top + ui->DPI(24);
    volumn_btn.rect = rc_tmp;
    volumn_btn.rect.DeflateRect(0, ui->DPI(4));
    volumn_down_btn.rect = volumn_btn.rect;
    volumn_down_btn.rect.bottom += ui->DPI(4);
    if (adj_btn_on_top)
    {
        volumn_down_btn.rect.MoveToY(volumn_btn.rect.top - volumn_down_btn.rect.Height());
    }
    else
    {
        volumn_down_btn.rect.MoveToY(volumn_btn.rect.bottom);
    }
    volumn_down_btn.rect.right = volumn_btn.rect.left + ui->DPI(27);      //设置单个音量调整按钮的宽度
    volumn_up_btn.rect = volumn_down_btn.rect;
    volumn_up_btn.rect.MoveToX(volumn_down_btn.rect.right);

    Element::Draw();
}

void UiElement::Volume::DrawTopMost()
{
    //绘制音量调节按钮
    if (m_show_volume_adj)
    {
        CRect& volume_down_rect = volumn_down_btn.rect;
        CRect& volume_up_rect = volumn_up_btn.rect;

        //判断音量调整按钮是否会超出界面之外，如果是，则将其移动至界面内
        int x_offset{}, y_offset{};     //移动的x和y偏移量
        CRect rect_text;                //音量文本的区域
        CString volume_str{};
        CRect draw_rect = ui->GetClientDrawRect();
        if (!show_text)
        {
            //如果不显示音量文本，则在音量调整按钮旁边显示音量。在这里计算文本的位置
            rect_text = volumn_up_btn.rect;
            if (CPlayer::GetInstance().GetVolume() <= 0)
                volume_str = theApp.m_str_table.LoadText(L"UI_TXT_VOLUME_MUTE").c_str();
            else
                volume_str.Format(_T("%d%%"), CPlayer::GetInstance().GetVolume());
            int width{ ui->GetDrawer().GetTextExtent(volume_str).cx };
            rect_text.left = rect_text.right + ui->DPI(2);
            rect_text.right = rect_text.left + width;

            if (rect_text.right > draw_rect.right)
                x_offset = draw_rect.right - rect_text.right;
            if (rect_text.bottom > draw_rect.bottom)
                y_offset = draw_rect.bottom - rect_text.bottom;
        }
        else
        {
            if (volume_up_rect.right > draw_rect.right)
                x_offset = draw_rect.right - volume_up_rect.right;
            if (volume_up_rect.bottom > draw_rect.bottom)
                y_offset = draw_rect.bottom - volume_up_rect.bottom;
        }

        if (x_offset != 0)
        {
            volume_up_rect.MoveToX(volume_up_rect.left + x_offset);
            volume_down_rect.MoveToX(volume_down_rect.left + x_offset);
            rect_text.MoveToX(rect_text.left + x_offset);
        }
        if (y_offset != 0)
        {
            volume_up_rect.MoveToY(volume_up_rect.top + y_offset);
            volume_down_rect.MoveToY(volume_down_rect.top + y_offset);
            rect_text.MoveToY(rect_text.top + y_offset);
        }

        BYTE alpha;
        if (ui->IsDrawBackgroundAlpha())
            alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency);
        else
            alpha = 255;

        COLORREF btn_up_back_color, btn_down_back_color;

        if (volumn_up_btn.pressed && volumn_up_btn.hover)
            btn_up_back_color = ui->GetUIColors().color_button_pressed;
        else if (volumn_up_btn.hover)
            btn_up_back_color = ui->GetUIColors().color_button_hover;
        else
            btn_up_back_color = ui->GetUIColors().color_text_2;

        if (volumn_down_btn.pressed && volumn_down_btn.hover)
            btn_down_back_color = ui->GetUIColors().color_button_pressed;
        else if (volumn_down_btn.hover)
            btn_down_back_color = ui->GetUIColors().color_button_hover;
        else
            btn_down_back_color = ui->GetUIColors().color_text_2;

        if (!theApp.m_app_setting_data.button_round_corners)
        {
            ui->GetDrawer().FillAlphaRect(volume_up_rect, btn_up_back_color, alpha);
            ui->GetDrawer().FillAlphaRect(volume_down_rect, btn_down_back_color, alpha);
        }
        else
        {
            CRect rc_buttons{ volume_up_rect | volume_down_rect };
            DrawAreaGuard guard(&ui->GetDrawer(), rc_buttons);
            ui->GetDrawer().DrawRoundRect(rc_buttons, ui->GetUIColors().color_text_2, ui->CalculateRoundRectRadius(rc_buttons), alpha);
            if (volumn_up_btn.pressed || volumn_up_btn.hover)
                ui->GetDrawer().DrawRoundRect(volume_up_rect, btn_up_back_color, ui->CalculateRoundRectRadius(volume_up_rect), alpha);
            if (volumn_down_btn.pressed || volumn_down_btn.hover)
                ui->GetDrawer().DrawRoundRect(volume_down_rect, btn_down_back_color, ui->CalculateRoundRectRadius(volume_down_rect), alpha);
        }

        if (volumn_down_btn.pressed)
            volume_down_rect.MoveToXY(volume_down_rect.left + theApp.DPI(1), volume_down_rect.top + theApp.DPI(1));
        if (volumn_up_btn.pressed)
            volume_up_rect.MoveToXY(volume_up_rect.left + theApp.DPI(1), volume_up_rect.top + theApp.DPI(1));

        ui->GetDrawer().DrawWindowText(volume_down_rect, L"-", ColorTable::WHITE, Alignment::CENTER);
        ui->GetDrawer().DrawWindowText(volume_up_rect, L"+", ColorTable::WHITE, Alignment::CENTER);

        //如果不显示音量文本且显示了音量调整按钮，则在按钮旁边显示音量
        if (!show_text)
        {
            ui->GetDrawer().DrawWindowText(rect_text, volume_str, ui->GetUIColors().color_text);
        }
    }
    else
    {
        volumn_up_btn.rect = CRect();
        volumn_down_btn.rect = CRect();
    }
}

bool UiElement::Volume::LButtonUp(CPoint point)
{
    TRACE("volumn button up.\n");
    volumn_btn.pressed = false;
    volumn_up_btn.pressed = false;
    volumn_down_btn.pressed = false;
    //点击音量按钮区域显示/隐藏音量按钮
    if (rect.PtInRect(point) && IsShown())
    {
        m_show_volume_adj = !m_show_volume_adj;
        return true;
    }
    return false;
}

bool UiElement::Volume::LButtonDown(CPoint point)
{
    TRACE("volumn button down.\n");
    if (rect.PtInRect(point))
        volumn_btn.pressed = true;
    if (volumn_up_btn.rect.PtInRect(point))
        volumn_up_btn.pressed = true;
    if (volumn_down_btn.rect.PtInRect(point))
        volumn_down_btn.pressed = true;
    return false;
}

bool UiElement::Volume::MouseMove(CPoint point)
{
    volumn_btn.hover = (rect.PtInRect(point));
    if (volumn_btn.hover)
        ui->UpdateMouseToolTipPosition(CPlayerUIBase::BTN_VOLUME, rect);
    volumn_up_btn.hover = volumn_up_btn.rect.PtInRect(point);
    volumn_down_btn.hover = volumn_down_btn.rect.PtInRect(point);
    if (volumn_btn.hover || volumn_up_btn.hover || volumn_down_btn.hover)
        return true;
    return false;
}

bool UiElement::Volume::MouseWheel(int delta, CPoint point)
{
    if (rect.PtInRect(point) || volumn_up_btn.rect.PtInRect(point) || volumn_down_btn.rect.PtInRect(point))
    {
        CMusicPlayerDlg::GetInstance()->MouseWheelAdjustVolume(static_cast<short>(delta));
        return true;
    }
    return false;
}

void UiElement::Volume::TopMostClicked(CPoint point)
{
    if (!rect.PtInRect(point) && IsShown())
    {
        //如果已经显示了音量调整按钮，则点击音量调整时保持音量调整按钮的显示
        if (m_show_volume_adj)
        {
            if (volumn_up_btn.rect.PtInRect(point))
            {
                m_show_volume_adj = true;
                CPlayer::GetInstance().MusicControl(Command::VOLUME_ADJ, theApp.m_nc_setting_data.volum_step);
            }
            else if (volumn_down_btn.rect.PtInRect(point))
            {
                m_show_volume_adj = true;
                CPlayer::GetInstance().MusicControl(Command::VOLUME_ADJ, -theApp.m_nc_setting_data.volum_step);
            }
            else
            {
                m_show_volume_adj = false;
            }
        }
    }
}

void UiElement::Volume::FromXmlNode(tinyxml2::XMLElement* xml_node)
{
    Element::FromXmlNode(xml_node);
    std::string str_show_text = CTinyXml2Helper::ElementAttribute(xml_node, "show_text");
    show_text = CTinyXml2Helper::StringToBool(str_show_text.c_str());
    std::string str_adj_btn_on_top = CTinyXml2Helper::ElementAttribute(xml_node, "adj_btn_on_top");
    adj_btn_on_top = CTinyXml2Helper::StringToBool(str_adj_btn_on_top.c_str());
}
