#include "stdafx.h"
#include "UserUi.h"
#include "UiSearchBox.h"
#include "UIPanel/ListPreviewPanel.h"
#include "UIElement/Helper/UiElementHelper.h"

CUserUi::CUserUi(CWnd* pMainWnd, const std::wstring& xml_path)
    : CPlayerUIBase(theApp.m_ui_data, pMainWnd), m_xml_path(xml_path)
{
    size_t length;
    const char* xml_contents = CCommon::GetFileContent(m_xml_path.c_str(), length);
    LoadFromContents(std::string(xml_contents, length));
    delete[] xml_contents;
}

CUserUi::CUserUi(CWnd* pMainWnd, UINT id)
    : CPlayerUIBase(theApp.m_ui_data, pMainWnd)
{
    string xml = CCommon::GetTextResourceRawData(id);
    LoadFromContents(xml);
}


CUserUi::~CUserUi()
{
}

void CUserUi::LoadFromContents(const std::string& xml_contents)
{
    tinyxml2::XMLDocument xml_doc;
    xml_doc.Parse(xml_contents.c_str());
    tinyxml2::XMLElement* root = xml_doc.RootElement();
    m_ui_name = CCommon::StrToUnicode(CTinyXml2Helper::ElementAttribute(root, "name"), CodeType::UTF8_NO_BOM);
    ReplaceUiStringRes(m_ui_name);
    std::string ui_index = CTinyXml2Helper::ElementAttribute(root, "index");
    if (!ui_index.empty())
        m_index = atoi(ui_index.c_str());
    CTinyXml2Helper::IterateChildNode(root, [&](tinyxml2::XMLElement* xml_child) {
        std::string item_name = CTinyXml2Helper::ElementName(xml_child);
        if (item_name == "ui")
        {
            std::string str_type = CTinyXml2Helper::ElementAttribute(xml_child, "type");
            if (str_type == "big")
                m_root_ui_big = BuildUiElementFromXmlNode(xml_child, this);
            else if (str_type == "narrow")
                m_root_ui_narrow = BuildUiElementFromXmlNode(xml_child, this);
            else if (str_type == "small")
                m_root_ui_small = BuildUiElementFromXmlNode(xml_child, this);
            else
                m_root_default = BuildUiElementFromXmlNode(xml_child, this);
        }
        else if (item_name == "panel")
        {
            std::shared_ptr<UiElement::Panel> panel_element = std::dynamic_pointer_cast<UiElement::Panel>(BuildUiElementFromXmlNode(xml_child, this));
            std::wstring panel_id = CCommon::StrToUnicode(CTinyXml2Helper::ElementAttribute(xml_child, "id"), CodeType::UTF8_NO_BOM);
            if (!panel_id.empty())
            {
                //在这里可以根据面板id创建自定义的面板类
                m_panel_mgr.AddPanel(CPanelManager::PanelKey(ePanelType::PanelFromUi, panel_id), std::make_unique<CPlayerUIPanel>(this, panel_element));
            }
        }
    });
}

void CUserUi::SetIndex(int index)
{
    m_index = index;
}

bool CUserUi::IsIndexValid() const
{
    return m_index != INT_MAX;
}

void CUserUi::IterateAllElements(std::function<bool(UiElement::Element*)> func, bool visible_only)
{
    std::shared_ptr<UiElement::Element> draw_element = GetCurrentTypeUi();
    draw_element->IterateAllElements(func, visible_only);
}

void CUserUi::IterateAllElementsInAllUi(std::function<bool(UiElement::Element*)> func)
{
    m_root_ui_big->IterateAllElements(func);
    m_root_ui_narrow->IterateAllElements(func);
    m_root_ui_small->IterateAllElements(func);
    m_root_default->IterateAllElements(func);
}

void CUserUi::VolumeAdjusted()
{
    IterateAllElements([this](UiElement::Element* element) ->bool
        {
            UiElement::Text* text_element{ dynamic_cast<UiElement::Text*>(element) };
            if (text_element != nullptr)
            {
                text_element->SetShowVolumn(true);
            }
            return false;
        });
    KillTimer(theApp.m_pMainWnd->GetSafeHwnd(), SHOW_VOLUME_TIMER_ID);
    //设置一个音量显示时间的定时器（音量显示保持1.5秒）
    SetTimer(theApp.m_pMainWnd->GetSafeHwnd(), SHOW_VOLUME_TIMER_ID, 1500, NULL);
}

