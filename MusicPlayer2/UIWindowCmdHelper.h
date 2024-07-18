#pragma once
#include "IPlayerUI.h"
class CUIWindowCmdHelper
{
public:
    CUIWindowCmdHelper(IPlayerUI* pUI);
    void OnUiCommand(DWORD command);
    void SetMenuState(CMenu* pMenu);

private:
    IPlayerUI* m_pUI;
};

