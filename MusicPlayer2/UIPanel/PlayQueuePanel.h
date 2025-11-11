#pragma once
#include "PlayerUIPanel.h"
class CPlayQueuePanel : public CPlayerUIPanel
{
public:
	CPlayQueuePanel(CPlayerUIBase* ui);

private:
	virtual CRect GetPanelRect() override;
};

