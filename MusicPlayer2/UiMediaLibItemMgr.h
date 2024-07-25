#pragma once
#include <map>
#include "MediaLibHelper.h"

class CUiMediaLibItemMgr
{
public:
    ~CUiMediaLibItemMgr();
    static CUiMediaLibItemMgr& Instance();

    void Init();

    int GetItemCount(CMediaClassifier::ClassificationType type) const;
    std::wstring GetItemDisplayName(CMediaClassifier::ClassificationType type, int index) const;
    const std::wstring& GetItemName(CMediaClassifier::ClassificationType type, int index) const;
    int GetItemSongCount(CMediaClassifier::ClassificationType type, int index) const;
    void SetCurrentName(CMediaClassifier::ClassificationType type, const std::wstring& name);
    int GetCurrentIndex(CMediaClassifier::ClassificationType type);

private:
    CUiMediaLibItemMgr();
    void GetClassifiedMeidaLibItemList(CMediaClassifier::ClassificationType type);

    static CUiMediaLibItemMgr m_instance;
    struct ItemInfo
    {
        std::wstring name;
        int count{};
    };

    const ItemInfo& GetItemInfo(CMediaClassifier::ClassificationType type, int index) const;

    std::map<CMediaClassifier::ClassificationType, std::vector<ItemInfo>> m_item_map;   //保存媒体库中所有分类的名称列表
    bool m_loading{};
    std::map<CMediaClassifier::ClassificationType, int> m_current_index_map;    //保存媒体库模式下每种模式正在播放的曲目
    std::map<CMediaClassifier::ClassificationType, std::wstring> m_current_name_map;    //保存媒体库模式下每种模式正在播放的曲目
};


class CUiMyFavouriteItemMgr
{
public:
    ~CUiMyFavouriteItemMgr();
    static CUiMyFavouriteItemMgr& Instance();

    int GetSongCount() const;
    const SongInfo& GetSongInfo(int index) const;
    void UpdateMyFavourite();
    bool IsLoading() const { return m_loading; }

private:
    CUiMyFavouriteItemMgr();
    static CUiMyFavouriteItemMgr m_instance;

    vector<SongInfo> m_may_favourite_song_list;     //“我喜欢的音乐”列表
    bool m_loading{};

};

