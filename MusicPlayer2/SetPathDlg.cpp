// SetPathDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "SetPathDlg.h"
#include "afxdialogex.h"
#include "MusicPlayerCmdHelper.h"


// CSetPathDlg 对话框

IMPLEMENT_DYNAMIC(CSetPathDlg, CTabDlg)

CSetPathDlg::CSetPathDlg(CWnd* pParent /*=NULL*/)
	: CTabDlg(IDD_SET_PATH_DIALOG, pParent), m_recent_path(CPlayer::GetInstance().GetRecentPath())
{

}

CSetPathDlg::~CSetPathDlg()
{
}

void CSetPathDlg::QuickSearch(const wstring & key_word)
{
	m_search_result.clear();
	for (size_t i{}; i < m_recent_path.size(); i++)
	{
		if (CCommon::StringFindNoCase(m_recent_path[i].path, key_word) != wstring::npos)
			m_search_result.push_back(i);
	}
}

void CSetPathDlg::AdjustColumnWidth()
{
    vector<int> width;
    CalculateColumeWidth(width);
    for (size_t i{}; i < width.size(); i++)
        m_path_list.SetColumnWidth(i, width[i]);
}

void CSetPathDlg::RefreshTabData()
{
    ShowPathList();
    SetButtonsEnable(IsSelectedPlayEnable());
}

void CSetPathDlg::ShowPathList()
{
	m_path_list.EnableWindow(TRUE);
	if (!m_searched)		//显示所有项目
	{
		m_path_list.DeleteAllItems();
		CString path_str;
		for (size_t i{}; i < m_recent_path.size(); i++)
		{
			CString str;
			str.Format(_T("%d"), i + 1);
			m_path_list.InsertItem(i, str);

			SetListRowData(i, m_recent_path[i]);
		}
	}
	else		//只显示搜索结果的曲目
	{
		if (m_search_result.empty())
		{
			m_path_list.DeleteAllItems();
			m_path_list.InsertItem(0, _T(""));
			m_path_list.SetItemText(0, 1, CCommon::LoadText(IDS_NO_RESULT_TO_SHOW));
			m_path_list.EnableWindow(FALSE);
			return;
		}

		int item_num_before = m_path_list.GetItemCount();
		int item_num_after = m_search_result.size();
		//如果当前列表中项目的数量小于原来的，则直接清空原来列表中所有的项目，重新添加
		if (item_num_after < item_num_before)
		{
			m_path_list.DeleteAllItems();
			item_num_before = 0;
		}
		CString str;
		for (int i{}; i < item_num_after; i++)
		{
			str.Format(_T("%u"), m_search_result[i] + 1);
			if (i >= item_num_before)	//如果当前列表中的项目数量大于之前的数量，则需要在不够时插入新的项目
			{
				m_path_list.InsertItem(i, str);
			}
			m_path_list.SetItemText(i, 0, str);
			SetListRowData(i, m_recent_path[m_search_result[i]]);
		}
	}
}

void CSetPathDlg::SetButtonsEnable(bool enable)
{
	//GetDlgItem(IDOK)->EnableWindow(enable);
	//GetDlgItem(IDC_DELETE_PATH_BUTTON)->EnableWindow(enable);
    CWnd* pParent = GetParentWindow();
    if (pParent != nullptr)
        ::SendMessage(pParent->GetSafeHwnd(), WM_PLAY_SELECTED_BTN_ENABLE, WPARAM(enable), 0);
}

void CSetPathDlg::CalculateColumeWidth(vector<int>& width)
{
	CRect rect;
	m_path_list.GetWindowRect(rect);
	width.resize(7);

    width[3] = rect.Width() / 12;
	width[4] = width[5] = rect.Width() / 10;
	width[6] = rect.Width() / 8;

	width[0] = theApp.DPI(40);
	width[1] = rect.Width() / 4;
	width[2] = rect.Width() - width[1] - width[3] - width[4] - width[5] - width[6] - width[0] - theApp.DPI(20) - 1;
}