void CUserUi::ResetVolumeToPlayTime()
{
    KillTimer(theApp.m_pMainWnd->GetSafeHwnd(), SHOW_VOLUME_TIMER_ID);
    IterateAllElements([this](UiElement::Element* element) ->bool
        {
            UiElement::Text* text_element{ dynamic_cast<UiElement::Text*>(element) };
            if (text_element != nullptr)
            {
                text_element->SetShowVolumn(false);
            }
            return false;
        });
}

void CUserUi::PlaylistLocateToCurrent()
{
    //遍历Playlist元素
    auto root_element = GetMouseEventResponseElement();
    root_element->IterateAllElements([&](UiElement::Element* element) ->bool {
        UiElement::Playlist* playlist_element{ dynamic_cast<UiElement::Playlist*>(element) };
        if (playlist_element != nullptr)
        {
            playlist_element->EnsureHighlightItemVisible();
        }
        return false;
    });
}

void CUserUi::ListLocateToCurrent()
{
    //遍历ListElement元素
    auto root_element = GetMouseEventResponseElement();
    root_element->IterateAllElements([&](UiElement::Element* element) ->bool {
        UiElement::AbstractListElement* playlist_element{ dynamic_cast<UiElement::AbstractListElement*>(element) };
        if (playlist_element != nullptr)
        {
            playlist_element->EnsureHighlightItemVisible();
        }
        return false;
    });
}

void CUserUi::PlaylistSelectAll()
{
    //遍历Playlist元素
    auto root_element = GetMouseEventResponseElement();
    root_element->IterateAllElements([&](UiElement::Element* element) ->bool {
        UiElement::Playlist* playlist_element{ dynamic_cast<UiElement::Playlist*>(element) };
        if (playlist_element != nullptr)
        {
            playlist_element->SelectAll();
        }
        return false;
    });
}

void CUserUi::PlaylistSelectNone()
{
    //遍历Playlist元素
    auto root_element = GetMouseEventResponseElement();
    root_element->IterateAllElements([&](UiElement::Element* element) ->bool {
        UiElement::Playlist* playlist_element{ dynamic_cast<UiElement::Playlist*>(element) };
        if (playlist_element != nullptr)
        {
            playlist_element->SelectNone();
        }
        return false;
    });
}

void CUserUi::PlaylistSelectRevert()
{
    //遍历Playlist元素
    auto root_element = GetMouseEventResponseElement();
    root_element->IterateAllElements([&](UiElement::Element* element) ->bool {
        UiElement::Playlist* playlist_element{ dynamic_cast<UiElement::Playlist*>(element) };
        if (playlist_element != nullptr)
        {
            playlist_element->SelectReversed();
        }
        return false;
    });
}

void CUserUi::SaveUiData(CArchive& archive)
{
    //使用了“显示/隐藏元素”属性的按钮指定的元素的显示/隐藏状态
    std::vector<std::pair<std::string, bool>> elements_show_hide_status;
    //保存StackElement元素
    IterateAllElementsInAllUi([&](UiElement::Element* element) ->bool {
        UiElement::StackElement* stack_element{ dynamic_cast<UiElement::StackElement*>(element) };
        if (stack_element != nullptr)
        {
            if (!stack_element->IsHoverSwitch() && !stack_element->IsSizeChangeSwitch())
                archive << static_cast<BYTE>(stack_element->GetCurIndex());
        }
        UiElement::Button* button_element{ dynamic_cast<UiElement::Button*>(element) };
        if (button_element != nullptr)
        {
            if (button_element->GetKey() == BtnKey::BTN_SHOW_HIDE_ELEMENT)
            {
                UiElement::Element* element = FindElementInAllUi<UiElement::Element>(button_element->GetRelatedElementId());
                if (element != nullptr)
                {
                    bool element_show = element->IsVisible();
                    elements_show_hide_status.push_back(std::make_pair(button_element->GetRelatedElementId(), element_show));
                }
            }
        }
        return false;
    });

    archive << static_cast<int>(elements_show_hide_status.size());
    for (const auto& element_show_hide : elements_show_hide_status)
    {
        archive << CString(CCommon::StrToUnicode(element_show_hide.first, CodeType::UTF8_NO_BOM).c_str());
        archive << (BYTE)element_show_hide.second;
    }
}

