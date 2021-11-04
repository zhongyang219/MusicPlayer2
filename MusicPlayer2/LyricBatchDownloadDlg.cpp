// LyricBatchDownloadDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "LyricBatchDownloadDlg.h"
#include "afxdialogex.h"
#include "SongDataManager.h"


// CLyricBatchDownloadDlg 对话框

IMPLEMENT_DYNAMIC(CLyricBatchDownloadDlg, CBaseDialog)

CLyricBatchDownloadDlg::CLyricBatchDownloadDlg(CWnd* pParent /*=NULL*/)
    : CBaseDialog(IDD_LYRIC_BATCH_DOWN_DIALOG, pParent)
{

}

CLyricBatchDownloadDlg::~CLyricBatchDownloadDlg()
{
}

CString CLyricBatchDownloadDlg::GetDialogName() const
{
    return _T("LyricBatchDownloadDlg");
}

void CLyricBatchDownloadDlg::SaveConfig() const
{
    CIniHelper ini(theApp.m_config_path);
    ini.WriteInt(L"lyric_batch_download", L"save_as_utf8", static_cast<int>(m_save_code));
    ini.WriteBool(L"lyric_batch_download", L"download_translate", m_download_translate);
    ini.WriteBool(L"lyric_batch_download", L"save_to_song_folder", m_save_to_song_folder);
    ini.Save();
}

void CLyricBatchDownloadDlg::LoadConfig()
{
    CIniHelper ini(theApp.m_config_path);
    m_save_code = static_cast<CodeType>(ini.GetInt(L"lyric_batch_download", L"save_as_utf8", 1));
    m_download_translate = ini.GetBool(L"lyric_batch_download", L"download_translate", true);
    m_save_to_song_folder = ini.GetBool(L"lyric_batch_download", L"save_to_song_folder", true);
}

void CLyricBatchDownloadDlg::EnableControls(bool enable)
{
    m_skip_exist_check.EnableWindow(enable);
    m_save_code_combo.EnableWindow(enable);
    m_download_translate_chk.EnableWindow(enable);
    GetDlgItem(IDC_SAVE_TO_SONG_FOLDER)->EnableWindow(enable);
    if (m_lyric_path_not_exit)
        GetDlgItem(IDC_SAVE_TO_LYRIC_FOLDER)->EnableWindow(FALSE);
    else
        GetDlgItem(IDC_SAVE_TO_LYRIC_FOLDER)->EnableWindow(enable);
    GetDlgItem(IDC_START_DOWNLOAD)->EnableWindow(enable);
}

bool CLyricBatchDownloadDlg::SaveLyric(const wchar_t* path, const wstring& lyric_wcs, CodeType code_type, bool* char_cannot_convert)
{
    string lyric_str = CCommon::UnicodeToStr(lyric_wcs, code_type, char_cannot_convert);
    ofstream out_put{ path, std::ios::binary };
    if (out_put.fail())
        return false;
    out_put << lyric_str;
    return true;
}

void CLyricBatchDownloadDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SKIP_EXIST_CHECK, m_skip_exist_check);
    DDX_Control(pDX, IDC_COMBO1, m_save_code_combo);
    DDX_Control(pDX, IDC_SONG_LIST1, m_song_list_ctrl);
    DDX_Control(pDX, IDC_DOWNLOAD_TRASNLATE_CHECK2, m_download_translate_chk);
    DDX_Control(pDX, IDC_INFO_STATIC, m_info_static);
    DDX_Control(pDX, IDC_PROGRESS_BAR, m_progress_bar);
}


BEGIN_MESSAGE_MAP(CLyricBatchDownloadDlg, CBaseDialog)
    ON_BN_CLICKED(IDC_START_DOWNLOAD, &CLyricBatchDownloadDlg::OnBnClickedStartDownload)
    ON_BN_CLICKED(IDC_SKIP_EXIST_CHECK, &CLyricBatchDownloadDlg::OnBnClickedSkipExistCheck)
    ON_WM_DESTROY()
    ON_CBN_SELCHANGE(IDC_COMBO1, &CLyricBatchDownloadDlg::OnCbnSelchangeCombo1)
    ON_BN_CLICKED(IDC_DOWNLOAD_TRASNLATE_CHECK2, &CLyricBatchDownloadDlg::OnBnClickedDownloadTrasnlateCheck2)
    ON_MESSAGE(WM_BATCH_DOWNLOAD_COMPLATE, &CLyricBatchDownloadDlg::OnBatchDownloadComplate)
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_SAVE_TO_SONG_FOLDER, &CLyricBatchDownloadDlg::OnBnClickedSaveToSongFolder)
    ON_BN_CLICKED(IDC_SAVE_TO_LYRIC_FOLDER, &CLyricBatchDownloadDlg::OnBnClickedSaveToLyricFolder)