void CSetPathDlg::SetListRowData(int index, const PathInfo & path_info)
{
	CFilePathHelper path_helper(path_info.path);
	m_path_list.SetItemText(index, 1, path_helper.GetFolderName().c_str());
	m_path_list.SetItemText(index, 2, path_info.path.c_str());

    if (path_info.contain_sub_folder)
        m_path_list.SetItemText(index, 3, CCommon::LoadText(IDS_YES));
    else
        m_path_list.SetItemText(index, 3, _T(""));

	CString str;
	str.Format(_T("%d"), path_info.track + 1);
	m_path_list.SetItemText(index, 4, str);

	str.Format(_T("%d"), path_info.track_num);
	m_path_list.SetItemText(index, 5, str);

	Time total_time{ path_info.total_time };
	m_path_list.SetItemText(index, 6, total_time.toString3().c_str());
}

bool CSetPathDlg::IsSelectedPlayEnable() const
{
	return !m_recent_path.empty() && SelectValid() && (GetSelPath().path != CPlayer::GetInstance().GetCurrentDir2() || CPlayer::GetInstance().IsPlaylistMode());
}

void CSetPathDlg::OnTabEntered()
{
    m_path_selected = m_path_list.GetCurSel();
    SetButtonsEnable(IsSelectedPlayEnable());
}

void CSetPathDlg::DoDataExchange(CDataExchange* pDX)
{
	CTabDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PATH_EDIT, m_path_name);
	//DDX_Control(pDX, IDC_LIST1, m_path_list);
	DDX_Control(pDX, IDC_PATH_LIST, m_path_list);
	DDX_Control(pDX, IDC_SEARCH_EDIT, m_search_edit);
}

PathInfo CSetPathDlg::GetSelPath() const
{
	if (SelectValid())
		return m_recent_path[m_path_selected];
	else
		return PathInfo();
}

bool CSetPathDlg::SelectValid() const
{
	return (m_path_selected >= 0 && m_path_selected < static_cast<int>(m_recent_path.size()));
}


BEGIN_MESSAGE_MAP(CSetPathDlg, CTabDlg)
//	ON_LBN_SELCHANGE(IDC_LIST1, &CSetPathDlg::OnLbnSelchangeList1)
	ON_WM_DESTROY()
	//ON_BN_CLICKED(IDC_DELETE_PATH_BUTTON, &CSetPathDlg::OnBnClickedDeletePathButton)
	ON_NOTIFY(NM_CLICK, IDC_PATH_LIST, &CSetPathDlg::OnNMClickPathList)
	ON_NOTIFY(NM_RCLICK, IDC_PATH_LIST, &CSetPathDlg::OnNMRClickPathList)
	ON_NOTIFY(NM_DBLCLK, IDC_PATH_LIST, &CSetPathDlg::OnNMDblclkPathList)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_OPEN_FOLDER, &CSetPathDlg::OnBnClickedOpenFolder)
	ON_COMMAND(ID_PLAY_PATH, &CSetPathDlg::OnPlayPath)
	ON_COMMAND(ID_DELETE_PATH, &CSetPathDlg::OnDeletePath)
	ON_COMMAND(ID_BROWSE_PATH, &CSetPathDlg::OnBrowsePath)
	ON_COMMAND(ID_CLEAR_INVALID_PATH, &CSetPathDlg::OnClearInvalidPath)
	ON_WM_INITMENU()
	ON_EN_CHANGE(IDC_SEARCH_EDIT, &CSetPathDlg::OnEnChangeSearchEdit)
	//ON_BN_CLICKED(IDC_CLEAR_BUTTON, &CSetPathDlg::OnBnClickedClearButton)
    ON_MESSAGE(WM_SEARCH_EDIT_BTN_CLICKED, &CSetPathDlg::OnSearchEditBtnClicked)
    ON_COMMAND(ID_CONTAIN_SUB_FOLDER, &CSetPathDlg::OnContainSubFolder)
