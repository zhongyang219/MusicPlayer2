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
        if (item.first != STR_OTHER_CLASSIFY_TYPE)
            item_list.push_back(item.first);
    }
}

void CMediaLibItemMgr::Init()
{
    m_loading = true;

    GetClassifiedMeidaLibItemList(CMediaClassifier::CT_ARTIST);
    GetClassifiedMeidaLibItemList(CMediaClassifier::CT_ALBUM);
    GetClassifiedMeidaLibItemList(CMediaClassifier::CT_GENRE);
    GetClassifiedMeidaLibItemList(CMediaClassifier::CT_YEAR);
    GetClassifiedMeidaLibItemList(CMediaClassifier::CT_TYPE);
    GetClassifiedMeidaLibItemList(CMediaClassifier::CT_BITRATE);
    GetClassifiedMeidaLibItemList(CMediaClassifier::CT_RATING);

    m_loading = false;
}

int CMediaLibItemMgr::GetItemCount(CMediaClassifier::ClassificationType type) const
{
    if (!m_loading)
    {
        auto iter = m_item_map.find(type);
        if (iter != m_item_map.end())
            return static_cast<int>(iter->second.size());
    }
    return 0;
}

std::wstring CMediaLibItemMgr::GetItemDisplayName(CMediaClassifier::ClassificationType type, int index) const
{
    if (!m_loading)
    {
        const std::wstring& name{ GetItemName(type, index) };
        return CMediaLibPlaylistMgr::GetMediaLibItemDisplayName(type, name);
    }
    return std::wstring();
}

const std::wstring& CMediaLibItemMgr::GetItemName(CMediaClassifier::ClassificationType type, int index) const
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
                return name;
            }
        }
    }
    static std::wstring empty_str{};
    return empty_str;
}

void CMediaLibItemMgr::SetCurrentName(CMediaClassifier::ClassificationType type, const std::wstring& name)
{
    m_current_name_map[type] = name;
    m_current_index_map.erase(type);
}

int CMediaLibItemMgr::GetCurrentIndex(CMediaClassifier::ClassificationType type)
{
    if (!m_loading)
    {
        auto index_iter = m_current_index_map.find(type);
        if (index_iter == m_current_index_map.end())
        {
            //根据名称查找所在的序号
            auto name_iter = m_current_name_map.find(type);
            if (name_iter != m_current_name_map.end())
            {
                const auto& name_list{ m_item_map[type] };
                auto iter = std::find(name_list.begin(), name_list.end(), name_iter->second);
                if (iter != name_list.end())
                {
                    int index = iter - name_list.begin();
                    //设置当前项目的序号
                    m_current_index_map[type] = index;
                    return index;
                }
            }
        }
        else
        {
            return index_iter->second;
        }
    }

    return -1;
}

CMediaLibItemMgr::CMediaLibItemMgr()
{
}
