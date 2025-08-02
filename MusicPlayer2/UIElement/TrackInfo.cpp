#include "stdafx.h"
#include "TrackInfo.h"

void UiElement::TrackInfo::Draw()
{
    CalculateRect();
    ui->DrawSongInfo(rect, font_size);
    Element::Draw();
}
