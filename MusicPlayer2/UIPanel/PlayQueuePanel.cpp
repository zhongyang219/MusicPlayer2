#include "stdafx.h"
#include "PlayQueuePanel.h"

CPlayQueuePanel::CPlayQueuePanel(CPlayerUIBase* ui)
	: CPlayerUIPanel(ui, ePanelType::PlayQueue)
{
}
CRect CPlayQueuePanel::CalculatePanelRect()
{
	CRect draw_rect = m_ui->GetDrawRect();
	int max_width = draw_rect.Width() - m_ui->DPI(48);
	int width = (std::min)(max_width, m_ui->DPI(320));
	draw_rect.left = draw_rect.right - width;

	return draw_rect;
}