END_MESSAGE_MAP()


// CSetPathDlg 消息处理程序


BOOL CSetPathDlg::OnInitDialog()
{
	CTabDlg::OnInitDialog();

	// TODO:  在此添加额外的初始化

    if(!CPlayer::GetInstance().IsPlaylistMode())
        m_path_name.SetWindowText(CPlayer::GetInstance().GetCurrentDir2().c_str());

    if (!m_recent_path.empty())
        m_current_folder_contain_sub_folder = m_recent_path[0].contain_sub_folder;

	//设置列表控件主题颜色
	//m_path_list.SetColor(theApp.m_app_setting_data.theme_color);

	//初始化播放列表控件
	vector<int> width;
	CalculateColumeWidth(width);
    m_path_list.SetExtendedStyle(m_path_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
	m_path_list.InsertColumn(0, CCommon::LoadText(IDS_NUMBER), LVCFMT_LEFT, width[0]);
	m_path_list.InsertColumn(1, CCommon::LoadText(IDS_FOLDER), LVCFMT_LEFT, width[1]);
	m_path_list.InsertColumn(2, CCommon::LoadText(IDS_PATH), LVCFMT_LEFT, width[2]);
	m_path_list.InsertColumn(3, CCommon::LoadText(IDS_CONTAIN_SUB_FOLDER), LVCFMT_LEFT, width[3]);
	m_path_list.InsertColumn(4, CCommon::LoadText(IDS_TRACK_PLAYED), LVCFMT_LEFT, width[4]);
	m_path_list.InsertColumn(5, CCommon::LoadText(IDS_TRACK_TOTAL_NUM), LVCFMT_LEFT, width[5]);
	m_path_list.InsertColumn(6, CCommon::LoadText(IDS_TOTAL_LENGTH), LVCFMT_LEFT, width[6]);

	ShowPathList();
	m_search_edit.SetFocus();		//初始时将焦点设置到搜索框

	m_search_edit.SetCueBanner(CCommon::LoadText(IDS_SEARCH_HERE), TRUE);

	//获取初始时窗口的大小
	CRect rect;
	GetWindowRect(rect);
	m_min_size.cx = rect.Width();
	m_min_size.cy = rect.Height();

	////初始化提示信息
	//m_Mytip.Create(this, TTS_ALWAYSTIP);
	//m_Mytip.AddTool(GetDlgItem(IDC_CLEAR_BUTTON), CCommon::LoadText(IDS_CLEAR_SEARCH_RESULT));
	//m_Mytip.AddTool(&m_search_edit, CCommon::LoadText(IDS_INPUT_KEY_WORD));

	////设置列表控件的提示总是置顶，用于解决如果弹出此窗口的父窗口具有置顶属性时，提示信息在窗口下面的问题
	//m_path_list.GetToolTips()->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	//m_Mytip.SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	return FALSE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


//void CSetPathDlg::OnLbnSelchangeList1()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	m_path_selected = m_path_list.GetCurSel();
//}


void CSetPathDlg::OnDestroy()
{
	CTabDlg::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	//m_path_selected = m_path_list.GetCurSel();
    if (m_folder_selected)
    {
        PathInfo path_info = GetSelPath();
        ::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_PATH_SELECTED, (WPARAM)&path_info, 0);
        m_folder_selected = false;
    }
    else if (!CPlayer::GetInstance().IsPlaylistMode())
    {
        //如果正在播放的文件夹是否包含子文件夹的状态被改变了
        if (!m_recent_path.empty() && m_current_folder_contain_sub_folder != m_recent_path[0].contain_sub_folder)
        {
            CPlayer::GetInstance().SetContainSubFolder(m_recent_path[0].contain_sub_folder);
        }
    }
}


//void CSetPathDlg::OnBnClickedDeletePathButton()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	//m_path_selected = m_path_list.GetCurSel();
//	if (SelectValid())
//	{
//		m_recent_path.erase(m_recent_path.begin() + m_path_selected);	//删除选中的路径
//		ShowPathList();		//重新显示路径列表
//	}
//}


//BOOL CSetPathDlg::OnCommand(WPARAM wParam, LPARAM lParam)
//{
//	// TODO: 在此添加专用代码和/或调用基类
//	if (wParam == ID_FILE_OPEN_FOLDER)
//	{
//		OnCancel();		//点击了“打开新路径”按钮后关闭设置路径对话框
//	}
//
//	return CTabDlg::OnCommand(wParam, lParam);
//}


void CSetPathDlg::OnNMClickPathList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (!m_searched)	//如果播放列表不在搜索状态，则当前选中项的行号就是曲目的索引
	{
		m_path_selected = pNMItemActivate->iItem;
	}
	else
	{
		CString str;
		str = m_path_list.GetItemText(pNMItemActivate->iItem, 0);
		m_path_selected = _ttoi(str) - 1;
	}

	bool enable = IsSelectedPlayEnable();
	SetButtonsEnable(enable);
	*pResult = 0;
}