void CUserUi::LoadUiData(CArchive& archive, int version)
{
    IterateAllElementsInAllUi([&](UiElement::Element* element) ->bool {
        UiElement::StackElement* stack_element{ dynamic_cast<UiElement::StackElement*>(element) };
        if (stack_element != nullptr)
        {
            if (!stack_element->IsHoverSwitch() && !stack_element->IsSizeChangeSwitch())
            {
                BYTE stack_element_index;
                archive >> stack_element_index;
                stack_element->SetCurrentElement(stack_element_index);
            }
        }
        return false;
    });
    //载入使用了“显示/隐藏元素”属性的按钮指定的元素的显示/隐藏状态
    if (version >= 1)
    {
        int element_size{};
        archive >> element_size;
        for (int i = 0; i < element_size; i++)
        {
            CString str_id;
            archive >> str_id;
            std::string str_element_id = CCommon::UnicodeToStr(str_id.GetString(), CodeType::UTF8_NO_BOM);
            BYTE show;
            archive >> show;
            UiElement::Element* element = FindElementInAllUi<UiElement::Element>(str_element_id);
            if (element != nullptr)
                element->SetVisible(show);
        }
    }
}


void CUserUi::_DrawInfo(CRect draw_rect, bool reset)
{
    std::shared_ptr<UiElement::Element> draw_element = GetCurrentTypeUi();
    if (draw_element != nullptr)
    {
        if (m_ui_data.full_screen)  //全屏模式下，最外侧的边距需要加宽
        {
            draw_rect.DeflateRect(EdgeMargin(true), EdgeMargin(false));
        }
        draw_element->SetRect(draw_rect);

        //仅当面板未占满整个窗口时才绘制界面
        if (!m_panel_mgr.IsPanelFullFill())
        {
            draw_element->Draw();
            draw_element->DrawTopMost();
        }
    }

    //绘制面板
    m_panel_mgr.DrawPanel();

    //绘制右上角图标
    DrawTopRightIcons();

    //全屏模式时在右上角绘制时间
    if (m_ui_data.full_screen)
    {
        DrawCurrentTime();
    }

    m_thumbnail_rect = draw_rect;
}

std::shared_ptr<UiElement::Element> CUserUi::GetCurrentTypeUi() const
{
    //根据不同的窗口大小选择不同的界面元素的根节点绘图
    auto ui_size = GetUiSize();
    return GetUiByUiSize(ui_size);
}

std::shared_ptr<UiElement::Element> CUserUi::GetUiByUiSize(UiSize ui_size) const
{
    std::shared_ptr<UiElement::Element> draw_element;
    //<ui type="small">
    if (ui_size == CPlayerUIBase::UiSize::SMALL)
    {
        if (m_root_ui_small != nullptr)
            draw_element = m_root_ui_small;
        else if (m_root_ui_narrow != nullptr)
            draw_element = m_root_ui_narrow;
        else
            draw_element = m_root_default;
    }
    //<ui type="big">
    else if (ui_size == CPlayerUIBase::UiSize::BIG)
    {
        if (m_root_ui_big != nullptr)
            draw_element = m_root_ui_big;
        else
            draw_element = m_root_default;
    }
    //<ui type="narrow">
    else
    {
        if (m_root_ui_narrow != nullptr)
            draw_element = m_root_ui_narrow;
        else
            draw_element = m_root_default;
    }
    return draw_element;
}

wstring CUserUi::GetUIName()
{
    return m_ui_name;
}

