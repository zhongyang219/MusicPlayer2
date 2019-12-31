// MediaLibSettingDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "MediaLibSettingDlg.h"
#include "afxdialogex.h"


// CMediaLibSettingDlg 对话框

IMPLEMENT_DYNAMIC(CMediaLibSettingDlg, CTabDlg)

CMediaLibSettingDlg::CMediaLibSettingDlg(CWnd* pParent /*=nullptr*/)
	: CTabDlg(IDD_MEDIA_LIB_SETTING_DIALOG, pParent)
{

}

CMediaLibSettingDlg::~CMediaLibSettingDlg()
{
}

void CMediaLibSettingDlg::ShowDataSizeInfo()
{
    CString info;
    if (m_data_size < 1024)
        info.Format(_T("%s: %d %s"), CCommon::LoadText(IDS_CURRENT_DATA_FILE_SIZE), m_data_size, CCommon::LoadText(IDS_BYTE));
    else if (m_data_size < 1024 * 1024)
        info.Format(_T("%s: %.2f KB (%d %s)"), CCommon::LoadText(IDS_CURRENT_DATA_FILE_SIZE), static_cast<float>(m_data_size) / 1024.0f, m_data_size, CCommon::LoadText(IDS_BYTE));
    else
        info.Format(_T("%s: %.2f MB (%d %s)"), CCommon::LoadText(IDS_CURRENT_DATA_FILE_SIZE), static_cast<float>(m_data_size) / 1024.0f / 1024.0f, m_data_size, CCommon::LoadText(IDS_BYTE));		//注：此处曾经由于“%.2fMB”漏掉了“f”导致出现了一打开这个对话框程序就停止工作的严重问题。
    SetDlgItemText(IDC_SIZE_STATIC, info);
}

void CMediaLibSettingDlg::DoDataExchange(CDataExchange* pDX)
{
    CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CLASSIFY_OTHER_CHECK, m_classify_other_chk);
    //DDX_Control(pDX, IDC_SHOW_TREE_TOOL_TIPS_CHECK, m_show_tree_tool_tips_chk);
    DDX_Control(pDX, IDC_DIR_LIST, m_dir_list_ctrl);
    DDX_Control(pDX, IDC_UPDATE_MEDIA_LIB_CHK, m_update_media_lib_chk);
}


BEGIN_MESSAGE_MAP(CMediaLibSettingDlg, CTabDlg)
    ON_BN_CLICKED(IDC_CLASSIFY_OTHER_CHECK, &CMediaLibSettingDlg::OnBnClickedClassifyOtherCheck)
    //ON_BN_CLICKED(IDC_SHOW_TREE_TOOL_TIPS_CHECK, &CMediaLibSettingDlg::OnBnClickedShowTreeToolTipsCheck)
    ON_BN_CLICKED(IDC_ADD_BUTTON, &CMediaLibSettingDlg::OnBnClickedAddButton)
    ON_BN_CLICKED(IDC_DELETE_BUTTON, &CMediaLibSettingDlg::OnBnClickedDeleteButton)
    ON_BN_CLICKED(IDC_UPDATE_MEDIA_LIB_CHK, &CMediaLibSettingDlg::OnBnClickedUpdateMediaLibChk)
    ON_BN_CLICKED(IDC_CLEAN_DATA_FILE_BUTTON, &CMediaLibSettingDlg::OnBnClickedCleanDataFileButton)
END_MESSAGE_MAP()


// CMediaLibSettingDlg 消息处理程序


BOOL CMediaLibSettingDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
    m_data_size = CCommon::GetFileSize(theApp.m_song_data_path);
    ShowDataSizeInfo();

    m_classify_other_chk.SetCheck(m_data.hide_only_one_classification);
    //m_show_tree_tool_tips_chk.SetCheck(m_data.show_tree_tool_tips);
    m_update_media_lib_chk.SetCheck(m_data.update_media_lib_when_start_up);

    for (const auto& str : m_data.media_folders)
        m_dir_list_ctrl.AddString(str.c_str());

    m_toolTip.Create(this);
    m_toolTip.SetMaxTipWidth(theApp.DPI(300));
    m_toolTip.AddTool(GetDlgItem(IDC_CLEAN_DATA_FILE_BUTTON), CCommon::LoadText(IDS_CLEAR_DATA_FILE_TIP_INFO));
    m_toolTip.AddTool(&m_update_media_lib_chk, CCommon::LoadText(IDS_UPDATE_MEDIA_LIB_TIP));

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CMediaLibSettingDlg::OnBnClickedClassifyOtherCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.hide_only_one_classification = (m_classify_other_chk.GetCheck() != 0);
}


