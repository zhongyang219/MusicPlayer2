// FindDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "FindDlg.h"
#include "afxdialogex.h"
#include "MusicPlayerCmdHelper.h"
#include "PropertyDlg.h"
#include "Playlist.h"
#include "AddToPlaylistDlg.h"
#include "SongDataManager.h"


// CFindDlg 对话框

IMPLEMENT_DYNAMIC(CFindDlg, CBaseDialog)

CFindDlg::CFindDlg(CWnd* pParent /*=NULL*/)
    : CBaseDialog(IDD_FIND_DIALOG, pParent)
{

}

CFindDlg::~CFindDlg()
{
}

CString CFindDlg::GetDialogName() const
{
    return _T("FindDlg");
}

void CFindDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_FIND_LIST, m_find_result_list);
    DDX_Control(pDX, IDC_FIND_FILE_CHECK, m_find_file_check);
    DDX_Control(pDX, IDC_FIND_TITLE_CHECK, m_find_title_check);
    DDX_Control(pDX, IDC_FIND_ARTIST_CHECK, m_find_artist_check);
    DDX_Control(pDX, IDC_FIND_ALBUM_CHECK, m_find_album_check);
}

void CFindDlg::ShowFindResult()
{
    m_find_result_list.DeleteAllItems();
    CString str;
    for (size_t i{}; i < m_find_result.size(); i++)
    {
        str.Format(_T("%u"), i + 1);
        m_find_result_list.InsertItem(i, str);
        m_find_result_list.SetItemText(i, COL_FILE_NAME, m_find_result[i].GetFileName().c_str());
        m_find_result_list.SetItemText(i, COL_TITLE, m_find_result[i].GetTitle().c_str());
        m_find_result_list.SetItemText(i, COL_ARTIST, m_find_result[i].GetArtist().c_str());
        m_find_result_list.SetItemText(i, COL_ALBUM, m_find_result[i].GetAlbum().c_str());
        m_find_result_list.SetItemText(i, COL_PATH, m_find_result[i].file_path.c_str());
    }
}

void CFindDlg::ShowFindInfo()
{
    CString str;
    int result_mun;
    if (m_find_current_playlist)
        result_mun = m_find_result.size();
    else
        result_mun = m_find_result.size();
    str = CCommon::LoadTextFormat(IDS_FIND_DLG_INFO, { m_key_word, result_mun });
    SetDlgItemText(IDC_FIND_INFO_STATIC, str);
}

bool CFindDlg::_OnAddToNewPlaylist(std::wstring& playlist_path)
{
    auto getSongList = [&](std::vector<SongInfo>& song_list)
    {
        GetSongsSelected(song_list);
    };
    CMusicPlayerCmdHelper cmd_helper(this);
    return cmd_helper.OnAddToNewPlaylist(getSongList, playlist_path);
}

UINT CFindDlg::ViewOnlineThreadFunc(LPVOID lpParam)
{
    CFindDlg* pThis = (CFindDlg*)(lpParam);
    if (pThis == nullptr)
        return 0;
    CCommon::SetThreadLanguage(theApp.m_general_setting_data.language);
    //此命令用于跳转到歌曲对应的网易云音乐的在线页面
    if (pThis->m_item_selected >= 0)
    {
        wstring file_path = pThis->m_find_result_list.GetItemText(pThis->m_item_selected, COL_PATH).GetString();
        if (CCommon::FileExist(file_path))
        {
            SongInfo song{ CSongDataManager::GetInstance().GetSongInfo(file_path) };
            song.file_path = file_path;
            CMusicPlayerCmdHelper cmd_helper(pThis);
            cmd_helper.VeiwOnline(song);
        }
    }
    return 0;
}

void CFindDlg::ClearFindResult()
{
    m_find_result.clear();
}

void CFindDlg::SaveConfig()
{
    m_find_option_data = 0;
    if (m_find_file)
        m_find_option_data |= 0x01;
    if (m_find_title)
        m_find_option_data |= 0x02;
    if (m_find_artist)
        m_find_option_data |= 0x04;
    if (m_find_album)
        m_find_option_data |= 0x08;
    if (m_find_current_playlist)
        m_find_option_data |= 0x10;

    CIniHelper ini(theApp.m_config_path);
    ini.WriteInt(L"config", L"find_option_data", m_find_option_data);
    ini.Save();
}