bool CUserUi::LButtonUp(CPoint point)
{
    auto root_element = GetMouseEventResponseElement();
    if (root_element->GlobalLButtonUp(point))
        return true;
    if (!CPlayerUIBase::LButtonUp(point) && !CPlayerUIBase::PointInMenubarArea(point) && !CPlayerUIBase::PointInTitlebarArea(point))
    {
        //显示了面板，且不是全屏面板的情况下，点击面板以外的地方关闭面板
        auto* panel = m_panel_mgr.GetTopPanel();
        if (panel != nullptr && !panel->IsFullFill() && !panel->GetPanelRect().PtInRect(point) && !panel->GetPanelRect().PtInRect(m_mouse_clicked_point))
        {
            OnPanelHide();
            m_panel_mgr.HidePanel();
            return true;
        }

        //遍历所有元素
        bool rtn = root_element->LButtonUp(point);
        return rtn;
    }
    return false;
}

bool CUserUi::LButtonDown(CPoint point)
{
    m_mouse_clicked_point = point;
    auto root_element = GetMouseEventResponseElement();
    if (root_element->GlobalLButtonDown(point))
        return true;
    if (!CPlayerUIBase::LButtonDown(point))
    {
        bool rtn = false;
        if (!CPlayerUIBase::PointInMenubarArea(point) && !CPlayerUIBase::PointInTitlebarArea(point))
        {
            rtn = root_element->LButtonDown(point);
        }
        return rtn;
    }
    return true;
}

bool CUserUi::MouseMove(CPoint point)
{
    bool mouse_leave = false;
    auto root_element = GetMouseEventResponseElement();
    if (root_element->GlobalMouseMove(point))
        return true;
    if (!CPlayerUIBase::MouseMove(point))
    {
        bool mouse_in_draw_area{ !CPlayerUIBase::PointInMenubarArea(point) && !CPlayerUIBase::PointInTitlebarArea(point) };
        if (mouse_in_draw_area)
        {
            root_element->MouseMove(point);
        }

        //鼠标离开绘图区域后发送MouseLeave消息
        if (m_last_mouse_in_draw_area && !mouse_in_draw_area)
        {
            mouse_leave = true;
        }

        m_last_mouse_in_draw_area = mouse_in_draw_area;
    }
    else
    {
        mouse_leave = true;
    }
    if (mouse_leave)
    {
        root_element->MouseLeave();
    }
    return true;
}

bool CUserUi::MouseLeave()
{
    //遍历所有元素
    auto root_element = GetMouseEventResponseElement();
    root_element->MouseLeave();

    return CPlayerUIBase::MouseLeave();
}


bool CUserUi::RButtonUp(CPoint point)
{
    //遍历所有元素
    bool rtn = false;
    if (!CPlayerUIBase::PointInMenubarArea(point) && !CPlayerUIBase::PointInTitlebarArea(point))
    {
        auto root_element = GetMouseEventResponseElement();
        rtn = root_element->RButtonUp(point);
        root_element->GlobalLButtonUp(point);
    }
    if (!rtn)
        rtn = CPlayerUIBase::RButtonUp(point);
    return rtn;
}


bool CUserUi::RButtonDown(CPoint point)
{
    //遍历所有元素
    if (!CPlayerUIBase::PointInMenubarArea(point) && !CPlayerUIBase::PointInTitlebarArea(point))
    {
        auto root_element = GetMouseEventResponseElement();
        root_element->RButtonDown(point);
    }
    return CPlayerUIBase::RButtonDown(point);
}

bool CUserUi::MouseWheel(int delta, CPoint point)
{
    //遍历所有元素
    auto root_element = GetMouseEventResponseElement();
    bool rtn = root_element->MouseWheel(delta, point);
    if (rtn)
        return true;
    return CPlayerUIBase::MouseWheel(delta, point);
}

bool CUserUi::DoubleClick(CPoint point)
{
    //遍历所有元素
    bool rtn = false;
    auto root_element = GetMouseEventResponseElement();
    rtn = root_element->DoubleClick(point);
    if (rtn)
        return true;
    return CPlayerUIBase::DoubleClick(point);
}

void CUserUi::UiSizeChanged(UiSize last_ui_size)
{
    ListLocateToCurrent();
    //清除之前UI的鼠标提示
    auto last_ui = GetUiByUiSize(last_ui_size);
    last_ui->IterateAllElements([&](UiElement::Element* element) ->bool {
        element->HideTooltip();
        return false;
    });
}

