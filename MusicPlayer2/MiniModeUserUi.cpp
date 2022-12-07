#include "stdafx.h"
#include "MiniModeUserUi.h"

CMiniModeUserUi::CMiniModeUserUi(UIData& ui_data, CWnd* pMainWnd, const std::wstring& xml_path)
    : CUserUi(ui_data, pMainWnd, xml_path)
{
}

CMiniModeUserUi::~CMiniModeUserUi()
{
}

bool CMiniModeUserUi::GetUiSize(int& width, int& height)
{
    if (m_root_default != nullptr)
    {
        //设置绘图区域
        width = m_root_default->width.GetValue(CRect(), this);
        height = m_root_default->height.GetValue(CRect(), this);
        return true;
    }
    return false;
}

void CMiniModeUserUi::_DrawInfo(CRect draw_rect, bool reset /*= false*/)
{
    if (m_root_default != nullptr)
    {
        m_root_default->SetRect(draw_rect);
        m_root_default->Draw(this);
        //绘制音量调整按钮
        DrawVolumnAdjBtn();
    }
}

void CMiniModeUserUi::PreDrawInfo()
{
    //设置颜色
    m_colors = CPlayerUIHelper::GetUIColors(theApp.m_app_setting_data.theme_color, theApp.m_app_setting_data.dark_mode);
    if (m_root_default != nullptr)
    {
        //设置绘图区域
        int width{}, height{};
        GetUiSize(width, height);
        m_draw_rect = CRect(CPoint(0, 0), CSize(width, height));
    }
}