void CFindDlg::LoadConfig()
{
    CIniHelper ini(theApp.m_config_path);
    m_find_option_data = ini.GetInt(L"config", L"find_option_data", 0xff);

    m_find_file = (m_find_option_data % 2 != 0);
    m_find_title = ((m_find_option_data >> 1) % 2 != 0);
    m_find_artist = ((m_find_option_data >> 2) % 2 != 0);
    m_find_album = ((m_find_option_data >> 3) % 2 != 0);
    m_find_current_playlist = ((m_find_option_data >> 4) % 2 != 0);
}

bool CFindDlg::IsFindCurrentPlaylist() const
{
    return m_result_in_current_playlist;
}


void CFindDlg::GetSongsSelected(vector<wstring>& files) const
{
    files.clear();
    for (int index : m_items_selected)
    {
        if (index >= 0 && index < m_find_result_list.GetItemCount())
        {
            wstring file_path = m_find_result_list.GetItemText(index, COL_PATH).GetString();
            files.push_back(file_path);
        }
    }
}

void CFindDlg::GetSongsSelected(vector<SongInfo>& songs) const
{
    songs.clear();
    vector<wstring> files;
    GetSongsSelected(files);
    for (const auto& file : files)
    {
        SongInfo song = CSongDataManager::GetInstance().GetSongInfo(file);
        songs.push_back(song);
    }
}

void CFindDlg::GetCurrentSongList(std::vector<SongInfo>& song_list)
{
    for (int index = 0; index < m_find_result_list.GetItemCount(); index++)
    {
        std::wstring file = m_find_result_list.GetItemText(index, COL_PATH);
        SongInfo song = CSongDataManager::GetInstance().GetSongInfo(file);
        song_list.push_back(song);
    }
}

BEGIN_MESSAGE_MAP(CFindDlg, CBaseDialog)
    ON_EN_CHANGE(IDC_FIND_EDIT, &CFindDlg::OnEnChangeFindEdit)
    ON_NOTIFY(NM_CLICK, IDC_FIND_LIST, &CFindDlg::OnNMClickFindList)
    ON_BN_CLICKED(IDC_FIND_BUTTON, &CFindDlg::OnBnClickedFindButton)
    ON_NOTIFY(NM_DBLCLK, IDC_FIND_LIST, &CFindDlg::OnNMDblclkFindList)
    ON_BN_CLICKED(IDC_FIND_FILE_CHECK, &CFindDlg::OnBnClickedFindFileCheck)
    ON_BN_CLICKED(IDC_FIND_TITLE_CHECK, &CFindDlg::OnBnClickedFindTitleCheck)
    ON_BN_CLICKED(IDC_FIND_ARTIST_CHECK, &CFindDlg::OnBnClickedFindArtistCheck)
    ON_BN_CLICKED(IDC_FIND_ALBUM_CHECK, &CFindDlg::OnBnClickedFindAlbumCheck)
    ON_BN_CLICKED(IDC_FIND_CURRENT_PLAYLIST_RADIO, &CFindDlg::OnBnClickedFindCurrentPlaylistRadio)
    ON_BN_CLICKED(IDC_FIND_ALL_PLAYLIST_RADIO, &CFindDlg::OnBnClickedFindAllPlaylistRadio)
    ON_WM_GETMINMAXINFO()
    ON_WM_SIZE()
    ON_COMMAND(ID_PLAY_ITEM, &CFindDlg::OnPlayItem)
    ON_COMMAND(ID_EXPLORE_TRACK, &CFindDlg::OnExploreTrack)
    ON_NOTIFY(NM_RCLICK, IDC_FIND_LIST, &CFindDlg::OnNMRClickFindList)
    ON_COMMAND(ID_COPY_TEXT, &CFindDlg::OnCopyText)
    ON_COMMAND(ID_PLAY_ITEM_IN_FOLDER_MODE, &CFindDlg::OnPlayItemInFolderMode)
    ON_COMMAND(ID_ADD_TO_NEW_PLAYLIST, &CFindDlg::OnAddToNewPlaylist)
    ON_COMMAND(ID_EXPLORE_ONLINE, &CFindDlg::OnExploreOnline)
    ON_COMMAND(ID_FORMAT_CONVERT, &CFindDlg::OnFormatConvert)
    ON_COMMAND(ID_ITEM_PROPERTY, &CFindDlg::OnItemProperty)
    ON_COMMAND(ID_ADD_TO_NEW_PALYLIST_AND_PLAY, &CFindDlg::OnAddToNewPalylistAndPlay)
    ON_WM_INITMENU()
    ON_COMMAND(ID_DELETE_FROM_DISK, &CFindDlg::OnDeleteFromDisk)
