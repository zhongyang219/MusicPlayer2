
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
				if (!cmd_line.empty())		//如果通过命令行传递了打开的文件名，且已有一个进程在运行，则将打开文件的命令和命令行参数传递给该进程
				{
					CCommon::CopyStringToClipboard(cmd_line);		//将命令行参数复制到剪贴板，通过剪贴板在不同进程间字符串
					::SendMessage(handle, WM_OPEN_FILE_COMMAND_LINE, 0, 0);
				}
				return FALSE;		//退出当前程序
			}
		}
	}
#endif

	//检查bass.dll的版本是否和API的版本匹配
	WORD dll_version{ HIWORD(BASS_GetVersion()) };
	//WORD dll_version{ 0x203 };
	if (dll_version != BASSVERSION)
	{
		CString info;
		info.Format(_T("bass.dll文件版本不匹配，版本为V%d.%d，期望的版本为V%d.%d。不匹配的版本可能会导致播放异常，仍要继续吗？"),
			HIBYTE(dll_version), LOBYTE(dll_version), HIBYTE(BASSVERSION), LOBYTE(BASSVERSION));
		if (AfxMessageBox(info, MB_ICONWARNING | MB_OKCANCEL) == IDCANCEL)
			return FALSE;
	}

	m_module_dir = CCommon::GetExePath();
#ifdef _DEBUG
	m_local_dir = L".\\";
#else
	m_local_dir = m_module_dir;
#endif // _DEBUG

	m_config_path = m_module_dir + L"config.ini";
	m_song_data_path = m_module_dir + L"song_data.dat";
	m_recent_path_dat_path = m_module_dir + L"recent_path.dat";
	m_desktop_path = CCommon::GetDesktopPath();
	//m_temp_path = CCommon::GetTemplatePath() + L"MusicPlayer2\\";
	LoadSongData();
	LoadConfig();

	//启动时检查更新
	if (m_general_setting_data.check_update_when_start)
	{
		AfxBeginThread(CheckUpdateThreadFunc, NULL);
	}

	m_default_cover = CDrawCommon::LoadIconResource(IDI_DEFAULT_COVER, 512, 512);
	m_skin_icon = CDrawCommon::LoadIconResource(IDI_SKIN, DPI(16), DPI(16));

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
			<< song_data.second.genre_idx
			<< song_data.second.track
			<< song_data.second.tag_type
			<< CString(song_data.second.song_id.c_str())
			<< song_data.second.info_acquired
			;
	}
	// 关闭CArchive对象
	ar.Close();
	// 关闭文件
	file.Close();

}

void CMusicPlayerApp::CheckUpdate(bool message)
{
	CWaitCursor wait_cursor;
	wstring version_info;
	if (!CInternetCommon::GetURL(L"https://raw.githubusercontent.com/zhongyang219/MusicPlayer2/master/version.info", version_info))		//获取版本信息
	{
		if (message)
			AfxMessageBox(_T("检查更新失败，请检查你的网络连接！"), MB_OK | MB_ICONWARNING);
		return;
	}

	size_t index, index1, index2, index3, index4, index5;
	index = version_info.find(L"<version>");
	index1 = version_info.find(L"</version>");
	index2 = version_info.find(L"<link>");
	index3 = version_info.find(L"</link>");
	index4 = version_info.find(L"<contents>");
	index5 = version_info.find(L"</contents>");
	wstring version;		//程序版本
	wstring link;			//下载链接
	wstring contents;		//更新内容
	version = version_info.substr(index + 9, index1 - index - 9);
	link = version_info.substr(index2 + 6, index3 - index2 - 6);
	contents = version_info.substr(index4 + 10, index5 - index4 - 10);
	CString contents_str = contents.c_str();
	contents_str.Replace(L"\\n", L"\r\n");
	if (index == wstring::npos || index1 == wstring::npos || index2 == wstring::npos || index3 == wstring::npos || version.empty() || link.empty())
	{
		if (message)
			theApp.m_pMainWnd->MessageBox(_T("检查更新出错，从远程获取到了错误的信息，请联系作者！"), NULL, MB_OK | MB_ICONWARNING);
		return;
	}
	if (version > VERSION)		//如果服务器上的版本大于本地版本
	{
		CString info;
		if (contents.empty())
			info.Format(_T("检测到新版本 V%s，是否前往更新？"), version.c_str());
		else
			info.Format(_T("检测到新版本 V%s，更新内容：\r\n%s\r\n是否前往更新？"), version.c_str(), contents_str);

		if (theApp.m_pMainWnd->MessageBox(info, NULL, MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			ShellExecute(NULL, _T("open"), link.c_str(), NULL, NULL, SW_SHOW);		//转到下载链接
		}
	}
	else
	{
		if (message)
			theApp.m_pMainWnd->MessageBox(_T("当前已经是最新版本。"), NULL, MB_OK | MB_ICONINFORMATION);
	}
}

UINT CMusicPlayerApp::CheckUpdateThreadFunc(LPVOID lpParam)
{
	CheckUpdate(false);		//检查更新
	return 0;
}

void CMusicPlayerApp::SaveConfig()
{
	CIniHelper ini;
	ini.SetPath(m_config_path);
	ini.WriteBool(L"general", L"check_update_when_start", m_general_setting_data.check_update_when_start);
}

void CMusicPlayerApp::LoadConfig()
{
	CIniHelper ini;
	ini.SetPath(m_config_path);
	m_general_setting_data.check_update_when_start = ini.GetBool(L"general", L"check_update_when_start", 1);
}

int CMusicPlayerApp::DPI(int pixel)
{
	return (m_dpi*(pixel) / 96);
}

int CMusicPlayerApp::DPI(double pixel)
{
	return static_cast<int>(m_dpi*(pixel) / 96);
}

int CMusicPlayerApp::DPIRound(double pixel, double round)
{
	double rtn;
	rtn = static_cast<double>(m_dpi)*pixel / 96;
	rtn += round;
	return static_cast<int>(rtn);
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
			ar >> song_info.genre_idx;
			ar >> song_info.track;
			ar >> song_info.tag_type;
			ar >> temp;
			song_info.song_id = temp;
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

