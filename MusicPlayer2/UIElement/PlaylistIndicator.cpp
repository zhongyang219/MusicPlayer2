#include "stdafx.h"
#include "PlaylistIndicator.h"
#include "TinyXml2Helper.h"

CListCache UiElement::PlaylistIndicator::m_list_cache(CListCache::SubsetType::ST_CURRENT);

void UiElement::PlaylistIndicator::Draw()
{
    m_list_cache.reload();
    CalculateRect();

    // 此m_list_cache为UI线程缓存当前列表，只有at(0)是有效的
    ASSERT(m_list_cache.size() == 1);
    const ListItem& list_item = m_list_cache.at(0);

    IconMgr::IconType icon_type = list_item.GetTypeIcon();
    wstring str = list_item.GetTypeDisplayName();
    //绘制图标
    CRect rect_icon{ rect };
    rect_icon.right = rect_icon.left + ui->DPI(26);
    ui->DrawUiIcon(rect_icon, icon_type);
    //设置字体
    UiFontGuard set_font(ui, font_size);
    //绘制文本
    CRect rect_text{ rect };
    rect_text.left = rect_icon.right;
    rect_text.right = rect_text.left + ui->GetDrawer().GetTextExtent(str.c_str()).cx;
    ui->GetDrawer().DrawWindowText(rect_text, str.c_str(), ui->GetUIColors().color_text, Alignment::LEFT, true);
    //绘制菜单按钮
    CRect menu_btn_rect{ rect };
    menu_btn_rect.left = rect.right - ui->DPI(26);
    const int icon_size{ (std::min)(ui->DPI(24), rect.Height()) };
    CRect menu_btn_icon_rect = CDrawCommon::CalculateCenterIconRect(menu_btn_rect, icon_size);
    ui->DrawUIButton(menu_btn_icon_rect, btn_menu, IconMgr::IconType::IT_Menu);
    //绘制当前播放列表名称
    CRect rect_name{ rect };
    rect_name.left = rect_text.right + ui->DPI(8);
    rect_name.right = menu_btn_rect.left - ui->DPI(4);
    BYTE alpha{ 255 };
    if (ui->IsDrawBackgroundAlpha())
        alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) / 2;
    if (theApp.m_app_setting_data.button_round_corners)
        ui->GetDrawer().DrawRoundRect(rect_name, ui->GetUIColors().color_control_bar_back, ui->DPI(4), alpha);
    else
        ui->GetDrawer().FillAlphaRect(rect_name, ui->GetUIColors().color_control_bar_back, alpha);
    rect_name = rect_name;
    rect_name.left += ui->DPI(6);
    rect_name.right -= ui->DPI(30);
    static CDrawCommon::ScrollInfo name_scroll_info;
    ui->GetDrawer().DrawScrollText(rect_name, list_item.GetDisplayName().c_str(), ui->GetUIColors().color_text_heighlight, ui->GetScrollTextPixel(), false, name_scroll_info);
    //绘制下拉按钮
    CRect rect_drop_down{ rect };
    rect_drop_down.left = rect_name.right + ui->DPI(2);
    rect_drop_down.right = menu_btn_rect.left - ui->DPI(6);
    CRect rect_drop_down_btn = CDrawCommon::CalculateCenterIconRect(rect_drop_down, icon_size);
    ui->DrawUIButton(rect_drop_down_btn, btn_drop_down, IconMgr::IconType::IT_DropDown);

    Element::Draw();
}

bool UiElement::PlaylistIndicator::LButtonUp(CPoint point)
{
    bool rtn = false;
    if (rect.PtInRect(point))
    {
        if (btn_drop_down.rect.PtInRect(point))
        {
            btn_drop_down.hover = false;
            CRect btn_rect = rect_name;
            AfxGetMainWnd()->ClientToScreen(&btn_rect);
            theApp.m_menu_mgr.GetMenu(MenuMgr::RecentFolderPlaylistMenu)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, btn_rect.left, btn_rect.bottom, AfxGetMainWnd());
        }
        else if (btn_menu.rect.PtInRect(point))
        {
            btn_menu.hover = false;
            CRect btn_rect = btn_menu.rect;
            AfxGetMainWnd()->ClientToScreen(&btn_rect);
            theApp.m_menu_mgr.GetMenu(MenuMgr::PlaylistToolBarMenu)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, btn_rect.left, btn_rect.bottom, AfxGetMainWnd());
        }
        rtn = true;
    }
    btn_drop_down.pressed = false;
    btn_menu.pressed = false;
    return rtn;
}

bool UiElement::PlaylistIndicator::LButtonDown(CPoint point)
{
    if (rect.PtInRect(point))
    {
        btn_drop_down.pressed = (btn_drop_down.rect.PtInRect(point) != FALSE);
        btn_menu.pressed = (btn_menu.rect.PtInRect(point) != FALSE);
        return true;
    }
    return false;
}

bool UiElement::PlaylistIndicator::MouseMove(CPoint point)
{
    btn_drop_down.hover = (btn_drop_down.rect.PtInRect(point) != FALSE);
    btn_menu.hover = (btn_menu.rect.PtInRect(point) != FALSE);

    if (btn_drop_down.hover)
        ui->UpdateMouseToolTipPosition(TooltipIndex::PLAYLIST_DROP_DOWN_BTN, btn_drop_down.rect);
    if (btn_menu.hover)
        ui->UpdateMouseToolTipPosition(TooltipIndex::PLAYLIST_MENU_BTN, btn_menu.rect);
    return false;
}

bool UiElement::PlaylistIndicator::MouseLeave()
{
    btn_drop_down.pressed = false;
    btn_drop_down.hover = false;
    btn_menu.pressed = false;
    btn_menu.hover = false;
    return true;
}

void UiElement::PlaylistIndicator::ClearRect()
{
    Element::ClearRect();
    btn_drop_down.rect = CRect();
    btn_menu.rect = CRect();
}

void UiElement::PlaylistIndicator::HideTooltip()
{
    ui->UpdateMouseToolTipPosition(TooltipIndex::PLAYLIST_DROP_DOWN_BTN, CRect());
    ui->UpdateMouseToolTipPosition(TooltipIndex::PLAYLIST_MENU_BTN, CRect());
}

void UiElement::PlaylistIndicator::FromXmlNode(tinyxml2::XMLElement* xml_node)
{
    Element::FromXmlNode(xml_node);
    CTinyXml2Helper::GetElementAttributeInt(xml_node, "font_size", font_size);
}
