#include "stdafx.h"
#include "Volume.h"
#include "TinyXml2Helper.h"
#include "Player.h"
#include "MusicPlayerDlg.h"
#include "Slider.h"
#include "ElementFactory.h"

void UiElement::Volume::Draw()
{
    CalculateRect();

    if (pressed)
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
        if (hover)        //鼠标指向音量区域时，以另外一种颜色显示
            ui->GetDrawer().DrawWindowText(rect_text, str, ui->GetUIColors().color_text_heighlight);
        else
            ui->GetDrawer().DrawWindowText(rect_text, str, ui->GetUIColors().color_text);
    }

    Element::Draw();
}

void UiElement::Volume::DrawTopMost()
{
    //绘制音量调节按钮
    if (m_show_volume_adj && IsEnable())
    {
        //计算音量调节控件的位置
        const CSize vol_adj_ctrl_size(ui->DPI(160), ui->DPI(32));
        if (adj_btn_on_top)
            rect_volume_adj.top = rect.top - ui->DPI(4) - vol_adj_ctrl_size.cy;
        else
            rect_volume_adj.top = rect.bottom + ui->DPI(4);
        rect_volume_adj.left = rect.left - (vol_adj_ctrl_size.cx - rect.Width()) / 2;
        rect_volume_adj.right = rect_volume_adj.left + vol_adj_ctrl_size.cx;
        rect_volume_adj.bottom = rect_volume_adj.top + vol_adj_ctrl_size.cy;
        CRect draw_rect = ui->GetClientDrawRect();
        CCommon::RestrictRectRange(rect_volume_adj, draw_rect);

        //绘制背景
        COLORREF ctrl_back = ui->GetUIColors().color_panel_back;
        ui->DrawRectangle(rect_volume_adj, ctrl_back);

        //滑动条的位置
        CRect rect_slider = rect_volume_adj;
        rect_slider.DeflateRect(ui->DPI(8), ui->DPI(8));

        //如果不显示音量文本，则在滑动条旁边显示音量
        if (!show_text)
        {
            const std::wstring str_mute = theApp.m_str_table.LoadText(L"UI_TXT_VOLUME_MUTE");
            //计算音量文本的宽度（取“静音”和“100%”中的较大值）
            int width{ (std::max)(ui->GetDrawer().GetTextExtent(str_mute.c_str()).cx, ui->GetDrawer().GetTextExtent(L"100%").cx)};
            CRect rect_text = rect_slider;
            rect_text.left = rect_text.right - width;
            //绘制音量文本
            CString volume_str{};
            if (CPlayer::GetInstance().GetVolume() <= 0)
                volume_str = str_mute.c_str();
            else
                volume_str.Format(_T("%d%%"), CPlayer::GetInstance().GetVolume());

            ui->GetDrawer().DrawWindowText(rect_text, volume_str, ui->GetUIColors().color_text);
            rect_slider.right = rect_text.left - ui->DPI(8);
        }

        //绘制滑动条
        volume_slider->SetRect(rect_slider);
        volume_slider->Draw();
    }
}

void UiElement::Volume::InitComplete()
{
    CElementFactory factory;
    volume_slider = std::dynamic_pointer_cast<Slider>(factory.CreateElement("slider", ui));
    volume_slider->SetRange(0, 100);
    volume_slider->SetPosChangedTrigger([&](UiElement::Slider* sender){
        //滚动条变化时设置音量
        CPlayer::GetInstance().SetVolume(sender->GetCurPos());
    });
}

bool UiElement::Volume::LButtonUp(CPoint point)
{
    bool cur_pressed = pressed;
    pressed = false;
    //点击音量按钮区域显示/隐藏音量按钮
    if (cur_pressed && rect.PtInRect(point) && IsEnable() && IsShown())
    {
        m_show_volume_adj = !m_show_volume_adj;
        if (m_show_volume_adj)
        {
            UpdateSliderValue();
            hover = false;
        }
        return true;
    }
    return false;
}

bool UiElement::Volume::LButtonDown(CPoint point)
{
    if (rect.PtInRect(point) && IsEnable() && IsShown())
        pressed = true;
    return  pressed;
}

bool UiElement::Volume::MouseMove(CPoint point)
{
    hover = (rect.PtInRect(point));
    if (hover)
        ui->UpdateMouseToolTipPosition(CPlayerUIBase::BTN_VOLUME, rect);
    return false;
}

bool UiElement::Volume::MouseWheel(int delta, CPoint point)
{
    if (rect.PtInRect(point) || rect_volume_adj.PtInRect(point))
    {
        CMusicPlayerDlg::GetInstance()->MouseWheelAdjustVolume(static_cast<short>(delta));
        UpdateSliderValue();
        return true;
    }
    return false;
}

bool UiElement::Volume::GlobalLButtonUp(CPoint point)
{
    bool rtn = false;
    if (IsShown() && IsEnable())
    {
        if (m_show_volume_adj && rect_volume_adj.PtInRect(mouse_pressed_point))
        {
            volume_slider->LButtonUp(point);
            rtn = true;
        }
        else
        {
            m_show_volume_adj = false;
        }

        if (!rect_volume_adj.PtInRect(point))
            volume_slider->MouseLeave();
    }
    return rtn;
}

bool UiElement::Volume::GlobalLButtonDown(CPoint point)
{
    mouse_pressed_point = point;
    if (IsShown() && IsEnable() && m_show_volume_adj)
    {
        if (rect_volume_adj.PtInRect(point))
            volume_slider->LButtonDown(point);
        return true;
    }
    return false;
}

bool UiElement::Volume::GlobalMouseMove(CPoint point)
{
    bool rtn = IsShown() && IsEnable() && m_show_volume_adj;
    volume_slider->MouseMove(point);
    return rtn;
}

void UiElement::Volume::FromXmlNode(tinyxml2::XMLElement* xml_node)
{
    Element::FromXmlNode(xml_node);
    CTinyXml2Helper::GetElementAttributeBool(xml_node, "show_text", show_text);
    CTinyXml2Helper::GetElementAttributeBool(xml_node, "adj_btn_on_top", adj_btn_on_top);
}

void UiElement::Volume::UpdateSliderValue()
{
    int volume = CPlayer::GetInstance().GetVolume();
    volume_slider->SetCurPos(volume);
}
