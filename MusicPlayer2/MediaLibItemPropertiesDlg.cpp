// MediaLibItemPropertiesDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "MediaLibItemPropertiesDlg.h"

// CMediaLibItemPropertiesDlg 对话框

IMPLEMENT_DYNAMIC(CMediaLibItemPropertiesDlg, CSimplePropertiesDlg)

CMediaLibItemPropertiesDlg::CMediaLibItemPropertiesDlg(const ListItem& item_info, CWnd* pParent /*=nullptr*/)
    : CSimplePropertiesDlg(pParent)
    , m_item_info(item_info)
{

}

CMediaLibItemPropertiesDlg::~CMediaLibItemPropertiesDlg()
{
}

CString CMediaLibItemPropertiesDlg::GetDialogName() const
{
    return _T("MediaLibItemPropertiesDlg");
}

BEGIN_MESSAGE_MAP(CMediaLibItemPropertiesDlg, CSimplePropertiesDlg)
END_MESSAGE_MAP()


// CMediaLibItemPropertiesDlg 消息处理程序


BOOL CMediaLibItemPropertiesDlg::OnInitDialog()
{
    CSimplePropertiesDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    SetWindowTextW(theApp.m_str_table.LoadText(L"TXT_TITLE_MEDIA_LIB_ITEM_PROPERTIES").c_str());
    SetIcon(IconMgr::IconType::IT_Info, FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}

void CMediaLibItemPropertiesDlg::InitData()
{
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_LIB_PROPERTIES_TYPE"), m_item_info.GetTypeDisplayName());
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_NAME"), m_item_info.GetDisplayName());
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_LAST_PLAYED_TRACK"), m_item_info.GetLastTrackDisplayName());
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_LIB_PROPERTIES_LAST_PLAYED_POSITION"), Time(m_item_info.last_position).toString());
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_LIB_PLAYLIST_SORT"), SongInfo::GetSortModeDisplayName(m_item_info.sort_mode));
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_NUM_OF_TRACK"), std::to_wstring(m_item_info.total_num));
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_TOTAL_LENGTH"), Time(m_item_info.total_time).toString3());
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_LAST_PLAYED_TIME"), m_item_info.last_played_time == 0 ? L"-" : CTime(m_item_info.last_played_time).Format(_T("%F %T")).GetString());
}
