// CoverDownloadDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "Player.h"
#include "CoverDownloadDlg.h"
#include "SongDataManager.h"
#include "IniHelper.h"


// CCoverDownloadDlg 对话框

IMPLEMENT_DYNAMIC(CCoverDownloadDlg, CBaseDialog)

CCoverDownloadDlg::CCoverDownloadDlg(CWnd* pParent /*=nullptr*/)
    : CBaseDialog(IDD_COVER_DOWNLOAD_DIALOG, pParent)
{

}

CCoverDownloadDlg::~CCoverDownloadDlg()
{
}

UINT CCoverDownloadDlg::SongSearchThreadFunc(LPVOID lpParam)
{
    CCommon::SetThreadLanguageList(theApp.m_str_table.GetLanguageTag());
    CCoverDownloadDlg* pThis = (CCoverDownloadDlg*)lpParam;
    wstring search_result;
    wstring m_search_url = pThis->m_search_url;
    bool m_search_rtn = CInternetCommon::HttpPost(m_search_url, search_result);     //向网易云音乐的歌曲搜索API发送http的POST请求
    if (theApp.m_cover_download_dialog_exit)
        return 0;
    pThis->m_search_rtn = m_search_rtn;
    pThis->m_search_result = search_result;
    ::PostMessage(pThis->m_hWnd, WM_SEARCH_COMPLATE, 0, 0);		//搜索完成后发送一个搜索完成的消息
    return 0;
}

UINT CCoverDownloadDlg::CoverDownloadThreadFunc(LPVOID lpParam)
{
    CCommon::SetThreadLanguageList(theApp.m_str_table.GetLanguageTag());
    CCoverDownloadDlg* pThis = (CCoverDownloadDlg*)lpParam;
    CInternetCommon::ItemInfo match_item = pThis->m_down_list[pThis->m_item_selected];
    wstring song_id = match_item.id;
    if (song_id.empty())
        return 0;
    wstring cover_url = CCoverDownloadCommon::GetAlbumCoverURL(song_id);
    if (cover_url.empty())
    {
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_NETWORK_COVER_DOWNLOAD_FAILED");
        AfxMessageBox(info.c_str(), MB_ICONWARNING | MB_OK);
    }

    //获取要保存的专辑封面的文件路径
    CFilePathHelper cover_file_path;
    wstring album_name;
    if (match_item.album == pThis->m_song.album)        // 如果在线搜索结果的唱片集名称和歌曲的相同，则以“唱片集”为文件名保存
        album_name = match_item.album;
    else                                                // 否则以歌曲文件名为文件名保存
        album_name = pThis->m_song.GetFileName();
    CCommon::FileNameNormalize(album_name);
    CFilePathHelper url_path(cover_url);
    cover_file_path.SetFilePath(pThis->GetSavedDir() + album_name + url_path.GetFileExtension(false, true));

    //下面专辑封面
    if (CCommon::FileExist(cover_file_path.GetFilePath()))
        ::DeleteFile(cover_file_path.GetFilePath().c_str());
    if (CCommon::FileExist(pThis->m_org_album_cover_path))
        ::DeleteFile(pThis->m_org_album_cover_path.c_str());
    URLDownloadToFile(0, cover_url.c_str(), cover_file_path.GetFilePath().c_str(), 0, NULL);

    //将下载的专辑封面改为隐藏属性
    if (pThis->m_save_to_song_folder)
        SetFileAttributes(cover_file_path.GetFilePath().c_str(), FILE_ATTRIBUTE_HIDDEN);

    ::PostMessage(pThis->m_hWnd, WM_DOWNLOAD_COMPLATE, 0, 0);		//下载完成后发送一个搜索完成的消息

    return 0;
}

void CCoverDownloadDlg::SaveConfig() const
{
    CIniHelper ini(theApp.m_config_path);
    ini.WriteBool(L"album_download", L"save_to_song_folder", m_save_to_song_folder);
    ini.Save();
}

void CCoverDownloadDlg::LoadConfig()
{
    CIniHelper ini(theApp.m_config_path);
    m_save_to_song_folder = ini.GetBool(L"album_download", L"save_to_song_folder", true);
}

void CCoverDownloadDlg::SetID(wstring id)
{
    SongInfo song_info_ori{ CSongDataManager::GetInstance().GetSongInfo3(m_song) };
    song_info_ori.SetSongId(id);
    CSongDataManager::GetInstance().AddItem(song_info_ori);
}

