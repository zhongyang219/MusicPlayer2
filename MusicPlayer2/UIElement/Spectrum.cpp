#include "stdafx.h"
#include "Spectrum.h"

void UiElement::Spectrum::Draw()
{
    CalculateRect();
    if (theApp.m_app_setting_data.show_spectrum)
    {
        ui->m_draw.DrawSpectrum(rect, type, draw_reflex, theApp.m_app_setting_data.spectrum_low_freq_in_center, fixed_width, align);
        Element::Draw();
    }
}

bool UiElement::Spectrum::IsEnable(CRect parent_rect) const
{
    if (theApp.m_app_setting_data.show_spectrum)
        return UiElement::Element::IsEnable(parent_rect);
    return false;
}
