#include "stdafx.h"
#include "UserUi.h"


CUserUi::CUserUi(UIData& ui_data, CWnd* pMainWnd, const std::wstring& xml_path, int id)
    : CPlayerUIBase(ui_data, pMainWnd), m_id(id)
{
}


CUserUi::~CUserUi()
{
}

void CUserUi::_DrawInfo(CRect draw_rect, bool reset)
{
}

CString CUserUi::GetUIName()
{
    return _T("Test UI");
}

int CUserUi::GetClassId()
{
    return m_id;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
