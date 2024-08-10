// PlaylistPropertiesDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "PlaylistPropertiesDlg.h"

// CPlaylistPropertiesDlg 对话框

IMPLEMENT_DYNAMIC(CPlaylistPropertiesDlg, CSimplePropertiesDlg)

CPlaylistPropertiesDlg::CPlaylistPropertiesDlg(const PlaylistInfo& playlist_info, CWnd* pParent /*=nullptr*/)
    : CSimplePropertiesDlg(pParent)
    , m_playlist_info(playlist_info)
{

}

CPlaylistPropertiesDlg::~CPlaylistPropertiesDlg()
{
}

CString CPlaylistPropertiesDlg::GetDialogName() const
{
    return _T("PlaylistPropertiesDlg");
}

BEGIN_MESSAGE_MAP(CPlaylistPropertiesDlg, CSimplePropertiesDlg)
END_MESSAGE_MAP()


// CPlaylistPropertiesDlg 消息处理程序


BOOL CPlaylistPropertiesDlg::OnInitDialog()
{
    CSimplePropertiesDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    SetWindowTextW(theApp.m_str_table.LoadText(L"TXT_TITLE_PLAYLIST_PROPERTIES").c_str());
    SetIcon(IconMgr::IconType::IT_Info, FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}

void CPlaylistPropertiesDlg::InitData()
{
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_NAME"), CPlaylistMgr::GetPlaylistDisplayName(m_playlist_info.path));
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_PATH"), m_playlist_info.path);
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_LAST_PLAYED_TRACK"), std::to_wstring(m_playlist_info.track + 1));
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_LIB_PROPERTIES_LAST_PLAYED_POSITION"), Time(m_playlist_info.position).toString());
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_NUM_OF_TRACK"), std::to_wstring(m_playlist_info.track_num));
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_TOTAL_LENGTH"), Time(m_playlist_info.total_time).toString3());
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_LAST_PLAYED_TIME"), m_playlist_info.last_played_time == 0 ? L"-" : CTime(m_playlist_info.last_played_time).Format(_T("%F %T")).GetString());
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_LIB_PROPERTIES_CREATE_TIME"), m_playlist_info.create_time == 0 ? L"-" : CTime(m_playlist_info.create_time).Format(_T("%F %T")).GetString());
}