void CSetPathDlg::OnNMRClickPathList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_path_selected = pNMItemActivate->iItem;
    bool enable = IsSelectedPlayEnable();
    SetButtonsEnable(enable);

	//弹出右键菜单
	CMenu* pContextMenu = theApp.m_menu_set.m_media_lib_folder_menu.GetSubMenu(0);	//获取第一个弹出菜单
    m_path_list.ShowPopupMenu(pContextMenu, pNMItemActivate->iItem, this);

	*pResult = 0;
}


void CSetPathDlg::OnNMDblclkPathList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (!m_searched)	//如果播放列表不在搜索状态，则当前选中项的行号就是曲目的索引
	{
		m_path_selected = pNMItemActivate->iItem;
	}
	else
	{
		CString str;
		str = m_path_list.GetItemText(pNMItemActivate->iItem, 0);
		m_path_selected = _ttoi(str) - 1;
	}

	bool enable = IsSelectedPlayEnable();
	SetButtonsEnable(enable);
	if(enable)
		OnOK();
	*pResult = 0;
}


void CSetPathDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//限制窗口最小大小
	lpMMI->ptMinTrackSize.x = m_min_size.cx;		//设置最小宽度
	lpMMI->ptMinTrackSize.y = m_min_size.cy;		//设置最小高度

	CTabDlg::OnGetMinMaxInfo(lpMMI);
}


void CSetPathDlg::OnSize(UINT nType, int cx, int cy)
{
	CTabDlg::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
}


//void CSetPathDlg::OnCancel()
//{
//	// TODO: 在此添加专用代码和/或调用基类
//	DestroyWindow();
//
//	//CTabDlg::OnCancel();
//}


void CSetPathDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	if (SelectValid())
    {
        //PathInfo path_info = GetSelPath();
        //::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_PATH_SELECTED, (WPARAM)&path_info, 0);
        m_folder_selected = true;
    }
	CTabDlg::OnOK();

    CWnd* pParent = GetParentWindow();
    if (pParent != nullptr)
    {
        ::SendMessage(pParent->GetSafeHwnd(), WM_COMMAND, IDOK, 0);
    }
}


void CSetPathDlg::OnBnClickedOpenFolder()
{
	// TODO: 在此添加控件通知处理程序代码
	::PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_COMMAND, ID_FILE_OPEN_FOLDER, 0);
	OnCancel();			//点击了“打开新路径”按钮后关闭设置路径对话框
    CWnd* pParent = GetParent();
    if (pParent != nullptr)
    {
        pParent = pParent->GetParent();
        if (pParent != nullptr)
            ::SendMessage(pParent->GetSafeHwnd(), WM_COMMAND, IDOK, 0);
    }

}


void CSetPathDlg::OnPlayPath()
{
	// TODO: 在此添加命令处理程序代码
	OnOK();
}


