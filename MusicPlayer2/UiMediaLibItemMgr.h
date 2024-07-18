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
    void SetCurrentName(CMediaClassifier::ClassificationType type, const std::wstring& name);
    int GetCurrentIndex(CMediaClassifier::ClassificationType type);

private:
    CUiMediaLibItemMgr();
    void GetClassifiedMeidaLibItemList(CMediaClassifier::ClassificationType type);

    static CUiMediaLibItemMgr m_instance;
    std::map<CMediaClassifier::ClassificationType, std::vector<std::wstring>> m_item_map;   //保存媒体库中所有分类的名称列表
    bool m_loading{};
    std::map<CMediaClassifier::ClassificationType, int> m_current_index_map;    //保存媒体库模式下每种模式正在播放的曲目
    std::map<CMediaClassifier::ClassificationType, std::wstring> m_current_name_map;    //保存媒体库模式下每种模式正在播放的曲目
};

