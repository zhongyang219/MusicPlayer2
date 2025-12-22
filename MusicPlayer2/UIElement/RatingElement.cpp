#include "stdafx.h"
#include "RatingElement.h"
#include "Player.h"
#include "MusicPlayerCmdHelper.h"

void UiElement::RatingElement::Draw()
{
    CalculateRect();

    //计算5个五角星的位置
    int icon_size = ui->DPI(16);
    for (int i = 0; i < STAR_COUNT; i++)
    {
        CRect icon_rect;
        icon_rect.top = rect.top + (rect.Height() - icon_size) / 2;
        icon_rect.bottom = icon_rect.top + icon_size;
        icon_rect.left = rect.left + icon_size * i;
        icon_rect.right = icon_rect.left + icon_size;
        m_icon_rect[i] = icon_rect;
    }

    //鼠标悬停时，判断鼠标在哪个五角星上
    int star_index = GetStarIndex(m_mouse_point);
    if (star_index >= 0)
    {
        for (int i = 0; i < STAR_COUNT; i++)
        {
            IconMgr::IconType icon{};
            if (i <= star_index)
                icon = IconMgr::IT_StarSelected;
            else
                icon = IconMgr::IT_Star;
            ui->DrawUiIcon(m_icon_rect[i], icon);
        }
    }
    //鼠标未悬停时，显示当前歌曲的分级
    else
    {
        CMusicPlayerCmdHelper helper;
        int rating = helper.GetRating(CPlayer::GetInstance().GetSafeCurrentSongInfo());
        for (int i = 0; i < STAR_COUNT; i++)
        {
            IconMgr::IconType icon{};
            if (rating >= 1 && rating <= 5 && i <= rating - 1)
                icon = IconMgr::IT_StarSelected;
            else
                icon = IconMgr::IT_Star;
            ui->DrawUiIcon(m_icon_rect[i], icon);
        }
    }

    Element::Draw();
}

bool UiElement::RatingElement::LButtonUp(CPoint point)
{
    CMusicPlayerCmdHelper helper;
    int star_index = GetStarIndex(m_mouse_point);
    if (star_index >= 0)
    {
        int cur_rating = helper.GetRating(CPlayer::GetInstance().GetSafeCurrentSongInfo());
        //如果点击是当前分级，则取消分级
        if (cur_rating == star_index + 1)
            helper.SetRating(CPlayer::GetInstance().GetSafeCurrentSongInfo(), 0);
        //否则设置分级
        else
            helper.SetRating(CPlayer::GetInstance().GetSafeCurrentSongInfo(), star_index + 1);
    }
    return false;
}

bool UiElement::RatingElement::MouseMove(CPoint point)
{
    m_mouse_point = point;
    return false;
}

int UiElement::RatingElement::GetStarIndex(const CPoint& point)
{
    int star_index = -1;
    for (int i = 0; i < STAR_COUNT; i++)
    {
        if (m_icon_rect[i].PtInRect(point))
            star_index = i;
    }
    return star_index;
}
