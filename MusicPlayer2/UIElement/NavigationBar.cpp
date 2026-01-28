#include "stdafx.h"
#include "NavigationBar.h"
#include "StackElement.h"
#include "UserUi.h"
#include "Player.h"
#include "TinyXml2Helper.h"
#include "Helper/UiElementHelper.h"

void UiElement::NavigationBar::Draw()
{
    CalculateRect();

    DrawAreaGuard guard(&ui->GetDrawer(), rect);
    bool draw_icon{ icon_type == ICON_AND_TEXT || icon_type == ICON_ONLY };
    bool draw_text{ icon_type == ICON_AND_TEXT || icon_type == TEXT_ONLY };
    int x_pos{ rect.left };
    int y_pos{ rect.top };
    int index{};
    item_rects.resize(tab_list.size());
    for (const auto& navigation_item : tab_list)
    {
        //计算矩形区域
        int icon_width{};
        int text_width{};
        if (draw_icon)
        {
            int cur_item_height{ orientation == Horizontal ? rect.Height() : ui->DPI(item_height) };
            icon_width = (std::max)(ui->DPI(24), cur_item_height - ui->DPI(4));
        }
        if (draw_text)
            text_width = ui->GetDrawer().GetTextExtent(navigation_item.text.c_str()).cx;
        CRect item_rect{ rect };
        if (orientation == Horizontal)
        {
            item_rect.left = x_pos;
            item_rect.right = item_rect.left + icon_width + text_width + ui->DPI(4);
            if (icon_type == TEXT_ONLY)
                item_rect.right += ui->DPI(4);
            else if (icon_type == ICON_AND_TEXT && item_height > 20)
                item_rect.right += ui->DPI(item_height - 20) / 2;      //基于高度值增加一些右侧的边距
        }
        else
        {
            item_rect.top = y_pos;
            item_rect.bottom = item_rect.top + ui->DPI(item_height);
        }
        item_rects[index] = item_rect;

        if ((rect & item_rect).IsRectEmpty())
            continue;

        //绘制背景
        if (hover_index == index)
        {
            DrawAreaGuard guard(&ui->GetDrawer(), rect);
            ui->DrawRectangle(item_rect, pressed ? ui->GetUIColors().color_button_pressed : ui->GetUIColors().color_button_hover);
        }

        //绘制图标
        CRect icon_rect{ item_rect };
        if (draw_icon)
        {
            DrawAreaGuard guard(&ui->GetDrawer(), rect);
            if (icon_type != ICON_ONLY)
            {
                icon_rect.right = icon_rect.left + icon_width;
                if (orientation == Vertical)
                    icon_rect.MoveToX(icon_rect.left + ui->DPI(item_left_space));
            }
            //使用跳动的频谱代替正在播放图标
            if (navigation_item.icon == IconMgr::IT_NowPlaying && CPlayer::GetInstance().GetPlayingState2() != PS_STOPED && !CPlayer::GetInstance().IsMciCore())
            {
                ui->DrawMiniSpectrum(icon_rect);
            }
            else
            {
                ui->DrawUiIcon(icon_rect, navigation_item.icon);
            }
        }
        else
        {
            icon_rect.right = icon_rect.left;
        }

        //绘制文本
        if (draw_text)
        {
            DrawAreaGuard guard(&ui->GetDrawer(), rect);
            CRect text_rect{ item_rect };
            if (icon_type != TEXT_ONLY)
            {
                text_rect.left = icon_rect.right;
            }
            else
            {
                text_rect.MoveToX(text_rect.left + ui->DPI(4));
                if (orientation == Vertical)
                    text_rect.left += ui->DPI(8);
            }
            CFont* old_font{};  //原先的字体
            bool big_font{ ui->IsDrawLargeIcon() };
            old_font = ui->GetDrawer().SetFont(&theApp.m_font_set.GetFontBySize(font_size).GetFont(big_font));
            ui->GetDrawer().DrawWindowText(text_rect, navigation_item.text.c_str(), ui->GetUIColors().color_text, Alignment::LEFT, true);
            ui->GetDrawer().SetFont(old_font);
        }

        //绘制选中指示
        if (SelectedIndex() == index)
        {
            DrawAreaGuard guard(&ui->GetDrawer(), rect);
            CRect selected_indicator_rect{ item_rect };
            //水平排列时选中指示在底部
            if (orientation == Horizontal)
            {
                selected_indicator_rect.left += ui->DPI(4);
                selected_indicator_rect.right -= ui->DPI(4);
                selected_indicator_rect.top = selected_indicator_rect.bottom - ui->DPI(4);
            }
            //垂直排列时选中指示在左侧
            else
            {
                selected_indicator_rect.top += ui->DPI(4);
                selected_indicator_rect.bottom -= ui->DPI(4);
                selected_indicator_rect.right = selected_indicator_rect.left + ui->DPI(4);
            }
            if (theApp.m_app_setting_data.button_round_corners)
                ui->GetDrawer().DrawRoundRect(selected_indicator_rect, ui->GetUIColors().color_text_heighlight, ui->DPI(2));
            else
                ui->GetDrawer().FillRect(selected_indicator_rect, ui->GetUIColors().color_text_heighlight, true);

        }

        if (orientation == Horizontal)
            x_pos = item_rect.right + ui->DPI(item_space);
        else
            y_pos = item_rect.bottom + ui->DPI(item_space);
        index++;
    }


    Element::Draw();
}