bool CUserUi::SetCursor()
{
    bool cursor_changed = false;
    auto root_element = GetMouseEventResponseElement();
    root_element->IterateAllElements([&](UiElement::Element* element) ->bool {
        if (element->SetCursor())
        {
            cursor_changed = true;
            return true;
        }
        return false;
    }, true);

    if (cursor_changed)
        return true;

    return CPlayerUIBase::SetCursor();
}

bool CUserUi::ButtonClicked(BtnKey btn_type, const UIButton& btn)
{
    if (btn_type == BTN_SHOW_PLAY_QUEUE)
    {
        ShowPanelByResId(IDR_PLAY_QUEUE_PANEL);
        return true;
    }
    else if (btn_type == BTN_CLOSE_PANEL || btn_type == BTN_CLOSE_PANEL_TITLE_BAR)
    {
        OnPanelHide();
        m_panel_mgr.HidePanel();
        return true;
    }
    else if (btn_type == BTN_SHOW_SETTINGS_PANEL)
    {
        ShowPanelByResId(IDR_SETTINGS_PANEL);
        return true;
    }
    return CPlayerUIBase::ButtonClicked(btn_type, btn);
}

bool CUserUi::IsDrawTitlebarLeftBtn() const
{
    //面板占满窗口时在标题栏左侧显示返回按钮
    return m_panel_mgr.IsPanelFullFill();
}

int CUserUi::GetUiIndex()
{
    return m_index;
}

std::shared_ptr<UiElement::Element> CUserUi::BuildUiElementFromXmlNode(tinyxml2::XMLElement* xml_node, CPlayerUIBase* ui)
{
    UiElement::CElementFactory factory;
    //获取节点名称
    std::string item_name = CTinyXml2Helper::ElementName(xml_node);
    //根据节点名称创建ui元素
    std::shared_ptr<UiElement::Element> element = factory.CreateElement(item_name, ui);
    if (element != nullptr)
    {
        //从xml节点获取元素属性
        element->FromXmlNode(xml_node);

        element->InitComplete();

        //递归调用此函数创建子节点
        CTinyXml2Helper::IterateChildNode(xml_node, [&](tinyxml2::XMLElement* xml_child)
            {
                std::shared_ptr<UiElement::Element> ui_child = BuildUiElementFromXmlNode(xml_child, ui);
                if (ui_child != nullptr)
                {
                    element->AddChild(ui_child);
                }
            });
    }
    return element;
}

void CUserUi::SwitchStackElement()
{
    UiElement::StackElement* stack_element = FindElement<UiElement::StackElement>();
    //当stackElement设置了hover_to_switch或size_change_to_switch时不允许主动切换
    if (stack_element != nullptr && !stack_element->IsHoverSwitch() && !stack_element->IsSizeChangeSwitch())
        stack_element->SwitchDisplay();
}

