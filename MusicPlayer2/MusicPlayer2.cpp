
// MusicPlayer2.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "MusicPlayerDlg.h"
#include "MessageDlg.h"
#include "crashtool.h"
#include <Gdiplus.h>
#include "UpdateHelper.h"
#include "MusicPlayerCmdHelper.h"

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
    //m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;		//不需要支持重新启动管理器

    // TODO: 在此处添加构造代码，
    // 将所有重要的初始化放置在 InitInstance 中

    CRASHREPORT::StartCrashReport();

    //初始化路径
    m_module_dir = CCommon::GetExePath();
#ifdef _DEBUG
    m_local_dir = L".\\";
#else
    m_local_dir = m_module_dir;
#endif // _DEBUG

    m_config_path = m_module_dir + L"config.ini";
    m_song_data_path = m_module_dir + L"song_data.dat";
    m_recent_path_dat_path = m_module_dir + L"recent_path.dat";
    m_recent_playlist_data_path = m_module_dir + L"playlist\\recent_playlist.dat";
    m_desktop_path = CCommon::GetDesktopPath();
    //m_temp_path = CCommon::GetTemplatePath() + L"MusicPlayer2\\";
    m_playlist_dir = m_module_dir + L"playlist\\";
    CCommon::CreateDir(m_playlist_dir);

    //获取当前DPI
    HDC hDC = ::GetDC(NULL);
    m_dpi = GetDeviceCaps(hDC, LOGPIXELSY);
    ::ReleaseDC(NULL, hDC);
    if (m_dpi == 0)
    {
        WriteErrorLog(L"Get system DPI failed!");
        m_dpi = 96;
    }
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

    wstring cmd_line{ m_lpCmdLine };
    //当程序被Windows重新启动时，直接退出程序
    if (cmd_line.find(L"RestartByRestartManager") != wstring::npos)
    {
        //将命令行参数写入日志文件
        CString info = CCommon::LoadTextFormat(IDS_RESTART_EXIT, { cmd_line });
        //swprintf_s(buff, CCommon::LoadText(IDS_RESTART_EXIT), cmd_line.c_str());
        WriteErrorLog(wstring{ info.GetString() });
        return FALSE;
    }

    ControlCmd cmd;
    bool cmd_control = CCommon::GetCmdLineCommand(cmd_line, cmd);		//命令行参数是否包含参数命令
    if (cmd_control)		//如果从命令行参数解析到了命令，则将命令行参数清除
        cmd_line.clear();

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
                HWND minidlg_handle = FindWindow(_T("MiniDlg_ByH87M"), NULL);
                if (!cmd_control)
                {
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
                }

                if (cmd_control)
                {
                    switch (cmd)
                    {
                    case ControlCmd::PLAY_PAUSE:
                        ::SendMessage(handle, WM_COMMAND, ID_PLAY_PAUSE, 0);
                        break;
                    case ControlCmd::_PREVIOUS:
                        ::SendMessage(handle, WM_COMMAND, ID_PREVIOUS, 0);
                        break;
                    case ControlCmd::_NEXT:
                        ::SendMessage(handle, WM_COMMAND, ID_NEXT, 0);
                        break;
                    case ControlCmd::STOP:
                        ::SendMessage(handle, WM_COMMAND, ID_STOP, 0);
                        break;
                    case ControlCmd::FF:
                        ::SendMessage(handle, WM_COMMAND, ID_FF, 0);
                        break;
                    case ControlCmd::REW:
                        ::SendMessage(handle, WM_COMMAND, ID_REW, 0);
                        break;
                    case ControlCmd::VOLUME_UP:
                        ::SendMessage(handle, WM_COMMAND, ID_VOLUME_UP, 0);
                        break;
                    case ControlCmd::VOLUME_DOWM:
                        ::SendMessage(handle, WM_COMMAND, ID_VOLUME_DOWN, 0);
                        break;

                    default:
                        break;
                    }
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

    //CString str = CCommon::LoadTextFormat(IDS_TEST_STR, { 3, L"asdfghhh", 1.2 });

    LoadSongData();
    LoadConfig();

    //初始化界面语言
    CCommon::SetThreadLanguage(m_general_setting_data.language);

    //检查bass.dll的版本是否和API的版本匹配
    WORD dll_version{ HIWORD(BASS_GetVersion()) };
    //WORD dll_version{ 0x203 };
    if (dll_version != BASSVERSION)
    {
        CString info = CCommon::LoadTextFormat(IDS_BASS_VERSION_WARNING, { HIBYTE(dll_version), LOBYTE(dll_version), HIBYTE(BASSVERSION), LOBYTE(BASSVERSION) });
        if (AfxMessageBox(info, MB_ICONWARNING | MB_OKCANCEL) == IDCANCEL)
            return FALSE;
    }

    //启动时检查更新
#ifndef _DEBUG		//DEBUG下不在启动时检查更新
    if (m_general_setting_data.check_update_when_start)
    {
        AfxBeginThread(CheckUpdateThreadFunc, NULL);
    }
#endif // !_DEBUG

    //启动后台线程将歌曲数据分类
    //StartClassifySongData();

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
    //SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

    //设置一个全局钩子以截获多媒体按键消息
    if(m_hot_key_setting_data.global_multimedia_key_enable)
        m_multimedia_key_hook = SetWindowsHookEx(WH_KEYBOARD_LL, CMusicPlayerApp::MultiMediaKeyHookProc, m_hInstance, 0);

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

    CMusicPlayerDlg dlg(cmd_line);
    //CMusicPlayerDlg dlg(L"\"D:\\音乐\\纯音乐\\班得瑞\\05. Chariots Of Fire 火战车.mp3\"");
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


    if (m_multimedia_key_hook != NULL)
    {
        UnhookWindowsHookEx(m_multimedia_key_hook);
        m_multimedia_key_hook = NULL;
    }

    // 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
    //  而不是启动应用程序的消息泵。
    return FALSE;
}