END_MESSAGE_MAP()


// CLyricBatchDownloadDlg 消息处理程序


BOOL CLyricBatchDownloadDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    SetIcon(theApp.m_icon_set.download1, FALSE);
    SetIcon(AfxGetApp()->LoadIcon(IDI_DOWNLOAD1_D), FALSE);
    SetButtonIcon(IDC_START_DOWNLOAD, theApp.m_icon_set.download1);

    CenterWindow();

    LoadConfig();

    //设置列表控件主题颜色
    //m_song_list_ctrl.SetColor(theApp.m_app_setting_data.theme_color);

    //初始化控件的状态
    m_skip_exist_check.SetCheck(m_skip_exist);
    m_save_code_combo.AddString(_T("ANSI"));
    m_save_code_combo.AddString(_T("UTF-8"));
    m_save_code_combo.SetCurSel(static_cast<int>(m_save_code));
    m_download_translate_chk.SetCheck(m_download_translate);
    if (m_save_to_song_folder)
        ((CButton*)GetDlgItem(IDC_SAVE_TO_SONG_FOLDER))->SetCheck(TRUE);
    else
        ((CButton*)GetDlgItem(IDC_SAVE_TO_LYRIC_FOLDER))->SetCheck(TRUE);
    //判断歌词文件夹是否存在
    bool lyric_path_exist = CCommon::FolderExist(theApp.m_lyric_setting_data.lyric_path);
    if (!lyric_path_exist)		//如果歌词文件不存在，则禁用“保存到歌词文件夹”单选按钮，并强制选中“保存到歌曲所在目录”
    {
        ((CButton*)GetDlgItem(IDC_SAVE_TO_LYRIC_FOLDER))->EnableWindow(FALSE);
        ((CButton*)GetDlgItem(IDC_SAVE_TO_LYRIC_FOLDER))->SetCheck(FALSE);
        ((CButton*)GetDlgItem(IDC_SAVE_TO_SONG_FOLDER))->SetCheck(TRUE);
        m_save_to_song_folder = true;
        m_lyric_path_not_exit = true;
    }

    //初始化歌曲列表控件
    //设置各列的宽度
    CRect rect;
    m_song_list_ctrl.GetWindowRect(rect);
    int width0, width1, width2, width3, width4;
    width0 = rect.Width() / 10;
    width1 = rect.Width() * 2 / 10;
    width2 = rect.Width() * 2 / 10;
    width3 = rect.Width() * 3 / 10;
    width4 = rect.Width() - width0 - width1 - width2 - width3 - theApp.DPI(20) - 1;
    //插入列
    m_song_list_ctrl.SetExtendedStyle(m_song_list_ctrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
    m_song_list_ctrl.InsertColumn(0, CCommon::LoadText(IDS_NUMBER), LVCFMT_LEFT, width0);		//插入第1列
    m_song_list_ctrl.InsertColumn(1, CCommon::LoadText(IDS_TITLE), LVCFMT_LEFT, width1);		//插入第2列
    m_song_list_ctrl.InsertColumn(2, CCommon::LoadText(IDS_ARTIST), LVCFMT_LEFT, width2);		//插入第3列
    m_song_list_ctrl.InsertColumn(3, CCommon::LoadText(IDS_FILE_NAME), LVCFMT_LEFT, width3);		//插入第3列
    m_song_list_ctrl.InsertColumn(4, CCommon::LoadText(IDS_STATE), LVCFMT_LEFT, width4);		//插入第4列
    //插入项目
    for (size_t i{}; i < m_playlist.size(); i++)
    {
        CString tmp;
        tmp.Format(_T("%d"), i + 1);
        m_song_list_ctrl.InsertItem(i, tmp);
        m_song_list_ctrl.SetItemText(i, 1, m_playlist[i].GetTitle().c_str());
        m_song_list_ctrl.SetItemText(i, 2, m_playlist[i].GetArtist().c_str());
        m_song_list_ctrl.SetItemText(i, 3, m_playlist[i].GetFileName().c_str());
    }

    m_progress_bar.SetBackgroundColor(GetSysColor(COLOR_BTNFACE));
    m_progress_bar.ShowWindow(SW_HIDE);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CLyricBatchDownloadDlg::OnBnClickedStartDownload()
{
    // TODO: 在此添加控件通知处理程序代码
    m_progress_bar.ShowWindow(SW_SHOW);

    //先清除“状态”一列的内容
    for (size_t i{}; i < m_playlist.size(); i++)
    {
        m_song_list_ctrl.SetItemText(i, 4, _T(""));
    }

    EnableControls(false);		//禁用控件

    //设置要向歌词下载工作线程传递的数据
    m_thread_info.hwnd = GetSafeHwnd();
    m_thread_info.download_translate = m_download_translate;
    m_thread_info.save_to_song_folder = m_save_to_song_folder;
    m_thread_info.skip_exist = m_skip_exist;
    m_thread_info.save_code = m_save_code;
    m_thread_info.list_ctrl = &m_song_list_ctrl;
    m_thread_info.static_ctrl = &m_info_static;
    m_thread_info.progress_bar = &m_progress_bar;
    m_thread_info.playlist = &m_playlist;
    theApp.m_batch_download_dialog_exit = false;

    //创建歌词批量下载的工作线程
    m_pThread = AfxBeginThread(ThreadFunc, &m_thread_info);
}


void CLyricBatchDownloadDlg::OnBnClickedSkipExistCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_skip_exist = (m_skip_exist_check.GetCheck() != 0);
}


