
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
#include "WIC.h"
#include "SongDataManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMusicPlayerApp

BEGIN_MESSAGE_MAP(CMusicPlayerApp, CWinApp)
    //ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
    ON_COMMAND(ID_HELP, &CMusicPlayerApp::OnHelp)
    ON_COMMAND(ID_HELP_UPDATE_LOG, &CMusicPlayerApp::OnHelpUpdateLog)
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
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    m_module_path_reg = path;
    if (m_module_path_reg.find(L' ') != std::wstring::npos)
    {
        //如果路径中有空格，则在程序路径前后添加双引号
        m_module_path_reg = L'\"' + m_module_path_reg;
        m_module_path_reg += L'\"';
    }

    //获取当前DPI
    HDC hDC = ::GetDC(NULL);
    m_dpi = GetDeviceCaps(hDC, LOGPIXELSY);
    ::ReleaseDC(NULL, hDC);
    if (m_dpi == 0)
    {
        WriteLog(L"Get system DPI failed!");
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
        WriteLog(wstring{ info.GetString() });
        return FALSE;
    }

    bool cmd_control = CCommon::GetCmdLineCommand(cmd_line, m_cmd);		//命令行参数是否包含参数命令
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
            bool add_files{ false };
            HWND handle = FindWindow(_T("MusicPlayer_l3gwYT"), NULL);		//根据类名查找已运行实例窗口的句柄
            if (handle == NULL)
            {
                //如果没有找到窗口句柄，则可能程序窗口还未创建，先延时一段时间再查找。
                //这种情况只可能是在同时打开多个音频文件时启动了多个进程，这些进程几乎是同时启动的，
                //此时虽然检测到已有另一个程序实例在运行，但是窗口还未创建，
                //此时将命令行参数的文件添加到现有实例的播放列表中（通过将add_files设置为true来实现）
                Sleep(500);
                handle = FindWindow(_T("MusicPlayer_l3gwYT"), NULL);
                add_files = true;
            }
            if (handle != NULL)
            {
                HWND minidlg_handle = FindWindow(_T("MiniDlg_ByH87M"), NULL);
                if (!cmd_control || m_cmd == ControlCmd::MINI_MODE)
                {
                    if (minidlg_handle == NULL)			//没有找到“迷你模式”窗口，则激活主窗口
                    {
                        ShowWindow(handle, SW_SHOWNORMAL);		//激活并显示窗口
                        SetForegroundWindow(handle);		//将窗口设置为焦点
                        ::SendMessage(handle, WM_MAIN_WINDOW_ACTIVATED, 0, 0);
                    }
                    else				//找到了“迷你模式”窗口，则激活“迷你模式”窗口
                    {
                        ShowWindow(minidlg_handle, SW_SHOWNORMAL);
                        SetForegroundWindow(minidlg_handle);
                    }
                }

                if (cmd_control)
                {
                    if (m_cmd & ControlCmd::PLAY_PAUSE)
                        ::SendMessage(handle, WM_COMMAND, ID_PLAY_PAUSE, 0);
                    if (m_cmd & ControlCmd::_PREVIOUS)
                        ::SendMessage(handle, WM_COMMAND, ID_PREVIOUS, 0);
                    if (m_cmd & ControlCmd::_NEXT)
                        ::SendMessage(handle, WM_COMMAND, ID_NEXT, 0);
                    if (m_cmd & ControlCmd::STOP)
                        ::SendMessage(handle, WM_COMMAND, ID_STOP, 0);
                    if (m_cmd & ControlCmd::FF)
                        ::SendMessage(handle, WM_COMMAND, ID_FF, 0);
                    if (m_cmd & ControlCmd::REW)
                        ::SendMessage(handle, WM_COMMAND, ID_REW, 0);
                    if (m_cmd & ControlCmd::VOLUME_UP)
                        ::SendMessage(handle, WM_COMMAND, ID_VOLUME_UP, 0);
                    if (m_cmd & ControlCmd::VOLUME_DOWM)
                        ::SendMessage(handle, WM_COMMAND, ID_VOLUME_DOWN, 0);
                    if (m_cmd & ControlCmd::MINI_MODE)
                        ::PostMessage(handle, WM_COMMAND, ID_MINI_MODE, 0);     //这里应该用PostMessage，因为响应ID_MINI_MODE的函数中有DoModel函数，只有当模态对话框关闭后函数才会返回
                }

                if (!cmd_line.empty())		//如果通过命令行传递了打开的文件名，且已有一个进程在运行，则将打开文件的命令和命令行参数传递给该进程
                {
                    //通过WM_COPYDATA消息向已有进程传递消息
                    COPYDATASTRUCT copy_data;
                    copy_data.dwData = add_files ? COPY_DATA_ADD_FILE : COPY_DATA_OPEN_FILE;
                    copy_data.cbData = cmd_line.size() * sizeof(wchar_t);
                    copy_data.lpData = (const PVOID)cmd_line.c_str();
                    ::SendMessage(handle, WM_COPYDATA, 0, (LPARAM)&copy_data);
                }
            }
            return FALSE;		//退出当前程序
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
    CShellManager* pShellManager = new CShellManager;

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
    if (m_hot_key_setting_data.global_multimedia_key_enable)
        m_multimedia_key_hook = SetWindowsHookEx(WH_KEYBOARD_LL, CMusicPlayerApp::MultiMediaKeyHookProc, m_hInstance, 0);

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

    m_hScintillaModule = LoadLibrary(_T("SciLexer.dll"));

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

    if (!dialog_exist)		//确保对话框已经存在时不再弹出
    {
        dialog_exist = true;
        CMessageDlg helpDlg;
        helpDlg.SetWindowTitle(CCommon::LoadText(IDS_HELP));
        helpDlg.SetInfoText(CCommon::LoadText(IDS_WELCOM_TO_USE));
        helpDlg.ShowLinkStatic(true);
        helpDlg.SetLinkInfo(CCommon::LoadText(IDS_SHOW_ONLINE_HELP_INFO), _T("https://github.com/zhongyang219/MusicPlayer2/wiki"));

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
    CSongDataManager::GetInstance().SaveSongData(m_song_data_path);
}

