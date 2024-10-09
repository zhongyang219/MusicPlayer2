#pragma once
#include "ListItem.h"

// 为UI缓存CRecentList持有的指定类型数据
// 并提供适合UI直接使用的特殊接口（要求在同一个线程调用那些小写的方法）
class CListCache
{
public:
    CListCache(ListType type) : m_type(type) { ASSERT(type != LT_MAX); }
    ~CListCache() {};

////////////////////////////////////////////////////////////////////////////////
//  以下所有小写的方法要求总是在同一个线程调用才能够保证线程安全
//  如果reload返回true表示数据可能有变化，必要重绘

    // 重新载入数据
    bool reload();
    // 获取ListItem总数
    size_t size() const { return m_ui_list.size(); }
    // 获取位于特定index的ListItem，不允许超过size()的参数
    const ListItem& at(size_t index) const { return m_ui_list.at(index); }
    // 当前播放的列表在m_ui_list中时返回其在m_ui_list的索引，否则返回-1
    int playing_index() const { return m_ui_current_play_index; }

////////////////////////////////////////////////////////////////////////////////
//  以下方法直接基于CRecentList的数据提供，有线程安全保证，但因为锁粒度问题不能用于绘制UI
//  作为CRecentList的友元，使用其中数据时同时也要维持其互斥量保护的数据不外露
//  当上面的小写方法在自绘UI线程中使用时以下方法供主线程的消息处理使用
//  但要求一次消息处理期间只能调用一个方法一次，以避开多次加锁间的同步问题 （可以在这里添加适合各自情况的各种方法）

    // 获取位于特定index的ListItem（index越界时返回空项目）
    ListItem GetItem(size_t index) const;
    // 获取特定ListItem的index（ListItem不存在时返回-1）
    int GetIndex(const ListItem& list_item) const;

    // 如果可能，再写一个方法，不要用这个
    // 注意func返回后sub_list就失去了安全保证，不能再使用，也不能保证下次调用此方法时sub_list仍然相同
    void ReadAllListItem(std::function<void(vector<const ListItem*>& sub_list)> func) const;

private:
    // 先锁定CRecentList的m_mutex再调用，返回的指针在解锁前有效
    vector<const ListItem*> BuildSubList() const;

private:
    // 在构造时确定，表示此对象用来缓存哪个列表集合
    const ListType m_type;
    // 约定m_ui_开头的成员变量只允许小写的成员方法访问
    vector<ListItem> m_ui_list;
    int m_ui_current_play_index{ -1 };
    int m_ui_ver{};
};