void CUserUi::SwitchStackElement(std::string id, int index)
{
    UiElement::StackElement* stack_element = FindElement<UiElement::StackElement>(id);
    //当stackElement设置了hover_to_switch或size_change_to_switch时不允许主动切换
    if (stack_element != nullptr && !stack_element->IsHoverSwitch() && !stack_element->IsSizeChangeSwitch())
    {
        if (index >= 0)
            stack_element->SetCurrentElement(index);
        else
            stack_element->SwitchDisplay();
    }
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void CUserUi::UniqueUiIndex(std::vector<std::shared_ptr<CUserUi>>& ui_list)
{
    for (auto& ui : ui_list)
    {
        if (ui != nullptr)
        {
            //遍历UI列表，获取当前UI的序号
            int ui_index = ui->GetUiIndex();
            //如果还有其他UI的序号和当前UI相同
            auto ui_matched_index = FindUiByIndex(ui_list, ui_index, ui);
            if (ui_matched_index != nullptr)
            {
                //将找到的UI的序号设置为最大序号加1
                ui_matched_index->SetIndex(GetMaxUiIndex(ui_list) + 1);
            }
        }
    }
}

std::shared_ptr<CUserUi> CUserUi::FindUiByIndex(const std::vector<std::shared_ptr<CUserUi>>& ui_list, int ui_index, std::shared_ptr<CUserUi> except)
{
    for (const auto& ui : ui_list)
    {
        if (ui != nullptr && ui != except && ui->GetUiIndex() == ui_index)
            return ui;
    }
    return nullptr;
}

int CUserUi::GetMaxUiIndex(const std::vector<std::shared_ptr<CUserUi>>& ui_list)
{
    int index_max{};
    for (const auto& ui : ui_list)
    {
        if (ui != nullptr && ui->GetUiIndex() > index_max)
            index_max = ui->GetUiIndex();
    }
    return index_max;
}

std::shared_ptr<UiElement::Element> CUserUi::GetMouseEventResponseElement()
{
    auto* panel = m_panel_mgr.GetTopPanel();
    if (panel != nullptr)
        return panel->GetRootElement();
    return GetCurrentTypeUi();
}

CPlayerUIPanel* CUserUi::ShowPanelByResId(UINT resId)
{
    auto* panel = m_panel_mgr.ShowPanel(CPanelManager::PanelKey(resId));
    //显示面板后隐藏界面中按钮的鼠标提示
    if (panel != nullptr)
        OnPanelShow();
    return panel;
}

CPlayerUIPanel* CUserUi::ShowPanelByFileName(const std::wstring panel_file_name)
{
    auto* panel = m_panel_mgr.ShowPanel(CPanelManager::PanelKey(ePanelType::PanelFromFile, panel_file_name));
    //显示面板后隐藏界面中按钮的鼠标提示
    if (panel != nullptr)
        OnPanelShow();
    return panel;
}

CPlayerUIPanel* CUserUi::ShowPanelById(const std::wstring panel_id)
{
    auto* panel = m_panel_mgr.ShowPanel(CPanelManager::PanelKey(ePanelType::PanelFromUi, panel_id));
    //显示面板后隐藏界面中按钮的鼠标提示
    if (panel != nullptr)
        OnPanelShow();
    return panel;
}

bool CUserUi::IsPanelShown() const
{
    return m_panel_mgr.GetTopPanel() != nullptr;
}

void CUserUi::ClosePanel()
{
    OnPanelHide();
    m_panel_mgr.HidePanel();
}

void CUserUi::ShowSongListPreviewPanel(const ListItem& list_item)
{
    CListPreviewPanel* panel = dynamic_cast<CListPreviewPanel*>(ShowPanelByResId(IDR_LIST_PREVIEW_PANEL));
    if (panel != nullptr)
        panel->SetListData(list_item);
}

CPlayerUIPanel* CUserUi::GetTopPanel() const
{
    return m_panel_mgr.GetTopPanel();
}

void CUserUi::OnPanelShow()
{
    IterateAllElements([&](UiElement::Element* element) ->bool {
        //隐藏按钮、进度条、音量的鼠标提示
        UiElement::Button* button = dynamic_cast<UiElement::Button*>(element);
        if (button != nullptr)
            UpdateMouseToolTipPosition(button->GetKey(), CRect());
        UiElement::ProgressBar* progress_bar = dynamic_cast<UiElement::ProgressBar*>(element);
        if (progress_bar != nullptr)
            UpdateMouseToolTipPosition(UiElement::TooltipIndex::PROGRESS_BAR, CRect());
        UiElement::Volume* volume = dynamic_cast<UiElement::Volume*>(element);
        if (volume !=nullptr)
            UpdateMouseToolTipPosition(CPlayerUIBase::BTN_VOLUME, CRect());
        //调用MouseLeave，以清除鼠标指向状态
        element->MouseLeave();
        element->HideTooltip();
        return false;
    }, true);
}

void CUserUi::OnPanelHide()
{
    HideTooltip();
    auto* panel = m_panel_mgr.GetTopPanel();
    if (panel != nullptr)
    {
        panel->GetRootElement()->IterateAllElements([&](UiElement::Element* element) ->bool {
            //调用MouseLeave，以清除鼠标指向状态
            element->MouseLeave();
            element->HideTooltip();
            //清除列表的选择行
            UiElement::AbstractListElement* list_element = dynamic_cast<UiElement::AbstractListElement*>(element);
            if (list_element != nullptr)
                list_element->SelectNone();
            return false;
        });
    }
}