END_MESSAGE_MAP()


// CFindDlg 消息处理程序

void CFindDlg::OnEnChangeFindEdit()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CBaseDialog::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    CString find_string;
    GetDlgItemText(IDC_FIND_EDIT, find_string);
    m_key_word = find_string;
}


void CFindDlg::OnNMClickFindList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    //NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    m_item_selected = pNMItemActivate->iItem;		//单击查找结果列表时保存选中的项目序号
    m_find_result_list.GetItemSelected(m_items_selected);
    GetDlgItem(IDOK)->EnableWindow(m_item_selected != -1);
    *pResult = 0;
}


void CFindDlg::OnBnClickedFindButton()
{
    // TODO: 在此添加控件通知处理程序代码
    if (!m_key_word.empty())
    {
        if (m_find_current_playlist)		//查找当前播放列表时，在m_playlist窗口中查找
        {
            m_find_result.clear();
            int index;
            bool find_flag;
            for (size_t i{ 0 }; i < CPlayer::GetInstance().GetSongNum(); i++)
            {
                find_flag = false;
                if (m_find_file && !find_flag)
                {
                    index = CCommon::StringFindNoCase(CPlayer::GetInstance().GetPlayList()[i].GetFileName(), m_key_word);
                    if (index != string::npos) find_flag = true;
                }
                if (m_find_title && !find_flag)
                {
                    index = CCommon::StringFindNoCase(CPlayer::GetInstance().GetPlayList()[i].title, m_key_word);
                    if (index != string::npos) find_flag = true;
                }
                if (m_find_artist && !find_flag)
                {
                    index = CCommon::StringFindNoCase(CPlayer::GetInstance().GetPlayList()[i].artist, m_key_word);
                    if (index != string::npos) find_flag = true;
                }
                if (m_find_album && !find_flag)
                {
                    index = CCommon::StringFindNoCase(CPlayer::GetInstance().GetPlayList()[i].album, m_key_word);
                    if (index != string::npos) find_flag = true;
                }

                if (find_flag)
                {
                    m_find_result.push_back(CPlayer::GetInstance().GetPlayList()[i]);
                }
            }
            ShowFindResult();
            if (!m_find_result.empty())
                SetDlgItemText(IDC_FIND_RESULT_STATIC, CCommon::LoadText(IDS_FIND_RESULT, _T(": ")));
            else
                SetDlgItemText(IDC_FIND_RESULT_STATIC, CCommon::LoadText(IDS_NO_RESULT));
        }
        else			//查找所有播放列表时，在theApp.m_song_data窗口中查找
        {
            m_find_result.clear();
            wstring a_result;
            int index;
            bool find_flag;
            for (const auto& item : CSongDataManager::GetInstance().GetSongData())
            {
                find_flag = false;
                if (m_find_file && !find_flag)
                {
                    wstring file_name = CFilePathHelper(item.first).GetFileName();
                    index = CCommon::StringFindNoCase(file_name, m_key_word);
                    if (index != string::npos) find_flag = true;
                }
                if (m_find_title && !find_flag)
                {
                    index = CCommon::StringFindNoCase(item.second.title, m_key_word);
                    if (index != string::npos) find_flag = true;
                }
                if (m_find_artist && !find_flag)
                {
                    index = CCommon::StringFindNoCase(item.second.artist, m_key_word);
                    if (index != string::npos) find_flag = true;
                }
                if (m_find_album && !find_flag)
                {
                    index = CCommon::StringFindNoCase(item.second.album, m_key_word);
                    if (index != string::npos) find_flag = true;
                }
                if (find_flag)
                {
                    SongInfo song = item.second;
                    song.file_path = item.first;
                    m_find_result.push_back(song);		//如果找到了，就保存结果
                }
            }
            ShowFindResult();
            if (!m_find_result.empty())
                SetDlgItemText(IDC_FIND_RESULT_STATIC, CCommon::LoadText(IDS_FIND_RESULT, _T(": ")));
            else
                SetDlgItemText(IDC_FIND_RESULT_STATIC, CCommon::LoadText(IDS_NO_RESULT));
        }
        m_item_selected = -1;
        m_items_selected.clear();
        ShowFindInfo();
        m_result_in_current_playlist = m_find_current_playlist;
    }
}


