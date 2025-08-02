// FolderPropertiesDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "FolderPropertiesDlg.h"

// CFolderPropertiesDlg 对话框

IMPLEMENT_DYNAMIC(CFolderPropertiesDlg, CSimplePropertiesDlg)

CFolderPropertiesDlg::CFolderPropertiesDlg(const ListItem& folder_info, CWnd* pParent /*=nullptr*/)
    : CSimplePropertiesDlg(pParent)
    , m_folder_info(folder_info)
{

}

CFolderPropertiesDlg::~CFolderPropertiesDlg()
{
}

CString CFolderPropertiesDlg::GetDialogName() const
{
    return _T("FolderPropertiesDlg");
}

BEGIN_MESSAGE_MAP(CFolderPropertiesDlg, CSimplePropertiesDlg)
END_MESSAGE_MAP()


// CFolderPropertiesDlg 消息处理程序


BOOL CFolderPropertiesDlg::OnInitDialog()
{
    CSimplePropertiesDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    SetWindowTextW(theApp.m_str_table.LoadText(L"TXT_TITLE_FOLDER_PROPERTIES").c_str());
    SetIcon(IconMgr::IconType::IT_Info, FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}

void CFolderPropertiesDlg::InitData()
{
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_PATH"), m_folder_info.path);
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_LAST_PLAYED_TRACK"), m_folder_info.GetLastTrackDisplayName());
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_LIB_PROPERTIES_LAST_PLAYED_POSITION"), CPlayTime(m_folder_info.last_position).toString());
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_LIB_PLAYLIST_SORT"), SongInfo::GetSortModeDisplayName(m_folder_info.sort_mode));
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_NUM_OF_TRACK"), std::to_wstring(m_folder_info.total_num));
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_TOTAL_LENGTH"), CPlayTime(m_folder_info.total_time).toString3());
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_LIB_PATH_IS_CONTAIN_SUB_FOLDER"), theApp.m_str_table.LoadText(m_folder_info.contain_sub_folder ? L"TXT_LIB_PATH_IS_CONTAIN_SUB_FOLDER_YES" : L"TXT_LIB_PATH_IS_CONTAIN_SUB_FOLDER_NO"));
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_LAST_PLAYED_TIME"), m_folder_info.last_played_time == 0 ? L"-" : CTime(m_folder_info.last_played_time).Format(_T("%F %T")).GetString());
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_LIB_PROPERTIES_ADD_TIME"), m_folder_info.create_time == 0 ? L"-" : CTime(m_folder_info.create_time).Format(_T("%F %T")).GetString());
}