void CLyricBatchDownloadDlg::OnDestroy()
{
    CBaseDialog::OnDestroy();

    // TODO: 在此处添加消息处理程序代码
    SaveConfig();

}


void CLyricBatchDownloadDlg::OnCbnSelchangeCombo1()
{
    // TODO: 在此添加控件通知处理程序代码
    //获取组合框中选中的编码格式
    switch (m_save_code_combo.GetCurSel())
    {
    case 1: m_save_code = CodeType::UTF8; break;
    default: m_save_code = CodeType::ANSI; break;
    }
}


void CLyricBatchDownloadDlg::OnBnClickedDownloadTrasnlateCheck2()
{
    // TODO: 在此添加控件通知处理程序代码
    m_download_translate = (m_download_translate_chk.GetCheck() != 0);
}

//工作线程函数
UINT CLyricBatchDownloadDlg::ThreadFunc(LPVOID lpParam)
{
    CCommon::SetThreadLanguage(theApp.m_general_setting_data.language);
    ThreadInfo* pInfo = (ThreadInfo*)lpParam;

    //依次下载列表中每一首歌曲的歌词
    for (size_t i{}; i < pInfo->playlist->size(); i++)
    {
        if (theApp.m_batch_download_dialog_exit)
            return 0;
        CString info;
        int percent = i * 100 / pInfo->playlist->size();
        info = CCommon::LoadTextFormat(IDS_LYRIC_BATCH_DOWNLOADING_INFO, { percent });
        pInfo->static_ctrl->SetWindowText(info);
        pInfo->progress_bar->SetProgress(percent);
        //SetDlgItemText(IDC_INFO_STATIC, info);

        pInfo->list_ctrl->SetItemText(i, 4, CCommon::LoadText(IDS_DOWNLOADING));
        pInfo->list_ctrl->EnsureVisible(i, FALSE);

        //设置要保存的歌词的路径
        wstring lyric_path;
        wstring file_name;
        wstring dir;
        dir = CFilePathHelper(pInfo->playlist->at(i).file_path).GetDir();
        if (!pInfo->playlist->at(i).is_cue)
            file_name = pInfo->playlist->at(i).GetFileName();
        else
            file_name = pInfo->playlist->at(i).artist + L" - " + pInfo->playlist->at(i).title + L".lrc";
        if (pInfo->save_to_song_folder)
            lyric_path = dir + file_name;
        else
            lyric_path = theApp.m_lyric_setting_data.lyric_path + file_name;
        size_t index = lyric_path.rfind(L'.');		//查找文件名最后一个点
        lyric_path = lyric_path.substr(0, index + 1) + L"lrc";	//将文件名的扩展名改为lrc

        //判断歌词是否已经存在
        bool lyric_exist = CCommon::FileExist(lyric_path) || (!pInfo->playlist->at(i).lyric_file.empty());
        if (pInfo->skip_exist && lyric_exist)		//如果设置了跳过已存在歌词的曲目，并且歌词已经存在，则跳过它
        {
            pInfo->list_ctrl->SetItemText(i, 4, CCommon::LoadText(IDS_SKIPED));
            continue;
        }

        //设置搜索关键字
        wstring search_result;		//查找歌曲返回的结果
        wstring lyric_str;			//下载好的歌词
        wstring keyword;		//查找的关键字
        if (pInfo->playlist->at(i).IsTitleEmpty())		//如果没有标题信息，就把文件名设为搜索关键字
        {
            keyword = pInfo->playlist->at(i).GetFileName();
            size_t index = keyword.rfind(L'.');		//查找最后一个点
            keyword = keyword.substr(0, index);		//去掉扩展名
        }
        else if (pInfo->playlist->at(i).IsArtistEmpty())	//如果有标题信息但是没有艺术家信息，就把标题设为搜索关键字
        {
            keyword = pInfo->playlist->at(i).title;
        }
        else		//否则将“艺术家 标题”设为搜索关键字
        {
            keyword = pInfo->playlist->at(i).artist + L' ' + pInfo->playlist->at(i).title;
        }

        //搜索歌曲
        wstring keyword_url = CInternetCommon::URLEncode(keyword);		//将搜索关键字转换成URL编码
        CString url;
        url.Format(L"http://music.163.com/api/search/get/?s=%s&limit=20&type=1&offset=0", keyword_url.c_str());
        int rtn = CInternetCommon::HttpPost(wstring(url), search_result);		//向网易云音乐的歌曲搜索API发送http的POST请求
        if (theApp.m_batch_download_dialog_exit)		//由于CLyricDownloadCommon::HttpPost函数执行的时间比较长，所有在这里执行判断是否退出线程的处理
            return 0;
        if (rtn != 0)
        {
            pInfo->list_ctrl->SetItemText(i, 4, CCommon::LoadText(IDS_NETWORK_CONNECTION_FAILED));
            continue;
        }

        //处理返回结果
        SongInfo& song_info_ori{ CSongDataManager::GetInstance().GetSongInfoRef2(pInfo->playlist->at(i).file_path) };
        vector<CInternetCommon::ItemInfo> down_list;
        CInternetCommon::DisposeSearchResult(down_list, search_result);		//处理返回的查找结果，并将结果保存在down_list容器里
        if (down_list.empty())
        {
            pInfo->list_ctrl->SetItemText(i, 4, CCommon::LoadText(IDS_CANNOT_FIND_THIS_SONG));
            song_info_ori.SetNoOnlineLyric(true);
            continue;
        }

        //计算最佳选择项
        wstring title = pInfo->playlist->at(i).title;
        wstring artist = pInfo->playlist->at(i).artist;
        wstring album = pInfo->playlist->at(i).album;
        if (title == CCommon::LoadText(IDS_DEFAULT_TITLE).GetString()) title.clear();
        if (artist == CCommon::LoadText(IDS_DEFAULT_ARTIST).GetString()) artist.clear();
        if (album == CCommon::LoadText(IDS_DEFAULT_ALBUM).GetString()) album.clear();
        int best_matched = CInternetCommon::SelectMatchedItem(down_list, title, artist, album, pInfo->playlist->at(i).GetFileName(), true);
        if (best_matched < 0)
        {
            song_info_ori.SetNoOnlineLyric(true);
            pInfo->list_ctrl->SetItemText(i, 4, CCommon::LoadText(IDS_NO_MATCHED_LYRIC));
            continue;
        }

        //下载歌词
        CLyricDownloadCommon::DownloadLyric(down_list[best_matched].id, lyric_str, pInfo->download_translate);
        if (lyric_str.empty())
        {
            pInfo->list_ctrl->SetItemText(i, 4, CCommon::LoadText(IDS_LYRIC_DOWNLOAD_FAILED));
            continue;
        }

        //处理歌词文本
        if (!CLyricDownloadCommon::DisposeLryic(lyric_str))
        {
            pInfo->list_ctrl->SetItemText(i, 4, CCommon::LoadText(IDS_SONG_NO_LYRIC));
            continue;
        }

        song_info_ori.SetSongId(down_list[best_matched].id);

        //在歌词前面添加标签
        CLyricDownloadCommon::AddLyricTag(lyric_str, down_list[best_matched].id, down_list[best_matched].title, down_list[best_matched].artist, down_list[best_matched].album);

        //保存歌词
        bool char_cannot_convert;
        if (CLyricBatchDownloadDlg::SaveLyric(lyric_path.c_str(), lyric_str, pInfo->save_code, &char_cannot_convert))
        {
            if (char_cannot_convert)
                pInfo->list_ctrl->SetItemText(i, 4, CCommon::LoadText(IDS_DOWNLOAD_ENCODE_WARNING));		//char_cannot_convert为true，则说明有无法转换的Unicode字符
            else
                pInfo->list_ctrl->SetItemText(i, 4, CCommon::LoadText(IDS_SUCCESSED));
        }
        else
        {
            pInfo->list_ctrl->SetItemText(i, 4, CCommon::LoadText(IDS_CANNOT_WRITE_TO_FILE));
        }

        if (pInfo->download_translate)
        {
            CLyrics lyrics{ lyric_path };		//打开保存过的歌词
            lyrics.DeleteRedundantLyric();		//删除多余的歌词
            lyrics.CombineSameTimeLyric();		//将歌词翻译和原始歌词合并成一句
            lyrics.SaveLyric2();
        }

    }
    ::PostMessage(pInfo->hwnd, WM_BATCH_DOWNLOAD_COMPLATE, 0, 0);
    return 0;
}


