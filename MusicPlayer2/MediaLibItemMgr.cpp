#include "stdafx.h"
#include "MediaLibItemMgr.h"
#include "MusicPlayer2.h"
#include "MediaLibPlaylistMgr.h"

CMediaLibItemMgr CMediaLibItemMgr::m_instance;

CMediaLibItemMgr::~CMediaLibItemMgr()
{
}

CMediaLibItemMgr& CMediaLibItemMgr::Instance()
{
    return m_instance;
}

void CMediaLibItemMgr::GetClassifiedMeidaLibItemList(CMediaClassifier::ClassificationType type)
{
    auto& item_list{ m_item_map[type] };
    item_list.clear();
    CMediaClassifier classifier(type, theApp.m_media_lib_setting_data.hide_only_one_classification);
    classifier.ClassifyMedia();
    for (const auto& item : classifier.GetMeidaList())
    {
        item_list.push_back(item.first);
    }
}

void CMediaLibItemMgr::Init()
{
    m_loading = true;

    GetClassifiedMeidaLibItemList(CMediaClassifier::CT_ARTIST);
    GetClassifiedMeidaLibItemList(CMediaClassifier::CT_ALBUM);

    m_loading = false;
}

int CMediaLibItemMgr::GetItemCount(CMediaClassifier::ClassificationType type)
{
    if (!m_loading)
    {
        auto iter = m_item_map.find(type);
        if (iter != m_item_map.end())
            return static_cast<int>(iter->second.size());
    }
    return 0;
}

std::wstring CMediaLibItemMgr::GetItemName(CMediaClassifier::ClassificationType type, int index)
{
    if (!m_loading)
    {
        auto iter = m_item_map.find(type);
        if (iter != m_item_map.end())
        {
            const auto& item_list{ iter->second };
            if (index >= 0 && index < static_cast<int>(item_list.size()))
            {
                const std::wstring& name{ item_list[index] };
                return CMediaLibPlaylistMgr::GetMediaLibItemDisplayName(type, name);
            }
        }
    }
    return std::wstring();
}

CMediaLibItemMgr::CMediaLibItemMgr()
{
}
