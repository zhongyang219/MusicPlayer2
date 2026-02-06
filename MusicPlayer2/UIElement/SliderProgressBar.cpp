#include "stdafx.h"
#include "SliderProgressBar.h"
#include "Player.h"

void UiElement::SliderProgressBar::Draw()
{
    //设置进度条的位置
    SetRange(0, CPlayer::GetInstance().GetSongLength());
    if (!pressed)
        SetCurPos(CPlayer::GetInstance().GetCurrentPosition());

    Slider::Draw();

    //绘制AB重复标记
    ui->DrawABRepeat(rect);
}

void UiElement::SliderProgressBar::InitComplete()
{
    Slider::InitComplete();
    SetDragFinishTrigger([this](UiElement::Slider* sender) {
        int progress = sender->GetCurPos();
        if (CPlayer::GetInstance().GetPlayStatusMutex().try_lock_for(std::chrono::milliseconds(1000)))
        {
            CPlayer::GetInstance().SeekTo(progress);
            CPlayer::GetInstance().GetPlayStatusMutex().unlock();
        }
    });
}

bool UiElement::SliderProgressBar::MouseMove(CPoint point)
{
    if (Slider::MouseMove(point))
    {
        __int64 song_pos;
        song_pos = static_cast<__int64>(point.x - GetBackRect().left) * CPlayer::GetInstance().GetSongLength() / GetBackRect().Width();
        CPlayTime song_pos_time;
        song_pos_time.fromInt(static_cast<int>(song_pos));
        static int last_sec{};
        if (last_sec != song_pos_time.sec)      //只有鼠标指向位置对应的秒数变化了才更新鼠标提示
        {
            wstring min = std::to_wstring(song_pos_time.min);
            wstring sec = std::to_wstring(song_pos_time.sec);
            wstring str = theApp.m_str_table.LoadTextFormat(L"UI_TIP_SEEK_TO_MINUTE_SECOND", { min, sec.size() <= 1 ? L'0' + sec : sec });
            ui->UpdateMouseToolTip(UiElement::TooltipIndex::PROGRESS_BAR, str.c_str());
            ui->UpdateMouseToolTipPosition(UiElement::TooltipIndex::PROGRESS_BAR, GetRect());
            last_sec = song_pos_time.sec;
            //TRACE("Progressbar mouse move\n");
        }

        return true;
    }

    bool hover = rect.PtInRect(point);
    if (last_hover && !hover)
        HideTooltip();
    last_hover = hover;

    return false;
}

bool UiElement::SliderProgressBar::MouseLeave()
{
    Slider::MouseLeave();
    HideTooltip();
    return false;
}

void UiElement::SliderProgressBar::HideTooltip()
{
    ui->UpdateMouseToolTipPosition(TooltipIndex::PROGRESS_BAR, CRect());
}

COLORREF UiElement::SliderProgressBar::GetBackColor(bool highlight_color)
{
    if (highlight_color)
        return ui->GetUIColors().color_spectrum;
    else
        return ui->GetUIColors().color_progress_back;
}

BYTE UiElement::SliderProgressBar::GetBackAlpha(bool highlight_color)
{
    if (highlight_color)
        return 255;
    else
        return ui->GetDefaultAlpha();
}
