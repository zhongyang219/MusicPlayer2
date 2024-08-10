// FolderPropertiesDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "FolderPropertiesDlg.h"

// CFolderPropertiesDlg 对话框

IMPLEMENT_DYNAMIC(CFolderPropertiesDlg, CSimplePropertiesDlg)

CFolderPropertiesDlg::CFolderPropertiesDlg(const PathInfo& folder_info, CWnd* pParent /*=nullptr*/)
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
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_LAST_PLAYED_TRACK"), std::to_wstring(m_folder_info.track + 1));
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_LIB_PROPERTIES_LAST_PLAYED_POSITION"), Time(m_folder_info.position).toString());
    std::wstring str_sort_mode;
    switch (m_folder_info.sort_mode)
    {
    case SM_U_FILE: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_U_FILE"); break;
    case SM_D_FILE: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_D_FILE"); break;
    case SM_U_PATH: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_U_PATH"); break;
    case SM_D_PATH: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_D_PATH"); break;
    case SM_U_TITLE: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_U_TITLE"); break;
    case SM_D_TITLE: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_D_TITLE"); break;
    case SM_U_ARTIST: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_U_ARTIST"); break;
    case SM_D_ARTIST: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_D_ARTIST"); break;
    case SM_U_ALBUM: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_U_ALBUM"); break;
    case SM_D_ALBUM: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_D_ALBUM"); break;
    case SM_U_TRACK: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_U_TRACK"); break;
    case SM_D_TRACK: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_D_TRACK"); break;
    case SM_U_LISTEN: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_U_LISTEN"); break;
    case SM_D_LISTEN: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_D_LISTEN"); break;
    case SM_U_TIME: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_U_TIME"); break;
    case SM_D_TIME: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_D_TIME"); break;
    case SM_UNSORT: str_sort_mode = theApp.m_str_table.LoadText(L"TXT_SM_UNSORT"); break;
    }
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_LIB_PLAYLIST_SORT"), str_sort_mode);
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_NUM_OF_TRACK"), std::to_wstring(m_folder_info.track_num));
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_TOTAL_LENGTH"), Time(m_folder_info.total_time).toString3());
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_LIB_PATH_IS_CONTAIN_SUB_FOLDER"), theApp.m_str_table.LoadText(m_folder_info.contain_sub_folder ? L"TXT_LIB_PATH_IS_CONTAIN_SUB_FOLDER_YES" : L"TXT_LIB_PATH_IS_CONTAIN_SUB_FOLDER_NO"));
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_LAST_PLAYED_TIME"), m_folder_info.last_played_time == 0 ? L"-" : CTime(m_folder_info.last_played_time).Format(_T("%F %T")).GetString());
    m_items.emplace_back(theApp.m_str_table.LoadText(L"TXT_LIB_PROPERTIES_ADD_TIME"), m_folder_info.add_time == 0 ? L"-" : CTime(m_folder_info.add_time).Format(_T("%F %T")).GetString());
}
