#pragma once
#include <map>
#include "MediaLibHelper.h"

class CMediaLibItemMgr
{
public:
    ~CMediaLibItemMgr();
    static CMediaLibItemMgr& Instance();

    void Init();

    int GetItemCount(CMediaClassifier::ClassificationType type);
    std::wstring GetItemName(CMediaClassifier::ClassificationType type, int index);

private:
    CMediaLibItemMgr();

    void GetClassifiedMeidaLibItemList(CMediaClassifier::ClassificationType type);

    static CMediaLibItemMgr m_instance;

    std::map<CMediaClassifier::ClassificationType, std::vector<std::wstring>> m_item_map;

    bool m_loading{};
};

