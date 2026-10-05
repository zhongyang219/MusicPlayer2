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
    Slider::MouseMove(point);
    progress_hover = rect.PtInRect(point) && !rect_handle.PtInRect(point);    //鼠标在进度条但是不在把手上悬停
    if (progress_hover || Slider::pressed)
    {
        __int64 song_pos = static_cast<__int64>(point.x - GetBackRect().left) * CPlayer::GetInstance().GetSongLength() / GetBackRect().Width();
        CCommon::SetNumRange<__int64>(song_pos, 0, CPlayer::GetInstance().GetSongLength());
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
        }

        return true;
    }

    if (last_hover && !progress_hover)
        HideTooltip();
    last_hover = progress_hover;

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

bool UiElement::SliderProgressBar::LButtonUp(CPoint point)
{
    if (Slider::LButtonUp(point))
        return true;
    if (rect.PtInRect(point) && !rect_handle.PtInRect(point))    //点击了进度条但是不在把手上
    {
        int click_pos = point.x - GetBackRect().left;
        CCommon::SetNumRange(click_pos, 0, GetBackRect().Width());
        double progress = static_cast<double>(click_pos) / GetBackRect().Width();
        if (CPlayer::GetInstance().GetPlayStatusMutex().try_lock_for(std::chrono::milliseconds(1000)))
        {
            CPlayer::GetInstance().SeekTo(progress);
            CPlayer::GetInstance().GetPlayStatusMutex().unlock();
        }
        return true;
    }
    return false;
}

bool UiElement::SliderProgressBar::SetCursor()
{
    if (progress_hover && !Slider::pressed)
    {
        ::SetCursor(::LoadCursor(NULL, IDC_HAND));
        return true;
    }
    return false;
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
