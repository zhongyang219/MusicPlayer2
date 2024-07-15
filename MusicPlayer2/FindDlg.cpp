// FindDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "Player.h"
#include "FindDlg.h"
#include "MusicPlayerCmdHelper.h"
#include "PropertyDlg.h"
#include "Playlist.h"
#include "AddToPlaylistDlg.h"
#include "SongDataManager.h"
#include "COSUPlayerHelper.h"
#include "IniHelper.h"


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

bool CFindDlg::InitializeControls()
{
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TITLE_FIND");
    SetWindowTextW(temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_FIND_KEY_WORD_INPUT");
    SetDlgItemTextW(IDC_TXT_FIND_KEY_WORD_INPUT_STATIC, temp.c_str());
    // IDC_FIND_EDIT
    temp = theApp.m_str_table.LoadText(L"TXT_FIND_FIND");
    SetDlgItemTextW(IDC_FIND_BUTTON, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_FIND_FIND_RANGE");
    SetDlgItemTextW(IDC_TXT_FIND_FIND_RANGE_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_FIND_CURRENT_PLAYLIST");
    SetDlgItemTextW(IDC_FIND_CURRENT_PLAYLIST_RADIO, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_FIND_MEDIA_LIB");
    SetDlgItemTextW(IDC_FIND_ALL_PLAYLIST_RADIO, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_FIND_OPT");
    SetDlgItemTextW(IDC_TXT_FIND_OPT_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_FIND_OPT_FILE_NAME");
    SetDlgItemTextW(IDC_FIND_FILE_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_FIND_OPT_TITLE");
    SetDlgItemTextW(IDC_FIND_TITLE_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_FIND_OPT_ARTIST");
    SetDlgItemTextW(IDC_FIND_ARTIST_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_FIND_OPT_ALBUM");
    SetDlgItemTextW(IDC_FIND_ALBUM_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_FIND_RESULT_LAST");
    SetDlgItemTextW(IDC_FIND_RESULT_STATIC, temp.c_str());
    temp = L"";
    SetDlgItemTextW(IDC_FIND_INFO_STATIC, temp.c_str());
    // IDC_FIND_LIST
    temp = theApp.m_str_table.LoadText(L"TXT_FIND_PLAY_SELECTED");
    SetDlgItemTextW(IDOK, temp.c_str());

    RepositionTextBasedControls({
        { CtrlTextInfo::L1, IDC_TXT_FIND_KEY_WORD_INPUT_STATIC },
        { CtrlTextInfo::C0, IDC_FIND_EDIT },
        { CtrlTextInfo::R1, IDC_FIND_BUTTON, CtrlTextInfo::W32 }
        }, CtrlTextInfo::W128);
    RepositionTextBasedControls({
        { CtrlTextInfo::R1, IDOK, CtrlTextInfo::W32 },
        { CtrlTextInfo::R2, IDCANCEL, CtrlTextInfo::W32 }
        });
    return true;
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
    wstring str = theApp.m_str_table.LoadTextFormat(L"TXT_FIND_RESULT_INFO", { m_key_word, m_find_result.size() });
    SetDlgItemText(IDC_FIND_INFO_STATIC, str.c_str());
    ShowDlgCtrl(IDC_FIND_INFO_STATIC, !m_key_word.empty());
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
    CCommon::SetThreadLanguageList(theApp.m_str_table.GetLanguageTag());
    //此命令用于跳转到歌曲对应的网易云音乐的在线页面
    SongInfo song{ pThis->m_find_result[pThis->m_item_selected] };
    if (CCommon::FileExist(song.file_path))
    {
        CMusicPlayerCmdHelper cmd_helper(pThis);
        cmd_helper.VeiwOnline(song);
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

void CFindDlg::GetSongsSelected(vector<SongInfo>& songs) const
{
    songs.clear();
    for (const auto& index : m_items_selected)
        songs.push_back(m_find_result[index]);
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
    ON_COMMAND(ID_ADD_TO_NEW_PLAYLIST_AND_PLAY, &CFindDlg::OnAddToNewPlaylistAndPlay)
    ON_WM_INITMENU()
    ON_COMMAND(ID_DELETE_FROM_DISK, &CFindDlg::OnDeleteFromDisk)
    ON_COMMAND(ID_PLAY_AS_NEXT, &CFindDlg::OnPlayAsNext)
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
    m_item_selected = pNMItemActivate->iItem;       //单击查找结果列表时保存选中的项目序号
    m_find_result_list.GetItemSelected(m_items_selected);
    GetDlgItem(IDOK)->EnableWindow(m_item_selected != -1);
    *pResult = 0;
}


void CFindDlg::OnBnClickedFindButton()
{
    // TODO: 在此添加控件通知处理程序代码
    if (!m_key_word.empty())
    {
        // 清除上次查找结果
        m_item_selected = -1;
        m_items_selected.clear();
        m_find_result.clear();
        bool find_flag;
        if (m_find_current_playlist)        //查找当前播放列表时，在m_playlist窗口中查找
        {
            for (int i{ 0 }; i < CPlayer::GetInstance().GetSongNum(); i++)
            {
                find_flag = false;
                if (m_find_file && !find_flag)
                {
                    if (theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(m_key_word, CPlayer::GetInstance().GetPlayList()[i].GetFileName()))
                        find_flag = true;
                }
                if (m_find_title && !find_flag)
                {
                    if (theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(m_key_word, CPlayer::GetInstance().GetPlayList()[i].title))
                        find_flag = true;
                }
                if (m_find_artist && !find_flag)
                {
                    if (theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(m_key_word, CPlayer::GetInstance().GetPlayList()[i].artist))
                        find_flag = true;
                }
                if (m_find_album && !find_flag)
                {
                    if (theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(m_key_word, CPlayer::GetInstance().GetPlayList()[i].album))
                        find_flag = true;
                }
                if (find_flag)
                {
                    m_find_result.push_back(CPlayer::GetInstance().GetPlayList()[i]);
                }
            }
        }
        else        // 查找所有播放列表时，在SongDataMap中查找
        {
            CSongDataManager::GetInstance().GetSongData([&](const CSongDataManager::SongDataMap& song_data_map)
                {
                    for (const auto& item : song_data_map)
                    {
                        find_flag = false;
                        if (m_find_file && !find_flag)
                        {
                            if (theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(m_key_word, item.second.GetFileName()))
                                find_flag = true;
                        }
                        if (m_find_title && !find_flag)
                        {
                            if (theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(m_key_word, item.second.title))
                                find_flag = true;
                        }
                        if (m_find_artist && !find_flag)
                        {
                            if (theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(m_key_word, item.second.artist))
                                find_flag = true;
                        }
                        if (m_find_album && !find_flag)
                        {
                            if (theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(m_key_word, item.second.album))
                                find_flag = true;
                        }
                        if (find_flag)
                        {
                            m_find_result.push_back(item.second);      //如果找到了，就保存结果
                        }
                    }
                });
        }
        // 显示查找结果
        ShowFindResult();
        if (!m_find_result.empty())
            SetDlgItemText(IDC_FIND_RESULT_STATIC, theApp.m_str_table.LoadText(L"TXT_FIND_RESULT_CURRENT").c_str());
        else
            SetDlgItemText(IDC_FIND_RESULT_STATIC, theApp.m_str_table.LoadText(L"TXT_FIND_RESULT_NONE").c_str());
        ShowFindInfo();
    }
}


BOOL CFindDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化

    SetIcon(IconMgr::IconType::IT_Find, FALSE);
    SetIcon(IconMgr::IconType::IT_Find, TRUE);
    SetButtonIcon(IDOK, IconMgr::IconType::IT_Play);
    SetButtonIcon(IDC_FIND_BUTTON, IconMgr::IconType::IT_Find);

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
    int list_width{ rect.Width() - theApp.DPI(20) - 1 };        //列表控件宽度减去留给垂直滚动条的宽度余量
    int width0, width1, width2;
    width0 = theApp.DPI(40);
    width2 = (list_width - width0) / 5;
    width1 = list_width - width0 - width2 * 4;
    m_find_result_list.InsertColumn(0, theApp.m_str_table.LoadText(L"TXT_SERIAL_NUMBER").c_str(), LVCFMT_LEFT, width0);
    m_find_result_list.InsertColumn(1, theApp.m_str_table.LoadText(L"TXT_FILE_NAME").c_str(), LVCFMT_LEFT, width1);
    m_find_result_list.InsertColumn(2, theApp.m_str_table.LoadText(L"TXT_TITLE").c_str(), LVCFMT_LEFT, width2);
    m_find_result_list.InsertColumn(3, theApp.m_str_table.LoadText(L"TXT_ARTIST").c_str(), LVCFMT_LEFT, width2);
    m_find_result_list.InsertColumn(4, theApp.m_str_table.LoadText(L"TXT_ALBUM").c_str(), LVCFMT_LEFT, width2);
    m_find_result_list.InsertColumn(5, theApp.m_str_table.LoadText(L"TXT_FILE_PATH").c_str(), LVCFMT_LEFT, width2);

    ShowFindResult();   //显示（上一次的）查找结果
    ShowFindInfo();

    m_key_word.clear();
    m_item_selected = -1;
    m_items_selected.clear();

    GetDlgItem(IDC_FIND_EDIT)->SetFocus();      //将焦点设置到搜索框

    //设置列表控件的提示总是置顶，用于解决如果弹出此窗口的父窗口具有置顶属性时，提示信息在窗口下面的问题
    m_find_result_list.GetToolTips()->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    GetDlgItem(IDOK)->EnableWindow(FALSE);  //禁用“播放选中曲目”按钮，除非选中了一个项目

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
    if (pMsg->message == WM_KEYUP && pMsg->wParam == VK_RETURN)     //按回车键执行搜索操作
    {
        OnBnClickedFindButton();
        return TRUE;
    }
    if (pMsg->wParam == VK_RETURN)      //屏蔽按回车键退出
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
        int list_width{ rect.Width() - theApp.DPI(20) - 1 };        //列表控件宽度减去留给垂直滚动条的宽度余量
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

    if (m_item_selected >= 0 && m_item_selected < static_cast<int>(m_find_result.size()))
    {
        //获取鼠标点击处的文本
        int sub_item;
        sub_item = pNMItemActivate->iSubItem;
        m_selected_string = m_find_result_list.GetItemText(m_item_selected, sub_item);
        //弹出右键菜单
        CMenu* pContextMenu = theApp.m_menu_mgr.GetMenu(MenuMgr::LibRightMenu);
        m_find_result_list.ShowPopupMenu(pContextMenu, pNMItemActivate->iItem, this);
    }

    *pResult = 0;
}


void CFindDlg::OnCopyText()
{
    // TODO: 在此添加命令处理程序代码
    if (!CCommon::CopyStringToClipboard(wstring(m_selected_string)))
        MessageBox(theApp.m_str_table.LoadText(L"MSG_COPY_CLIPBOARD_FAILED").c_str(), NULL, MB_ICONWARNING);
}


void CFindDlg::OnPlayItemInFolderMode()
{
    // TODO: 在此添加命令处理程序代码
    if (m_item_selected >= 0 && m_item_selected < static_cast<int>(m_find_result.size()))
    {
        if (!CPlayer::GetInstance().OpenASongInFolderMode(m_find_result[m_item_selected], true))
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
            MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
        }
        else
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
    if (m_item_selected >= 0 && m_item_selected < static_cast<int>(m_find_result.size()))
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
    if (m_item_selected < 0 || m_item_selected >= static_cast<int>(m_find_result.size()))
        return;
    CPropertyDlg propertyDlg(m_find_result, m_item_selected, true);
    propertyDlg.DoModal();
}


void CFindDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    if (m_item_selected < 0 || m_item_selected >= static_cast<int>(m_find_result.size())) return;

    int selected_track = CPlayer::GetInstance().IsSongInPlayList(m_find_result[m_item_selected]);
    if (selected_track != -1)      //如果查找结果是当前播放列表中的曲目，则在当前播放列表中查找选中的曲目，并播放
    {
        if (!CPlayer::GetInstance().PlayTrack(selected_track))
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
            MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
        }
    }
    else
    {
        vector<SongInfo> songs;
        GetSongsSelected(songs);
        if (!songs.empty())
        {
            bool ok{};
            if (songs.size() == 1)
                ok = CPlayer::GetInstance().OpenSongsInDefaultPlaylist(songs);
            else
                ok = CPlayer::GetInstance().OpenSongsInTempPlaylist(songs);
            if (!ok)
            {
                const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
                MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
            }
        }
    }

    CBaseDialog::OnOK();
}


void CFindDlg::OnAddToNewPlaylistAndPlay()
{
    // TODO: 在此添加命令处理程序代码
    wstring playlist_path;
    if (_OnAddToNewPlaylist(playlist_path))
    {
        if (!CPlayer::GetInstance().SetPlaylist(playlist_path, 0, 0, true))
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
            MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
        }
        else
            OnCancel();
    }
}


void CFindDlg::OnInitMenu(CMenu* pMenu)
{
    CBaseDialog::OnInitMenu(pMenu);

    // TODO: 在此处添加消息处理程序代码
    vector<SongInfo> songs;
    GetSongsSelected(songs);
    bool select_all_in_playing_list = CPlayer::GetInstance().IsSongsInPlayList(songs);
    // 选中歌曲全部为cue或osu!文件时禁用从磁盘删除菜单项
    bool can_del = !theApp.m_media_lib_setting_data.disable_delete_from_disk &&
        std::find_if(songs.begin(), songs.end(), [&](const SongInfo& song_info) { return song_info.is_cue || COSUPlayerHelper::IsOsuFile(song_info.file_path); }) != songs.end();

    pMenu->EnableMenuItem(ID_PLAY_AS_NEXT, MF_BYCOMMAND | (select_all_in_playing_list ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_DELETE_FROM_DISK, MF_BYCOMMAND | (can_del ? MF_ENABLED : MF_GRAYED));
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


void CFindDlg::OnPlayAsNext()
{
    // TODO: 在此添加命令处理程序代码
    vector<SongInfo> songs;
    GetSongsSelected(songs);
    CPlayer::GetInstance().PlayAfterCurrentTrack(songs);
    ::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_SET_UI_FORCE_FRESH_FLAG, 0, 0);  // 主动触发更新状态栏显示
}