BOOL CFindDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化

    SetIcon(theApp.m_icon_set.find_songs.GetIcon(true), FALSE);
    SetIcon(AfxGetApp()->LoadIcon(IDI_FIND_SONGS_D), TRUE);
    SetButtonIcon(IDOK, theApp.m_icon_set.play_new.GetIcon(true));
    SetButtonIcon(IDC_FIND_BUTTON, theApp.m_icon_set.find_songs.GetIcon(true));

    //设置查找选项复选按钮的状态
    m_find_file_check.SetCheck(m_find_file);
    m_find_title_check.SetCheck(m_find_title);
    m_find_artist_check.SetCheck(m_find_artist);
    m_find_album_check.SetCheck(m_find_album);

    if (m_find_current_playlist)
        ((CButton*)GetDlgItem(IDC_FIND_CURRENT_PLAYLIST_RADIO))->SetCheck(TRUE);
    else
        ((CButton*)GetDlgItem(IDC_FIND_ALL_PLAYLIST_RADIO))->SetCheck(TRUE);

    //初始化列表控件
    CRect rect;
    m_find_result_list.GetClientRect(rect);
    m_find_result_list.SetExtendedStyle(m_find_result_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
    int list_width{ rect.Width() - theApp.DPI(20) - 1 };		//列表控件宽度减去留给垂直滚动条的宽度余量
    int width0, width1, width2;
    width0 = theApp.DPI(40);
    width2 = (list_width - width0) / 5;
    width1 = list_width - width0 - width2 * 4;
    m_find_result_list.InsertColumn(0, CCommon::LoadText(IDS_NUMBER), LVCFMT_LEFT, width0);		//插入第0列
    m_find_result_list.InsertColumn(1, CCommon::LoadText(IDS_FILE_NAME), LVCFMT_LEFT, width1);		//插入第1列
    m_find_result_list.InsertColumn(2, CCommon::LoadText(IDS_TITLE), LVCFMT_LEFT, width2);		//插入第2列
    m_find_result_list.InsertColumn(3, CCommon::LoadText(IDS_ARTIST), LVCFMT_LEFT, width2);		//插入第3列
    m_find_result_list.InsertColumn(4, CCommon::LoadText(IDS_ALBUM), LVCFMT_LEFT, width2);		//插入第4列
    m_find_result_list.InsertColumn(5, CCommon::LoadText(IDS_FILE_PATH), LVCFMT_LEFT, width2);		//插入第5列

    ShowFindResult();	//显示（上一次的）查找结果
    ShowFindInfo();

    m_key_word.clear();

    GetDlgItem(IDC_FIND_EDIT)->SetFocus();		//将焦点设置到搜索框

    //设置列表控件的提示总是置顶，用于解决如果弹出此窗口的父窗口具有置顶属性时，提示信息在窗口下面的问题
    m_find_result_list.GetToolTips()->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    //初始化右键菜单
    //if (m_menu.m_hMenu == NULL)
    //	m_menu.LoadMenu(IDR_FIND_POPUP_MENU);
    //m_menu.GetSubMenu(0)->SetDefaultItem(ID_FD_PLAY);

    GetDlgItem(IDOK)->EnableWindow(FALSE);	//禁用“播放选中曲目”按钮，除非选中了一个项目

    return FALSE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CFindDlg::OnNMDblclkFindList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    m_item_selected = pNMItemActivate->iItem;
    m_find_result_list.GetItemSelected(m_items_selected);
    GetDlgItem(IDOK)->EnableWindow(m_item_selected != -1);

    //双击列表项目后关闭对话框并播放选中项目
    OnPlayItem();
    *pResult = 0;
}


