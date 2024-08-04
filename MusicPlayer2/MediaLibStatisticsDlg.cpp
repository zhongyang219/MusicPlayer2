// MediaLibStatisticsDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "MediaLibStatisticsDlg.h"
#include "SongDataManager.h"
#include "MediaLibHelper.h"

// CMediaLibStatisticsDlg 对话框

IMPLEMENT_DYNAMIC(CMediaLibStatisticsDlg, CSimplePropertiesDlg)

CMediaLibStatisticsDlg::CMediaLibStatisticsDlg(CWnd* pParent /*=nullptr*/)
    : CSimplePropertiesDlg(pParent)
{

}

CMediaLibStatisticsDlg::~CMediaLibStatisticsDlg()
{
}

CString CMediaLibStatisticsDlg::GetDialogName() const
{
    return _T("MediaLibStatisticsDlg");
}

BEGIN_MESSAGE_MAP(CMediaLibStatisticsDlg, CSimplePropertiesDlg)
END_MESSAGE_MAP()


// CMediaLibStatisticsDlg 消息处理程序


BOOL CMediaLibStatisticsDlg::OnInitDialog()
{
    CSimplePropertiesDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    SetWindowTextW(theApp.m_str_table.LoadText(L"TITLE_LIB_STATISTICS").c_str());
    SetIcon(IconMgr::IconType::IT_Info, FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}

void CMediaLibStatisticsDlg::InitData()
{
    std::set<std::wstring, StringComparerNoCase> artist_set;
    std::set<std::wstring, StringComparerNoCase> album_set;
    std::set<std::wstring, StringComparerNoCase> genre_set;
    int played_num{};
    size_t total_size{};
    CSongDataManager::GetInstance().GetSongData([&](const CSongDataManager::SongDataMap& song_data_map) {
        for (const auto& item : song_data_map)
        {
            //处理多个艺术家情况
            std::vector<std::wstring> artist_list;
            item.second.GetArtistList(artist_list);
            for (const auto& artist : artist_list)
                artist_set.emplace(artist);

            album_set.emplace(item.second.album);
            genre_set.emplace(item.second.genre);
            if (item.second.last_played_time > 0)
                played_num++;
        }
        total_size = song_data_map.size();
    });

    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_ARTIST"), std::to_wstring(artist_set.size()));   //艺术家
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_ALBUM"), std::to_wstring(album_set.size()));     //唱片集
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_GENRE"), std::to_wstring(genre_set.size()));     //流派
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_LIB_STATISTICS_TOTAL_NUM_OF_TRACK"), std::to_wstring(total_size));       //曲目总数
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_LIB_STATISTICS_NUM_OF_TRACK_PLAYED"), std::to_wstring(played_num));      //播放过的曲目数
}
