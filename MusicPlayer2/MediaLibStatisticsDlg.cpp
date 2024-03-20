// MediaLibStatisticsDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "MediaLibStatisticsDlg.h"
#include "SongDataManager.h"
#include "MediaLibHelper.h"

// CMediaLibStatisticsDlg 对话框

IMPLEMENT_DYNAMIC(CMediaLibStatisticsDlg, CBaseDialog)

CMediaLibStatisticsDlg::CMediaLibStatisticsDlg(CWnd* pParent /*=nullptr*/)
    : CBaseDialog(IDD_SELECT_ITEM_DIALOG, pParent)
{

}

CMediaLibStatisticsDlg::~CMediaLibStatisticsDlg()
{
}

CString CMediaLibStatisticsDlg::GetDialogName() const
{
    return _T("MediaLibStatisticsDlg");
}

bool CMediaLibStatisticsDlg::InitializeControls()
{
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TITLE_LIB_STATISTICS");
    SetWindowTextW(temp.c_str());
    // IDC_LIST1
    // IDOK
    temp = theApp.m_str_table.LoadText(L"TXT_CLOSE");
    SetDlgItemTextW(IDCANCEL, temp.c_str());

    RepositionTextBasedControls({
        { CtrlTextInfo::R1, IDOK, CtrlTextInfo::W32 },
        { CtrlTextInfo::R2, IDCANCEL, CtrlTextInfo::W32 }
        });
    return true;
}

void CMediaLibStatisticsDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
}

BEGIN_MESSAGE_MAP(CMediaLibStatisticsDlg, CBaseDialog)
END_MESSAGE_MAP()


// CMediaLibStatisticsDlg 消息处理程序


BOOL CMediaLibStatisticsDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化

    SetIcon(theApp.m_icon_set.info.GetIcon(true), FALSE);

    //初始化控件
    ShowDlgCtrl(IDOK, false);
    if (auto pWnd = GetDlgItem(IDCANCEL))
        pWnd->SetFocus();

    //初始化列表
    CRect rect;
    m_list_ctrl.GetWindowRect(rect);
    m_list_ctrl.SetExtendedStyle(m_list_ctrl.GetExtendedStyle() | LVS_EX_GRIDLINES);
    int width0 = rect.Width() / 2;
    int width1 = rect.Width() - width0 - theApp.DPI(20) - 1;
    m_list_ctrl.InsertColumn(0, theApp.m_str_table.LoadText(L"TXT_ITEM").c_str(), LVCFMT_LEFT, width0);
    m_list_ctrl.InsertColumn(1, theApp.m_str_table.LoadText(L"TXT_VALUE").c_str(), LVCFMT_LEFT, width1);

    //插入列
    m_list_ctrl.InsertItem(RI_ARTIST, theApp.m_str_table.LoadText(L"TXT_ARTIST").c_str());   //艺术家
    m_list_ctrl.InsertItem(RI_ALBUM, theApp.m_str_table.LoadText(L"TXT_ALBUM").c_str());     //唱片
    m_list_ctrl.InsertItem(RI_GENRE, theApp.m_str_table.LoadText(L"TXT_GENRE").c_str());     //流派
    m_list_ctrl.InsertItem(RI_TOTAL, theApp.m_str_table.LoadText(L"TXT_LIB_STATISTICS_TOTAL_NUM_OF_TRACK").c_str());      //曲目总数
    m_list_ctrl.InsertItem(RI_PLAYED, theApp.m_str_table.LoadText(L"TXT_LIB_STATISTICS_NUM_OF_TRACK_PLAYED").c_str());    //播放过的曲目数

    //设置数值
    std::set<std::wstring, StringComparerNoCase> artist_set;
    std::set<std::wstring, StringComparerNoCase> album_set;
    std::set<std::wstring, StringComparerNoCase> genre_set;
    int played_num{};
    size_t total_size{};
    CSongDataManager::GetInstance().GetSongData([&](const CSongDataManager::SongDataMap& song_data_map)
        {
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
    m_list_ctrl.SetItemText(RI_ARTIST, 1, std::to_wstring(artist_set.size()).c_str());
    m_list_ctrl.SetItemText(RI_ALBUM, 1, std::to_wstring(album_set.size()).c_str());
    m_list_ctrl.SetItemText(RI_GENRE, 1, std::to_wstring(genre_set.size()).c_str());
    m_list_ctrl.SetItemText(RI_TOTAL, 1, std::to_wstring(total_size).c_str());
    m_list_ctrl.SetItemText(RI_PLAYED, 1, std::to_wstring(played_num).c_str());

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}
