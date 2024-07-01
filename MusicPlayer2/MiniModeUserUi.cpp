#include "stdafx.h"
#include "MiniModeUserUi.h"

CMiniModeUserUi::CMiniModeUserUi(CWnd* pMainWnd, const std::wstring& xml_path)
    : CUserUi(pMainWnd, xml_path)
{
}

CMiniModeUserUi::CMiniModeUserUi(CWnd* pMainWnd, UINT id)
    : CUserUi(pMainWnd, id)
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
        width = m_root_default->width.GetValue(CRect());
        height = m_root_default->height.GetValue(CRect());
        return true;
    }
    return false;
}

void CMiniModeUserUi::_DrawInfo(CRect draw_rect, bool reset /*= false*/)
{
    if (m_root_default != nullptr)
    {
        m_root_default->SetRect(draw_rect);
        m_root_default->Draw();
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

bool CMiniModeUserUi::LButtonUp(CPoint point)
{
    for (auto& btn : m_buttons)
    {
        if (btn.second.rect.PtInRect(point))
        {
            switch (btn.first)
            {
            case BTN_MINI:
                btn.second.hover = false;
                btn.second.pressed = false;
                m_pMainWnd->SendMessage(WM_COMMAND, IDOK);
                return true;
            case BTN_CLOSE:
                if (theApp.m_general_setting_data.minimize_to_notify_icon)
                    m_pMainWnd->ShowWindow(HIDE_WINDOW);
                else
                    m_pMainWnd->SendMessage(WM_COMMAND, ID_MINI_MODE_EXIT);
                return true;
            case BTN_SHOW_PLAYLIST:
                btn.second.hover = false;
                btn.second.pressed = false;
                m_pMainWnd->SendMessage(WM_COMMAND, ID_SHOW_PLAY_LIST);
                return true;
            case BTN_SKIN:
            {
                btn.second.hover = false;
                btn.second.pressed = false;
                CPoint point1;
                GetCursorPos(&point1);
                CMenu* pMenu = theApp.m_menu_mgr.GetMenu(MenuMgr::MiniModeSwitchUiMenu);
                ASSERT(pMenu != nullptr);
                if (pMenu != nullptr)
                    pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, m_pMainWnd);
                return true;
            }
            }

        }
    }

    return CUserUi::LButtonUp(point);
}

bool CMiniModeUserUi::PointInControlArea(CPoint point) const
{
    if (!__super::PointInControlArea(point))
    {
        const auto& stack_elements{ GetStackElements() };
        for (const auto& element : stack_elements)
        {
            UiElement::StackElement* stack_element = dynamic_cast<UiElement::StackElement*>(element.get());
            if (stack_element != nullptr)
            {
                if (stack_element->indicator.rect.PtInRect(point))
                    return true;
            }
        }
        return false;
    }
    else
    {
        return true;
    }
}
