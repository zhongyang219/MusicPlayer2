#include "stdafx.h"
#include "UserUi.h"
#include "UiSearchBox.h"
#include "UIPanel/ListPreviewPanel.h"

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
                text_element->show_volume = true;
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
                text_element->show_volume = false;
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
        UiElement::ListElement* playlist_element{ dynamic_cast<UiElement::ListElement*>(element) };
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
            if (!stack_element->hover_to_switch && !stack_element->size_change_to_switch)
                archive << static_cast<BYTE>(stack_element->GetCurIndex());
        }
        UiElement::Button* button_element{ dynamic_cast<UiElement::Button*>(element) };
        if (button_element != nullptr)
        {
            if (button_element->key == BtnKey::BTN_SHOW_HIDE_ELEMENT)
            {
                UiElement::Element* element = FindElementInAllUi<UiElement::Element>(button_element->related_element_id);
                if (element != nullptr)
                {
                    bool element_show = element->IsVisible();
                    elements_show_hide_status.push_back(std::make_pair(button_element->related_element_id, element_show));
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
            if (!stack_element->hover_to_switch && !stack_element->size_change_to_switch)
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
            //绘制音量调整按钮
            DrawVolumnAdjBtn();
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
    if (!CPlayerUIBase::LButtonUp(point) && !CPlayerUIBase::PointInMenubarArea(point) && !CPlayerUIBase::PointInTitlebarArea(point))
    {
        //显示了面板的情况下，点击面板以外的地方关闭面板
        auto* panel = m_panel_mgr.GetTopPanel();
        if (panel != nullptr && !panel->GetPanelRect().PtInRect(point) && !panel->GetPanelRect().PtInRect(m_mouse_clicked_point))
        {
            OnPanelHide();
            m_panel_mgr.HidePanel();
            return true;
        }

        auto root_element = GetMouseEventResponseElement();
        //遍历所有元素
        root_element->IterateAllElements([&](UiElement::Element* element) ->bool {
            if (element != nullptr)
            {
                element->LButtonUp(point);
            }
            return false;
        }, true);
    }
    return false;
}

bool CUserUi::LButtonDown(CPoint point)
{
    m_mouse_clicked_point = point;
    if (!CPlayerUIBase::LButtonDown(point))
    {
        bool rtn = false;
        if (!CPlayerUIBase::PointInMenubarArea(point) && !CPlayerUIBase::PointInTitlebarArea(point))
        {
            auto root_element = GetMouseEventResponseElement();
            //遍历所有元素
            root_element->IterateAllElements([&](UiElement::Element* element) ->bool {
                if (element != nullptr)
                {
                    if (element->LButtonDown(point))
                    {
                        rtn = true;
                        return true;
                    }
                }
                return false;
            }, true);
        }
        return rtn;
    }
    return true;
}

bool CUserUi::MouseMove(CPoint point)
{
    bool mouse_leave = false;
    if (!CPlayerUIBase::MouseMove(point))
    {
        bool mouse_in_draw_area{ !CPlayerUIBase::PointInMenubarArea(point) && !CPlayerUIBase::PointInTitlebarArea(point) };
        if (mouse_in_draw_area)
        {
            auto root_element = GetMouseEventResponseElement();
            //遍历所有元素
            root_element->IterateAllElements([&](UiElement::Element* element) ->bool {
                if (element != nullptr)
                {
                    element->MouseMove(point);
                }
                return false;
                }, true);
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
        IterateAllElements([point](UiElement::Element* element) ->bool {
            if (element != nullptr)
                element->MouseLeave();
            return false;
        });
    }
    return true;
}

bool CUserUi::MouseLeave()
{
    //遍历所有元素
    auto root_element = GetMouseEventResponseElement();
    root_element->IterateAllElements([](UiElement::Element* element) ->bool {
        if (element != nullptr)
        {
            element->MouseLeave();
        }
        return false;
    });

    return CPlayerUIBase::MouseLeave();
}


bool CUserUi::RButtonUp(CPoint point)
{
    //遍历所有元素
    bool rtn = false;
    if (!CPlayerUIBase::PointInMenubarArea(point) && !CPlayerUIBase::PointInTitlebarArea(point))
    {
        auto root_element = GetMouseEventResponseElement();
        root_element->IterateAllElements([&](UiElement::Element* element) ->bool {
            if (element != nullptr)
            {
                if (element->RButtonUp(point))
                {
                    rtn = true;
                    return true;
                }
            }
            return false;
        }, true);
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
        root_element->IterateAllElements([point](UiElement::Element* element) ->bool {
            if (element != nullptr)
            {
                element->RButtonDown(point);
            }
            return false;
        });
    }
    return CPlayerUIBase::RButtonDown(point);
}

bool CUserUi::MouseWheel(int delta, CPoint point)
{
    //遍历所有元素
    auto root_element = GetMouseEventResponseElement();
    bool rtn = false;
    root_element->IterateAllElements([&](UiElement::Element* element) ->bool {
        if (element != nullptr)
        {
            UiElement::StackElement* stack_element{ dynamic_cast<UiElement::StackElement*>(element) };
            //非stackElement元素
            if (stack_element == nullptr && element->MouseWheel(delta, point))
            {
                rtn = true;
                return true;
            }
        }
        return false;
    }, true);

    if (!rtn)
    {
        //遍历stackElement元素
        root_element->IterateAllElements([&](UiElement::Element* element) ->bool {
            UiElement::StackElement* stack_element{ dynamic_cast<UiElement::StackElement*>(element) };
            if (stack_element != nullptr && stack_element->MouseWheel(delta, point))
            {
                rtn = true;
                return true;
            }
            return false;
        }, true);
    }

    if (rtn)
        return true;
    return CPlayerUIBase::MouseWheel(delta, point);
}

bool CUserUi::DoubleClick(CPoint point)
{
    //遍历所有元素
    bool rtn = false;
    auto root_element = GetMouseEventResponseElement();
    root_element->IterateAllElements([&](UiElement::Element* element) ->bool {
        if (element != nullptr)
        {
            if (element->DoubleClick(point))
            {
                rtn = true;
                return true;
            }
        }
        return false;
    }, true);

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
        element->name = item_name;
        element->id = CTinyXml2Helper::ElementAttribute(xml_node, "id");
        bool visible{ true };
        CTinyXml2Helper::GetElementAttributeBool(xml_node, "visible", visible);
        element->SetVisible(visible);
        //设置元素的基类属性
        std::string str_x = CTinyXml2Helper::ElementAttribute(xml_node, "x");
        std::string str_y = CTinyXml2Helper::ElementAttribute(xml_node, "y");
        std::string str_proportion = CTinyXml2Helper::ElementAttribute(xml_node, "proportion");
        std::string str_width = CTinyXml2Helper::ElementAttribute(xml_node, "width");
        std::string str_height = CTinyXml2Helper::ElementAttribute(xml_node, "height");
        std::string str_max_width = CTinyXml2Helper::ElementAttribute(xml_node, "max-width");
        std::string str_max_height = CTinyXml2Helper::ElementAttribute(xml_node, "max-height");
        std::string str_min_width = CTinyXml2Helper::ElementAttribute(xml_node, "min-width");
        std::string str_min_height = CTinyXml2Helper::ElementAttribute(xml_node, "min-height");
        std::string str_margin = CTinyXml2Helper::ElementAttribute(xml_node, "margin");
        std::string str_margin_left = CTinyXml2Helper::ElementAttribute(xml_node, "margin-left");
        std::string str_margin_right = CTinyXml2Helper::ElementAttribute(xml_node, "margin-right");
        std::string str_margin_top = CTinyXml2Helper::ElementAttribute(xml_node, "margin-top");
        std::string str_margin_bottom = CTinyXml2Helper::ElementAttribute(xml_node, "margin-bottom");
        std::string str_hide_width = CTinyXml2Helper::ElementAttribute(xml_node, "hide-width");
        std::string str_hide_height = CTinyXml2Helper::ElementAttribute(xml_node, "hide-height");
        if (!str_x.empty())
            element->x.FromString(str_x);
        if (!str_y.empty())
            element->y.FromString(str_y);
        if (!str_proportion.empty())
            element->proportion = max(atoi(str_proportion.c_str()), 1);
        if (!str_width.empty())
            element->width.FromString(str_width);
        if (!str_height.empty())
            element->height.FromString(str_height);
        if (!str_max_width.empty())
            element->max_width.FromString(str_max_width);
        if (!str_max_height.empty())
            element->max_height.FromString(str_max_height);
        if (!str_min_width.empty())
            element->min_width.FromString(str_min_width);
        if (!str_min_height.empty())
            element->min_height.FromString(str_min_height);

        if (!str_margin.empty())
        {
            element->margin_left.FromString(str_margin);
            element->margin_right.FromString(str_margin);
            element->margin_top.FromString(str_margin);
            element->margin_bottom.FromString(str_margin);
        }
        if (!str_margin_left.empty())
            element->margin_left.FromString(str_margin_left);
        if (!str_margin_right.empty())
            element->margin_right.FromString(str_margin_right);
        if (!str_margin_top.empty())
            element->margin_top.FromString(str_margin_top);
        if (!str_margin_bottom.empty())
            element->margin_bottom.FromString(str_margin_bottom);

        if (!str_hide_width.empty())
            element->hide_width.FromString(str_hide_width);
        if (!str_hide_height.empty())
            element->hide_height.FromString(str_hide_height);

        //设置ListElement的属性
        UiElement::ListElement* list_element = dynamic_cast<UiElement::ListElement*>(element.get());
        if (list_element != nullptr)
        {
            int item_height{};
            CTinyXml2Helper::GetElementAttributeInt(xml_node, "item_height", item_height);
            if (item_height > 0)
                list_element->item_height = item_height;
            CTinyXml2Helper::GetElementAttributeInt(xml_node, "font_size", list_element->font_size);
        }

        //根据节点的类型设置元素独有的属性
        //按钮
        if (item_name == "button")
        {
            UiElement::Button* button = dynamic_cast<UiElement::Button*>(element.get());
            if (button != nullptr)
            {
                std::string str_key = CTinyXml2Helper::ElementAttribute(xml_node, "key");   //按钮的类型
                button->FromString(str_key);
                std::string str_big_icon = CTinyXml2Helper::ElementAttribute(xml_node, "bigIcon");
                button->big_icon = CTinyXml2Helper::StringToBool(str_big_icon.c_str());
                CTinyXml2Helper::GetElementAttributeBool(xml_node, "show_text", button->show_text);
                CTinyXml2Helper::GetElementAttributeInt(xml_node, "font_size", button->font_size);
                std::string str_text = CTinyXml2Helper::ElementAttribute(xml_node, "text");
                button->text = CCommon::StrToUnicode(str_text, CodeType::UTF8_NO_BOM);
                ReplaceUiStringRes(button->text);
                std::string str_icon = CTinyXml2Helper::ElementAttribute(xml_node, "icon");
                button->IconTypeFromString(str_icon);
                button->panel_file_name = CCommon::StrToUnicode(CTinyXml2Helper::ElementAttribute(xml_node, "panel_file_name"), CodeType::UTF8_NO_BOM);
                button->panel_id = CCommon::StrToUnicode(CTinyXml2Helper::ElementAttribute(xml_node, "panel_id"), CodeType::UTF8_NO_BOM);
                button->related_element_id = CTinyXml2Helper::ElementAttribute(xml_node, "related_element_id");
                CTinyXml2Helper::GetElementAttributeBool(xml_node, "hand_cursor", button->hand_cursor);
                CTinyXml2Helper::GetElementAttributeBool(xml_node, "empty_btn", button->empty_btn);
            }
        }
        else if (item_name == "rectangle")
        {
            UiElement::Rectangle* rectangle = dynamic_cast<UiElement::Rectangle*>(element.get());
            if (rectangle != nullptr)
            {
                std::string str_no_corner_radius = CTinyXml2Helper::ElementAttribute(xml_node, "no_corner_radius");
                rectangle->no_corner_radius = CTinyXml2Helper::StringToBool(str_no_corner_radius.c_str());
                std::string str_theme_color = CTinyXml2Helper::ElementAttribute(xml_node, "theme_color");
                if (!str_theme_color.empty())
                    rectangle->theme_color = CTinyXml2Helper::StringToBool(str_theme_color.c_str());
                std::string str_color_mode = CTinyXml2Helper::ElementAttribute(xml_node, "color_mode");
                if (str_color_mode == "dark")
                    rectangle->color_mode = CPlayerUIBase::RCM_DARK;
                else if (str_color_mode == "light")
                    rectangle->color_mode = CPlayerUIBase::RCM_LIGHT;
                else
                    rectangle->color_mode = CPlayerUIBase::RCM_AUTO;
            }
        }
        //文本
        else if (item_name == "text")
        {
            UiElement::Text* text = dynamic_cast<UiElement::Text*>(element.get());
            if (text != nullptr)
            {
                //text
                std::string str_text = CTinyXml2Helper::ElementAttribute(xml_node, "text");
                text->text = CCommon::StrToUnicode(str_text, CodeType::UTF8_NO_BOM);
                ReplaceUiStringRes(text->text);
                //alignment
                std::string str_alignment = CTinyXml2Helper::ElementAttribute(xml_node, "alignment");
                if (str_alignment == "left")
                    text->align = Alignment::LEFT;
                else if (str_alignment == "right")
                    text->align = Alignment::RIGHT;
                else if (str_alignment == "center")
                    text->align = Alignment::CENTER;
                //style
                std::string str_style = CTinyXml2Helper::ElementAttribute(xml_node, "style");
                if (str_style == "static")
                    text->style = UiElement::Text::Static;
                else if (str_style == "scroll")
                    text->style = UiElement::Text::Scroll;
                else if (str_style == "scroll2")
                    text->style = UiElement::Text::Scroll2;
                //type
                std::string str_type = CTinyXml2Helper::ElementAttribute(xml_node, "type");
                if (str_type == "userDefine")
                    text->type = UiElement::Text::UserDefine;
                else if (str_type == "title")
                    text->type = UiElement::Text::Title;
                else if (str_type == "artist")
                    text->type = UiElement::Text::Artist;
                else if (str_type == "album")
                    text->type = UiElement::Text::Album;
                else if (str_type == "artist_title")
                    text->type = UiElement::Text::ArtistTitle;
                else if (str_type == "artist_album")
                    text->type = UiElement::Text::ArtistAlbum;
                else if (str_type == "format")
                    text->type = UiElement::Text::Format;
                else if (str_type == "play_time")
                    text->type = UiElement::Text::PlayTime;
                else if (str_type == "play_time_and_volume")
                    text->type = UiElement::Text::PlayTimeAndVolume;
                //font_size
                std::string str_font_size = CTinyXml2Helper::ElementAttribute(xml_node, "font_size");
                text->font_size = atoi(str_font_size.c_str());
                if (text->font_size == 0)
                    text->font_size = 9;
                else if (text->font_size < 8)
                    text->font_size = 8;
                else if (text->font_size > 16)
                    text->font_size = 16;
                // max_width_follow_text 优先级低于 max-width
                std::string str_width_follow_text = CTinyXml2Helper::ElementAttribute(xml_node, "width_follow_text");
                if (str_width_follow_text == "true")
                    text->width_follow_text = true;
                else if (str_width_follow_text == "false")
                    text->width_follow_text = false;
                std::string str_color_mode = CTinyXml2Helper::ElementAttribute(xml_node, "color_mode");
                if (str_color_mode == "dark")
                    text->color_mode = CPlayerUIBase::RCM_DARK;
                else if (str_color_mode == "light")
                    text->color_mode = CPlayerUIBase::RCM_LIGHT;
                else
                    text->color_mode = CPlayerUIBase::RCM_AUTO;
                std::string str_color_style = CTinyXml2Helper::ElementAttribute(xml_node, "color_style");
                if (str_color_style == "emphasis1")
                    text->color_style = UiElement::Text::Emphasis1;
                else if (str_color_style == "emphasis2")
                    text->color_style = UiElement::Text::Emphasis2;
                else
                    text->color_style = UiElement::Text::Default;
            }
        }
        //专辑封面
        else if (item_name == "albumCover")
        {
            UiElement::AlbumCover* album_cover = dynamic_cast<UiElement::AlbumCover*>(element.get());
            if (album_cover != nullptr)
            {
                std::string str_square = CTinyXml2Helper::ElementAttribute(xml_node, "square");
                album_cover->square = CTinyXml2Helper::StringToBool(str_square.c_str());
                std::string str_show_info = CTinyXml2Helper::ElementAttribute(xml_node, "show_info");
                album_cover->show_info = CTinyXml2Helper::StringToBool(str_show_info.c_str());
            }
        }
        //频谱分析
        else if (item_name == "spectrum")
        {
            UiElement::Spectrum* spectrum = dynamic_cast<UiElement::Spectrum*>(element.get());
            if (spectrum != nullptr)
            {
                std::string str_draw_reflex = CTinyXml2Helper::ElementAttribute(xml_node, "draw_reflex");
                spectrum->draw_reflex = CTinyXml2Helper::StringToBool(str_draw_reflex.c_str());
                std::string str_fixed_width = CTinyXml2Helper::ElementAttribute(xml_node, "fixed_width");
                spectrum->fixed_width = CTinyXml2Helper::StringToBool(str_fixed_width.c_str());
                std::string str_type = CTinyXml2Helper::ElementAttribute(xml_node, "type");
                if (str_type == "auto_col")
                    spectrum->type = CUIDrawer::SC_AUTO;
                else if (str_type == "128col")
                    spectrum->type = CUIDrawer::SC_128;
                else if (str_type == "64col")
                    spectrum->type = CUIDrawer::SC_64;
                else if (str_type == "32col")
                    spectrum->type = CUIDrawer::SC_32;
                else if (str_type == "16col")
                    spectrum->type = CUIDrawer::SC_16;
                else if (str_type == "8col")
                    spectrum->type = CUIDrawer::SC_8;
                else if (str_type == "4col")
                    spectrum->type = CUIDrawer::SC_4;
                //alignment
                std::string str_alignment = CTinyXml2Helper::ElementAttribute(xml_node, "alignment");
                if (str_alignment == "left")
                    spectrum->align = Alignment::LEFT;
                else if (str_alignment == "right")
                    spectrum->align = Alignment::RIGHT;
                else if (str_alignment == "center")
                    spectrum->align = Alignment::CENTER;
            }
        }
        //进度条
        else if (item_name == "progressBar")
        {
            UiElement::ProgressBar* progress_bar = dynamic_cast<UiElement::ProgressBar*>(element.get());
            if (progress_bar != nullptr)
            {
                std::string str_show_play_time = CTinyXml2Helper::ElementAttribute(xml_node, "show_play_time");
                progress_bar->show_play_time = CTinyXml2Helper::StringToBool(str_show_play_time.c_str());
                std::string str_play_time_both_side = CTinyXml2Helper::ElementAttribute(xml_node, "play_time_both_side");
                progress_bar->play_time_both_side = CTinyXml2Helper::StringToBool(str_play_time_both_side.c_str());
            }
        }
        //音量
        else if (item_name == "volume")
        {
            UiElement::Volume* volume = dynamic_cast<UiElement::Volume*>(element.get());
            if (volume != nullptr)
            {
                std::string str_show_text = CTinyXml2Helper::ElementAttribute(xml_node, "show_text");
                volume->show_text = CTinyXml2Helper::StringToBool(str_show_text.c_str());
                std::string str_adj_btn_on_top = CTinyXml2Helper::ElementAttribute(xml_node, "adj_btn_on_top");
                volume->adj_btn_on_top = CTinyXml2Helper::StringToBool(str_adj_btn_on_top.c_str());
            }
        }
        //堆叠元素
        else if (item_name == "stackElement")
        {
            UiElement::StackElement* stack_element = dynamic_cast<UiElement::StackElement*>(element.get());
            if (stack_element != nullptr)
            {
                CTinyXml2Helper::GetElementAttributeBool(xml_node, "click_to_switch", stack_element->click_to_switch);
                CTinyXml2Helper::GetElementAttributeBool(xml_node, "hover_to_switch", stack_element->hover_to_switch);
                CTinyXml2Helper::GetElementAttributeBool(xml_node, "scroll_to_switch", stack_element->scroll_to_switch);
                CTinyXml2Helper::GetElementAttributeBool(xml_node, "sweep_to_switch", stack_element->sweep_to_switch);
                CTinyXml2Helper::GetElementAttributeBool(xml_node, "show_indicator", stack_element->show_indicator);
                CTinyXml2Helper::GetElementAttributeInt(xml_node, "indicator_offset", stack_element->indicator_offset);
                CTinyXml2Helper::GetElementAttributeBool(xml_node, "size_change_to_switch", stack_element->size_change_to_switch);
                std::string str_size_change_condition = CTinyXml2Helper::ElementAttribute(xml_node, "size_change_condition");
                if (str_size_change_condition == "widthGreaterThan")
                    stack_element->size_change_condition = UiElement::StackElement::SizeChangeSwitchCondition::WIDTH_GREATER_THAN;
                else if (str_size_change_condition == "widthLessThan")
                    stack_element->size_change_condition = UiElement::StackElement::SizeChangeSwitchCondition::WIDTH_LESS_THAN;
                else if (str_size_change_condition == "heightGreaterThan")
                    stack_element->size_change_condition = UiElement::StackElement::SizeChangeSwitchCondition::HEIGHT_GREATER_THAN;
                else if (str_size_change_condition == "heightLessThan")
                    stack_element->size_change_condition = UiElement::StackElement::SizeChangeSwitchCondition::HEIGHT_LESS_THAN;
                std::string str_size_change_value = CTinyXml2Helper::ElementAttribute(xml_node, "size_change_value");
                if (!str_size_change_value.empty())
                    stack_element->size_change_value = stack_element->GetUI()->DPI(atoi(str_size_change_value.c_str()));
                std::string str_related_stack_elements = CTinyXml2Helper::ElementAttribute(xml_node, "related_stack_elements");
                std::vector<std::string> vec_related_stack_elements;
                CCommon::StringSplit(str_related_stack_elements, ',', vec_related_stack_elements);
                for (const auto& str : vec_related_stack_elements)
                    stack_element->related_stack_elements.insert(str);
            }
        }
        //播放控制栏
        else if (item_name == "classicalControlBar")
        {
            UiElement::ClassicalControlBar* classicalControlBar = dynamic_cast<UiElement::ClassicalControlBar*>(element.get());
            if (classicalControlBar != nullptr)
            {
                CTinyXml2Helper::GetElementAttributeBool(xml_node, "show_switch_display_btn", classicalControlBar->show_switch_display_btn);
            }
        }
        //歌词
        else if (item_name == "lyrics")
        {
            UiElement::Lyrics* lyrics = dynamic_cast<UiElement::Lyrics*>(element.get());
            if (lyrics != nullptr)
            {
                CTinyXml2Helper::GetElementAttributeBool(xml_node, "no_background", lyrics->no_background);
                CTinyXml2Helper::GetElementAttributeBool(xml_node, "use_default_font", lyrics->use_default_font);
                CTinyXml2Helper::GetElementAttributeInt(xml_node, "font_size", lyrics->font_size);
                CTinyXml2Helper::GetElementAttributeBool(xml_node, "show_song_info", lyrics->show_song_info);
            }
        }
        //媒体库列表
        else if (item_name == "mediaLibItemList")
        {
            UiElement::MediaLibItemList* mediaLibItemList = dynamic_cast<UiElement::MediaLibItemList*>(element.get());
            if (mediaLibItemList != nullptr)
            {
                std::string str_type = CTinyXml2Helper::ElementAttribute(xml_node, "type");
                if (str_type == "artist")
                    mediaLibItemList->type = ListItem::ClassificationType::CT_ARTIST;
                else if (str_type == "album")
                    mediaLibItemList->type = ListItem::ClassificationType::CT_ALBUM;
                else if (str_type == "genre")
                    mediaLibItemList->type = ListItem::ClassificationType::CT_GENRE;
                else if (str_type == "year")
                    mediaLibItemList->type = ListItem::ClassificationType::CT_YEAR;
                else if (str_type == "file_type")
                    mediaLibItemList->type = ListItem::ClassificationType::CT_TYPE;
                else if (str_type == "bitrate")
                    mediaLibItemList->type = ListItem::ClassificationType::CT_BITRATE;
                else if (str_type == "rating")
                    mediaLibItemList->type = ListItem::ClassificationType::CT_RATING;
                mediaLibItemList->track_list_element_id = CTinyXml2Helper::ElementAttribute(xml_node, "track_list_element_id");
            }
        }
        else if (item_name == "mediaLibFolder")
        {
            UiElement::MediaLibFolder* medialib_folder = dynamic_cast<UiElement::MediaLibFolder*>(element.get());
            if (medialib_folder != nullptr)
            {
                medialib_folder->track_list_element_id = CTinyXml2Helper::ElementAttribute(xml_node, "track_list_element_id");
            }

        }
        else if (item_name == "mediaLibPlaylist")
        {
            UiElement::MediaLibPlaylist* medialib_playlist = dynamic_cast<UiElement::MediaLibPlaylist*>(element.get());
            if (medialib_playlist != nullptr)
            {
                medialib_playlist->track_list_element_id = CTinyXml2Helper::ElementAttribute(xml_node, "track_list_element_id");
            }
        }
        else if (item_name == "recentPlayedList")
        {
            UiElement::RecentPlayedList* recent_played_list = dynamic_cast<UiElement::RecentPlayedList*>(element.get());
            if (recent_played_list != nullptr)
            {
                recent_played_list->track_list_element_id = CTinyXml2Helper::ElementAttribute(xml_node, "track_list_element_id");
            }
        }
        else if (item_name == "medialibFolderExplore")
        {
            UiElement::FolderExploreTree* folder_explore = dynamic_cast<UiElement::FolderExploreTree*>(element.get());
            if (folder_explore != nullptr)
            {
                folder_explore->track_list_element_id = CTinyXml2Helper::ElementAttribute(xml_node, "track_list_element_id");
            }
        }
        //导航栏
        else if (item_name == "navigationBar")
        {
            UiElement::NavigationBar* tab_emelent = dynamic_cast<UiElement::NavigationBar*>(element.get());
            if (tab_emelent != nullptr)
            {
                //兼容旧版的“item_list”属性
                std::string str_item_list = CTinyXml2Helper::ElementAttribute(xml_node, "item_list");
                std::vector<std::string> item_list;
                CCommon::StringSplit(str_item_list, ',', item_list);
                for (const std::string& item_str : item_list)
                {
                    UiElement::NavigationBar::NavigationItem navigation_item;
                    if (item_str == "album_cover")
                    {
                        navigation_item.icon = IconMgr::IT_Album_Cover;
                        navigation_item.text = theApp.m_str_table.LoadText(L"UI_TXT_ALBUM_COVER");
                    }
                    else if (item_str == "spectrum")
                    {
                        navigation_item.icon = IconMgr::IT_Reverb;
                        navigation_item.text = theApp.m_str_table.LoadText(L"UI_TXT_SPECTRUM");
                    }
                    else if (item_str == "lyrics")
                    {
                        navigation_item.icon = IconMgr::IT_Lyric;
                        navigation_item.text = theApp.m_str_table.LoadText(L"UI_TXT_LYRICS");
                    }
                    else if (item_str == "now_playing")
                    {
                        navigation_item.icon = IconMgr::IT_NowPlaying;
                        navigation_item.text = theApp.m_str_table.LoadText(L"UI_TXT_PLAYSTATUS_PLAYING");
                    }
                    else if (item_str == "play_queue")
                    {
                        navigation_item.icon = IconMgr::IT_Play_In_Playlist;
                        navigation_item.text = theApp.m_str_table.LoadText(L"UI_TXT_PLAY_QUEUE");
                    }
                    else if (item_str == "recently_played")
                    {
                        navigation_item.icon = IconMgr::IT_History;
                        navigation_item.text = theApp.m_str_table.LoadText(L"TXT_RECENT_PLAYED");
                    }
                    else if (item_str == "folder")
                    {
                        navigation_item.icon = IconMgr::IT_Folder;
                        navigation_item.text = theApp.m_str_table.LoadText(L"TXT_FOLDER");
                    }
                    else if (item_str == "playlist")
                    {
                        navigation_item.icon = IconMgr::IT_Playlist;
                        navigation_item.text = theApp.m_str_table.LoadText(L"TXT_PLAYLIST");
                    }
                    else if (item_str == "artist")
                    {
                        navigation_item.icon = IconMgr::IT_Artist;
                        navigation_item.text = theApp.m_str_table.LoadText(L"TXT_ARTIST");
                    }
                    else if (item_str == "album")
                    {
                        navigation_item.icon = IconMgr::IT_Album;
                        navigation_item.text = theApp.m_str_table.LoadText(L"TXT_ALBUM");
                    }
                    else if (item_str == "genre")
                    {
                        navigation_item.icon = IconMgr::IT_Genre;
                        navigation_item.text = theApp.m_str_table.LoadText(L"TXT_GENRE");
                    }
                    else if (item_str == "year")
                    {
                        navigation_item.icon = IconMgr::IT_Year;
                        navigation_item.text = theApp.m_str_table.LoadText(L"TXT_YEAR");
                    }
                    else if (item_str == "file_type")
                    {
                        navigation_item.icon = IconMgr::IT_File_Relate;
                        navigation_item.text = theApp.m_str_table.LoadText(L"TXT_FILE_TYPE");
                    }
                    else if (item_str == "bitrate")
                    {
                        navigation_item.icon = IconMgr::IT_Bitrate;
                        navigation_item.text = theApp.m_str_table.LoadText(L"TXT_BITRATE");
                    }
                    else if (item_str == "rating")
                    {
                        navigation_item.icon = IconMgr::IT_Star;
                        navigation_item.text = theApp.m_str_table.LoadText(L"TXT_RATING");
                    }
                    else if (item_str == "media_lib")
                    {
                        navigation_item.icon = IconMgr::IT_Media_Lib;
                        navigation_item.text = theApp.m_str_table.LoadText(L"TXT_MEDIA_LIB");
                    }
                    else if (item_str == "my_favourite")
                    {
                        navigation_item.icon = IconMgr::IT_Favorite_On;
                        navigation_item.text = theApp.m_str_table.LoadText(L"TXT_MY_FAVOURITE");
                    }
                    else if (item_str == "all_tracks")
                    {
                        navigation_item.icon = IconMgr::IT_Media_Lib;
                        navigation_item.text = theApp.m_str_table.LoadText(L"TXT_ALL_TRACKS");
                    }
                    else if (item_str == "folder_explore")
                    {
                        navigation_item.icon = IconMgr::IT_Folder_Explore;
                        navigation_item.text = theApp.m_str_table.LoadText(L"TXT_FOLDER_EXPLORE");
                    }
                    if (navigation_item.icon != IconMgr::IT_NO_ICON)
                        tab_emelent->tab_list.push_back(navigation_item);
                }

                //读取导航栏按钮列表
                CTinyXml2Helper::IterateChildNode(xml_node, [&](tinyxml2::XMLElement* child_node) {
                    std::string child_name = CTinyXml2Helper::ElementName(child_node);
                    if (child_name == "navigationItem")
                    {
                        UiElement::NavigationBar::NavigationItem navigation_item;
                        std::string str_text = CTinyXml2Helper::ElementAttribute(child_node, "text");
                        navigation_item.text = CCommon::StrToUnicode(str_text, CodeType::UTF8_NO_BOM);
                        ReplaceUiStringRes(navigation_item.text);
                        std::string str_icon = CTinyXml2Helper::ElementAttribute(child_node, "icon");
                        navigation_item.icon = UiElement::Button::NameToIconType(str_icon);
                        tab_emelent->tab_list.push_back(navigation_item);
                    }
                });

                std::string str_icon_type = CTinyXml2Helper::ElementAttribute(xml_node, "icon_type");
                if (str_icon_type == "icon_and_text")
                    tab_emelent->icon_type = UiElement::NavigationBar::ICON_AND_TEXT;
                else if (str_icon_type == "icon_only")
                    tab_emelent->icon_type = UiElement::NavigationBar::ICON_ONLY;
                else if (str_icon_type == "text_only")
                    tab_emelent->icon_type = UiElement::NavigationBar::TEXT_ONLY;

                std::string str_orientation = CTinyXml2Helper::ElementAttribute(xml_node, "orientation");
                if (str_orientation == "horizontal")
                    tab_emelent->orientation = UiElement::NavigationBar::Horizontal;
                else if (str_orientation == "vertical")
                    tab_emelent->orientation = UiElement::NavigationBar::Vertical;

                CTinyXml2Helper::GetElementAttributeInt(xml_node, "item_space", tab_emelent->item_space);
                CTinyXml2Helper::GetElementAttributeInt(xml_node, "item_height", tab_emelent->item_height);
                CTinyXml2Helper::GetElementAttributeInt(xml_node, "font_size", tab_emelent->font_size);
                tab_emelent->stack_element_id = CTinyXml2Helper::ElementAttribute(xml_node, "stack_element_id");
                CTinyXml2Helper::GetElementAttributeInt(xml_node, "item_left_space", tab_emelent->item_left_space);

            }
        }
        else if (item_name == "playlistIndicator")
        {
            UiElement::PlaylistIndicator* playlist_indicator = dynamic_cast<UiElement::PlaylistIndicator*>(element.get());
            if (playlist_indicator != nullptr)
            {
                CTinyXml2Helper::GetElementAttributeInt(xml_node, "font_size", playlist_indicator->font_size);
            }
        }
        else if (item_name == "trackInfo")
        {
            UiElement::TrackInfo* track_info = dynamic_cast<UiElement::TrackInfo*>(element.get());
            if (track_info != nullptr)
            {
                CTinyXml2Helper::GetElementAttributeInt(xml_node, "font_size", track_info->font_size);
            }
        }
        else if (item_name == "placeHolder")
        {
            UiElement::PlaceHolder* place_holder = dynamic_cast<UiElement::PlaceHolder*>(element.get());
            if (place_holder != nullptr)
            {
                CTinyXml2Helper::GetElementAttributeBool(xml_node, "show_when_use_system_titlebar", place_holder->show_when_use_system_titlebar);
            }
        }
        else if (item_name == "searchBox")
        {
            UiElement::SearchBox* search_box = dynamic_cast<UiElement::SearchBox*>(element.get());
            if (search_box != nullptr)
            {
                search_box->list_element_id = CTinyXml2Helper::ElementAttribute(xml_node, "list_element_id");
            }
        }
        //界面切换器
        else if (item_name == "elementSwitcher")
        {
            UiElement::ElementSwitcher* element_switcher = dynamic_cast<UiElement::ElementSwitcher*>(element.get());
            if (element_switcher != nullptr)
            {
                std::string style = CTinyXml2Helper::ElementAttribute(xml_node, "style");
                if (style == "empty")
                    element_switcher->style = UiElement::ElementSwitcher::Style::Empty;
                else if (style == "album_cover")
                    element_switcher->style = UiElement::ElementSwitcher::Style::AlbumCover;
                else if (style == "button")
                    element_switcher->style = UiElement::ElementSwitcher::Style::Button;
                
                element_switcher->stack_element_id = CTinyXml2Helper::ElementAttribute(xml_node, "stack_element_id");
                CTinyXml2Helper::GetElementAttributeInt(xml_node, "stack_element_index", element_switcher->stack_element_index);

                std::string str_text = CTinyXml2Helper::ElementAttribute(xml_node, "text");
                element_switcher->text = CCommon::StrToUnicode(str_text, CodeType::UTF8_NO_BOM);
                ReplaceUiStringRes(element_switcher->text);
                std::string str_icon = CTinyXml2Helper::ElementAttribute(xml_node, "icon");
                element_switcher->IconTypeFromString(str_icon);
            }
        }
        else if (item_name == "icon")
        {
            UiElement::Icon* icon = dynamic_cast<UiElement::Icon*>(element.get());
            if (icon != nullptr)
            {
                std::string str_icon = CTinyXml2Helper::ElementAttribute(xml_node, "icon");
                icon->IconTypeFromString(str_icon);
            }
        }
        else if (item_name == "checkBox")
        {
            UiElement::CheckBox* check_box = dynamic_cast<UiElement::CheckBox*>(element.get());
            if (check_box != nullptr)
            {
                std::string str_text = CTinyXml2Helper::ElementAttribute(xml_node, "text");
                check_box->text = CCommon::StrToUnicode(str_text, CodeType::UTF8_NO_BOM);
            }
        }
        else if (item_name == "toggleSettingGroup")
        {
            UiElement::ToggleSettingGroup* ele = dynamic_cast<UiElement::ToggleSettingGroup*>(element.get());
            if (ele != nullptr)
            {
                ele->icon_name = CTinyXml2Helper::ElementAttribute(xml_node, "icon");
                ele->main_text = CCommon::StrToUnicode(CTinyXml2Helper::ElementAttribute(xml_node, "mainText"), CodeType::UTF8_NO_BOM);
                ele->sub_text = CCommon::StrToUnicode(CTinyXml2Helper::ElementAttribute(xml_node, "subText"), CodeType::UTF8_NO_BOM);
            }
        }

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
    if (stack_element != nullptr && !stack_element->hover_to_switch && !stack_element->size_change_to_switch)
        stack_element->SwitchDisplay();
}

void CUserUi::SwitchStackElement(std::string id, int index)
{
    UiElement::StackElement* stack_element = FindElement<UiElement::StackElement>(id);
    //当stackElement设置了hover_to_switch或size_change_to_switch时不允许主动切换
    if (stack_element != nullptr && !stack_element->hover_to_switch && !stack_element->size_change_to_switch)
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
            UpdateMouseToolTipPosition(button->key, CRect());
        UiElement::ProgressBar* progress_bar = dynamic_cast<UiElement::ProgressBar*>(element);
        if (progress_bar != nullptr)
            UpdateMouseToolTipPosition(UiElement::TooltipIndex::PROGRESS_BAR, CRect());
        UiElement::Volume* volume = dynamic_cast<UiElement::Volume*>(element);
        if (volume !=nullptr)
            UpdateMouseToolTipPosition(CPlayerUIBase::BTN_VOLUME, CRect());
        //调用MouseLeave，以清除鼠标指向状态
        element->MouseLeave();
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
            UiElement::ListElement* list_element = dynamic_cast<UiElement::ListElement*>(element);
            if (list_element != nullptr)
                list_element->SelectNone();
            return false;
        });
    }
}
