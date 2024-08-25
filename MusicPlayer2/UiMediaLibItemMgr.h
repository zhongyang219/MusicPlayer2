#pragma once
#include <map>
#include "MediaLibHelper.h"
#include "UIElement.h"

class CUiMediaLibItemMgr
{
public:
    ~CUiMediaLibItemMgr();
    static CUiMediaLibItemMgr& Instance();

    void Init();

    int GetItemCount(CMediaClassifier::ClassificationType type) const;                              //获取指定类别下项目的数量
    std::wstring GetItemDisplayName(CMediaClassifier::ClassificationType type, int index) const;    //获取指定类别下项目显示到界面中的名称
    const std::wstring& GetItemName(CMediaClassifier::ClassificationType type, int index) const;    //获取指定项的原始名称，如果是<未知xxx>返回的是空
    int GetItemSongCount(CMediaClassifier::ClassificationType type, int index) const;               //获取指定类别下项目的曲目数量
    void SetCurrentName(CMediaClassifier::ClassificationType type, const std::wstring& name);       //设置指定类别下正在播放项目的名称，其中name为原始名称
    int GetCurrentIndex(CMediaClassifier::ClassificationType type);                                 //获取指定类别下正在播放项目的序号
    bool IsLoading() const { return m_loading; }
    bool IsInited() const { return m_inited; }

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
    bool m_inited{};                        //如果已经初始化过，则为true
    std::map<CMediaClassifier::ClassificationType, int> m_current_index_map;    //保存媒体库模式下每种模式正在播放的曲目
    std::map<CMediaClassifier::ClassificationType, std::wstring> m_current_name_map;    //保存媒体库模式下每种模式正在播放的曲目
    mutable std::shared_mutex m_shared_mutex;
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
    bool IsInited() const { return m_inited; }
    void GetSongList(std::vector<SongInfo>& song_list) const;
    bool Contains(const SongInfo& song) const;

private:
    CUiMyFavouriteItemMgr();
    static CUiMyFavouriteItemMgr m_instance;

    vector<SongInfo> m_may_favourite_song_list;     //“我喜欢的音乐”列表
    bool m_loading{};
    bool m_inited{};                        //如果已经初始化过，则为true
    mutable std::shared_mutex m_shared_mutex;

};


class CUiAllTracksMgr
{
public:
    ~CUiAllTracksMgr();
    static CUiAllTracksMgr& Instance();

    //用于在UI中显示的曲目信息
    struct UTrackInfo
    {
        SongKey song_key;
        std::wstring name;
        Time length;
        bool is_favourite{};
    };

    int GetSongCount() const;
    SongInfo GetSongInfo(int index) const;
    const UTrackInfo& GetItem(int index) const;
    int GetCurrentIndex() const;                //获取正在播放的曲目在m_all_tracks_list中的序号
    void SetCurrentSong(const SongInfo& song);  //设置正在播放的曲目，将其在m_all_tracks_list中的序号保存起来
    void UpdateAllTracks();                     //从CSongDataManager中更新所有曲目信息
    bool IsLoading() const { return m_loading; }
    bool IsInited() const { return m_inited; }
    void GetSongList(std::vector<SongInfo>& song_list) const;
    void AddOrRemoveMyFavourite(int index);     //仅更新UI中显示的“我喜欢”的状态，不更新到“我喜欢的音乐”播放列表中

private:
    CUiAllTracksMgr();
    static CUiAllTracksMgr m_instance;

    std::vector<UTrackInfo> m_all_tracks_list;  //所有曲目信息列表
    bool m_loading{};                       //如果正在初始化中，则为true
    int m_current_index{ -1 };              //正在播放的曲目在m_all_tracks_list中的序号
    bool m_inited{};                        //如果已经初始化过，则为true
    mutable std::shared_mutex m_shared_mutex;
};


class CUiFolderExploreMgr
{
public:
    static CUiFolderExploreMgr& Instance();

    std::vector<std::shared_ptr<UiElement::TreeElement::Node>>& GetRootNodes();
    void UpdateFolders();
    bool IsLoading() const { return m_loading; }
    bool IsInited() const { return m_inited; }

private:
    CUiFolderExploreMgr();
    void CreateFolderNodeByPath(std::wstring path, std::shared_ptr<UiElement::TreeElement::Node> parent);
    int GetAudioFilesNum(std::wstring path);

    static CUiFolderExploreMgr m_instance;

    bool m_loading{};                       //如果正在初始化中，则为true
    bool m_inited{};                        //如果已经初始化过，则为true

    std::vector<std::shared_ptr<UiElement::TreeElement::Node>> m_root_nodes;
    std::map<std::wstring, int> m_folder_audio_files_num;       //保存每个文件夹下音频文件的数量
};
