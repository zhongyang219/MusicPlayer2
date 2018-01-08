
// MusicPlayer2.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "MusicPlayerDlg.h"
#include "HelpDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMusicPlayerApp

BEGIN_MESSAGE_MAP(CMusicPlayerApp, CWinApp)
	//ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
	ON_COMMAND(ID_HELP, &CMusicPlayerApp::OnHelp)
END_MESSAGE_MAP()


// CMusicPlayerApp 构造

CMusicPlayerApp::CMusicPlayerApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CMusicPlayerApp 对象

CMusicPlayerApp theApp;


// CMusicPlayerApp 初始化

BOOL CMusicPlayerApp::InitInstance()
{
	//替换掉对话框程序的默认类名
	WNDCLASS wc;
	::GetClassInfo(AfxGetInstanceHandle(), _T("#32770"), &wc);	//MFC默认的所有对话框的类名为#32770
	wc.lpszClassName = _T("MusicPlayer_l3gwYT");	//将对话框的类名修改为新类名
	AfxRegisterClass(&wc);

	//当程序被Windows重新启动时，直接退出程序
	wstring cmd_line{ m_lpCmdLine };
	if (cmd_line.find(L"RestartByRestartManager") != wstring::npos)
	{
		//将命令行参数写入日志文件
		wchar_t buff[256];
		swprintf_s(buff, L"程序已被Windows的RestartManager重启，重启参数：%s，程序已退出。", cmd_line.c_str());
		CCommon::WriteLog((CCommon::GetExePath() + L"error.log").c_str(), wstring{ buff });
		return FALSE;
	}

	//检查是否已有实例正在运行（Debug时不检查）
#ifndef _DEBUG
	HANDLE hMutex = ::CreateMutex(NULL, TRUE, _T("bXS1E7joK0Kh"));		//使用一个随机的字符串创建一个互斥量
	if (hMutex != NULL)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)		//互斥量创建失败，说明已经有一个程序的实例正在运行
		{
			//AfxMessageBox(_T("已经有一个程序正在运行。"));
			HWND handle = FindWindow(_T("MusicPlayer_l3gwYT"), NULL);		//根据类名查找已运行实例窗口的句柄
			if (handle != NULL)
			{
				HWND minidlg_handle = FindWindow(NULL, _T("MiniDlg_ByH87M"));	//根据窗口标题查找“迷你模式”窗口的句柄
				if (minidlg_handle == NULL)			//没有找到“迷你模式”窗口，则激活主窗口
				{
					ShowWindow(handle, SW_SHOWNORMAL);		//激活并显示窗口
					SetForegroundWindow(handle);		//将窗口设置为焦点
				}
				else				//找到了“迷你模式”窗口，则激活“迷你模式”窗口
				{
					ShowWindow(minidlg_handle, SW_SHOWNORMAL);
					SetForegroundWindow(minidlg_handle);
				}
				return FALSE;		//退出当前程序
			}
		}
	}
#endif

	m_config_path = CCommon::GetExePath() + L"config.ini";
	m_song_data_path = CCommon::GetExePath() + L"song_data.dat";
	m_recent_path_dat_path = CCommon::GetExePath() + L"recent_path.dat";
	m_desktop_path = CCommon::GetDesktopPath();
	LoadSongData();

	//判断当前操作系统是否是Windows10。结果储存在CMusicPlayerApp类中的成员变量中，以便在全局范围内可访问
	m_is_windows10 = CCommon::IsWindows10OrLater();

	CColorConvert::Initialize();

	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CMusicPlayerDlg dlg(cmd_line);
	//CMusicPlayerDlg dlg(L"D:\\音乐2\\Test\\1.wma \"D:\\音乐2\\Test\\Sweety - 樱花草.mp3\"");
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	SaveSongData();

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}



void CMusicPlayerApp::OnHelp()
{
	// TODO: 在此添加命令处理程序代码
	//AfxMessageBox(_T("打开了帮助"), NULL, MB_ICONINFORMATION);
	CHelpDlg helpDlg;
	helpDlg.DoModal();
}

void CMusicPlayerApp::SaveSongData() const
{
	// 打开或者新建文件
	CFile file;
	BOOL bRet = file.Open(m_song_data_path.c_str(),
		CFile::modeCreate | CFile::modeWrite);
	if (!bRet)		//打开文件失败
	{
		return;
	}
	// 构造CArchive对象
	CArchive ar(&file, CArchive::store);
	// 写数据
	ar << m_song_data.size();		//写入映射容器的大小
	for (auto& song_data : m_song_data)
	{
		ar << CString(song_data.first.c_str())		//保存映射容器的键，即歌曲的绝对路径
			//<< CString(song_data.second.lyric_file.c_str())
			<< song_data.second.lengh.time2int()
			<< song_data.second.bitrate
			<< CString(song_data.second.title.c_str())
			<< CString(song_data.second.artist.c_str())
			<< CString(song_data.second.album.c_str())
			<< CString(song_data.second.year.c_str())
			<< CString(song_data.second.comment.c_str())
			<< CString(song_data.second.genre.c_str())
			<< song_data.second.track
			<< song_data.second.tag_type
			<< song_data.second.info_acquired;
	}
	// 关闭CArchive对象
	ar.Close();
	// 关闭文件
	file.Close();

}

void CMusicPlayerApp::LoadSongData()
{
	// 打开文件
	CFile file;
	BOOL bRet = file.Open(m_song_data_path.c_str(), CFile::modeRead);
	if (!bRet) return;
	// 构造CArchive对象
	CArchive ar(&file, CArchive::load);
	// 读数据
	size_t size{};
	SongInfo song_info;
	CString song_path;
	CString temp;
	int song_length;
	try
	{
		ar >> size;		//读取映射容器的长度
		for (size_t i{}; i < size; i++)
		{
			ar >> song_path;
			//ar >> temp;
			//song_info.lyric_file = temp;
			ar >> song_length;
			song_info.lengh.int2time(song_length);
			ar >> song_info.bitrate;
			ar >> temp;
			song_info.title = temp;
			ar >> temp;
			song_info.artist = temp;
			ar >> temp;
			song_info.album = temp;
			ar >> temp;
			song_info.year = temp;
			ar >> temp;
			song_info.comment = temp;
			ar >> temp;
			song_info.genre = temp;
			ar >> song_info.track;
			ar >> song_info.tag_type;
			m_song_data[wstring{ song_path }] = song_info;		//将读取到的一首歌曲信息添加到映射容器中
			ar >> song_info.info_acquired;
		}
	}
	catch(CArchiveException* exception)
	{
		CString info;
		info.Format(_T("读取数据文件时发生了序列化异常，异常类型：CArchiveException，m_cause = %d。可能是song_data.dat文件被损坏或版本不匹配造成的。"), exception->m_cause);
//		info.Format(_T("警告：读取数据文件时发生了序列化异常，异常类型：CArchiveException，m_cause = %d。\
//可能是song_data.dat文件被损坏或版本不匹配造成的，你可以忽略这个信息，因为song_data.dat会重新生成。"), exception->m_cause);
//		AfxMessageBox(info, MB_ICONWARNING);
		CCommon::WriteLog((CCommon::GetExePath() + L"error.log").c_str(), wstring{ info });
	}
	// 关闭对象
	ar.Close();
	// 关闭文件
	file.Close();
}