void CMusicPlayerApp::OnHelp()
{
    // TODO: 在此添加命令处理程序代码
    static bool dialog_exist{ false };

    if(!dialog_exist)		//确保对话框已经存在时不再弹出
    {
        dialog_exist = true;
        CMessageDlg helpDlg;
        helpDlg.SetWindowTitle(CCommon::LoadText(IDS_HELP));
        helpDlg.SetInfoText(CCommon::LoadText(IDS_WELCOM_TO_USE));

        CString info{ GetHelpString() };
        info += _T("\r\n\r\n");
        info += GetSystemInfoString();

        helpDlg.SetMessageText(info);
        helpDlg.DoModal();

        dialog_exist = false;
    }
}

void CMusicPlayerApp::SaveSongData()
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
    ar << CString(_T("2.664"));			//写入数据版本
    ar << static_cast<int>(m_song_data.size());		//写入映射容器的大小
    for (auto& song_data : m_song_data)
    {
        ar << CString(song_data.first.c_str())		//保存映射容器的键，即歌曲的绝对路径
           //<< CString(song_data.second.lyric_file.c_str())
           << song_data.second.lengh.toInt()
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
           << song_data.second.listen_time
           << song_data.second.info_acquired
           //<< song_data.second.is_favourite
           << song_data.second.no_online_album_cover
           << song_data.second.no_online_lyric
           ;
    }
    // 关闭CArchive对象
    ar.Close();
    // 关闭文件
    file.Close();

    m_song_data_modified = false;
}

