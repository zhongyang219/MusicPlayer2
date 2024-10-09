#include "stdafx.h"
#include "ListCache.h"
#include "CRecentList.h"
#include "Common.h"


bool CListCache::reload()
{
    // 如果m_ver没变则返回false表示CRecentList数据没有变化
    if (m_ui_ver == CRecentList::m_instance.m_ver)
        return false;
    // 加锁后不允许再调用CRecentList的方法防止锁重入 (为保证同步，reload期间只能加这一次锁，不能反复加解锁)
    std::lock_guard<std::mutex> lock(CRecentList::m_instance.m_mutex);

    // 虽然是CRecentList的友元但此处只应进行只读访问
    const auto& instance = CRecentList::m_instance;
    // m_ui_list
    m_ui_list.clear();
    vector<const ListItem*> sub_list = BuildSubList();
    for (const ListItem* pListItem : sub_list)
        m_ui_list.push_back(*pListItem);
    // m_ui_current_play_index
    ASSERT(!instance.m_list.empty());   // CRecentList的m_list在初始化后就从不为空，因为有特殊播放列表不会被删除
    auto iter = std::find(m_ui_list.begin(), m_ui_list.end(), instance.m_list.front());
    m_ui_current_play_index = (iter != m_ui_list.end()) ? iter - m_ui_list.begin() : -1;
    // m_ui_ver
    m_ui_ver = instance.m_ver;

    return true;
}

ListItem CListCache::GetItem(size_t index) const
{
    std::lock_guard<std::mutex> lock(CRecentList::m_instance.m_mutex);
    vector<const ListItem*> sub_list = BuildSubList();
    if (index < sub_list.size())
        return *sub_list[index];
    return ListItem();
}

int CListCache::GetIndex(const ListItem& list_item) const
{
    std::lock_guard<std::mutex> lock(CRecentList::m_instance.m_mutex);
    vector<const ListItem*> sub_list = BuildSubList();
    auto it = std::find_if(sub_list.begin(), sub_list.end(), [&](const ListItem* pListItem) { return list_item == *pListItem; });
    if (it == sub_list.end())
        return -1;
    return it - sub_list.begin();
}

void CListCache::ReadAllListItem(std::function<void(vector<const ListItem*>& sub_list)> func) const
{
    std::lock_guard<std::mutex> lock(CRecentList::m_instance.m_mutex);
    vector<const ListItem*> sub_list = BuildSubList();
    func(sub_list);
}

vector<const ListItem*> CListCache::BuildSubList() const
{
    vector<const ListItem*> sub_list;
    const auto& instance = CRecentList::m_instance;

    if (m_type == LT_CURRENT)
    {
        sub_list.push_back(&instance.m_list.front());
        return sub_list;
    }

    for (const ListItem& item : instance.m_list)
        if ((m_type == LT_PLAYLIST_NO_SPEC && item.type == LT_PLAYLIST && !CRecentList::IsSpecPlaylist(item)) ||
            (m_type == LT_RECENT && item.last_played_time > 0) ||
            (m_type < LT_MAX && m_type == item.type))
            sub_list.push_back(&item);

    CRecentList::listSortMode sort_mode{ CRecentList::SM_RECENT_PLAYED };
    if (m_type == LT_FOLDER)
        sort_mode = instance.m_sort_mode[LT_FOLDER];
    if (m_type == LT_PLAYLIST || m_type == LT_PLAYLIST_NO_SPEC)
        sort_mode = instance.m_sort_mode[LT_PLAYLIST];

    if (sub_list.size() > 1)
    {
        // m_list本身是按照最近播放排序的，此时不用再次排序
        switch (sort_mode)
        {
        case CRecentList::SM_RECENT_CREATED:
            std::stable_sort(sub_list.begin(), sub_list.end(),
                [](const ListItem* a, const ListItem* b) { return a->create_time > b->create_time; });
            break;
        case CRecentList::SM_PATH:  // 如果将来LT_MEDIA_LIB需要SM_PATH排序了，这里要注意一下，简单的排path可能不再符合要求
            std::stable_sort(sub_list.begin(), sub_list.end(),
                [](const ListItem* a, const ListItem* b) { return CCommon::StringCompareInLocalLanguage(a->path, b->path) < 0; });
            break;
        default:
            break;
        }
        // 排序播放列表列表时固定特殊播放列表位置
        if (m_type == LT_PLAYLIST)
        {
            vector<const ListItem*> tmp_list;
            tmp_list.resize(sub_list.size(), nullptr);
            int offset{ 2 };
            for (const ListItem* pListItem : sub_list)
            {
                if (CRecentList::IsSpecPlaylist(*pListItem, CRecentList::PT_DEFAULT))
                    tmp_list[0] = pListItem;
                else if (CRecentList::IsSpecPlaylist(*pListItem, CRecentList::PT_FAVOURITE))
                    tmp_list[1] = pListItem;
                else if (CRecentList::IsSpecPlaylist(*pListItem, CRecentList::PT_TEMP))
                    tmp_list.back() = pListItem;
                else
                    tmp_list[offset++] = pListItem;
            }
            ASSERT(std::find(tmp_list.begin(), tmp_list.end(), nullptr) == tmp_list.end());
            std::swap(sub_list, tmp_list);
        }
    }
    return sub_list;
}
