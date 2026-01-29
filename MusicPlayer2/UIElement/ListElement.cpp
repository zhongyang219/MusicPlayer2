#include "stdafx.h"
#include "ListElement.h"

void UiElement::ListElement::SetColumnCount(int column_count)
{
	m_column_count = column_count;
}

void UiElement::ListElement::SetColumnWidth(int col, int width)
{
	m_col_width[col] = width;
}

void UiElement::ListElement::AddRow(const std::map<int, std::wstring>& row_data)
{
	std::lock_guard<std::mutex> guard(m_list_data_sync);
	m_list_data.push_back(row_data);
}

void UiElement::ListElement::AddRow(const std::wstring& str)
{
	std::lock_guard<std::mutex> guard(m_list_data_sync);
	std::map<int, std::wstring> row_data;
	row_data[0] = str;
	m_list_data.push_back(row_data);
}

bool UiElement::ListElement::DeleteRow(int row)
{
	if (row >= 0 && row < GetRowCount())
	{
		std::lock_guard<std::mutex> guard(m_list_data_sync);
		m_list_data.erase(m_list_data.begin() + row);
		return true;
	}
	return false;
}

bool UiElement::ListElement::SetColumnText(int row, int col, const wstring& text)
{
	if (row >= 0 && row < GetRowCount() && col >= 0 && col < GetColumnCount())
	{
		std::lock_guard<std::mutex> guard(m_list_data_sync);
		m_list_data[row][col] = text;
		return true;
	}
	return false;
}

void UiElement::ListElement::SetIcom(int row, IconMgr::IconType icon)
{
	std::lock_guard<std::mutex> guard(m_list_icon_sync);
	m_icons[row] = icon;
}

void UiElement::ListElement::ClearData()
{
	std::lock_guard<std::mutex> guard(m_list_data_sync);
	m_list_data.clear();
}

void UiElement::ListElement::SetHighlightRow(int row)
{
	m_highlight_row = row;
}

int UiElement::ListElement::GetHighlightRow() const
{
	return m_highlight_row;
}

std::wstring UiElement::ListElement::GetItemText(int row, int col)
{
	if (row >= 0 && row < GetRowCount() && col >= 0 && col < GetColumnCount())
	{
		std::lock_guard<std::mutex> guard(m_list_data_sync);
		const auto& row_data = m_list_data[row];
		auto iter = row_data.find(col);
		if (iter != row_data.end())
			return iter->second;
	}
	return std::wstring();
}

int UiElement::ListElement::GetRowCount()
{
	std::lock_guard<std::mutex> guard(m_list_data_sync);
	return static_cast<int>(m_list_data.size());
}

int UiElement::ListElement::GetColumnCount()
{
	return m_column_count;
}

int UiElement::ListElement::GetColumnWidth(int col, int total_width)
{
	int width = _GetColumnWidth(col);

	//如果设置了宽度，则返回设置的宽度
	if (width > 0)
	{
		return width;
	}
	//否则，用没设置过宽度的列平分剩下的宽度
	else
	{
		int no_width_coumt = 0;
		for (int i = 0; i < GetColumnCount(); i++)
		{
			if (_GetColumnWidth(i) == 0)
				no_width_coumt++;
		}
		return total_width / no_width_coumt;
	}
}

IconMgr::IconType UiElement::ListElement::GetIcon(int row)
{
	std::lock_guard<std::mutex> guard(m_list_icon_sync);
	auto iter = m_icons.find(row);
	if (iter != m_icons.end())
		return iter->second;
	return IconMgr::IT_NO_ICON;
}

bool UiElement::ListElement::HasIcon()
{
	std::lock_guard<std::mutex> guard(m_list_icon_sync);
	return !m_icons.empty();
}

bool UiElement::ListElement::IsHighlightRow(int row)
{
	return row == m_highlight_row;
}

int UiElement::ListElement::_GetColumnWidth(int col)
{
	int width = 0;
	auto iter = m_col_width.find(col);
	if (iter != m_col_width.end())
		width = iter->second;
	return width;
}