void CMusicPlayerApp::CheckUpdate(bool message)
{
    if (m_checking_update)      //如果还在检查更新，则直接返回
        return;
    CFlagLocker update_locker(m_checking_update);
    CWaitCursor wait_cursor;

    wstring version;		//程序版本
    wstring link;			//下载链接
    wstring contents_zh_cn;	//更新内容（简体中文）
    wstring contents_en;	//更新内容（English）
    CUpdateHelper update_helper;
    update_helper.SetUpdateSource(static_cast<CUpdateHelper::UpdateSource>(m_general_setting_data.update_source));
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

void CMusicPlayerApp::CheckUpdateInThread(bool message)
{
    AfxBeginThread(CheckUpdateThreadFunc, (LPVOID)message);
}

UINT CMusicPlayerApp::CheckUpdateThreadFunc(LPVOID lpParam)
{
    CCommon::SetThreadLanguage(theApp.m_general_setting_data.language);
    theApp.CheckUpdate(lpParam);		//检查更新
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

void CMusicPlayerApp::LoadIconResource()
{
#ifdef _DEBUG
    m_icon_set.app.Load(IDI_APP_DEBUG, NULL, DPI(16));
#else
    m_icon_set.app.Load(IDR_MAINFRAME, NULL, DPI(16));
#endif
    m_icon_set.default_cover = CDrawCommon::LoadIconResource(IDI_DEFAULT_COVER, 512, 512);
    m_icon_set.default_cover_small = CDrawCommon::LoadIconResource(IDI_DEFAULT_COVER, DPI(32), DPI(32));
    m_icon_set.default_cover_not_played = CDrawCommon::LoadIconResource(IDI_DEFAULT_COVER_NOT_PLAYED, 512, 512);
    m_icon_set.default_cover_small_not_played = CDrawCommon::LoadIconResource(IDI_DEFAULT_COVER_NOT_PLAYED, DPI(32), DPI(32));
    m_icon_set.default_cover_toolbar.Load(IDI_DEFAULT_COVER, IDI_DEFAULT_COVER, DPI(16));
    m_icon_set.default_cover_toolbar_not_played.Load(IDI_DEFAULT_COVER_NOT_PLAYED, IDI_DEFAULT_COVER_NOT_PLAYED, DPI(16));
    m_icon_set.skin.Load(IDI_SKIN, IDI_SKIN_D, DPI(16));
    m_icon_set.eq.Load(IDI_EQ, IDI_EQ_D, DPI(16));
    m_icon_set.setting.Load(IDI_SETTING, IDI_SETTING_D, DPI(16));
    m_icon_set.mini.Load(IDI_MINI, IDI_MINI_D, DPI(16));
    m_icon_set.play_oder.Load(IDI_PLAY_ORDER, IDI_PLAY_ORDER_D, DPI(16));
    m_icon_set.play_shuffle.Load(IDI_PLAY_SHUFFLE, IDI_PLAY_SHUFFLE_D, DPI(16));
    m_icon_set.play_random.Load(IDI_PLAY_RANDOM, IDI_PLAY_RANDOM_D, DPI(16));
    m_icon_set.loop_playlist.Load(IDI_LOOP_PLAYLIST, IDI_LOOP_PLAYLIST_D, DPI(16));
    m_icon_set.loop_track.Load(IDI_LOOP_TRACK, IDI_LOOP_TRACK_D, DPI(16));
    m_icon_set.play_track.Load(IDI_PLAY_TRACK, IDI_PLAY_TRACK_D, DPI(16));
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
    m_icon_set.genre.Load(IDI_GENRE_D, NULL, DPI(16));
    m_icon_set.year.Load(IDI_YEAR_D, NULL, DPI(16));
    m_icon_set.folder_explore.Load(IDI_FOLDER_EXPLORE_D, NULL, DPI(16));
    m_icon_set.lyric_forward.Load(IDI_LYRIC_FORWARD_D, NULL, DPI(16));
    m_icon_set.lyric_delay.Load(IDI_LYRIC_DELAY_D, NULL, DPI(16));
    m_icon_set.recent_songs.Load(IDI_RECENT_SONG_D, NULL, DPI(16));

    m_icon_set.previous.Load(IDI_PREVIOUS, NULL, DPI(16));
    m_icon_set.play.Load(IDI_PLAY, NULL, DPI(16));
    m_icon_set.pause.Load(IDI_PAUSE, NULL, DPI(16));
    m_icon_set.next.Load(IDI_NEXT1, NULL, DPI(16));
    m_icon_set.stop.Load(IDI_STOP, NULL, DPI(16));

    //用于主界面的播放控制图标，大小为20像素
    m_icon_set.play_l.Load(IDI_PLAY_NEW, IDI_PLAY_NEW_D, DPI(20));
    m_icon_set.pause_l.Load(IDI_PAUSE_NEW, IDI_PAUSE_NEW_D, DPI(20));
    m_icon_set.previous_l.Load(IDI_PREVIOUS_NEW, IDI_PREVIOUS_NEW_D, DPI(20));
    m_icon_set.next_l.Load(IDI_NEXT_NEW, IDI_NEXT_NEW_D, DPI(20));
    m_icon_set.stop_l.Load(IDI_STOP_NEW, IDI_STOP_NEW_D, DPI(20));

    //用于迷你模式的播放控制图标，大小为16像素
    m_icon_set.play_new.Load(IDI_PLAY_NEW, IDI_PLAY_NEW_D, DPI(16));
    m_icon_set.pause_new.Load(IDI_PAUSE_NEW, IDI_PAUSE_NEW_D, DPI(16));
    m_icon_set.previous_new.Load(IDI_PREVIOUS_NEW, IDI_PREVIOUS_NEW_D, DPI(16));
    m_icon_set.next_new.Load(IDI_NEXT_NEW, IDI_NEXT_NEW_D, DPI(16));

    m_icon_set.app_close.Load(IDI_CLOSE, IDI_CLOSE_D, DPI(16));
    m_icon_set.maximize.Load(IDI_MAXIMIZE, IDI_MAXIMIZE_D, DPI(16));
    m_icon_set.minimize.Load(IDI_MINIMIZE, IDI_MINIMIZE_D, DPI(16));
    m_icon_set.restore.Load(IDI_RESTORE, IDI_RESTORE_D, DPI(16));

    //菜单图标
    m_icon_set.stop_new = CDrawCommon::LoadIconResource(IDI_STOP_NEW_D, DPI(16), DPI(16));
    m_icon_set.save_new = CDrawCommon::LoadIconResource(IDI_SAVE_NEW_D, DPI(16), DPI(16));
    m_icon_set.save_as = CDrawCommon::LoadIconResource(IDI_SAVE_AS_D, DPI(16), DPI(16));
    m_icon_set.music = CDrawCommon::LoadIconResource(IDI_MUSIC_D, DPI(16), DPI(16));
    m_icon_set.file_relate = CDrawCommon::LoadIconResource(IDI_FILE_RELATE_D, DPI(16), DPI(16));
    m_icon_set.online = CDrawCommon::LoadIconResource(IDI_ONLINE_D, DPI(16), DPI(16));
    m_icon_set.play_pause = CDrawCommon::LoadIconResource(IDI_PLAY_PAUSE_D, DPI(16), DPI(16));
    m_icon_set.convert = CDrawCommon::LoadIconResource(IDI_CONVERT_D, DPI(16), DPI(16));
    m_icon_set.download = CDrawCommon::LoadIconResource(IDI_DOWNLOAD_D, DPI(16), DPI(16));
    m_icon_set.download1 = CDrawCommon::LoadIconResource(IDI_DOWNLOAD1_D, DPI(16), DPI(16));
    m_icon_set.help = CDrawCommon::LoadIconResource(IDI_HELP_D, DPI(16), DPI(16));
    m_icon_set.ff_new = CDrawCommon::LoadIconResource(IDI_FF_NEW_D, DPI(16), DPI(16));
    m_icon_set.rew_new = CDrawCommon::LoadIconResource(IDI_REW_NEW_D, DPI(16), DPI(16));
    m_icon_set.playlist_dock = CDrawCommon::LoadIconResource(IDI_PLAYLIST_DOCK_D, DPI(16), DPI(16));
    m_icon_set.playlist_float = CDrawCommon::LoadIconResource(IDI_PLAYLIST_FLOAT_D, DPI(16), DPI(16));
    m_icon_set.statistics = CDrawCommon::LoadIconResource(IDI_STATISTICS_D, DPI(16), DPI(16));
    m_icon_set.pin = CDrawCommon::LoadIconResource(IDI_PIN_D, DPI(16), DPI(16));
    m_icon_set.exit = CDrawCommon::LoadIconResource(IDI_EXIT_D, DPI(16), DPI(16));
    m_icon_set.album_cover = CDrawCommon::LoadIconResource(IDI_ALBUM_COVER_D, DPI(16), DPI(16));
    m_icon_set.dark_mode = CDrawCommon::LoadIconResource(IDI_DARK_MODE_D, DPI(16), DPI(16));
    m_icon_set.lyric = CDrawCommon::LoadIconResource(IDI_LYRIC_D, DPI(16), DPI(16));
    m_icon_set.rename = CDrawCommon::LoadIconResource(IDI_RENAME_D, DPI(16), DPI(16));
    m_icon_set.tag = CDrawCommon::LoadIconResource(IDI_TAG, DPI(16), DPI(16));
    m_icon_set.star = CDrawCommon::LoadIconResource(IDI_STAR, DPI(16), DPI(16));

    m_icon_set.ok = CDrawCommon::LoadIconResource(IDI_OK_D, DPI(16), DPI(16));
    m_icon_set.locate.Load(IDI_LOCATE_D, NULL, DPI(16));

    //加载通知区图标
    m_icon_set.notify_icons[0] = m_icon_set.app.GetIcon();      //应用图标直接使用前面加载过的
    m_icon_set.notify_icons[1] = CDrawCommon::LoadIconResource(IDI_APP_LIGHT, DPI(16), DPI(16));
    m_icon_set.notify_icons[2] = CDrawCommon::LoadIconResource(IDI_APP_DARK, DPI(16), DPI(16));

    //加载图片资源
    m_image_set.default_cover = CCommon::GetPngImageResource(IDB_DEFAULT_ALBUM_COVER);
    m_image_set.default_cover_not_played = CCommon::GetPngImageResource(IDB_DEFAULT_ALBUM_COVER_NOT_PLAYED);
}

void CMusicPlayerApp::InitMenuResourse()
{
    m_menu_set.m_main_menu.LoadMenu(IDR_MENU1);
    m_menu_set.m_list_popup_menu.LoadMenu(IDR_POPUP_MENU);		//装载播放列表右键菜单
    m_menu_set.m_playlist_toolbar_menu.LoadMenu(IDR_PLAYLIST_TOOLBAR_MENU);

    m_menu_set.m_popup_menu.LoadMenu(IDR_LYRIC_POPUP_MENU);	//装载歌词右键菜单
    m_menu_set.m_main_popup_menu.LoadMenu(IDR_MAIN_POPUP_MENU);

    m_menu_set.m_playlist_btn_menu.LoadMenu(IDR_PLAYLIST_BTN_MENU);
    m_menu_set.m_lyric_default_style.LoadMenu(IDR_LRYIC_DEFAULT_STYLE_MENU);
    m_menu_set.m_media_lib_popup_menu.LoadMenu(IDR_MEDIA_LIB_POPUP_MENU);

    m_menu_set.m_media_lib_folder_menu.LoadMenu(IDR_SET_PATH_POPUP_MENU);
    m_menu_set.m_media_lib_folder_menu.GetSubMenu(0)->SetDefaultItem(ID_PLAY_PATH);

    m_menu_set.m_media_lib_playlist_menu.LoadMenu(IDR_SELETE_PLAYLIST_POPUP_MENU);
    m_menu_set.m_media_lib_playlist_menu.GetSubMenu(0)->SetDefaultItem(ID_PLAY_PLAYLIST);

    m_menu_set.m_notify_menu.LoadMenu(IDR_NOTIFY_MENU);
    m_menu_set.m_mini_mode_menu.LoadMenu(IDR_MINI_MODE_MENU);
    m_menu_set.m_property_cover_menu.LoadMenu(IDR_PROPERTY_COVER_MENU);
    m_menu_set.m_property_menu.LoadMenu(IDR_PROPERTY_MENU);

    //为菜单添加图标
    //主菜单
    //文件
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_FILE_OPEN, FALSE, m_icon_set.music);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_FILE_OPEN_FOLDER, FALSE, m_icon_set.select_folder.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_FILE_OPEN_PLAYLIST, FALSE, m_icon_set.show_playlist.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_MENU_EXIT, FALSE, m_icon_set.exit);
    //播放控制
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_PLAY_PAUSE, FALSE, m_icon_set.play_pause);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_STOP, FALSE, m_icon_set.stop_new);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_PREVIOUS, FALSE, m_icon_set.previous_new.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_NEXT, FALSE, m_icon_set.next_new.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_REW, FALSE, m_icon_set.rew_new);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_FF, FALSE, m_icon_set.ff_new);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_PLAY_ORDER, FALSE, m_icon_set.play_oder.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_PLAY_SHUFFLE, FALSE, m_icon_set.play_shuffle.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_PLAY_RANDOM, FALSE, m_icon_set.play_random.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_LOOP_PLAYLIST, FALSE, m_icon_set.loop_playlist.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_LOOP_TRACK, FALSE, m_icon_set.loop_track.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_PLAY_TRACK, FALSE, m_icon_set.play_track.GetIcon(true));
    //播放列表
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSubMenu(2)->GetSafeHmenu(), 0, TRUE, m_icon_set.add.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_PLAYLIST_ADD_FILE, FALSE, m_icon_set.music);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_PLAYLIST_ADD_FOLDER, FALSE, m_icon_set.select_folder.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSubMenu(2)->GetSafeHmenu(), 1, TRUE, m_icon_set.close.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_RELOAD_PLAYLIST, FALSE, m_icon_set.loop_playlist.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_SAVE_AS_NEW_PLAYLIST, FALSE, m_icon_set.save_new);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_SAVE_CURRENT_PLAYLIST_AS, FALSE, m_icon_set.save_as);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_LOCATE_TO_CURRENT, FALSE, m_icon_set.locate.GetIcon(true));
    //歌词
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_RELOAD_LYRIC, FALSE, m_icon_set.loop_playlist.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_EDIT_LYRIC, FALSE, m_icon_set.edit.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_LYRIC_FORWARD, FALSE, m_icon_set.lyric_forward.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_LYRIC_DELAY, FALSE, m_icon_set.lyric_delay.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_SAVE_MODIFIED_LYRIC, FALSE, m_icon_set.save_new);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_RELATE_LOCAL_LYRIC, FALSE, m_icon_set.lyric);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_DELETE_LYRIC, FALSE, m_icon_set.close.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_BROWSE_LYRIC, FALSE, m_icon_set.folder_explore.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_DOWNLOAD_LYRIC, FALSE, m_icon_set.download);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_LYRIC_BATCH_DOWNLOAD, FALSE, m_icon_set.download1);
    //视图
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_SHOW_PLAYLIST, FALSE, m_icon_set.playlist_dock);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_FLOAT_PLAYLIST, FALSE, m_icon_set.playlist_float);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_SHOW_MENU_BAR, FALSE, m_icon_set.menu.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_ALWAYS_ON_TOP, FALSE, m_icon_set.pin);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_MINI_MODE, FALSE, m_icon_set.mini.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_FULL_SCREEN, FALSE, m_icon_set.full_screen1.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_DARK_MODE, FALSE, m_icon_set.dark_mode);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_SWITCH_UI, FALSE, m_icon_set.skin.GetIcon(true));
    //工具
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_SET_PATH, FALSE, m_icon_set.media_lib.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_FIND, FALSE, m_icon_set.find_songs.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_EXPLORE_PATH, FALSE, m_icon_set.folder_explore.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_SONG_INFO, FALSE, m_icon_set.info.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_EQUALIZER, FALSE, m_icon_set.eq.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_FORMAT_CONVERT1, FALSE, m_icon_set.convert);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_CURRENT_EXPLORE_ONLINE, FALSE, m_icon_set.online);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_RE_INI_BASS, FALSE, m_icon_set.loop_playlist.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_LISTEN_STATISTICS, FALSE, m_icon_set.statistics);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_TOOL_FILE_RELATE, FALSE, m_icon_set.file_relate);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_OPTION_SETTINGS, FALSE, m_icon_set.setting.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSubMenu(5)->GetSafeHmenu(), 11, TRUE, m_icon_set.album_cover);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_DOWNLOAD_ALBUM_COVER, FALSE, m_icon_set.download);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_ALBUM_COVER_SAVE_AS, FALSE, m_icon_set.save_as);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_DELETE_ALBUM_COVER, FALSE, m_icon_set.close.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_ALBUM_COVER_INFO, FALSE, m_icon_set.album_cover);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSubMenu(5)->GetSafeHmenu(), 12, TRUE, m_icon_set.close.GetIcon(true));
    //帮助
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_HELP, FALSE, m_icon_set.help);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu.GetSafeHmenu(), ID_APP_ABOUT, FALSE, m_icon_set.app.GetIcon());

    //界面右键菜单
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_popup_menu.GetSafeHmenu(), ID_SONG_INFO, FALSE, m_icon_set.info.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_popup_menu.GetSafeHmenu(), ID_PLAY_ORDER, FALSE, m_icon_set.play_oder.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_popup_menu.GetSafeHmenu(), ID_PLAY_SHUFFLE, FALSE, m_icon_set.play_shuffle.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_popup_menu.GetSafeHmenu(), ID_PLAY_RANDOM, FALSE, m_icon_set.play_random.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_popup_menu.GetSafeHmenu(), ID_LOOP_PLAYLIST, FALSE, m_icon_set.loop_playlist.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_popup_menu.GetSafeHmenu(), ID_LOOP_TRACK, FALSE, m_icon_set.loop_track.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_popup_menu.GetSafeHmenu(), ID_PLAY_TRACK, FALSE, m_icon_set.play_track.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_popup_menu.GetSubMenu(0)->GetSafeHmenu(), 2, TRUE, m_icon_set.add.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_popup_menu.GetSubMenu(0)->GetSafeHmenu(), 3, TRUE, m_icon_set.star);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_popup_menu.GetSafeHmenu(), ID_ADD_TO_NEW_PLAYLIST, FALSE, m_icon_set.add.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_popup_menu.GetSafeHmenu(), ID_ADD_TO_MY_FAVOURITE, FALSE, m_icon_set.favourite.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_popup_menu.GetSafeHmenu(), ID_VIEW_ARTIST, FALSE, m_icon_set.artist.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_popup_menu.GetSafeHmenu(), ID_VIEW_ALBUM, FALSE, m_icon_set.album.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_popup_menu.GetSafeHmenu(), ID_SET_PATH, FALSE, m_icon_set.media_lib.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_popup_menu.GetSafeHmenu(), ID_FIND, FALSE, m_icon_set.find_songs.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_popup_menu.GetSafeHmenu(), ID_EXPLORE_PATH, FALSE, m_icon_set.folder_explore.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_popup_menu.GetSafeHmenu(), ID_EQUALIZER, FALSE, m_icon_set.eq.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_popup_menu.GetSafeHmenu(), ID_CURRENT_EXPLORE_ONLINE, FALSE, m_icon_set.online);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_popup_menu.GetSafeHmenu(), ID_DOWNLOAD_ALBUM_COVER, FALSE, m_icon_set.album_cover);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_popup_menu.GetSafeHmenu(), ID_ALBUM_COVER_SAVE_AS, FALSE, m_icon_set.save_as);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_popup_menu.GetSafeHmenu(), ID_DELETE_ALBUM_COVER, FALSE, m_icon_set.close.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_popup_menu.GetSafeHmenu(), ID_ALBUM_COVER_INFO, FALSE, m_icon_set.album_cover);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_popup_menu.GetSafeHmenu(), ID_SWITCH_UI, FALSE, m_icon_set.skin.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_popup_menu.GetSafeHmenu(), ID_OPTION_SETTINGS, FALSE, m_icon_set.setting.GetIcon(true));

    //歌词右键菜单
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_popup_menu.GetSafeHmenu(), ID_RELOAD_LYRIC, FALSE, m_icon_set.loop_playlist.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_popup_menu.GetSafeHmenu(), ID_EDIT_LYRIC, FALSE, m_icon_set.edit.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_popup_menu.GetSafeHmenu(), ID_LYRIC_FORWARD, FALSE, m_icon_set.lyric_forward.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_popup_menu.GetSafeHmenu(), ID_LYRIC_DELAY, FALSE, m_icon_set.lyric_delay.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_popup_menu.GetSafeHmenu(), ID_SAVE_MODIFIED_LYRIC, FALSE, m_icon_set.save_new);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_popup_menu.GetSafeHmenu(), ID_RELATE_LOCAL_LYRIC, FALSE, m_icon_set.lyric);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_popup_menu.GetSafeHmenu(), ID_DELETE_LYRIC, FALSE, m_icon_set.close.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_popup_menu.GetSafeHmenu(), ID_BROWSE_LYRIC, FALSE, m_icon_set.folder_explore.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_popup_menu.GetSafeHmenu(), ID_DOWNLOAD_LYRIC, FALSE, m_icon_set.download);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_popup_menu.GetSafeHmenu(), ID_LYRIC_BATCH_DOWNLOAD, FALSE, m_icon_set.download1);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_popup_menu.GetSafeHmenu(), ID_SWITCH_UI, FALSE, m_icon_set.skin.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_popup_menu.GetSafeHmenu(), ID_OPTION_SETTINGS, FALSE, m_icon_set.setting.GetIcon(true));

    //播放列表菜单
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_playlist_toolbar_menu.GetSafeHmenu(), ID_PLAYLIST_ADD_FILE, FALSE, m_icon_set.music);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_playlist_toolbar_menu.GetSafeHmenu(), ID_PLAYLIST_ADD_FOLDER, FALSE, m_icon_set.select_folder.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_playlist_toolbar_menu.GetSafeHmenu(), ID_REMOVE_FROM_PLAYLIST, FALSE, m_icon_set.close.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_playlist_toolbar_menu.GetSafeHmenu(), ID_RELOAD_PLAYLIST, FALSE, m_icon_set.loop_playlist.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_playlist_toolbar_menu.GetSafeHmenu(), ID_SAVE_AS_NEW_PLAYLIST, FALSE, m_icon_set.save_new);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_playlist_toolbar_menu.GetSafeHmenu(), ID_SAVE_CURRENT_PLAYLIST_AS, FALSE, m_icon_set.save_as);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_playlist_toolbar_menu.GetSubMenu(4)->GetSafeHmenu(), 0, TRUE, m_icon_set.add.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_playlist_toolbar_menu.GetSafeHmenu(), ID_ADD_TO_NEW_PLAYLIST, FALSE, m_icon_set.add.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_playlist_toolbar_menu.GetSafeHmenu(), ID_ADD_TO_MY_FAVOURITE, FALSE, m_icon_set.favourite.GetIcon(true));

    //播放列表右键菜单
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_list_popup_menu.GetSafeHmenu(), ID_PLAY_ITEM, FALSE, m_icon_set.play_new.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_list_popup_menu.GetSafeHmenu(), ID_EXPLORE_ONLINE, FALSE, m_icon_set.online);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_list_popup_menu.GetSafeHmenu(), ID_FORMAT_CONVERT, FALSE, m_icon_set.convert);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_list_popup_menu.GetSubMenu(0)->GetSafeHmenu(), 4, TRUE, m_icon_set.star);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_list_popup_menu.GetSafeHmenu(), ID_RELOAD_PLAYLIST, FALSE, m_icon_set.loop_playlist.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_list_popup_menu.GetSafeHmenu(), ID_REMOVE_FROM_PLAYLIST, FALSE, m_icon_set.close.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_list_popup_menu.GetSubMenu(0)->GetSafeHmenu(), 11, TRUE, m_icon_set.add.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_list_popup_menu.GetSafeHmenu(), ID_ADD_TO_NEW_PLAYLIST, FALSE, m_icon_set.add.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_list_popup_menu.GetSafeHmenu(), ID_ADD_TO_MY_FAVOURITE, FALSE, m_icon_set.favourite.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_list_popup_menu.GetSafeHmenu(), ID_RENAME, FALSE, m_icon_set.rename);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_list_popup_menu.GetSafeHmenu(), ID_EXPLORE_TRACK, FALSE, m_icon_set.folder_explore.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_list_popup_menu.GetSafeHmenu(), ID_ITEM_PROPERTY, FALSE, m_icon_set.info.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_list_popup_menu.GetSafeHmenu(), ID_PLAYLIST_VIEW_ARTIST, FALSE, m_icon_set.artist.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_list_popup_menu.GetSafeHmenu(), ID_PLAYLIST_VIEW_ALBUM, FALSE, m_icon_set.album.GetIcon(true));

    //媒体库-文件夹的右键菜单
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_media_lib_folder_menu.GetSafeHmenu(), ID_PLAY_PATH, FALSE, m_icon_set.play_new.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_media_lib_folder_menu.GetSafeHmenu(), ID_DELETE_PATH, FALSE, m_icon_set.close.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_media_lib_folder_menu.GetSafeHmenu(), ID_BROWSE_PATH, FALSE, m_icon_set.folder_explore.GetIcon(true));

    //媒体库-播放列表的右键菜单
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_media_lib_playlist_menu.GetSafeHmenu(), ID_PLAY_PLAYLIST, FALSE, m_icon_set.play_new.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_media_lib_playlist_menu.GetSafeHmenu(), ID_DELETE_PLAYLIST, FALSE, m_icon_set.close.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_media_lib_playlist_menu.GetSafeHmenu(), ID_RENAME_PLAYLIST, FALSE, m_icon_set.rename);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_media_lib_playlist_menu.GetSafeHmenu(), ID_NEW_PLAYLIST, FALSE, m_icon_set.add.GetIcon(true));

    //媒体库右键菜单
    //左侧菜单
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_media_lib_popup_menu.GetSafeHmenu(), ID_PLAY_ITEM, FALSE, m_icon_set.play_new.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_media_lib_popup_menu.GetSubMenu(0)->GetSafeHmenu(), 1, TRUE, m_icon_set.add.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_media_lib_popup_menu.GetSafeHmenu(), ID_ADD_TO_NEW_PLAYLIST, FALSE, m_icon_set.add.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_media_lib_popup_menu.GetSafeHmenu(), ID_ADD_TO_MY_FAVOURITE, FALSE, m_icon_set.favourite.GetIcon(true));
    //右侧菜单
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_media_lib_popup_menu.GetSubMenu(1)->GetSafeHmenu(), ID_PLAY_ITEM, FALSE, m_icon_set.play_new.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_media_lib_popup_menu.GetSubMenu(1)->GetSafeHmenu(), 3, TRUE, m_icon_set.add.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_media_lib_popup_menu.GetSubMenu(1)->GetSafeHmenu(), ID_ADD_TO_NEW_PLAYLIST, FALSE, m_icon_set.add.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_media_lib_popup_menu.GetSubMenu(1)->GetSafeHmenu(), ID_ADD_TO_MY_FAVOURITE, FALSE, m_icon_set.favourite.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_media_lib_popup_menu.GetSubMenu(1)->GetSafeHmenu(), ID_EXPLORE_ONLINE, FALSE, m_icon_set.online);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_media_lib_popup_menu.GetSubMenu(1)->GetSafeHmenu(), ID_FORMAT_CONVERT, FALSE, m_icon_set.convert);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_media_lib_popup_menu.GetSubMenu(1)->GetSafeHmenu(), ID_EXPLORE_TRACK, FALSE, m_icon_set.folder_explore.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_media_lib_popup_menu.GetSubMenu(1)->GetSafeHmenu(), ID_DELETE_FROM_DISK, FALSE, m_icon_set.close.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_media_lib_popup_menu.GetSubMenu(1)->GetSafeHmenu(), ID_ITEM_PROPERTY, FALSE, m_icon_set.info.GetIcon(true));

    //通知区图标右键菜单
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_notify_menu.GetSafeHmenu(), ID_PLAY_PAUSE, FALSE, m_icon_set.play_pause);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_notify_menu.GetSafeHmenu(), ID_PREVIOUS, FALSE, m_icon_set.previous_new.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_notify_menu.GetSafeHmenu(), ID_NEXT, FALSE, m_icon_set.next_new.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_notify_menu.GetSafeHmenu(), ID_PLAY_ORDER, FALSE, m_icon_set.play_oder.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_notify_menu.GetSafeHmenu(), ID_PLAY_SHUFFLE, FALSE, m_icon_set.play_shuffle.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_notify_menu.GetSafeHmenu(), ID_PLAY_RANDOM, FALSE, m_icon_set.play_random.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_notify_menu.GetSafeHmenu(), ID_LOOP_PLAYLIST, FALSE, m_icon_set.loop_playlist.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_notify_menu.GetSafeHmenu(), ID_LOOP_TRACK, FALSE, m_icon_set.loop_track.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_notify_menu.GetSafeHmenu(), ID_PLAY_TRACK, FALSE, m_icon_set.play_track.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_notify_menu.GetSafeHmenu(), ID_MINIMODE_RESTORE, FALSE, m_icon_set.mini.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_notify_menu.GetSafeHmenu(), ID_SHOW_DESKTOP_LYRIC, FALSE, m_icon_set.lyric);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_notify_menu.GetSafeHmenu(), ID_LOCK_DESKTOP_LRYIC, FALSE, m_icon_set.lock.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_notify_menu.GetSafeHmenu(), ID_OPTION_SETTINGS, FALSE, m_icon_set.setting.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_notify_menu.GetSafeHmenu(), ID_MENU_EXIT, FALSE, m_icon_set.exit);

    //迷你模式右键菜单
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_mini_mode_menu.GetSafeHmenu(), ID_SONG_INFO, FALSE, m_icon_set.info.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_mini_mode_menu.GetSafeHmenu(), ID_PLAY_ORDER, FALSE, m_icon_set.play_oder.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_mini_mode_menu.GetSafeHmenu(), ID_PLAY_SHUFFLE, FALSE, m_icon_set.play_shuffle.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_mini_mode_menu.GetSafeHmenu(), ID_PLAY_RANDOM, FALSE, m_icon_set.play_random.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_mini_mode_menu.GetSafeHmenu(), ID_LOOP_PLAYLIST, FALSE, m_icon_set.loop_playlist.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_mini_mode_menu.GetSafeHmenu(), ID_LOOP_TRACK, FALSE, m_icon_set.loop_track.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_mini_mode_menu.GetSafeHmenu(), ID_PLAY_TRACK, FALSE, m_icon_set.play_track.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_mini_mode_menu.GetSubMenu(0)->GetSafeHmenu(), 2, TRUE, m_icon_set.add.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_mini_mode_menu.GetSafeHmenu(), ID_ADD_TO_NEW_PLAYLIST, FALSE, m_icon_set.add.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_mini_mode_menu.GetSafeHmenu(), ID_ADD_TO_MY_FAVOURITE, FALSE, m_icon_set.favourite.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_mini_mode_menu.GetSafeHmenu(), ID_DOWNLOAD_LYRIC, FALSE, m_icon_set.download);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_mini_mode_menu.GetSafeHmenu(), ID_DOWNLOAD_ALBUM_COVER, FALSE, m_icon_set.album_cover);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_mini_mode_menu.GetSafeHmenu(), ID_ADD_REMOVE_FROM_FAVOURITE, FALSE, m_icon_set.favourite.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_mini_mode_menu.GetSafeHmenu(), ID_SET_PATH, FALSE, m_icon_set.media_lib.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_mini_mode_menu.GetSafeHmenu(), ID_FIND, FALSE, m_icon_set.find_songs.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_mini_mode_menu.GetSafeHmenu(), ID_EXPLORE_PATH, FALSE, m_icon_set.folder_explore.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_mini_mode_menu.GetSafeHmenu(), ID_EQUALIZER, FALSE, m_icon_set.eq.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_mini_mode_menu.GetSafeHmenu(), ID_OPTION_SETTINGS, FALSE, m_icon_set.setting.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_mini_mode_menu.GetSafeHmenu(), ID_SHOW_PLAY_LIST, FALSE, m_icon_set.show_playlist.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_mini_mode_menu.GetSafeHmenu(), ID_MINI_MODE_ALWAYS_ON_TOP, FALSE, m_icon_set.pin);
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_mini_mode_menu.GetSafeHmenu(), ID_MINI_MODE_EXIT, FALSE, m_icon_set.exit);

    //属性——专辑封面右键菜单
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_property_cover_menu.GetSafeHmenu(), ID_COVER_BROWSE, FALSE, m_icon_set.folder_explore.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_property_cover_menu.GetSafeHmenu(), ID_COVER_DELETE, FALSE, m_icon_set.close.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_property_cover_menu.GetSafeHmenu(), ID_COVER_SAVE_AS, FALSE, m_icon_set.save_as);


    //初始化按Shift键弹出的右键菜单
    m_menu_set.m_main_menu_popup.CreatePopupMenu();
    CCommon::AppendMenuOp(m_menu_set.m_main_menu_popup.GetSafeHmenu(), m_menu_set.m_main_menu.GetSafeHmenu());
    m_menu_set.m_main_menu_popup.AppendMenu(MF_SEPARATOR);
    CString exitStr;
    m_menu_set.m_main_menu.GetMenuString(ID_MENU_EXIT, exitStr, 0);
    m_menu_set.m_main_menu_popup.AppendMenu(MF_STRING, ID_MENU_EXIT, exitStr);
    //为一级菜单添加图标
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu_popup.GetSafeHmenu(), 0, TRUE, m_icon_set.select_folder.GetIcon(true));      //文件
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu_popup.GetSafeHmenu(), 1, TRUE, m_icon_set.play_new.GetIcon(true));           //播放控制
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu_popup.GetSafeHmenu(), 2, TRUE, m_icon_set.show_playlist.GetIcon(true));      //播放列表
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu_popup.GetSafeHmenu(), 3, TRUE, m_icon_set.lyric);                            //歌词
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu_popup.GetSafeHmenu(), 4, TRUE, m_icon_set.playlist_dock);                    //视图
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu_popup.GetSafeHmenu(), 5, TRUE, m_icon_set.setting.GetIcon(true));            //工具
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu_popup.GetSafeHmenu(), 6, TRUE, m_icon_set.help);                             //帮助
    int index = m_menu_set.m_main_menu_popup.GetMenuItemCount();
    CMenuIcon::AddIconToMenuItem(m_menu_set.m_main_menu_popup.GetSafeHmenu(), index - 1, TRUE, m_icon_set.exit);                     //退出

    //将主菜单添加到系统菜单中
    CMenu* pSysMenu = m_pMainWnd->GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        pSysMenu->AppendMenu(MF_SEPARATOR);
        index = pSysMenu->GetMenuItemCount();
        CCommon::AppendMenuOp(pSysMenu->GetSafeHmenu(), m_menu_set.m_main_menu.GetSafeHmenu());		//将主菜单添加到系统菜单
        //为一级菜单添加图标
        CMenuIcon::AddIconToMenuItem(pSysMenu->GetSafeHmenu(), index + 0, TRUE, m_icon_set.select_folder.GetIcon(true));      //文件
        CMenuIcon::AddIconToMenuItem(pSysMenu->GetSafeHmenu(), index + 1, TRUE, m_icon_set.play_new.GetIcon(true));           //播放控制
        CMenuIcon::AddIconToMenuItem(pSysMenu->GetSafeHmenu(), index + 2, TRUE, m_icon_set.show_playlist.GetIcon(true));      //播放列表
        CMenuIcon::AddIconToMenuItem(pSysMenu->GetSafeHmenu(), index + 3, TRUE, m_icon_set.music);                            //歌词
        CMenuIcon::AddIconToMenuItem(pSysMenu->GetSafeHmenu(), index + 4, TRUE, m_icon_set.playlist_dock);                    //视图
        CMenuIcon::AddIconToMenuItem(pSysMenu->GetSafeHmenu(), index + 5, TRUE, m_icon_set.setting.GetIcon(true));            //工具
        CMenuIcon::AddIconToMenuItem(pSysMenu->GetSafeHmenu(), index + 6, TRUE, m_icon_set.help);                             //帮助

        pSysMenu->AppendMenu(MF_SEPARATOR);
        pSysMenu->AppendMenu(MF_STRING, IDM_MINIMODE, CCommon::LoadText(IDS_MINI_MODE2, _T("\tCtrl+M")));
        CMenuIcon::AddIconToMenuItem(pSysMenu->GetSafeHmenu(), IDM_MINIMODE, FALSE, m_icon_set.mini.GetIcon(true));

        CString exitStr;
        m_menu_set.m_main_menu.GetMenuString(ID_MENU_EXIT, exitStr, 0);
        pSysMenu->AppendMenu(MF_STRING, ID_MENU_EXIT, exitStr);
        index = pSysMenu->GetMenuItemCount();
        CMenuIcon::AddIconToMenuItem(pSysMenu->GetSafeHmenu(), index - 1, TRUE, m_icon_set.exit);       //由于“文件”子菜单下已经有一个“退出”，命令，因此这里只能通过菜单项的序号指定

        //添加一个测试命令
