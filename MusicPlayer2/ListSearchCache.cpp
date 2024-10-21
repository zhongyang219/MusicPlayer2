#include "stdafx.h"
#include "ListSearchCache.h"
#include "MusicPlayer2.h"

CListSearchCache::CListSearchCache(CListCache::SubsetType type)
    : m_list_cache(type)
{
}

CListSearchCache::~CListSearchCache()
{
}

bool CListSearchCache::reload()
{
    if (!m_list_cache.reload())

    m_ui_searched_playing_index = -1;
    m_ui_search_result.clear();
    if (!m_ui_search_str.empty())
    {
        for (size_t i{}; i < m_list_cache.size(); ++i)
        {
            if (theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(m_ui_search_str, m_list_cache.at(i).GetDisplayName()))
                m_ui_search_result.push_back(i);
        }
        auto iter = std::find(m_ui_search_result.begin(), m_ui_search_result.end(), m_list_cache.playing_index());
        if (iter != m_ui_search_result.end())
            m_ui_searched_playing_index = iter - m_ui_search_result.begin();
    }
    return true;
}

size_t CListSearchCache::size() const
{
    if (!m_ui_search_str.empty())
        return m_ui_search_result.size();
    return m_list_cache.size();
}

const ListItem& CListSearchCache::at(size_t index) const
{
    if (!m_ui_search_str.empty())
        return m_list_cache.at(m_ui_search_result.at(index));
    return m_list_cache.at(index);
}

wstring CListSearchCache::display_index(size_t index) const
{
    if (!m_ui_search_str.empty())
        return std::to_wstring(m_ui_search_result.at(index) + 1);
    return std::to_wstring(index + 1);
}

int CListSearchCache::playing_index() const
{
    if (!m_ui_search_str.empty())
        return m_ui_searched_playing_index;
    return m_list_cache.playing_index();
}

void CListSearchCache::SetSearchStr(const wstring& search_str)
{
    m_ui_search_str = search_str;
}

ListItem CListSearchCache::GetItem(int index) const
{
    if (index >= 0 && index < static_cast<int>(size()))
        return at(index);
    return ListItem();
}

int CListSearchCache::GetIndex(const ListItem& list_item) const
{
    for (size_t i{}; i < size(); ++i)
        if (list_item == at(i))
            return i;
    return -1;
}