bool UiElement::NavigationBar::LButtonUp(CPoint point)
{
    pressed = false;
    if (rect.PtInRect(point))
    {
        FindStackElement();
        if (stack_element != nullptr)
        {
            //查找点击的标签
            int _selected_index = -1;
            for (size_t i{}; i < item_rects.size(); i++)
            {
                if (item_rects[i].PtInRect(point))
                {
                    _selected_index = i;
                    break;
                }
            }
            if (_selected_index >= 0)
            {
                selected_index = _selected_index;
                stack_element->SetCurrentElement(selected_index);
                //点击导航栏标签时，如果导航栏在面板中但是关联的stackElement在面板外，则关闭面板
                if (CheckNavigationBarInPanel())
                {
                    CUserUi* user_ui = dynamic_cast<CUserUi*>(ui);
                    if (user_ui != nullptr)
                        user_ui->ClosePanel();
                }
            }
        }
        return true;
    }
    return false;
}

bool UiElement::NavigationBar::LButtonDown(CPoint point)
{
    if (rect.PtInRect(point))
    {
        pressed = true;
        return true;
    }
    return false;
}

bool UiElement::NavigationBar::MouseMove(CPoint point)
{
    int _hover_index{ -1 };
    if (rect.PtInRect(point))
    {
        for (size_t i{}; i < item_rects.size(); i++)
        {
            if (item_rects[i].PtInRect(point))
            {
                _hover_index = i;
                break;
            }
        }
    }
    hover_index = _hover_index;

    //显示鼠标提示
    if (icon_type == ICON_ONLY && hover_index >= 0)
    {
        if (last_hover_index != hover_index)
        {
            last_hover_index = hover_index;
            if (hover_index >= 0 && hover_index < static_cast<int>(tab_list.size()))
            {
                std::wstring str_tip = tab_list[hover_index].text;
                ui->UpdateMouseToolTip(TooltipIndex::TAB_ELEMENT, str_tip.c_str());
                ui->UpdateMouseToolTipPosition(TooltipIndex::TAB_ELEMENT, item_rects[hover_index] & rect);
            }
        }
    }
    return false;
}

bool UiElement::NavigationBar::RButtonUp(CPoint point)
{
    //不弹出右键菜单
    return rect.PtInRect(point);
}

bool UiElement::NavigationBar::MouseLeave()
{
    hover_index = -1;
    pressed = false;
    return true;
}