BOOL CFindDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_KEYUP && pMsg->wParam == VK_RETURN)		//按回车键执行搜索操作
    {
        OnBnClickedFindButton();
        return TRUE;
    }
    if (pMsg->wParam == VK_RETURN)		//屏蔽按回车键退出
    {
        return TRUE;
    }

    return CBaseDialog::PreTranslateMessage(pMsg);
}


void CFindDlg::OnBnClickedFindFileCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_find_file = (m_find_file_check.GetCheck() != 0);
}


void CFindDlg::OnBnClickedFindTitleCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_find_title = (m_find_title_check.GetCheck() != 0);
}


void CFindDlg::OnBnClickedFindArtistCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_find_artist = (m_find_artist_check.GetCheck() != 0);
}


void CFindDlg::OnBnClickedFindAlbumCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_find_album = (m_find_album_check.GetCheck() != 0);
}



void CFindDlg::OnBnClickedFindCurrentPlaylistRadio()
{
    // TODO: 在此添加控件通知处理程序代码
    m_find_current_playlist = true;
}


void CFindDlg::OnBnClickedFindAllPlaylistRadio()
{
    // TODO: 在此添加控件通知处理程序代码
    m_find_current_playlist = false;
}


void CFindDlg::OnSize(UINT nType, int cx, int cy)
{
    CBaseDialog::OnSize(nType, cx, cy);

    // TODO: 在此处添加消息处理程序代码
    if (m_find_result_list.m_hWnd != NULL && nType != SIZE_MINIMIZED)
    {
        //调整列表中项目的宽度
        CRect rect;
        m_find_result_list.GetWindowRect(rect);
        int list_width{ rect.Width() - theApp.DPI(20) - 1 };		//列表控件宽度减去留给垂直滚动条的宽度余量
        int width0, width1, width2;
        width0 = theApp.DPI(40);
        width2 = (list_width - width0) / 5;
        width1 = list_width - width0 - width2 * 4;
        m_find_result_list.SetColumnWidth(1, width1);
        m_find_result_list.SetColumnWidth(2, width2);
        m_find_result_list.SetColumnWidth(3, width2);
        m_find_result_list.SetColumnWidth(4, width2);
        m_find_result_list.SetColumnWidth(5, width2);
    }
}


void CFindDlg::OnPlayItem()
{
    // TODO: 在此添加命令处理程序代码
    OnOK();
}


void CFindDlg::OnExploreTrack()
{
    // TODO: 在此添加命令处理程序代码
    wstring file;
    if (m_item_selected >= 0 && m_item_selected < static_cast<int>(m_find_result.size()))
    {
        file = m_find_result[m_item_selected].file_path;
        CString str;
        str.Format(_T("/select,\"%s\""), file.c_str());
        ShellExecute(NULL, _T("open"), _T("explorer"), str, NULL, SW_SHOWNORMAL);
    }
}


void CFindDlg::OnNMRClickFindList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    m_item_selected = pNMItemActivate->iItem;
    m_find_result_list.GetItemSelected(m_items_selected);
    GetDlgItem(IDOK)->EnableWindow(m_item_selected != -1);

    if (m_find_current_playlist && (m_item_selected >= 0 && m_item_selected < static_cast<int>(m_find_result.size()))
        || !m_find_current_playlist && (m_item_selected >= 0 && m_item_selected < static_cast<int>(m_find_result.size())))
    {
        //获取鼠标点击处的文本
        int sub_item;
        sub_item = pNMItemActivate->iSubItem;
        m_selected_string = m_find_result_list.GetItemText(m_item_selected, sub_item);
        //弹出右键菜单
        CMenu* pContextMenu = theApp.m_menu_set.m_media_lib_popup_menu.GetSubMenu(1);
        m_find_result_list.ShowPopupMenu(pContextMenu, pNMItemActivate->iItem, this);
    }

    *pResult = 0;
}


void CFindDlg::OnCopyText()
{
    // TODO: 在此添加命令处理程序代码
    if (!CCommon::CopyStringToClipboard(wstring(m_selected_string)))
        MessageBox(CCommon::LoadText(IDS_COPY_CLIPBOARD_FAILED), NULL, MB_ICONWARNING);
}


void CFindDlg::OnPlayItemInFolderMode()
{
    // TODO: 在此添加命令处理程序代码
    if (m_item_selected >= 0)
    {
        std::wstring file_path = m_find_result_list.GetItemText(m_item_selected, COL_PATH);
        CPlayer::GetInstance().OpenAFile(file_path, true);

        OnCancel();
    }

}