wstring CCoverDownloadDlg::GetSavedDir()
{
    if (m_save_to_song_folder || !CCommon::FolderExist(theApp.m_app_setting_data.album_cover_path))
        return CFilePathHelper(m_song.file_path).GetDir();
    else
        return theApp.m_app_setting_data.album_cover_path;
}

SongInfo CCoverDownloadDlg::GetSongInfo() const
{
    return CPlayer::GetInstance().GetCurrentSongInfo();
}

CString CCoverDownloadDlg::GetDialogName() const
{
    return _T("CoverDownloadDlg");
}

bool CCoverDownloadDlg::InitializeControls()
{
    SetIcon(IconMgr::IconType::IT_Album_Cover, FALSE);
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TITLE_COVER_DL");
    SetWindowTextW(temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_LYRIC_DL_TITLE");
    SetDlgItemTextW(IDC_TXT_COVER_DL_TITLE_STATIC, temp.c_str());
    // IDC_TITLE_EDIT
    temp = theApp.m_str_table.LoadText(L"TXT_LYRIC_DL_SEARCH");
    SetDlgItemTextW(IDC_SEARCH_BUTTON, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_LYRIC_DL_ARTIST");
    SetDlgItemTextW(IDC_TXT_COVER_DL_ARTIST_STATIC, temp.c_str());
    // IDC_ARTIST_EDIT
    temp = theApp.m_str_table.LoadText(L"TXT_LYRIC_DL_INFO");
    SetDlgItemTextW(IDC_STATIC_INFO, temp.c_str());
    temp = L"<a>" + theApp.m_str_table.LoadText(L"TXT_LYRIC_DL_UNLINK") + L"</a>";
    SetDlgItemTextW(IDC_UNASSOCIATE_LINK, temp.c_str());
    // IDC_COVER_DOWN_LIST
    temp = theApp.m_str_table.LoadText(L"TXT_LYRIC_DL_OPT");
    SetDlgItemTextW(IDC_DOWNLOAD_OPTION_GROUPBOX, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_COVER_DL_LOCATION_SEL");
    SetDlgItemTextW(IDC_COVER_LOCATION_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_COVER_DL_LOCATION_FOLDER_SONG");
    SetDlgItemTextW(IDC_SAVE_TO_SONG_FOLDER2, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_COVER_DL_LOCATION_FOLDER_COVER");
    SetDlgItemTextW(IDC_SAVE_TO_ALBUM_FOLDER2, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_LYRIC_DL_SEL_DL");
    SetDlgItemTextW(IDC_DOWNLOAD_SELECTED, temp.c_str());

    SetButtonIcon(IDC_SEARCH_BUTTON, IconMgr::IconType::IT_Find);
    SetButtonIcon(IDC_DOWNLOAD_SELECTED, IconMgr::IconType::IT_Download);

    RepositionTextBasedControls({
        { CtrlTextInfo::L1, IDC_TXT_COVER_DL_TITLE_STATIC },
        { CtrlTextInfo::C0, IDC_TITLE_EDIT },
        { CtrlTextInfo::R1, IDC_SEARCH_BUTTON, CtrlTextInfo::W32 },
        { CtrlTextInfo::L1, IDC_TXT_COVER_DL_ARTIST_STATIC },
        { CtrlTextInfo::C0, IDC_ARTIST_EDIT }
        }, CtrlTextInfo::W64);
    RepositionTextBasedControls({
        { CtrlTextInfo::C0, IDC_STATIC_INFO },
        { CtrlTextInfo::R1, IDC_UNASSOCIATE_LINK }
        }, CtrlTextInfo::W128);
    RepositionTextBasedControls({
        { CtrlTextInfo::R1, IDC_DOWNLOAD_SELECTED, CtrlTextInfo::W32 },
        { CtrlTextInfo::R2, IDCANCEL, CtrlTextInfo::W32 }
        });
    return true;
}

void CCoverDownloadDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COVER_DOWN_LIST, m_down_list_ctrl);
    DDX_Control(pDX, IDC_UNASSOCIATE_LINK, m_unassciate_lnk);
}

void CCoverDownloadDlg::ShowDownloadList()
{
    m_down_list_ctrl.DeleteAllItems();
    for (size_t i{}; i < m_down_list.size(); i++)
    {
        CString tmp;
        tmp.Format(_T("%d"), i + 1);
        m_down_list_ctrl.InsertItem(i, tmp);
        m_down_list_ctrl.SetItemText(i, 1, m_down_list[i].title.c_str());
        m_down_list_ctrl.SetItemText(i, 2, m_down_list[i].artist.c_str());
        m_down_list_ctrl.SetItemText(i, 3, m_down_list[i].album.c_str());
    }
}


BEGIN_MESSAGE_MAP(CCoverDownloadDlg, CBaseDialog)
    ON_BN_CLICKED(IDC_SEARCH_BUTTON, &CCoverDownloadDlg::OnBnClickedSearchButton)
    ON_MESSAGE(WM_SEARCH_COMPLATE, &CCoverDownloadDlg::OnSearchComplate)
    ON_BN_CLICKED(IDC_DOWNLOAD_SELECTED, &CCoverDownloadDlg::OnBnClickedDownloadSelected)
    ON_NOTIFY(NM_CLICK, IDC_COVER_DOWN_LIST, &CCoverDownloadDlg::OnNMClickCoverDownList)
    ON_NOTIFY(NM_DBLCLK, IDC_COVER_DOWN_LIST, &CCoverDownloadDlg::OnNMDblclkCoverDownList)
    ON_NOTIFY(NM_RCLICK, IDC_COVER_DOWN_LIST, &CCoverDownloadDlg::OnNMRClickCoverDownList)
    ON_MESSAGE(WM_DOWNLOAD_COMPLATE, &CCoverDownloadDlg::OnDownloadComplate)
    ON_EN_CHANGE(IDC_TITLE_EDIT, &CCoverDownloadDlg::OnEnChangeTitleEdit)
    ON_EN_CHANGE(IDC_ARTIST_EDIT, &CCoverDownloadDlg::OnEnChangeArtistEdit)
    ON_NOTIFY(NM_CLICK, IDC_UNASSOCIATE_LINK, &CCoverDownloadDlg::OnNMClickUnassociateLink)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_SAVE_TO_SONG_FOLDER2, &CCoverDownloadDlg::OnBnClickedSaveToSongFolder2)
    ON_BN_CLICKED(IDC_SAVE_TO_ALBUM_FOLDER2, &CCoverDownloadDlg::OnBnClickedSaveToAlbumFolder2)
END_MESSAGE_MAP()


// CCoverDownloadDlg 消息处理程序


BOOL CCoverDownloadDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    LoadConfig();

    m_song = GetSongInfo(); // 初始化复制Songinfo使用，防止随着播放GetSongInfo获取到另一首
    m_org_album_cover_path = CPlayer::GetInstance().GetAlbumCoverPath();

    m_title = m_song.title;
    m_artist = m_song.artist;
    m_album = m_song.album;

    if (m_song.IsTitleEmpty())    // 如果没有标题信息，就把文件名设为标题
    {
        m_title = m_song.GetFileName();
        size_t index = m_title.rfind(L'.');
        m_title = m_title.substr(0, index);
    }
    if (m_song.IsArtistEmpty())	//没有艺术家信息，清空艺术家的文本
    {
        m_artist.clear();
    }
    if (m_song.IsAlbumEmpty())	//没有唱片集信息，清空唱片集的文本
    {
        m_album.clear();
    }
    m_file_name = m_song.GetFileName();

    SetDlgItemText(IDC_TITLE_EDIT, m_title.c_str());
    SetDlgItemText(IDC_ARTIST_EDIT, m_artist.c_str());

    ////设置列表控件主题颜色
    //m_down_list_ctrl.SetColor(theApp.m_app_setting_data.theme_color);

    //初始化搜索结果列表控件
    CRect rect;
    m_down_list_ctrl.GetWindowRect(rect);
    int width0, width1, width2, width3;
    width0 = rect.Width() / 10;
    width1 = rect.Width() * 3 / 10;
    width2 = rect.Width() * 2 / 10;
    width3 = rect.Width() - theApp.DPI(20) - 1 - width0 - width1 - width2;

    m_down_list_ctrl.SetExtendedStyle(m_down_list_ctrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
    m_down_list_ctrl.InsertColumn(0, theApp.m_str_table.LoadText(L"TXT_SERIAL_NUMBER").c_str(), LVCFMT_LEFT, width0);
    m_down_list_ctrl.InsertColumn(1, theApp.m_str_table.LoadText(L"TXT_TITLE").c_str(), LVCFMT_LEFT, width1);
    m_down_list_ctrl.InsertColumn(2, theApp.m_str_table.LoadText(L"TXT_ARTIST").c_str(), LVCFMT_LEFT, width2);
    m_down_list_ctrl.InsertColumn(3, theApp.m_str_table.LoadText(L"TXT_ALBUM").c_str(), LVCFMT_LEFT, width3);

    m_unassciate_lnk.ShowWindow(SW_HIDE);

    if (m_save_to_song_folder)
        ((CButton*)GetDlgItem(IDC_SAVE_TO_SONG_FOLDER2))->SetCheck(TRUE);
    else
        ((CButton*)GetDlgItem(IDC_SAVE_TO_ALBUM_FOLDER2))->SetCheck(TRUE);

    //判断封面文件夹是否存在
    bool lyric_path_exist = CCommon::FolderExist(theApp.m_app_setting_data.album_cover_path);
    if (!lyric_path_exist)		//如果封面文件夹不存在，则禁用“保存到封面文件夹”单选按钮，并强制选中“保存到歌曲所在目录”
    {
        ((CButton*)GetDlgItem(IDC_SAVE_TO_ALBUM_FOLDER2))->EnableWindow(FALSE);
        ((CButton*)GetDlgItem(IDC_SAVE_TO_ALBUM_FOLDER2))->SetCheck(FALSE);
        ((CButton*)GetDlgItem(IDC_SAVE_TO_SONG_FOLDER2))->SetCheck(TRUE);
        m_save_to_song_folder = true;
    }

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CCoverDownloadDlg::OnBnClickedSearchButton()
{
    // TODO: 在此添加控件通知处理程序代码
    SetDlgItemText(IDC_STATIC_INFO, theApp.m_str_table.LoadText(L"TXT_LYRIC_DL_INFO_SEARCHING").c_str());    // 这里使用的是歌词下载对话框的字符串
    GetDlgItem(IDC_SEARCH_BUTTON)->EnableWindow(FALSE);		//点击“搜索”后禁用该按钮
    wstring keyword = CInternetCommon::URLEncode(m_artist + L' ' + m_title);	//搜索关键字为“艺术家 标题”，并将其转换成URL编码
    CString url;
    url.Format(L"http://music.163.com/api/search/get/?s=%s&limit=%d&type=1&offset=0", keyword.c_str(), 30);
    //int rtn = CLyricDownloadCommon::HttpPost(buff, m_search_result);		//向网易云音乐的歌曲搜索API发送http的POST请求
    m_search_url = url;
    theApp.m_cover_download_dialog_exit = false;
    m_pSearchThread = AfxBeginThread(SongSearchThreadFunc, this);
}


afx_msg LRESULT CCoverDownloadDlg::OnSearchComplate(WPARAM wParam, LPARAM lParam)
{
    //响应WM_SEARCH_CONPLATE消息
    GetDlgItem(IDC_SEARCH_BUTTON)->EnableWindow(TRUE);	//搜索完成之后启用该按钮
    switch (m_search_rtn)
    {
    case CInternetCommon::FAILURE:
    {
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_NETWORK_SEARCH_FAILED");
        MessageBox(info.c_str(), NULL, MB_ICONWARNING);
        return 0;
    }
    case CInternetCommon::OUTTIME:
    {
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_NETWORK_SEARCH_TIME_OUT");
        MessageBox(info.c_str(), NULL, MB_ICONWARNING);
        return 0;
    }
    default: break;
    }

    CInternetCommon::DisposeSearchResult(m_down_list, m_search_result);		//处理返回的结果
    ShowDownloadList();			//将搜索的结果显示在列表控件中

    //计算搜索结果中最佳匹配项目
    int best_matched;
    bool id_releated{ false };
    CSongDataManager::GetInstance().GetSongID(m_song, m_song.song_id);  // 从媒体库读取id
    if (m_song.song_id != 0)        // 如果当前歌曲已经有关联的ID，则根据该ID在搜索结果列表中查找对应的项目
    {
        for (size_t i{}; i < m_down_list.size(); i++)
        {
            if (m_song.GetSongId() == m_down_list[i].id)
            {
                id_releated = true;
                best_matched = i;
                break;
            }
        }
    }
    if (!id_releated)
        best_matched = CInternetCommon::SelectMatchedItem(m_down_list, m_title, m_artist, m_album, m_file_name, true);
    wstring info;
    m_unassciate_lnk.ShowWindow(SW_HIDE);
    if (m_down_list.empty())
        info = theApp.m_str_table.LoadText(L"TXT_LYRIC_DL_INFO_SEARCH_NO_SONG");
    else if (best_matched == -1)
        info = theApp.m_str_table.LoadText(L"TXT_LYRIC_DL_INFO_SEARCH_NO_MATCHED");
    else if (id_releated)
    {
        info = theApp.m_str_table.LoadTextFormat(L"TXT_LYRIC_DL_INFO_SEARCH_RELATED", { best_matched + 1 });
        m_unassciate_lnk.ShowWindow(SW_SHOW);
    }
    else
        info = theApp.m_str_table.LoadTextFormat(L"TXT_LYRIC_DL_INFO_SEARCH_BEST_MATCHED", { best_matched + 1 });
    SetDlgItemText(IDC_STATIC_INFO, info.c_str());
    //自动选中列表中最佳匹配的项目
    m_down_list_ctrl.SetFocus();
    m_down_list_ctrl.SetItemState(best_matched, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);	//选中行
    m_down_list_ctrl.EnsureVisible(best_matched, FALSE);		//使选中行保持可见
    m_item_selected = best_matched;
    return 0;
}


void CCoverDownloadDlg::OnBnClickedDownloadSelected()
{
    // TODO: 在此添加控件通知处理程序代码
    if (m_item_selected < 0 || m_item_selected >= static_cast<int>(m_down_list.size())) return;
    GetDlgItem(IDC_DOWNLOAD_SELECTED)->EnableWindow(FALSE);     // 点击“下载选中项”后禁用该按钮
    SetID(m_down_list[m_item_selected].id);                     // 将选中项目的歌曲ID关联到歌曲
    m_pDownThread = AfxBeginThread(CoverDownloadThreadFunc, this);
}


void CCoverDownloadDlg::OnNMClickCoverDownList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    m_item_selected = pNMItemActivate->iItem;
    *pResult = 0;
}


void CCoverDownloadDlg::OnNMDblclkCoverDownList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    m_item_selected = pNMItemActivate->iItem;
    if (m_item_selected >= 0 && m_item_selected < static_cast<int>(m_down_list.size()))
    {
        OnBnClickedDownloadSelected();
    }
    *pResult = 0;
}


void CCoverDownloadDlg::OnNMRClickCoverDownList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    m_item_selected = pNMItemActivate->iItem;
    *pResult = 0;
}


void CCoverDownloadDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    theApp.m_cover_download_dialog_exit = true;
    if (m_pSearchThread != nullptr)
        WaitForSingleObject(m_pSearchThread->m_hThread, 1000);	//等待线程退出
    if (m_pDownThread != nullptr)
        WaitForSingleObject(m_pDownThread->m_hThread, 1000);	//等待线程退出

    CBaseDialog::OnOK();
}