void UiElement::NavigationBar::HideTooltip()
{
    ui->UpdateMouseToolTipPosition(TooltipIndex::TAB_ELEMENT, CRect());
}

int UiElement::NavigationBar::SelectedIndex()
{
    FindStackElement();
    if (stack_element != nullptr)
        return stack_element->GetCurIndex();
    else
        return selected_index;
}

void UiElement::NavigationBar::FindStackElement()
{
    if (!find_stack_element)
    {
        stack_element = FindRelatedElement<StackElement>(stack_element_id);
        find_stack_element = true;  //找过一次没找到就不找了
    }
}

bool UiElement::NavigationBar::CheckNavigationBarInPanel()
{
    //当前导航栏是否面板中
    bool is_in_panel = false;
    //向上遍历父节点
    Element* ele = this;
    Element* panel = nullptr;
    while (ele != nullptr)
    {
        if (ele->GetName() == "panel")
        {
            is_in_panel = true;
            panel = ele;
            break;
        }
        ele = ele->GetParent();
    }

    if (is_in_panel && stack_element != nullptr)
    {
        //判断stack_element是否在面板中
        bool stack_element_in_panel = false;
        panel->IterateAllElements([&](Element* cur_ele)->bool {
            if (cur_ele == stack_element)
            {
                stack_element_in_panel = true;
                return true;
            }
            return false;
        });
        if (!stack_element_in_panel)
            return true;
    }
    return false;
}

void UiElement::NavigationBar::FromXmlNode(tinyxml2::XMLElement* xml_node)
{
    Element::FromXmlNode(xml_node);
    //兼容旧版的“item_list”属性
    std::string str_item_list = CTinyXml2Helper::ElementAttribute(xml_node, "item_list");
    std::vector<std::string> item_list;
    CCommon::StringSplit(str_item_list, ',', item_list);
    for (const std::string& item_str : item_list)
    {
        NavigationItem navigation_item;
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
            tab_list.push_back(navigation_item);
    }

    //读取导航栏按钮列表
    CTinyXml2Helper::IterateChildNode(xml_node, [&](tinyxml2::XMLElement* child_node) {
        std::string child_name = CTinyXml2Helper::ElementName(child_node);
        if (child_name == "navigationItem")
        {
            NavigationItem navigation_item;
            std::string str_text = CTinyXml2Helper::ElementAttribute(child_node, "text");
            navigation_item.text = CCommon::StrToUnicode(str_text, CodeType::UTF8_NO_BOM);
            CPlayerUIBase::ReplaceUiStringRes(navigation_item.text);
            std::string str_icon = CTinyXml2Helper::ElementAttribute(child_node, "icon");
            navigation_item.icon = UiElementHelper::NameToIconType(str_icon);
            tab_list.push_back(navigation_item);
        }
    });

    std::string str_icon_type = CTinyXml2Helper::ElementAttribute(xml_node, "icon_type");
    if (str_icon_type == "icon_and_text")
        icon_type = ICON_AND_TEXT;
    else if (str_icon_type == "icon_only")
        icon_type = ICON_ONLY;
    else if (str_icon_type == "text_only")
        icon_type = TEXT_ONLY;

    std::string str_orientation = CTinyXml2Helper::ElementAttribute(xml_node, "orientation");
    if (str_orientation == "horizontal")
        orientation = Horizontal;
    else if (str_orientation == "vertical")
        orientation = Vertical;

    CTinyXml2Helper::GetElementAttributeInt(xml_node, "item_space", item_space);
    CTinyXml2Helper::GetElementAttributeInt(xml_node, "item_height", item_height);
    CTinyXml2Helper::GetElementAttributeInt(xml_node, "font_size", font_size);
    stack_element_id = CTinyXml2Helper::ElementAttribute(xml_node, "stack_element_id");
    CTinyXml2Helper::GetElementAttributeInt(xml_node, "item_left_space", item_left_space);
}
