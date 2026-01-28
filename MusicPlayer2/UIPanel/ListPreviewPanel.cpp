#include "stdafx.h"
#include "ListPreviewPanel.h"

CListPreviewPanel::CListPreviewPanel(CPlayerUIBase* ui)
	: CPlayerUIPanel(ui, IDR_LIST_PREVIEW_PANEL)
{
	//查找标题栏和列表控件
	m_title_text = dynamic_cast<UiElement::Text*>(m_root_element->FindElement("title_bar_text"));
	m_icon = dynamic_cast<UiElement::Icon*>(m_root_element->FindElement("title_bar_icon"));
	m_track_list = dynamic_cast<UiElement::TrackList*>(m_root_element->FindElement("track_list"));
	m_track_num_text = dynamic_cast<UiElement::Text*>(m_root_element->FindElement("track_num_text"));
}

void CListPreviewPanel::SetListData(const ListItem& list_item)
{
	//设置标题文本
	if (m_title_text != nullptr)
	{
		m_title_text->SetText(list_item.GetDisplayName());
	}
	//设置图标
	if (m_icon != nullptr)
	{
		m_icon->SetIcon(list_item.GetTypeIcon());
		m_icon->SetVisible(true);
	}
	//设置列表数据
	if (m_track_list != nullptr)
	{
		m_track_list->SetListItem(list_item);
	}
	//设置列表项目数量文本
	if (m_track_num_text != nullptr)
	{
		int track_num = 0;
		if (m_track_list != nullptr)
			track_num = m_track_list->GetRowCount();
		std::wstring str_track_num = theApp.m_str_table.LoadTextFormat(L"UI_TXT_TRACK_LIST_TOTLA_NUM", { track_num });
		m_track_num_text->SetText(str_track_num);
	}
}
