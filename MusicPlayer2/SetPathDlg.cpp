// SetPathDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "SetPathDlg.h"
#include "afxdialogex.h"


// CSetPathDlg 对话框

IMPLEMENT_DYNAMIC(CSetPathDlg, CDialog)

CSetPathDlg::CSetPathDlg(deque<PathInfo>& recent_path, wstring current_path, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_SET_PATH_DIALOG, pParent), m_recent_path(recent_path), m_current_path(current_path)
{

}

CSetPathDlg::~CSetPathDlg()
{
}

void CSetPathDlg::ShowPathList()
{
	m_path_list.DeleteAllItems();
	CString path_str;
	for (int i{}; i < m_recent_path.size(); i++)
	{
		size_t index = m_recent_path[i].path.find_last_of(L'\\', m_recent_path[i].path.size() - 2);	//查找倒数第2个“\”
		wstring folder = m_recent_path[i].path.substr(index + 1);
		folder.pop_back();		//去掉末尾的“\”
		m_path_list.InsertItem(i, folder.c_str());
		m_path_list.SetItemText(i, 1, m_recent_path[i].path.c_str());

		CString str;
		str.Format(_T("%d"), m_recent_path[i].track + 1);
		m_path_list.SetItemText(i, 2, str);

		str.Format(_T("%d"), m_recent_path[i].track_num);
		m_path_list.SetItemText(i, 3, str);

		Time total_time{ m_recent_path[i].total_time };
		m_path_list.SetItemText(i, 4, total_time.time2str3().c_str());

		//path_str.Format(_T("%s (播放到第%d首)"), a_path_info.path.c_str(), a_path_info.track + 1);
		//m_path_list.AddString(path_str);
	}
	////根据列表中最长项目的宽度自动设置列表控件的宽度
	//int list_width;
	//list_width = CCommon::GetListWidth(m_path_list) * 108 / 100;
	//m_path_list.SetHorizontalExtent(list_width);
}

void CSetPathDlg::SetButtonsEnable(bool enable)
{
	GetDlgItem(IDOK)->EnableWindow(enable);
	GetDlgItem(IDC_DELETE_PATH_BUTTON)->EnableWindow(enable);
}

void CSetPathDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PATH_EDIT, m_path_name);
	//DDX_Control(pDX, IDC_LIST1, m_path_list);
	DDX_Control(pDX, IDC_PATH_LIST, m_path_list);
}

wstring CSetPathDlg::GetSelPath() const
{
	if (SelectValid())
		return m_recent_path[m_path_selected].path;
	else
		return wstring();
}

int CSetPathDlg::GetTrack() const
{
	if (SelectValid())
		return m_recent_path[m_path_selected].track;
	else
		return 0;
}

int CSetPathDlg::GetPosition() const
{
	if (SelectValid())
		return m_recent_path[m_path_selected].position;
	else
		return 0;
}

SortMode CSetPathDlg::GetSortMode() const
{
	if (SelectValid())
		return m_recent_path[m_path_selected].sort_mode;
	else
		return SM_FILE;
}

bool CSetPathDlg::SelectValid() const
{
	return (m_path_selected >= 0 && m_path_selected < m_recent_path.size());
}


BEGIN_MESSAGE_MAP(CSetPathDlg, CDialog)
//	ON_LBN_SELCHANGE(IDC_LIST1, &CSetPathDlg::OnLbnSelchangeList1)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_DELETE_PATH_BUTTON, &CSetPathDlg::OnBnClickedDeletePathButton)
	ON_NOTIFY(NM_CLICK, IDC_PATH_LIST, &CSetPathDlg::OnNMClickPathList)
	ON_NOTIFY(NM_RCLICK, IDC_PATH_LIST, &CSetPathDlg::OnNMRClickPathList)
	ON_NOTIFY(NM_DBLCLK, IDC_PATH_LIST, &CSetPathDlg::OnNMDblclkPathList)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CSetPathDlg 消息处理程序


