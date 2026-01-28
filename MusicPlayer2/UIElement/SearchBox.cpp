#include "stdafx.h"
#include "SearchBox.h"
#include "UiSearchBox.h"
#include "AbstractListElement.h"
#include "TinyXml2Helper.h"

UiElement::SearchBox::SearchBox()
{
}

UiElement::SearchBox::~SearchBox()
{
    CCommon::DeleteModelessDialog(search_box_ctrl);
}

void UiElement::SearchBox::InitSearchBoxControl(CWnd* pWnd)
{
    if (!m_init)
    {
        CCommon::DeleteModelessDialog(search_box_ctrl);
        search_box_ctrl = new CUiSearchBox(pWnd);
        search_box_ctrl->Create();
        m_init = true;
    }
}

void UiElement::SearchBox::OnKeyWordsChanged()
{
    FindListElement();
    if (list_element != nullptr)
        list_element->QuickSearch(key_word);
}

void UiElement::SearchBox::Clear()
{
    search_box_ctrl->Clear();
}

void UiElement::SearchBox::Draw()
{
    CalculateRect();

    //绘制背景
    COLORREF back_color;
    if (hover)
        back_color = ui->GetUIColors().color_button_hover;
    else
        back_color = ui->GetUIColors().color_control_bar_back;
    bool draw_background{ ui->IsDrawBackgroundAlpha() };
    BYTE alpha;
    if (!draw_background)
        alpha = 255;
    else if (theApp.m_app_setting_data.dark_mode || hover)
        alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3;
    else
        alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency);
    if (!theApp.m_app_setting_data.button_round_corners)
        ui->GetDrawer().FillAlphaRect(rect, back_color, alpha);
    else
        ui->GetDrawer().DrawRoundRect(rect, back_color, ui->CalculateRoundRectRadius(rect), alpha);
    //绘制文本
    CRect rect_text{ rect };
    rect_text.left += ui->DPI(4);
    rect_text.right -= rect.Height();
    std::wstring text = key_word;
    COLORREF text_color = ui->GetUIColors().color_text;
    if (text.empty())
    {
        text = theApp.m_str_table.LoadText(L"TXT_SEARCH_PROMPT");
        text_color = ui->GetUIColors().color_text_heighlight;
    }
    ui->GetDrawer().DrawWindowText(rect_text, text.c_str(), text_color, Alignment::LEFT, true);
    //绘制图标
    icon_rect = rect;;
    icon_rect.left = rect_text.right;
    if (key_word.empty())
    {
        ui->DrawUiIcon(icon_rect, IconMgr::IT_Find);
    }
    else
    {
        CRect btn_rect{ icon_rect };
        btn_rect.DeflateRect(ui->DPI(2), ui->DPI(2));
        ui->DrawUIButton(btn_rect, clear_btn, IconMgr::IT_Close);
    }

    Element::Draw();
}

bool UiElement::SearchBox::MouseMove(CPoint point)
{
    hover = false;
    clear_btn.hover = false;
    //鼠标指向图标区域
    if (icon_rect.PtInRect(point))
    {
        clear_btn.hover = true;
        //更新鼠标提示
        if (!key_word.empty())
            ui->UpdateMouseToolTipPosition(TooltipIndex::SEARCHBOX_CLEAR_BTN, clear_btn.rect);
        else
            ui->UpdateMouseToolTipPosition(TooltipIndex::SEARCHBOX_CLEAR_BTN, CRect());
    }
    //指向搜索框区域
    else if (rect.PtInRect(point))
    {
        hover = true;
    }
    return true;
}

bool UiElement::SearchBox::MouseLeave()
{
    hover = false;
    clear_btn.hover = false;
    return true;
}

bool UiElement::SearchBox::LButtonUp(CPoint point)
{
    clear_btn.pressed = false;
    if (rect.PtInRect(point))
    {
        //点击清除按钮时清除搜索结果
        if (icon_rect.PtInRect(point))
        {
            if (search_box_ctrl != nullptr)
                search_box_ctrl->Clear();
        }
        //点击搜索框区域时显示搜索框控件
        else if (rect.PtInRect(point))
        {
            bool big_font{ ui->IsDrawLargeIcon() };
            CWnd* pWnd = ui->GetOwner();
            InitSearchBoxControl(pWnd);
            if (search_box_ctrl != nullptr)
                search_box_ctrl->Show(this, big_font);
        }
        return true;
    }
    return false;
}

bool UiElement::SearchBox::LButtonDown(CPoint point)
{
    if (rect.PtInRect(point))
    {
        if (icon_rect.PtInRect(point))
        {
            clear_btn.pressed = true;
        }
        return true;
    }
    return false;
}

void UiElement::SearchBox::HideTooltip()
{
    ui->UpdateMouseToolTipPosition(TooltipIndex::SEARCHBOX_CLEAR_BTN, CRect());
}

bool UiElement::SearchBox::SetCursor()
{
    if (hover)
    {
        ::SetCursor(::LoadCursor(NULL, IDC_IBEAM));
        return true;
    }
    return false;
}

void UiElement::SearchBox::FindListElement()
{
    if (!find_list_element)
    {
        list_element = FindRelatedElement<AbstractListElement>(list_element_id);
        if (list_element != nullptr)
            list_element->SetRelatedSearchBox(this);
        find_list_element = true;  //找过一次没找到就不找了
    }
}

void UiElement::SearchBox::FromXmlNode(tinyxml2::XMLElement* xml_node)
{
    Element::FromXmlNode(xml_node);
    list_element_id = CTinyXml2Helper::ElementAttribute(xml_node, "list_element_id");
}