void CCoverDownloadDlg::OnCancel()
{
    // TODO: 在此添加专用代码和/或调用基类
    theApp.m_cover_download_dialog_exit = true;
    if (m_pSearchThread != nullptr)
        WaitForSingleObject(m_pSearchThread->m_hThread, 1000);	//等待线程退出
    if (m_pDownThread != nullptr)
        WaitForSingleObject(m_pDownThread->m_hThread, 1000);	//等待线程退出

    CBaseDialog::OnCancel();
}


void CCoverDownloadDlg::OnDestroy()
{
    CBaseDialog::OnDestroy();

    // TODO: 在此处添加消息处理程序代码
    SaveConfig();
}


afx_msg LRESULT CCoverDownloadDlg::OnDownloadComplate(WPARAM wParam, LPARAM lParam)
{
    //重新从本地获取专辑封面
    if (CPlayer::GetInstance().GetCurrentSongInfo().IsSameSong(m_song))
    {
        CPlayer::GetInstance().SearchOutAlbumCover();
        CPlayer::GetInstance().AlbumCoverGaussBlur();
    }
    GetDlgItem(IDC_DOWNLOAD_SELECTED)->EnableWindow(TRUE);		//下载完成后启用该按钮
    const wstring& info = theApp.m_str_table.LoadText(L"MSG_NETWORK_DOWNLOAD_COMPLETE");
    MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
    return 0;
}


void CCoverDownloadDlg::OnEnChangeTitleEdit()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CBaseDialog::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    CString tmp;
    GetDlgItemText(IDC_TITLE_EDIT, tmp);
    m_title = tmp;
}


void CCoverDownloadDlg::OnEnChangeArtistEdit()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CBaseDialog::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    CString tmp;
    GetDlgItemText(IDC_ARTIST_EDIT, tmp);
    m_artist = tmp;
}


void CCoverDownloadDlg::OnNMClickUnassociateLink(NMHDR* pNMHDR, LRESULT* pResult)
{
    // TODO: 在此添加控件通知处理程序代码
    SetID(wstring());
    m_unassciate_lnk.ShowWindow(SW_HIDE);

    *pResult = 0;
}



void CCoverDownloadDlg::OnBnClickedSaveToSongFolder2()
{
    // TODO: 在此添加控件通知处理程序代码
    m_save_to_song_folder = true;
}


void CCoverDownloadDlg::OnBnClickedSaveToAlbumFolder2()
{
    // TODO: 在此添加控件通知处理程序代码
    m_save_to_song_folder = false;
}
