#include "stdafx.h"
#include "PlaylistIndicator.h"


CListCache UiElement::PlaylistIndicator::m_list_cache(CListCache::SubsetType::ST_CURRENT);

void UiElement::PlaylistIndicator::Draw()
{
    m_list_cache.reload();
    CalculateRect();
    ui->DrawCurrentPlaylistIndicator(rect, this);
    Element::Draw();
}

bool UiElement::PlaylistIndicator::LButtonUp(CPoint point)
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
    btn_drop_down.pressed = false;
    btn_menu.pressed = false;
    return true;
}

bool UiElement::PlaylistIndicator::LButtonDown(CPoint point)
{
    btn_drop_down.pressed = (btn_drop_down.rect.PtInRect(point) != FALSE);
    btn_menu.pressed = (btn_menu.rect.PtInRect(point) != FALSE);
    return true;
}

bool UiElement::PlaylistIndicator::MouseMove(CPoint point)
{
    btn_drop_down.hover = (btn_drop_down.rect.PtInRect(point) != FALSE);
    btn_menu.hover = (btn_menu.rect.PtInRect(point) != FALSE);

    if (btn_drop_down.hover)
        ui->UpdateMouseToolTipPosition(TooltipIndex::PLAYLIST_DROP_DOWN_BTN, btn_drop_down.rect);
    if (btn_menu.hover)
        ui->UpdateMouseToolTipPosition(TooltipIndex::PLAYLIST_MENU_BTN, btn_menu.rect);
    return true;
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
