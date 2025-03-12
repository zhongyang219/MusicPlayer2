#pragma once
#include "ListItem.h"

// 维护程序所有的列表对象即实际持有&管理所有“ListItem”
// 此类不直接提供面向UI的接口，因为UI不应从这里直接读数据，同步有问题
// 请构造一个CListCache对象来为UI创建只读数据副本
// 需要修改时仍然使用CRecentList的接口，CListCache成员方法reload能够刷新其持有的显示缓存

class CListCache;

class CRecentList
{
    friend class CListCache;
public:
    ~CRecentList() {};
    static CRecentList& Instance() { return m_instance; }

    // 列表排序方式
    enum listSortMode
    {
        SM_UNSORTED,        // 未排序
        SM_RECENT_PLAYED,   // 最近播放
        SM_RECENT_CREATED,  // 最近创建/添加
        SM_PATH,            // 路径/名称
    };
    bool SetSortMode(ListType type, listSortMode sort_mode);
    listSortMode GetSortMode(ListType type) const;

    // 更新(不存在时添加)当前正在播放的列表的信息（列表进入/播放曲目/列表退出 时使用这个）
    // 约定只有此方法能够变动m_list.front()，因为第一项即程序的“当前播放”列表
    void SetCurrentList(ListItem list_item);
    // 获取当前播放的列表的信息副本
    ListItem GetCurrentList() const;
    // 判断list_item是否为正在播放的列表
    bool IsCurrentList(const ListItem& list_item) const;
    // 如果list_item不存在则将其加入m_list末尾并返回true，否则返回false
    // 对LT_PLAYLIST，文件不存在时会负责创建空播放列表文件
    bool AddNewItem(ListItem list_item);
    // 从m_list加载指定list_item的信息，不存在时返回false
    bool LoadItem(ListItem& list_item) const;

    // 实际存储时不再区分播放列表类型，此处枚举仅用作接下来的接口的参数
    enum PlaylistType
    {
        PT_DEFAULT,
        PT_FAVOURITE,
        PT_TEMP,
        PT_MAX,
    };
    // 获取指定的特殊播放列表信息副本
    ListItem GetSpecPlaylist(PlaylistType type) const;
    // 判断是否正在播放指定类型的特殊播放列表 (type为PT_MAX时对任意特殊播放列表返回true)
    bool IsPlayingSpecPlaylist(PlaylistType type = PlaylistType::PT_MAX);
    // 判断ListItem是否是指定类型的特殊播放列表 (type为PT_MAX时对任意特殊播放列表返回true)
    static bool IsSpecPlaylist(const ListItem& list_item, PlaylistType type = PlaylistType::PT_MAX);

    // 移除指定的列表信息（不存在时返回false）
    // 特别的，当前播放的列表 & 默认播放列表 & 我喜欢的播放列表 禁止移除
    // 对于LT_PLAYLIST，请先成功删除播放列表文件再调用此方法（因为不应在持有锁期间打开模态窗口）
    bool RemoveItem(const ListItem& list_item);
    // 遍历m_list，移除func返回true的项目
    // 特别的，当前播放的列表 & 默认播放列表 & 我喜欢的播放列表 禁止移除
    // 对于LT_PLAYLIST，请先成功删除播放列表文件再调用此方法（因为不应在持有锁期间打开模态窗口）
    int RemoveItemIf(std::function<bool(const ListItem& list_item)> func);

    // 将上次播放时间清空，使它从“最近播放”中移除 (不会对当前播放生效)
    // 特别的，对于LT_MEDIA_LIB实际执行的是RemoveItem
    bool ResetLastPlayedTime(const ListItem& list_item);

    // 翻转m_list中指定项目的contain_sub_folder成员
    // ListItem的type必须为LT_FOLDER，不会对当前播放生效
    bool SetContainSubFolder(const ListItem& list_item);
    // 将m_list内对应项目的 total_time & total_num 修改为参数的值
    // ListItem的type必须为LT_PLAYLIST，不会对当前播放生效，有修改发生时返回true
    bool SetPlaylistTotalInfo(const ListItem& list_item, int total_num, int total_time);
    // 重命名播放列表 (之后要改成不会对当前播放生效)
    bool RenamePlaylist(const ListItem& list_item, const wstring& new_path);

private:
    // 不存在时创建&读取对应playlist文件的创建时间（CRecentList内部使用）
    ListItem CreateNewPlaylistListItem(ListItem list_item);

public:
    // 将数据写入文件
    void SaveData() const;
    // 从文件读取数据
    bool LoadData();
private:
    // 读取数据后调用一次
    void AfterLoadData();
    // 读取旧版未整合数据
    void LoadOldData();

private:
    CRecentList() {};
    static CRecentList m_instance;                  // 此类唯一的对象

    std::atomic<int> m_ver{};                       // 每次m_list/m_sort_mode变化时加一，即数据的修改标记
    std::array<listSortMode, LT_MAX> m_sort_mode{}; // 列表的列表的排序方式，此类不直接使用，由友元CListCache读取使用
    std::list<ListItem> m_list;                     // 列表的列表，总是保持按最近播放时间降序排序

    mutable std::mutex m_mutex;                     // 此类的成员变量读/写时先锁定此互斥量
    // 特殊播放列表的原型，用来做==比较，初始化之后只读
    static std::array<ListItem, CRecentList::PlaylistType::PT_MAX> SP_PLAYLIST;
};

