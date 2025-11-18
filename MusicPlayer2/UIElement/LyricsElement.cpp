#include "stdafx.h"
#include "LyricsElement.h"
#include "Rectangle.h"
void UiElement::Lyrics::Draw()
{
    CalculateRect();

    bool big_font{ ui->m_ui_data.full_screen && ui->IsDrawLargeIcon() };
    CFont* lyric_font = &theApp.m_font_set.lyric.GetFont(big_font);
    CFont* lyric_tr_font = &theApp.m_font_set.lyric_translate.GetFont(big_font);

    if (use_default_font)   // 目前这个bool有些冗余，当字体与字号在m_font_set中解耦后有用
    {
        lyric_font = &theApp.m_font_set.GetFontBySize(font_size).GetFont(big_font);
        lyric_tr_font = &theApp.m_font_set.GetFontBySize(font_size - 1).GetFont(big_font);
    }

    //如果父元素中包含了矩形元素，则即使在“外观设置”中勾选了“歌词界面背景”，也不再为歌词区域绘制半透明背景
    ui->DrawLyrics(rect, lyric_font, lyric_tr_font, (!no_background && !IsParentRectangle()), show_song_info);

    Element::Draw();
}

void UiElement::Lyrics::ClearRect()
{
    rect = CRect();
}

bool UiElement::Lyrics::RButtonUp(CPoint point)
{
    if (rect.PtInRect(point))
    {
        CPoint point1;
        GetCursorPos(&point1);
        theApp.m_menu_mgr.GetMenu(MenuMgr::MainAreaLrcMenu)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, theApp.m_pMainWnd);
        return true;
    }
    return false;
}

bool UiElement::Lyrics::IsParentRectangle() const
{
    const Element* ele{ this };
    while (ele != nullptr && ele->pParent != nullptr)
    {
        if (dynamic_cast<const Rectangle*>(ele) != nullptr)
            return true;
        ele = ele->pParent;
    }
    return false;
}
