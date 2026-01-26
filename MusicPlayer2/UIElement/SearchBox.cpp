#include "stdafx.h"
#include "SearchBox.h"
#include "UiSearchBox.h"
#include "AbstractListElement.h"

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
    ui->DrawSearchBox(rect, this);
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
            bool big_font{ ui->m_ui_data.full_screen && ui->IsDrawLargeIcon() };
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