//void CMediaLibSettingDlg::OnBnClickedShowTreeToolTipsCheck()
//{
//    // TODO: 在此添加控件通知处理程序代码
//    m_data.show_tree_tool_tips = (m_show_tree_tool_tips_chk.GetCheck() != 0);
//}


void CMediaLibSettingDlg::OnBnClickedAddButton()
{
    // TODO: 在此添加控件通知处理程序代码
    CFolderPickerDialog dlg;
    if (dlg.DoModal() == IDOK)
    {
        CString dir_str = dlg.GetPathName();
        if (!CCommon::IsItemInVector(m_data.media_folders, wstring(dir_str)))
        {
            m_data.media_folders.push_back(wstring(dir_str));
            m_dir_list_ctrl.AddString(dir_str);
        }
    }
}


void CMediaLibSettingDlg::OnBnClickedDeleteButton()
{
    // TODO: 在此添加控件通知处理程序代码
    int index = m_dir_list_ctrl.GetCurSel();
    if(index >= 0 && index < m_data.media_folders.size())
    {
        m_data.media_folders.erase(m_data.media_folders.begin() + index);
        m_dir_list_ctrl.DeleteItem(index);
    }
}


void CMediaLibSettingDlg::OnBnClickedUpdateMediaLibChk()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.update_media_lib_when_start_up = (m_update_media_lib_chk.GetCheck() != 0);
}


void CMediaLibSettingDlg::OnBnClickedCleanDataFileButton()
{
    // TODO: 在此添加控件通知处理程序代码
    CWaitCursor wait_cursor;	//显示等待光标
    int clear_cnt{};		//统计删除的项目的数量
    //遍历映射容器，删除不必要的条目。
    for (auto iter{ theApp.m_song_data.begin() }; iter != theApp.m_song_data.end();)
    {
        ////检查该条目对应的文件所在的路径是否在“最近播放路径”列表里
        //bool path_exist{ false };	//如果iter指向的条目的文件路径在“最近播放路径”列表(CPlayer::GetInstance().GetRecentPath())里，则为true
        //wstring item_path;
        //size_t index = iter->first.rfind(L'\\');
        //item_path = iter->first.substr(0, index + 1);		//获取iter指向项目的文件目录
        //for (size_t i{}; i < CPlayer::GetInstance().GetRecentPath().size(); i++)
        //{
        //	if (item_path == CPlayer::GetInstance().GetRecentPath()[i].path)
        //	{
        //		path_exist = true;
        //		break;
        //	}
        //}
        //如果该条目对应的文件所在的路径不在“最近播放路径”列表里，或该条目对应的文件不存在，则删除该条目
        if (/*!path_exist || */!CCommon::FileExist(iter->first))
        {
            iter = theApp.m_song_data.erase(iter);		//删除条目之后将迭代器指向被删除条目的前一个条目
            clear_cnt++;
        }
        else
        {
            iter++;
        }
    }
    theApp.SaveSongData();		//清理后将数据写入文件

    size_t data_size = CCommon::GetFileSize(theApp.m_song_data_path);	 //清理后数据文件的大小
    int size_reduced = m_data_size - data_size;		//清理后数据文件减少的字节数
    if (size_reduced < 0) size_reduced = 0;
    CString info;
    info = CCommon::LoadTextFormat(IDS_CLEAR_COMPLETE_INFO, { clear_cnt, size_reduced });
    MessageBox(info, NULL, MB_ICONINFORMATION);
    m_data_size = data_size;
    ShowDataSizeInfo();
}


BOOL CMediaLibSettingDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_MOUSEMOVE)
        m_toolTip.RelayEvent(pMsg);

    return CTabDlg::PreTranslateMessage(pMsg);
}
