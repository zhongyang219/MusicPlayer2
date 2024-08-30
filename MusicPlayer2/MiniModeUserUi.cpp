#include "stdafx.h"
#include "MiniModeUserUi.h"
#include "MiniModeDlg.h"

CMiniModeUserUi::CMiniModeUserUi(CWnd* pMainWnd, const std::wstring& xml_path)
    : CUserUi(pMainWnd, xml_path)
{
    InitUiPlaylist();
}

CMiniModeUserUi::CMiniModeUserUi(CWnd* pMainWnd, UINT id)
    : CUserUi(pMainWnd, id)
{
    InitUiPlaylist();
}

CMiniModeUserUi::~CMiniModeUserUi()
{
}

void CMiniModeUserUi::InitUiPlaylist()
{
    //保存原来的UI
    m_ui_element = m_root_default;
    CMiniModeDlg* pMinimodeDlg = dynamic_cast<CMiniModeDlg*>(m_pMainWnd);
    if (pMinimodeDlg != nullptr)
    {
        //如果使用UI播放列表，则向UI中添加一个播放列表元素
        if (pMinimodeDlg->IsUseUiPlaylist())
        {
            CElementFactory factory;
            //新的UI
            std::shared_ptr<UiElement::Element> ui_new = factory.CreateElement("element", this);
            //创建一个垂直布局
            std::shared_ptr<UiElement::Element> vertical_layout = factory.CreateElement("verticalLayout", this);
            //垂直布局添加到新的UI中
            ui_new->AddChild(vertical_layout);
            //原来的UI添加到垂直布局中
            vertical_layout->AddChild(m_ui_element);
            //添加一个搜索框
            std::shared_ptr<UiElement::Element> search_box = factory.CreateElement("searchBox", this);
            search_box->height.FromString("26");
            vertical_layout->AddChild(search_box);
            //添加一个播放列表
            m_playlist_emelment = factory.CreateElement("playlist", this);
            vertical_layout->AddChild(m_playlist_emelment);
            //使用新的UI
            m_root_default = ui_new;
        }
    }
}

bool CMiniModeUserUi::GetUiSize(int& width, int& height, int& height_with_playlist)
{
    if (m_ui_element != nullptr)
    {
        //设置绘图区域
        width = m_ui_element->width.GetValue(CRect());
        height = m_ui_element->height.GetValue(CRect());
        height_with_playlist = height + theApp.DPI(292);
        return true;
    }
    return false;
}

std::shared_ptr<UiElement::Playlist> CMiniModeUserUi::GetPlaylist() const
{
    return std::dynamic_pointer_cast<UiElement::Playlist>(m_playlist_emelment);
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
    m_colors = CPlayerUIHelper::GetUIColors(theApp.m_app_setting_data.theme_color, theApp.m_app_setting_data.dark_mode, IsDrawBackgroundAlpha());

    //设置绘图区域
    int width{}, height{}, height_width_playlist;
    GetUiSize(width, height, height_width_playlist);
    CSize window_size;
    if (IsShowUiPlaylist())
        window_size = CSize(width, height_width_playlist);
    else
        window_size = CSize(width, height);

    m_draw_rect = CRect(CPoint(0, 0), window_size);
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
        bool rtn = false;
        m_root_default->IterateAllElements([&](UiElement::Element* ele) -> bool {
            UiElement::Button* button = dynamic_cast<UiElement::Button*>(ele);
            if (button != nullptr && button->GetRect().PtInRect(point))
            {
                rtn = true;
                return true;
            }

            UiElement::StackElement* stack_element = dynamic_cast<UiElement::StackElement*>(ele);
            if (stack_element != nullptr && stack_element->indicator.rect.PtInRect(point))
            {
                rtn = true;
                return true;
            }

            UiElement::ListElement* list_emement = dynamic_cast<UiElement::ListElement*>(ele);
            if (list_emement != nullptr && list_emement->GetRect().PtInRect(point))
            {
                rtn = true;
                return true;
            }

            UiElement::SearchBox* search_box = dynamic_cast<UiElement::SearchBox*>(ele);
            if (search_box != nullptr && search_box->GetRect().PtInRect(point))
            {
                rtn = true;
                return true;
            }
            return false;
        });

        return rtn;
    }
    else
    {
        return true;
    }
}

const std::vector<std::shared_ptr<UiElement::Element>>& CMiniModeUserUi::GetStackElements() const
{
    //由于迷你模式只会有一个UI，因此这里直接返回m_stack_elements中的第一个
    if (!m_stack_elements.empty())
        return m_stack_elements.begin()->second;
    static std::vector<std::shared_ptr<UiElement::Element>> vec_empty;
    return vec_empty;
}

bool CMiniModeUserUi::IsShowUiPlaylist() const
{
    CMiniModeDlg* pMinimodeDlg = dynamic_cast<CMiniModeDlg*>(m_pMainWnd);
    if (pMinimodeDlg != nullptr)
    {
        return pMinimodeDlg->IsUseUiPlaylist() && pMinimodeDlg->IsShowPlaylist();
    }
    return false;
}