void CMusicPlayerApp::CheckUpdate(bool message)
{
    CWaitCursor wait_cursor;

    wstring version;		//程序版本
    wstring link;			//下载链接
    wstring contents_zh_cn;	//更新内容（简体中文）
    wstring contents_en;	//更新内容（English）
    CUpdateHelper update_helper;
    if (!update_helper.CheckForUpdate())
    {
        if (message)
            AfxMessageBox(CCommon::LoadText(IDS_CHECK_UPDATA_FAILED), MB_OK | MB_ICONWARNING);
        return;
    }

    version = update_helper.GetVersion();
#ifdef _M_X64
    link = update_helper.GetLink64();
#else
    link = update_helper.GetLink();
#endif
    contents_zh_cn = update_helper.GetContentsZhCn();
    contents_en = update_helper.GetContentsEn();

    if (version.empty() || link.empty())
    {
        if (message)
        {
            CString info = CCommon::LoadText(IDS_CHECK_UPDATA_ERROR);
            info += _T("\r\nrow_data=");
            info += std::to_wstring(update_helper.IsRowData()).c_str();
            theApp.m_pMainWnd->MessageBox(info, NULL, MB_OK | MB_ICONWARNING);
        }
        return;
    }
    if (version > VERSION)		//如果服务器上的版本大于本地版本
    {
        CString info;
        //根据语言设置选择对应语言版本的更新内容
        int language_code = _ttoi(CCommon::LoadText(IDS_LANGUAGE_CODE));
        wstring contents_lan;
        switch (language_code)
        {
        case 2:
            contents_lan = contents_zh_cn;
            break;
        default:
            contents_lan = contents_en;
            break;
        }

        if (contents_lan.empty())
            info = CCommon::LoadTextFormat(IDS_UPDATE_AVLIABLE, { version });
        else
            info = CCommon::LoadTextFormat(IDS_UPDATE_AVLIABLE2, { version, contents_lan });

        if (theApp.m_pMainWnd->MessageBox(info, NULL, MB_YESNO | MB_ICONQUESTION) == IDYES)
        {
            ShellExecute(NULL, _T("open"), link.c_str(), NULL, NULL, SW_SHOW);		//转到下载链接
        }
    }
    else
    {
        if (message)
            theApp.m_pMainWnd->MessageBox(CCommon::LoadText(IDS_ALREADY_UPDATED), NULL, MB_OK | MB_ICONINFORMATION);
    }
}

UINT CMusicPlayerApp::CheckUpdateThreadFunc(LPVOID lpParam)
{
    CCommon::SetThreadLanguage(theApp.m_general_setting_data.language);
    CheckUpdate(false);		//检查更新
    return 0;
}

//UINT CMusicPlayerApp::ClassifySongDataThreadFunc(LPVOID lpParam)
//{
//    //theApp.m_artist_classifer.ClassifyMedia();
//    //theApp.m_album_classifer.ClassifyMedia();
//    //theApp.m_genre_classifer.ClassifyMedia();
//    //theApp.m_year_classifer.ClassifyMedia();
//    return 0;
//}

void CMusicPlayerApp::SaveConfig()
{
    CIniHelper ini(m_config_path);
    ini.WriteBool(L"general", L"check_update_when_start", m_general_setting_data.check_update_when_start);
    ini.WriteInt(_T("general"), _T("language"), static_cast<int>(m_general_setting_data.language));
    ini.WriteBool(L"hot_key", L"global_multimedia_key_enable", m_hot_key_setting_data.global_multimedia_key_enable);
    ini.Save();
}

void CMusicPlayerApp::LoadConfig()
{
    CIniHelper ini(m_config_path);
    m_general_setting_data.check_update_when_start = ini.GetBool(L"general", L"check_update_when_start", true);
    m_general_setting_data.language = static_cast<Language>(ini.GetInt(L"general", L"language", 0));
    m_hot_key_setting_data.global_multimedia_key_enable = ini.GetBool(L"hot_key", L"global_multimedia_key_enable", false);
}

void CMusicPlayerApp::SaveSongInfo(const SongInfo& song_info)
{
    if (song_info.file_path.empty())
        return;
    SongInfo& song = m_song_data[song_info.file_path];
    song.CopyAudioTag(song_info);
    song.lengh = song_info.lengh;
    song.bitrate = song_info.bitrate;
    song.song_id = song_info.song_id;
    //song.is_favourite = song_info.is_favourite;

    SetSongDataModified();
}