void CFindDlg::OnAddToNewPlaylist()
{
    // TODO: 在此添加命令处理程序代码
    wstring playlist_path;
    _OnAddToNewPlaylist(playlist_path);
}


void CFindDlg::OnExploreOnline()
{
    // TODO: 在此添加命令处理程序代码
    AfxBeginThread(ViewOnlineThreadFunc, (void*)this);
}


void CFindDlg::OnFormatConvert()
{
    // TODO: 在此添加命令处理程序代码
    std::vector<SongInfo> songs;
    GetSongsSelected(songs);
    CMusicPlayerCmdHelper cmd_helper(this);
    cmd_helper.FormatConvert(songs);
}


void CFindDlg::OnItemProperty()
{
    // TODO: 在此添加命令处理程序代码
    if (m_item_selected < 0)
        return;
    std::vector<SongInfo> songs;
    GetCurrentSongList(songs);
    CPropertyDlg propertyDlg(songs, m_item_selected, true);
    propertyDlg.DoModal();
}


void CFindDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类

    SongInfo song_selected;
    if (m_item_selected < 0 || m_item_selected >= static_cast<int>(m_find_result.size()))
        return;
    song_selected = m_find_result[m_item_selected];
    auto iter = std::find_if(CPlayer::GetInstance().GetPlayList().begin(), CPlayer::GetInstance().GetPlayList().end(), [&](const SongInfo& song)
        {
            return song.IsSameSong(song_selected);
        });
    if (iter != CPlayer::GetInstance().GetPlayList().end())      //如果查找结果是当前播放列表中的曲目，则在当前播放列表中查找选中的曲目，并播放
    {
        int selected_track = iter - CPlayer::GetInstance().GetPlayList().begin();
        CPlayer::GetInstance().PlayTrack(selected_track);
        m_result_in_current_playlist = true;
    }
    else
    {
        vector<wstring> files;
        GetSongsSelected(files);
        if (!files.empty())
        {
            if (files.size() == 1)
                CPlayer::GetInstance().OpenFiles(files);
            else
                CPlayer::GetInstance().OpenFilesInTempPlaylist(files);
        }
        m_result_in_current_playlist = false;
    }

    CBaseDialog::OnOK();
}


void CFindDlg::OnAddToNewPalylistAndPlay()
{
    // TODO: 在此添加命令处理程序代码
    wstring playlist_path;
    if (_OnAddToNewPlaylist(playlist_path))
    {
        CPlayer::GetInstance().SetPlaylist(playlist_path, 0, 0, false, true);
        CPlayer::GetInstance().SaveRecentPath();
        OnCancel();
    }
}


void CFindDlg::OnInitMenu(CMenu* pMenu)
{
    CBaseDialog::OnInitMenu(pMenu);

    // TODO: 在此处添加消息处理程序代码
    pMenu->SetDefaultItem(ID_PLAY_ITEM);
}


BOOL CFindDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    // TODO: 在此添加专用代码和/或调用基类
    WORD command = LOWORD(wParam);
    auto getSelectedItems = [&](std::vector<SongInfo>& item_list)
    {
        GetSongsSelected(item_list);
    };
    //响应播放列表右键菜单中的“添加到播放列表”
    CMusicPlayerCmdHelper cmd_helper;
    cmd_helper.OnAddToPlaylistCommand(getSelectedItems, command);

    return CBaseDialog::OnCommand(wParam, lParam);
}


void CFindDlg::OnDeleteFromDisk()
{
    // TODO: 在此添加命令处理程序代码
    vector<SongInfo> songs_selected;
    GetSongsSelected(songs_selected);
    CMusicPlayerCmdHelper helper;
    if (helper.DeleteSongsFromDisk(songs_selected))
    {
        //删除成功，则刷新列表
        auto isRemoved = [&](const SongInfo& song)
        {
            for (const auto& item : songs_selected)
            {
                if (item.IsSameSong(song))
                    return true;
            }
            return false;
        };
        auto iter_removed = std::remove_if(m_find_result.begin(), m_find_result.end(), isRemoved);
        m_find_result.erase(iter_removed, m_find_result.end());
        ShowFindResult();
    }
}
