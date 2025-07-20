#pragma once
#include "ListCache.h"

class CListSearchCache
{
public:
    CListSearchCache(CListCache::SubsetType type);
    ~CListSearchCache();

    // 重新载入数据 & 执行搜索
    bool reload();
    // 获取ListItem总数
    size_t size() const;
    // 获取位于特定index的ListItem，不允许超过size()的参数
    const ListItem& at(size_t index) const;
    // 获取特定index的显示序号
    wstring display_index(size_t index) const;
    // 当前播放的列表在显示列表中时返回索引，否则返回-1
    int playing_index() const;

    // 设置搜索字符串，设置空字符串即取消搜索（需调用reload实际执行搜索）
    void SetSearchStr(const wstring& search_str = {});
    // 获取位于指定index的ListItem，参数越界时返回空项目
    ListItem GetItem(int index) const;
    // 获取位于指定ListItem的index，参数不存在时返回-1
    int GetIndex(const ListItem& list_item) const;

private:
    CListCache m_list_cache;
    wstring m_ui_search_str;
    vector<size_t> m_ui_search_result;
    int m_ui_searched_playing_index{ -1 };
};