void CMusicPlayerApp::LoadIconResource()
{
    m_icon_set.app.Load(IDR_MAINFRAME, NULL, DPI(16));
    m_icon_set.default_cover.Load(IDI_DEFAULT_COVER, NULL, 512);
    m_icon_set.default_cover_small.Load(IDI_DEFAULT_COVER, NULL, DPI(32));
    m_icon_set.skin.Load(IDI_SKIN, IDI_SKIN_D, DPI(16));
    m_icon_set.eq.Load(IDI_EQ, IDI_EQ_D, DPI(16));
    m_icon_set.setting.Load(IDI_SETTING, IDI_SETTING_D, DPI(16));
    m_icon_set.mini.Load(IDI_MINI, IDI_MINI_D, DPI(16));
    m_icon_set.play_oder.Load(IDI_PLAY_ORDER, IDI_PLAY_ORDER_D, DPI(16));
    m_icon_set.play_shuffle.Load(IDI_PLAY_SHUFFLE, IDI_PLAY_SHUFFLE_D, DPI(16));
    m_icon_set.loop_playlist.Load(IDI_LOOP_PLAYLIST, IDI_LOOP_PLAYLIST_D, DPI(16));
    m_icon_set.loop_track.Load(IDI_LOOP_TRACK, IDI_LOOP_TRACK_D, DPI(16));
    m_icon_set.info.Load(IDI_SONG_INFO, IDI_SONG_INFO_D, DPI(16));
    m_icon_set.select_folder.Load(IDI_SELECT_FOLDER, IDI_SELECT_FOLDER_D, DPI(16));
    m_icon_set.media_lib.Load(IDI_MEDIA_LIB, IDI_MEDIA_LIB_D, DPI(16));
    m_icon_set.show_playlist.Load(IDI_PLAYLIST, IDI_PLAYLIST_D, DPI(16));
    m_icon_set.find_songs.Load(IDI_FIND_SONGS, IDI_FIND_SONGS_D, DPI(16));
    m_icon_set.full_screen.Load(IDI_FULL_SCREEN, IDI_FULL_SCREEN_D, DPI(16));
    m_icon_set.full_screen1.Load(IDI_FULL_SCREEN1, IDI_FULL_SCREEN1_D, DPI(16));
    m_icon_set.menu.Load(IDI_MENU, IDI_MENU_D, DPI(16));
    m_icon_set.favourite.Load(IDI_FAVOURITE, IDI_FAVOURITE_D, DPI(16));
    m_icon_set.heart.Load(IDI_HEART, NULL, DPI(16));
    m_icon_set.double_line.Load(IDI_DOUBLE_LINE_D, NULL, DPI(16));
    m_icon_set.lock.Load(IDI_LOCK_D, NULL, DPI(16));
    m_icon_set.close.Load(IDI_CLOSE_D, NULL, DPI(16));
    m_icon_set.edit.Load(IDI_EDIT_D, NULL, DPI(16));
    m_icon_set.add.Load(IDI_ADD_D, NULL, DPI(16));
    m_icon_set.artist.Load(IDI_ARTIST_D, NULL, DPI(16));
    m_icon_set.album.Load(IDI_ALBUM_D, NULL, DPI(16));
    m_icon_set.year.Load(IDI_YEAR_D, NULL, DPI(16));
    m_icon_set.folder_explore.Load(IDI_FOLDER_EXPLORE_D, NULL, DPI(16));

    m_icon_set.previous.Load(IDI_PREVIOUS, NULL, DPI(16));
    m_icon_set.play.Load(IDI_PLAY, NULL, DPI(16));
    m_icon_set.pause.Load(IDI_PAUSE, NULL, DPI(16));
    m_icon_set.next.Load(IDI_NEXT1, NULL, DPI(16));

    m_icon_set.stop_l.Load(IDI_STOP, NULL, DPI(20));
    m_icon_set.previous_l.Load(IDI_PREVIOUS, NULL, DPI(20));
    m_icon_set.play_l.Load(IDI_PLAY, NULL, DPI(20));
    m_icon_set.pause_l.Load(IDI_PAUSE, NULL, DPI(20));
    m_icon_set.next_l.Load(IDI_NEXT1, NULL, DPI(20));

}