BOOL CSetPathDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_path_name.SetWindowText(m_current_path.c_str());

	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);		// 设置小图标

	//设置列表控件主题颜色
	m_path_list.SetColor(theApp.m_app_setting_data.m_theme_color);

	//初始化播放列表控件
	CRect rect;
	m_path_list.GetClientRect(rect);
	int width0, width1, width2, width3, width4;
	width2 = width3 = rect.Width() / 10;
	width4 = rect.Width() / 7;
	width0 = width1 = (rect.Width() - 2*width2 - width4 - DPI(20)) / 2;

	m_path_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
	m_path_list.InsertColumn(0, _T("文件夹"), LVCFMT_LEFT, width0);		//插入第1列
	m_path_list.InsertColumn(1, _T("路径"), LVCFMT_LEFT, width1);		//插入第2列
	m_path_list.InsertColumn(2, _T("播放到第几首"), LVCFMT_LEFT, width2);		//插入第3列
	m_path_list.InsertColumn(3, _T("曲目数"), LVCFMT_LEFT, width3);		//插入第4列
	m_path_list.InsertColumn(4, _T("总时长"), LVCFMT_LEFT, width4);		//插入第5列

	ShowPathList();
	m_path_list.SetFocus();		//初始时将焦点设置到列表控件

	SetButtonsEnable(false);

	//获取初始时窗口的大小
	GetWindowRect(rect);
	m_min_size.cx = rect.Width();
	m_min_size.cy = rect.Height();

	//设置列表控件的提示总是置顶，用于解决如果弹出此窗口的父窗口具有置顶属性时，提示信息在窗口下面的问题
	m_path_list.GetToolTips()->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

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
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	//m_path_selected = m_path_list.GetCurSel();
}


void CSetPathDlg::OnBnClickedDeletePathButton()
{
	// TODO: 在此添加控件通知处理程序代码
	//m_path_selected = m_path_list.GetCurSel();
	if (SelectValid())
	{
		m_recent_path.erase(m_recent_path.begin() + m_path_selected);	//删除选中的路径
		ShowPathList();		//重新显示路径列表
	}
}


BOOL CSetPathDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (wParam == ID_FILE_OPEN_FOLDER)
	{
		OnCancel();		//点击了“打开新路径”按钮后关闭设置路径对话框
	}

	return CDialog::OnCommand(wParam, lParam);
}


void CSetPathDlg::OnNMClickPathList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_path_selected = pNMItemActivate->iItem;
	SetButtonsEnable(m_path_selected != -1);
	*pResult = 0;
}


void CSetPathDlg::OnNMRClickPathList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_path_selected = pNMItemActivate->iItem;
	SetButtonsEnable(m_path_selected != -1);
	*pResult = 0;
}


void CSetPathDlg::OnNMDblclkPathList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_path_selected = pNMItemActivate->iItem;
	SetButtonsEnable(m_path_selected != -1);
	OnOK();
	*pResult = 0;
}


void CSetPathDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//限制窗口最小大小
	lpMMI->ptMinTrackSize.x = m_min_size.cx;		//设置最小宽度
	lpMMI->ptMinTrackSize.y = m_min_size.cy;		//设置最小高度

	CDialog::OnGetMinMaxInfo(lpMMI);
}


void CSetPathDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (nType != SIZE_MINIMIZED && m_path_list.m_hWnd)
	{
		int list_width{ cx - DPI(29) };
		int width0, width1, width2, width3, width4;
		width2 = width3 = list_width / 10;
		width4 = list_width / 7;
		width0 = width1 = (list_width - 2 * width2 - width4 - DPI(20)) / 2;
		m_path_list.SetColumnWidth(0, width0);
		m_path_list.SetColumnWidth(1, width1);
		m_path_list.SetColumnWidth(2, width2);
		m_path_list.SetColumnWidth(3, width3);
		m_path_list.SetColumnWidth(4, width4);

	}
}


void CSetPathDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	DestroyWindow();

	//CDialog::OnCancel();
}


void CSetPathDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	if (SelectValid())
		::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(),WM_PATH_SELECTED, 0, 0);
	CDialog::OnOK();
}