void CSetPathDlg::OnDeletePath()
{
	// TODO: 在此添加命令处理程序代码
	if (SelectValid())
	{
		m_recent_path.erase(m_recent_path.begin() + m_path_selected);	//删除选中的路径
		ShowPathList();		//重新显示路径列表
	}
}


void CSetPathDlg::OnBrowsePath()
{
	// TODO: 在此添加命令处理程序代码
	if (SelectValid())
		ShellExecute(NULL, _T("open"), _T("explorer"), m_recent_path[m_path_selected].path.c_str(), NULL, SW_SHOWNORMAL);
}


void CSetPathDlg::OnClearInvalidPath()
{
	// TODO: 在此添加命令处理程序代码
	if (MessageBox(CCommon::LoadText(IDS_CLEAR_PATH_INQUARY), NULL, MB_ICONQUESTION | MB_OKCANCEL) == IDCANCEL)
		return;
    int cleard_cnt = CMusicPlayerCmdHelper::CleanUpRecentFolders();
    ShowPathList();		//重新显示路径列表
	CString info;
	info = CCommon::LoadTextFormat(IDS_PATH_CLEAR_COMPLETE, { cleard_cnt });
	MessageBox(info, NULL, MB_ICONINFORMATION | MB_OK);
}


void CSetPathDlg::OnInitMenu(CMenu* pMenu)
{
	CTabDlg::OnInitMenu(pMenu);

	// TODO: 在此处添加消息处理程序代码
	bool select_valid{ SelectValid() };
	pMenu->EnableMenuItem(ID_PLAY_PATH, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
	pMenu->EnableMenuItem(ID_DELETE_PATH, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
	pMenu->EnableMenuItem(ID_BROWSE_PATH, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_CONTAIN_SUB_FOLDER, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    bool contain_sub_folder{false};
    if (select_valid)
        contain_sub_folder = m_recent_path[m_path_selected].contain_sub_folder;
    pMenu->CheckMenuItem(ID_CONTAIN_SUB_FOLDER, MF_BYCOMMAND | (contain_sub_folder ? MF_CHECKED : MF_UNCHECKED));
}


void CSetPathDlg::OnEnChangeSearchEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CTabDlg::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString key_word;
	m_search_edit.GetWindowText(key_word);
	m_searched = (key_word.GetLength() != 0);
	QuickSearch(wstring(key_word));
	ShowPathList();

}


//void CSetPathDlg::OnBnClickedClearButton()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	if (m_searched)
//	{
//		//清除搜索结果
//		m_searched = false;
//		m_search_edit.SetWindowText(_T(""));
//		ShowPathList();
//	}
//}


BOOL CSetPathDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd != m_search_edit.GetSafeHwnd())
	{
		if (pMsg->wParam == 'F')	//按F键快速查找
		{
			m_search_edit.SetFocus();
			return TRUE;
		}
	}

	//if (pMsg->message == WM_MOUSEMOVE)
	//	m_Mytip.RelayEvent(pMsg);

	return CTabDlg::PreTranslateMessage(pMsg);
}


afx_msg LRESULT CSetPathDlg::OnSearchEditBtnClicked(WPARAM wParam, LPARAM lParam)
{
    if (m_searched)
    {
        //清除搜索结果
        m_searched = false;
        m_search_edit.SetWindowText(_T(""));
        ShowPathList();
    }
    return 0;
}


void CSetPathDlg::OnContainSubFolder()
{
    // TODO: 在此添加命令处理程序代码
    if (SelectValid())
    {
        m_recent_path[m_path_selected].contain_sub_folder = !m_recent_path[m_path_selected].contain_sub_folder;
        SetListRowData(m_path_selected, m_recent_path[m_path_selected]);       //刷新当前行
        //if (m_path_selected == 0)       //如果更改了正在播放的文件夹
        //{
        //    PathInfo path_info = m_recent_path[0];
        //    ::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_PATH_SELECTED, (WPARAM)&path_info, 0);
        //}
    }

}