void CMusicPlayerApp::InitMenuResourse()
{
    m_menu_set.m_main_menu.LoadMenu(IDR_MENU1);
    m_menu_set.m_list_popup_menu.LoadMenu(IDR_POPUP_MENU);		//装载播放列表右键菜单
    m_menu_set.m_playlist_toolbar_menu.LoadMenu(IDR_PLAYLIST_TOOLBAR_MENU);

    //将主菜单添加到系统菜单中
    CMenu* pSysMenu = m_pMainWnd->GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        pSysMenu->AppendMenu(MF_SEPARATOR);
        CCommon::AppendMenuOp(pSysMenu->GetSafeHmenu(), m_menu_set.m_main_menu.GetSafeHmenu());		//将主菜单添加到系统菜单

        pSysMenu->AppendMenu(MF_SEPARATOR);
        pSysMenu->AppendMenu(MF_STRING, IDM_MINIMODE, CCommon::LoadText(IDS_MINI_MODE2, _T("\tCtrl+M")));

        CString exitStr;
        m_menu_set.m_main_menu.GetMenuString(ID_MENU_EXIT, exitStr, 0);
        pSysMenu->AppendMenu(MF_STRING, ID_MENU_EXIT, exitStr);

        //添加一个测试命令
#ifdef _DEBUG
        pSysMenu->AppendMenu(MF_STRING, ID_TEST, _T("Test Command"));
        pSysMenu->AppendMenu(MF_STRING, ID_TEST_DIALOG, _T("Test Dialog"));
#endif
    }

    //初始化按Shift键弹出的右键菜单
    m_menu_set.m_main_menu_popup.CreatePopupMenu();
    CCommon::AppendMenuOp(m_menu_set.m_main_menu_popup.GetSafeHmenu(), m_menu_set.m_main_menu.GetSafeHmenu());
    m_menu_set.m_main_menu_popup.AppendMenu(MF_SEPARATOR);
    CString exitStr;
    m_menu_set.m_main_menu.GetMenuString(ID_MENU_EXIT, exitStr, 0);
    m_menu_set.m_main_menu_popup.AppendMenu(MF_STRING, ID_MENU_EXIT, exitStr);

    m_menu_set.m_popup_menu.LoadMenu(IDR_LYRIC_POPUP_MENU);	//装载歌词右键菜单
    m_menu_set.m_main_popup_menu.LoadMenu(IDR_MAIN_POPUP_MENU);

    m_menu_set.m_playlist_btn_menu.LoadMenu(IDR_PLAYLIST_BTN_MENU);
    m_menu_set.m_lyric_default_style.LoadMenu(IDR_LRYIC_DEFAULT_STYLE_MENU);
    m_menu_set.m_media_lib_popup_menu.LoadMenu(IDR_MEDIA_LIB_POPUP_MENU);
}

int CMusicPlayerApp::DPI(int pixel)
{
    return (m_dpi * (pixel) / 96);
}

int CMusicPlayerApp::DPI(double pixel)
{
    return static_cast<int>(m_dpi * (pixel) / 96);
}

int CMusicPlayerApp::DPIRound(double pixel, double round)
{
    double rtn;
    rtn = static_cast<double>(m_dpi) * pixel / 96;
    rtn += round;
    return static_cast<int>(rtn);
}

void CMusicPlayerApp::GetDPIFromWindow(CWnd * pWnd)
{
    CWindowDC dc(pWnd);
    HDC hDC = dc.GetSafeHdc();
    m_dpi = GetDeviceCaps(hDC, LOGPIXELSY);
}

SongInfo CMusicPlayerApp::GetSongInfo(wstring& file_path)
{
    SongInfo song;
    auto iter = m_song_data.find(file_path);
    if (iter != m_song_data.end())
        song = iter->second;
    song.file_path = file_path;
    return song;
}

WORD CMusicPlayerApp::GetCurrentLanguage() const
{
    switch (m_general_setting_data.language)
    {
    case Language::SIMPLIFIED_CHINESE:
        return MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED);
    default:
        return MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
    }
}

bool CMusicPlayerApp::IsGlobalMultimediaKeyEnabled() const
{
    return m_multimedia_key_hook != NULL;
}

CString CMusicPlayerApp::GetHelpString()
{
    CString help_info;
    HRSRC hRes;
    if (m_general_setting_data.language == Language::FOLLOWING_SYSTEM)
        hRes = FindResource(NULL, MAKEINTRESOURCE(IDR_TEXT1), _T("TEXT"));
    else
        hRes = FindResourceEx(NULL, _T("TEXT"), MAKEINTRESOURCE(IDR_TEXT1), GetCurrentLanguage());
    if (hRes != NULL)
    {
        HGLOBAL hglobal = LoadResource(NULL, hRes);
        if (hglobal != NULL)
            help_info.Format(_T("%s"), (LPVOID)hglobal);
    }

    return help_info;
}

CString CMusicPlayerApp::GetSystemInfoString()
{
    CString info;
    info += _T("System Info:\r\n");

    CString strTmp;
    strTmp.Format(_T("Windows Version: %d.%d build %d\r\n"), CWinVersionHelper::GetMajorVersion(),
                  CWinVersionHelper::GetMinorVersion(), CWinVersionHelper::GetBuildNumber());
    info += strTmp;

    strTmp.Format(_T("DPI: %d"), GetDPI());
    info += strTmp;
    info += _T("\r\n");

    return info;
}

void CMusicPlayerApp::SetSongDataModified()
{
    m_song_data_modified = true;
}

