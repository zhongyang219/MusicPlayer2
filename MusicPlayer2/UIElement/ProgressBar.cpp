#include "stdafx.h"
#include "ProgressBar.h"
#include "Player.h"
#include "TinyXml2Helper.h"

void UiElement::ProgressBar::Draw()
{
    CalculateRect();
    if (show_play_time)
    {
        btn.rect = ui->DrawProgressBar(rect, play_time_both_side);
    }
    else
    {
        btn.rect = ui->DrawProgess(rect);
    }
    Element::Draw();
}

bool UiElement::ProgressBar::LButtonUp(CPoint point)
{
    if (hover() && btn.rect.PtInRect(point))
    {
        int ckick_pos = point.x - btn.rect.left;
        double progress = static_cast<double>(ckick_pos) / btn.rect.Width();
        if (CPlayer::GetInstance().GetPlayStatusMutex().try_lock_for(std::chrono::milliseconds(1000)))
        {
            CPlayer::GetInstance().SeekTo(progress);
            CPlayer::GetInstance().GetPlayStatusMutex().unlock();
        }
        return true;
    }
    return false;
}

bool UiElement::ProgressBar::LButtonDown(CPoint point)
{
    return btn.hover;
}

bool UiElement::ProgressBar::RButtonUp(CPoint point)
{
    //进度条不弹出右键菜单
    return btn.rect.PtInRect(point);
}

bool UiElement::ProgressBar::MouseMove(CPoint point)
{
    btn.hover = btn.rect.PtInRect(point);

    //鼠标指向进度条时显示定位到几分几秒
    if (btn.hover)
    {
        __int64 song_pos;
        song_pos = static_cast<__int64>(point.x -btn.rect.left) * CPlayer::GetInstance().GetSongLength() /btn.rect.Width();
        CPlayTime song_pos_time;
        song_pos_time.fromInt(static_cast<int>(song_pos));
        static int last_sec{};
        if (last_sec != song_pos_time.sec)      //只有鼠标指向位置对应的秒数变化了才更新鼠标提示
        {
            wstring min = std::to_wstring(song_pos_time.min);
            wstring sec = std::to_wstring(song_pos_time.sec);
            wstring str = theApp.m_str_table.LoadTextFormat(L"UI_TIP_SEEK_TO_MINUTE_SECOND", { min, sec.size() <= 1 ? L'0' + sec : sec });
            ui->UpdateMouseToolTip(UiElement::TooltipIndex::PROGRESS_BAR, str.c_str());
            ui->UpdateMouseToolTipPosition(UiElement::TooltipIndex::PROGRESS_BAR, btn.rect);
            last_sec = song_pos_time.sec;
        }
    }
    return false;
}

bool UiElement::ProgressBar::SetCursor()
{
    if (hover())
    {
        ::SetCursor(::LoadCursor(NULL, IDC_HAND));
        return true;
    }
    return false;
}

void UiElement::ProgressBar::HideTooltip()
{
    ui->UpdateMouseToolTipPosition(TooltipIndex::PROGRESS_BAR, CRect());
}

bool UiElement::ProgressBar::hover() const
{
    return btn.hover;
}

void UiElement::ProgressBar::FromXmlNode(tinyxml2::XMLElement* xml_node)
{
    Element::FromXmlNode(xml_node);
    std::string str_show_play_time = CTinyXml2Helper::ElementAttribute(xml_node, "show_play_time");
    show_play_time = CTinyXml2Helper::StringToBool(str_show_play_time.c_str());
    std::string str_play_time_both_side = CTinyXml2Helper::ElementAttribute(xml_node, "play_time_both_side");
    play_time_both_side = CTinyXml2Helper::StringToBool(str_play_time_both_side.c_str());
}
