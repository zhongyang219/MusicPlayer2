#pragma once
#include "CPlayerUIBase.h"
#include "UIElement.h"

class CUserUi :
    public CPlayerUIBase
{
public:
    CUserUi(UIData& ui_data, CWnd* pMainWnd, const std::wstring& xml_path, int id);
    ~CUserUi();

    // 通过 CPlayerUIBase 继承
    virtual void _DrawInfo(CRect draw_rect, bool reset = false) override;
    virtual CString GetUIName() override;

private:
    int m_id;

protected:
    virtual int GetClassId() override;

};