bool CMusicPlayerApp::IsSongDataModified() const
{
    return m_song_data_modified;
}

void CMusicPlayerApp::WriteErrorLog(const wstring & log_str)
{
    CCommon::WriteLog((m_module_dir + L"error.log").c_str(), log_str);
}

void CMusicPlayerApp::StartUpdateMediaLib()
{
    if(!m_media_lib_updating)
    {
        m_media_lib_updating = true;
        AfxBeginThread([](LPVOID lpParam)->UINT
        {
            CMusicPlayerCmdHelper::CleanUpSongData();
            CMusicPlayerCmdHelper::CleanUpRecentFolders();
            CMusicPlayerCmdHelper::UpdateMediaLib();
            theApp.m_media_lib_updating = false;
            return 0;
        }, NULL);
    }
}

//void CMusicPlayerApp::StartClassifySongData()
//{
//    AfxBeginThread(ClassifySongDataThreadFunc, NULL);
//}

void CMusicPlayerApp::LoadSongData()
{
    // 打开文件
    CFile file;
    BOOL bRet = file.Open(m_song_data_path.c_str(), CFile::modeRead);
    if (!bRet) return;
    // 构造CArchive对象
    CArchive ar(&file, CArchive::load);
    // 读数据
    int size{};
    SongInfo song_info;
    CString song_path;
    CString temp;
    int song_length;
    try
    {
        //读取版本
        CString version_str;
        ar >> version_str;
		if (!CCommon::StringIsVersion(version_str))
			version_str = _T("0.00");
        if (version_str >= _T("2.664"))
        {
            ar >> size;		//读取映射容器的长度
        }
        else
        {
            size_t size_1;
            ar >> size_1;
            size = static_cast<int>(size_1);
        }
        for (size_t i{}; i < size; i++)
        {
            ar >> song_path;
            //ar >> temp;
            //song_info.lyric_file = temp;
            ar >> song_length;
            song_info.lengh.fromInt(song_length);
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
			if (version_str >= _T("2.66"))
			{
				ar >> song_info.track;
			}
			else
			{
				BYTE track;
				ar >> track;
				song_info.track = track;
			}
            ar >> song_info.tag_type;
            ar >> temp;
            song_info.song_id = temp;

            if(version_str >= _T("2.64"))		//版本号大于等于2.64
            {
                ar >> song_info.listen_time;
                ar >> song_info.info_acquired;
            }

            if (version_str == _T("2.661"))
            {
                ar >> song_info.is_favourite;
            }

            if (version_str >= _T("2.663"))
            {
                ar >> song_info.no_online_album_cover;
                ar >> song_info.no_online_lyric;
            }
            m_song_data[wstring{ song_path }] = song_info;		//将读取到的一首歌曲信息添加到映射容器中
        }
    }
    catch(CArchiveException* exception)
    {
        CString info;
        info = CCommon::LoadTextFormat(IDS_SERIALIZE_ERROR, { m_song_data_path, exception->m_cause });
        WriteErrorLog(wstring{ info });
    }
    // 关闭对象
    ar.Close();
    // 关闭文件
    file.Close();
}

LRESULT CMusicPlayerApp::MultiMediaKeyHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
//	if (wParam == HSHELL_APPCOMMAND)
    //{
    //	int a = 0;
    //}

    //截获全局的多媒体按键消息
    if (wParam == WM_KEYUP)
    {
        KBDLLHOOKSTRUCT* pKBHook = (KBDLLHOOKSTRUCT*)lParam;
        switch (pKBHook->vkCode)
        {
        case VK_MEDIA_PLAY_PAUSE:
            SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, ID_PLAY_PAUSE, 0);
            return TRUE;
        case VK_MEDIA_PREV_TRACK:
            SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, ID_PREVIOUS, 0);
            return TRUE;
        case VK_MEDIA_NEXT_TRACK:
            SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, ID_NEXT, 0);
            return TRUE;
        case VK_MEDIA_STOP:
            SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, ID_STOP, 0);
            return TRUE;
        default:
            break;
        }
    }

    CallNextHookEx(theApp.m_multimedia_key_hook, nCode, wParam, lParam);

    return LRESULT();
}



int CMusicPlayerApp::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类
	// 卸载GDI+
	Gdiplus::GdiplusShutdown(m_gdiplusToken);

	return CWinApp::ExitInstance();
}
