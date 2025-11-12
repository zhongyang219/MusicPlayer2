#include "stdafx.h"
#include "Volume.h"

void UiElement::Volume::Draw()
{
    CalculateRect();
    ui->DrawVolumeButton(rect, adj_btn_on_top, show_text);
    Element::Draw();
}

bool UiElement::Volume::MouseMove(CPoint point)
{
    bool hover = (rect.PtInRect(point));
    if (hover)
        ui->UpdateMouseToolTipPosition(CPlayerUIBase::BTN_VOLUME, rect);
    else
        ui->UpdateMouseToolTipPosition(CPlayerUIBase::BTN_VOLUME, CRect());
    return true;
}