afx_msg LRESULT CLyricBatchDownloadDlg::OnBatchDownloadComplate(WPARAM wParam, LPARAM lParam)
{
    SetDlgItemText(IDC_INFO_STATIC, CCommon::LoadText(IDS_DOWNLOAD_COMPLETE));
    //下载完成后重新载入歌词
    CPlayer::GetInstance().SearchLyrics(true);
    CPlayer::GetInstance().IniLyrics();
    EnableControls(true);		//启用控件
    m_progress_bar.SetProgress(100);
    return 0;
}


void CLyricBatchDownloadDlg::OnClose()
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    CBaseDialog::OnClose();
}


void CLyricBatchDownloadDlg::OnCancel()
{
    // TODO: 在此添加专用代码和/或调用基类
    //对话框将要关闭时，将退出标志置为true
    theApp.m_batch_download_dialog_exit = true;
    int rtn;
    if (m_pThread != nullptr)
    {
        rtn = WaitForSingleObject(m_pThread->m_hThread, 2000);	//等待线程退出
#ifdef DEBUG
        CString info;
        info.Format(CCommon::LoadText(IDS_RETURN_VALUE_OF_WAIT_FOR_SINGLE_OBJECT), rtn);
        MessageBox(info, NULL, MB_ICONINFORMATION);
#endif // DEBUG

    }
    DestroyWindow();
    //CBaseDialog::OnCancel();
}


void CLyricBatchDownloadDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    //对话框将要关闭时，将退出标志置为true
    theApp.m_batch_download_dialog_exit = true;
    if (m_pThread != nullptr)
        WaitForSingleObject(m_pThread->m_hThread, 2000);	//等待线程退出

    CBaseDialog::OnOK();
}


void CLyricBatchDownloadDlg::OnBnClickedSaveToSongFolder()
{
    // TODO: 在此添加控件通知处理程序代码
    m_save_to_song_folder = true;
}


void CLyricBatchDownloadDlg::OnBnClickedSaveToLyricFolder()
{
    // TODO: 在此添加控件通知处理程序代码
    m_save_to_song_folder = false;
}
