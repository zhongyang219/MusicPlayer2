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
    int total_play_count{};
    __int64 total_listen_time{};
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
            total_play_count += item.second.play_count;
            total_listen_time += item.second.listen_time;
        }
        total_size = song_data_map.size();
    });

    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_ARTIST"), std::to_wstring(artist_set.size()));   //艺术家
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_ALBUM"), std::to_wstring(album_set.size()));     //唱片集
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_GENRE"), std::to_wstring(genre_set.size()));     //流派
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_LIB_STATISTICS_TOTAL_NUM_OF_TRACK"), std::to_wstring(total_size));       //曲目总数
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_LIB_STATISTICS_NUM_OF_TRACK_PLAYED"), std::to_wstring(played_num));      //播放过的曲目数
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_LIB_STATISTICS_TOTAL_PLAY_COUNT"), std::to_wstring(total_play_count));    //总播放次数

    //格式化累计播放时间
    wstring listen_time_str;
    int days = static_cast<int>(total_listen_time / (24 * 3600));
    int hours = static_cast<int>((total_listen_time % (24 * 3600)) / 3600);
    int minutes = static_cast<int>((total_listen_time % 3600) / 60);
    int seconds = static_cast<int>(total_listen_time % 60);
    if (days > 0)
        listen_time_str += std::to_wstring(days) + L"天 ";
    if (hours > 0 || days > 0)
        listen_time_str += std::to_wstring(hours) + L"时 ";
    if (minutes > 0 || hours > 0 || days > 0)
        listen_time_str += std::to_wstring(minutes) + L"分 ";
    listen_time_str += std::to_wstring(seconds) + L"秒";
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_LIB_STATISTICS_TOTAL_LISTEN_TIME"), listen_time_str);    //累计播放时间
}