#ifdef _DEBUG
        pSysMenu->AppendMenu(MF_STRING, ID_TEST, _T("Test Command"));
        pSysMenu->AppendMenu(MF_STRING, ID_TEST_DIALOG, _T("Test Dialog"));
#endif
    }
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

void CMusicPlayerApp::GetDPIFromWindow(CWnd* pWnd)
{
    CWindowDC dc(pWnd);
    HDC hDC = dc.GetSafeHdc();
    m_dpi = GetDeviceCaps(hDC, LOGPIXELSY);
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

void CMusicPlayerApp::SetAutoRun(bool auto_run)
{
    CRegKey key;
    //打开注册表项
    if (key.Open(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run")) != ERROR_SUCCESS)
    {
        return;
    }
    if (auto_run)		//写入注册表项
    {
        if (key.SetStringValue(APP_NAME, m_module_path_reg.c_str()) != ERROR_SUCCESS)
        {
            return;
        }
    }
    else		//删除注册表项
    {
        //删除前先检查注册表项是否存在，如果不存在，则直接返回
        wchar_t buff[256];
        ULONG size{ 256 };
        if (key.QueryStringValue(APP_NAME, buff, &size) != ERROR_SUCCESS)
            return;
        if (key.DeleteValue(APP_NAME) != ERROR_SUCCESS)
        {
            return;
        }
    }
}

bool CMusicPlayerApp::GetAutoRun()
{
    CRegKey key;
    if (key.Open(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run")) != ERROR_SUCCESS)
    {
        //打开注册表“Software\\Microsoft\\Windows\\CurrentVersion\\Run”失败，则返回false
        return false;
    }
    wchar_t buff[256];
    ULONG size{ 256 };
    if (key.QueryStringValue(APP_NAME, buff, &size) == ERROR_SUCCESS)		//如果找到了APP_NAME键
    {
        return (m_module_path_reg == buff);	//如果APP_NAME的值是当前程序的路径，就返回true，否则返回false
    }
    else
    {
        return false;		//没有找到APP_NAME键，返回false
    }
}

void CMusicPlayerApp::WriteLog(const wstring& log_str, int log_type)
{
    if (((log_type & NonCategorizedSettingData::LT_ERROR) != 0) && ((m_nc_setting_data.debug_log & NonCategorizedSettingData::LT_ERROR) != 0))
        CCommon::WriteLog((m_module_dir + L"error.log").c_str(), log_str);
#ifdef _DEBUG
    if (((log_type & NonCategorizedSettingData::LT_NORMAL) != 0) && ((m_nc_setting_data.debug_log & NonCategorizedSettingData::LT_NORMAL) != 0))
        CCommon::WriteLog((m_module_dir + L"debug.log").c_str(), log_str);
#endif
}

void CMusicPlayerApp::StartUpdateMediaLib(bool refresh)
{
    if (!m_media_lib_updating)
    {
        m_media_lib_updating = true;
        AfxBeginThread([](LPVOID lpParam)->UINT
            {
                CMusicPlayerCmdHelper::CleanUpSongData();
                CMusicPlayerCmdHelper::CleanUpRecentFolders();
                CMusicPlayerCmdHelper::UpdateMediaLib(lpParam);
                theApp.m_media_lib_updating = false;
                return 0;
            }, (LPVOID)refresh);
    }
}

void CMusicPlayerApp::AutoSelectNotifyIcon()
{
    if (CWinVersionHelper::IsWindows10OrLater())
    {
        bool light_mode = CWinVersionHelper::IsWindows10LightTheme();
        if (light_mode)     //浅色模式下，如果图标是白色，则改成黑色
        {
            if (m_app_setting_data.notify_icon_selected == 1)
                m_app_setting_data.notify_icon_selected = 2;
        }
        else     //深色模式下，如果图标是黑色，则改成白色
        {
            if (m_app_setting_data.notify_icon_selected == 2)
                m_app_setting_data.notify_icon_selected = 1;
        }
    }
}

HICON CMusicPlayerApp::GetNotifyIncon(int index)
{
    if (index < 0 || index >= MAX_NOTIFY_ICON)
        index = 0;
    return m_icon_set.notify_icons[index];
}

bool CMusicPlayerApp::IsScintillaLoaded() const
{
    return m_hScintillaModule != NULL;
}

//void CMusicPlayerApp::StartClassifySongData()
//{
//    AfxBeginThread(ClassifySongDataThreadFunc, NULL);
//}

void CMusicPlayerApp::LoadSongData()
{
    CSongDataManager::GetInstance().LoadSongData(m_song_data_path);
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


void CMusicPlayerApp::OnHelpUpdateLog()
{
    // TODO: 在此添加命令处理程序代码
    CString language = CCommon::LoadText(IDS_LANGUAGE_CODE);
    if (language == _T("2"))
        ShellExecute(NULL, _T("open"), _T("https://github.com/zhongyang219/MusicPlayer2/blob/master/Documents/update_log.md"), NULL, NULL, SW_SHOW);
    else
        ShellExecute(NULL, _T("open"), _T("https://github.com/zhongyang219/MusicPlayer2/blob/master/Documents/update_log_en-us.md"), NULL, NULL, SW_SHOW);

}
