// MediaClassfyDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "MediaClassifyDlg.h"
#include "afxdialogex.h"
#include "Playlist.h"
#include "InputDlg.h"
#include "MusicPlayerCmdHelper.h"
#include "PropertyDlg.h"


// CMediaClassifyDlg 对话框

IMPLEMENT_DYNAMIC(CMediaClassifyDlg, CTabDlg)

CMediaClassifyDlg::CMediaClassifyDlg(CMediaClassifier::ClassificationType type, CWnd* pParent /*=nullptr*/)
	: CTabDlg(IDD_MEDIA_CLASSIFY_DIALOG, pParent), m_type(type), 
    m_classifer(type == CMediaClassifier::CT_ARTIST ? theApp.m_artist_classifer : 
    (type == CMediaClassifier::CT_ALBUM ? theApp.m_album_classifer : 
        (type == CMediaClassifier::CT_GENRE ? theApp.m_genre_classifer : 
            theApp.m_year_classifer)))
{
    if (m_type == CMediaClassifier::CT_ARTIST)
        m_default_str = CCommon::LoadText(IDS_DEFAULT_ARTIST);
    else if (m_type == CMediaClassifier::CT_ALBUM)
        m_default_str = CCommon::LoadText(IDS_DEFAULT_ALBUM);
    else if (m_type == CMediaClassifier::CT_GENRE)
        m_default_str = CCommon::LoadText(IDS_DEFAULT_GENRE);
    else if (m_type == CMediaClassifier::CT_YEAR)
        m_default_str = CCommon::LoadText(IDS_DEFAULT_YEAR);
}

CMediaClassifyDlg::~CMediaClassifyDlg()
{
}

void CMediaClassifyDlg::GetSongsSelected(std::vector<wstring>& song_list) const
{
    std::vector<SongInfo> song_info_list;
    GetSongsSelected(song_info_list);
    for (const auto& song : song_info_list)
        song_list.push_back(song.file_path);
}

void CMediaClassifyDlg::GetSongsSelected(std::vector<SongInfo>& song_list) const
{
    auto& media_list{ m_searched ? m_search_result : m_classifer.GetMeidaList() };
    song_list.clear();
    if (m_left_selected)
    {
        //如果选中了左侧列表，则把选中分类下的所有曲目的路径添加到song_list中
        for (int index : m_left_selected_items)
        {
            CString str_selected = GetClassifyListSelectedString(index);
            auto iter = media_list.find(wstring(str_selected));
            if (iter != media_list.end())
            {
                for (const auto& item : iter->second)
                {
                    song_list.push_back(item);
                }
            }
        }
    }
    else
    {
        auto iter = media_list.find(wstring(m_classify_selected));
        if (iter != media_list.end())
        {
            for (int index : m_right_selected_items)
            {
                if (index >= 0 && index < iter->second.size())
                    song_list.push_back(iter->second[index]);
            }
        }
    }
}

void CMediaClassifyDlg::GetCurrentSongList(std::vector<SongInfo>& song_list) const
{
    auto& media_list{ m_searched ? m_search_result : m_classifer.GetMeidaList() };
    auto iter = media_list.find(wstring(m_classify_selected));
    if (iter != media_list.end())
    {
        song_list = iter->second;
    }
}

void CMediaClassifyDlg::ShowClassifyList(bool size_changed)
{
    CWaitCursor wait_cursor;
    auto& media_list{ m_searched ? m_search_result : m_classifer.GetMeidaList() };
    if (size_changed)
        m_classify_list_ctrl.DeleteAllItems();
    int index = 0;
    for(const auto& item : media_list)
    {
        if(item.first == STR_OTHER_CLASSIFY_TYPE)       //跳过“其他”分类
            continue;

        CString item_name = item.first.c_str();
        if (item_name.IsEmpty())
        {
            item_name = m_default_str;
        }
        if (size_changed)
            m_classify_list_ctrl.InsertItem(index, item_name);
        else
            m_classify_list_ctrl.SetItemText(index, 0, item_name);
        m_classify_list_ctrl.SetItemText(index, 1, std::to_wstring(item.second.size()).c_str());
        index++;
    }

    //将“其他”分类放到列表的最后面
    auto iter = media_list.find(STR_OTHER_CLASSIFY_TYPE);
    if (iter != media_list.end())
    {
        CString item_name = CCommon::LoadText(_T("<"), IDS_OTHER, _T(">"));
        if (size_changed)
            m_classify_list_ctrl.InsertItem(index, item_name);
        else
            m_classify_list_ctrl.SetItemText(index, 0, item_name);
        m_classify_list_ctrl.SetItemText(index, 1, std::to_wstring(iter->second.size()).c_str());
    }
}

void CMediaClassifyDlg::ShowSongList(bool size_changed)
{
    CWaitCursor wait_cursor;
    auto& media_list{ m_searched ? m_search_result : m_classifer.GetMeidaList() };
    if (size_changed)
        m_song_list_ctrl.DeleteAllItems();

    int item_index = 0;
    for (int index : m_left_selected_items)
    {
        CString str_selected = GetClassifyListSelectedString(index);

        auto iter = media_list.find(wstring(str_selected));
        if (iter != media_list.end())
        {
            for (const auto& item : iter->second)
            {
                if (size_changed)
                    m_song_list_ctrl.InsertItem(item_index, item.GetTitle().c_str());
                else
                    m_song_list_ctrl.SetItemText(item_index, COL_TITLE, item.GetTitle().c_str());
                m_song_list_ctrl.SetItemText(item_index, COL_ARTIST, item.GetArtist().c_str());
                m_song_list_ctrl.SetItemText(item_index, COL_ALBUM, item.GetAlbum().c_str());
                std::wstring track_str;
                if (item.track != 0)
                    track_str = std::to_wstring(item.track);
                m_song_list_ctrl.SetItemText(item_index, COL_TRACK, track_str.c_str());
                m_song_list_ctrl.SetItemText(item_index, COL_GENRE, item.GetGenre().c_str());
                m_song_list_ctrl.SetItemText(item_index, COL_PATH, item.file_path.c_str());
                item_index++;
            }
        }
    }
}

CString CMediaClassifyDlg::GetClassifyListSelectedString(int index) const
{
    CString str_selected = m_classify_list_ctrl.GetItemText(index, 0);
    if (str_selected == m_default_str)
        str_selected.Empty();
    if (str_selected == CCommon::LoadText(_T("<"), IDS_OTHER, _T(">")))
        str_selected = STR_OTHER_CLASSIFY_TYPE;
    return str_selected;
}

void CMediaClassifyDlg::ClassifyListClicked(int index)
{
    m_left_selected = true;
    m_classify_list_ctrl.GetItemSelected(m_left_selected_items);    //获取选中的项目
    if (index < 0)
    {
        SetButtonsEnable(false);
        return;
    }

    CString str_selected = GetClassifyListSelectedString(index);
    static size_t last_selected_count = 0;
    if (last_selected_index != index || last_selected_count != m_left_selected_items.size())
    {
        m_classify_selected = str_selected;
        ShowSongList();
        last_selected_index = index;
        last_selected_count = m_left_selected_items.size();
    }

    SetButtonsEnable(/*(index >= 0 && index < m_classify_list_ctrl.GetItemCount()) ||*/ !m_left_selected_items.empty());

}

void CMediaClassifyDlg::SongListClicked(int index)
{
    m_left_selected = false;
    m_right_selected_item = index;
    m_song_list_ctrl.GetItemSelected(m_right_selected_items);
    SetButtonsEnable(/*(index >=0 && index < m_song_list_ctrl.GetItemCount()) ||*/ !m_right_selected_items.empty());
}

bool CMediaClassifyDlg::IsItemMatchKeyWord(const SongInfo& song, const wstring& key_word)
{
    if (m_type == CMediaClassifier::CT_ARTIST)
        return IsItemMatchKeyWord(song.artist, key_word);
    else if(m_type == CMediaClassifier::CT_ALBUM)
        return IsItemMatchKeyWord(song.album, key_word);
    else if (m_type == CMediaClassifier::CT_GENRE)
        return IsItemMatchKeyWord(song.genre, key_word);
    else if (m_type == CMediaClassifier::CT_YEAR)
        return IsItemMatchKeyWord(song.year, key_word);
    return false;
}

bool CMediaClassifyDlg::IsItemMatchKeyWord(const wstring& str, const wstring& key_word)
{
    return CCommon::StringFindNoCase(str, key_word) != wstring::npos;
}

void CMediaClassifyDlg::QuickSearch(const wstring& key_word)
{
    m_search_result.clear();
    std::vector<SongInfo> other_list;
    for (const auto& item : m_classifer.GetMeidaList())
    {
        if (item.first == STR_OTHER_CLASSIFY_TYPE)
        {
            for (const auto& song : item.second)
            {
                if (IsItemMatchKeyWord(song, key_word))
                    other_list.push_back(song);
            }
        }
        else
        {
            if (IsItemMatchKeyWord(item.first, key_word))
                m_search_result[item.first] = item.second;
        }
    }
    if (!other_list.empty())
        m_search_result[STR_OTHER_CLASSIFY_TYPE] = other_list;
}

void CMediaClassifyDlg::SetButtonsEnable()
{
    bool play_enable;
    if (m_left_selected)
        play_enable = (m_classify_list_ctrl.GetCurSel() >= 0);
    else
        play_enable = (m_song_list_ctrl.GetCurSel() >= 0);
    SetButtonsEnable(play_enable);
}

void CMediaClassifyDlg::SetButtonsEnable(bool enable)
{
    CWnd* pParent = GetParentWindow();
    ::SendMessage(pParent->GetSafeHwnd(), WM_PLAY_SELECTED_BTN_ENABLE, WPARAM(enable), 0);
}

void CMediaClassifyDlg::OnTabEntered()
{
    SetButtonsEnable();
}

bool CMediaClassifyDlg::_OnAddToNewPlaylist(std::wstring& playlist_path)
{
    CInputDlg imput_dlg;
    imput_dlg.SetTitle(CCommon::LoadText(IDS_NEW_PLAYLIST));
    imput_dlg.SetInfoText(CCommon::LoadText(IDS_INPUT_PLAYLIST_NAME));
    //如果选中了左侧列表，则添加到新建播放列表时名称自动填上选中项的名称

    if(m_classify_selected != STR_OTHER_CLASSIFY_TYPE)
        imput_dlg.SetEditText(m_classify_selected);
    if (imput_dlg.DoModal() == IDOK)
    {
        CString playlist_name = imput_dlg.GetEditText();
        if (!CCommon::IsFileNameValid(wstring(playlist_name.GetString())))
        {
            MessageBox(CCommon::LoadText(IDS_FILE_NAME_INVALID_WARNING), NULL, MB_ICONWARNING | MB_OK);
            return false;
        }
        playlist_path = theApp.m_playlist_dir + playlist_name.GetString() + PLAYLIST_EXTENSION;
        if (CCommon::FileExist(playlist_path))
        {
            MessageBox(CCommon::LoadTextFormat(IDS_PLAYLIST_EXIST_WARNING, { playlist_name }), NULL, MB_ICONWARNING | MB_OK);
            return false;
        }
        //添加空的播放列表
        CPlayer::GetInstance().GetRecentPlaylist().AddNewPlaylist(playlist_path);

        //获取选中的曲目的路径
        std::vector<SongInfo> selected_item_path;
        GetSongsSelected(selected_item_path);

        CPlaylist playlist;
        playlist.LoadFromFile(playlist_path);
        playlist.AddFiles(selected_item_path);
        playlist.SaveToFile(playlist_path);
        theApp.m_pMainWnd->SendMessage(WM_INIT_ADD_TO_MENU);
        return true;
    }
    return false;
}

void CMediaClassifyDlg::CalculateClassifyListColumeWidth(std::vector<int>& width)
{
    CRect rect;
    m_classify_list_ctrl.GetWindowRect(rect);
    width.resize(2);

    width[1] = theApp.DPI(50);
    width[0] = rect.Width() - width[1] - theApp.DPI(20) - 1;
}

UINT CMediaClassifyDlg::ViewOnlineThreadFunc(LPVOID lpParam)
{
    CMediaClassifyDlg* pThis = (CMediaClassifyDlg*)(lpParam);
    if (pThis == nullptr)
        return 0;
    CCommon::SetThreadLanguage(theApp.m_general_setting_data.language);
    //此命令用于跳转到歌曲对应的网易云音乐的在线页面
    if (pThis->m_right_selected_item >= 0)
    {
        wstring file_path = pThis->m_song_list_ctrl.GetItemText(pThis->m_right_selected_item, COL_PATH).GetString();
        if (CCommon::FileExist(file_path))
        {
            SongInfo song{ theApp.m_song_data[file_path] };
            song.file_path = file_path;
            CMusicPlayerCmdHelper cmd_helper(pThis);
            cmd_helper.VeiwOnline(song);
        }

    }
    return 0;

}

void CMediaClassifyDlg::DoDataExchange(CDataExchange* pDX)
{
    CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CLASSIFY_LIST, m_classify_list_ctrl);
    DDX_Control(pDX, IDC_SONG_LIST, m_song_list_ctrl);
    DDX_Control(pDX, IDC_MFCEDITBROWSE1, m_search_edit);
}


BEGIN_MESSAGE_MAP(CMediaClassifyDlg, CTabDlg)
    ON_NOTIFY(NM_CLICK, IDC_CLASSIFY_LIST, &CMediaClassifyDlg::OnNMClickClassifyList)
    ON_NOTIFY(NM_RCLICK, IDC_CLASSIFY_LIST, &CMediaClassifyDlg::OnNMRClickClassifyList)
    ON_EN_CHANGE(IDC_MFCEDITBROWSE1, &CMediaClassifyDlg::OnEnChangeMfceditbrowse1)
    ON_MESSAGE(WM_SEARCH_EDIT_BTN_CLICKED, &CMediaClassifyDlg::OnSearchEditBtnClicked)
    ON_NOTIFY(NM_CLICK, IDC_SONG_LIST, &CMediaClassifyDlg::OnNMClickSongList)
    ON_NOTIFY(NM_RCLICK, IDC_SONG_LIST, &CMediaClassifyDlg::OnNMRClickSongList)
    ON_COMMAND(ID_PLAY_ITEM, &CMediaClassifyDlg::OnPlayItem)
    ON_COMMAND(ID_EXPLORE_ONLINE, &CMediaClassifyDlg::OnExploreOnline)
    ON_COMMAND(ID_FORMAT_CONVERT, &CMediaClassifyDlg::OnFormatConvert)
    ON_COMMAND(ID_EXPLORE_TRACK, &CMediaClassifyDlg::OnExploreTrack)
    ON_COMMAND(ID_ITEM_PROPERTY, &CMediaClassifyDlg::OnItemProperty)
    ON_WM_INITMENU()
    ON_COMMAND(ID_ADD_TO_NEW_PLAYLIST, &CMediaClassifyDlg::OnAddToNewPlaylist)
    ON_COMMAND(ID_ADD_TO_NEW_PALYLIST_AND_PLAY, &CMediaClassifyDlg::OnAddToNewPalylistAndPlay)
    ON_NOTIFY(NM_DBLCLK, IDC_CLASSIFY_LIST, &CMediaClassifyDlg::OnNMDblclkClassifyList)
    ON_NOTIFY(NM_DBLCLK, IDC_SONG_LIST, &CMediaClassifyDlg::OnNMDblclkSongList)
    ON_NOTIFY(HDN_ITEMCLICK, 0, &CMediaClassifyDlg::OnHdnItemclickSongList)
    ON_COMMAND(ID_PLAY_ITEM_IN_FOLDER_MODE, &CMediaClassifyDlg::OnPlayItemInFolderMode)
    ON_COMMAND(ID_COPY_TEXT, &CMediaClassifyDlg::OnCopyText)
    ON_WM_SIZE()
END_MESSAGE_MAP()


// CMediaClassifyDlg 消息处理程序


BOOL CMediaClassifyDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    CCommon::SetDialogFont(this, theApp.m_pMainWnd->GetFont());     //由于此对话框资源由不同语言共用，所以这里要设置一下字体

    //初始化左侧列表
    m_classify_list_ctrl.SetExtendedStyle(m_classify_list_ctrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
    CString title_name;
    if (m_type == CMediaClassifier::CT_ARTIST)
        title_name = CCommon::LoadText(IDS_ARTIST);
    else if (m_type == CMediaClassifier::CT_ALBUM)
        title_name = CCommon::LoadText(IDS_ALBUM);
    else if (m_type == CMediaClassifier::CT_GENRE)
        title_name = CCommon::LoadText(IDS_GENRE);
    else if (m_type == CMediaClassifier::CT_YEAR)
        title_name = CCommon::LoadText(IDS_YEAR);
    CRect rc_classify_list;
    m_classify_list_ctrl.GetWindowRect(rc_classify_list);
    std::vector<int> width;
    CalculateClassifyListColumeWidth(width);
    m_classify_list_ctrl.InsertColumn(0, title_name, LVCFMT_LEFT, width[0]);
    m_classify_list_ctrl.InsertColumn(1, CCommon::LoadText(IDS_TRACK_TOTAL_NUM), LVCFMT_LEFT, width[1]);
    ShowClassifyList();

    //初始化右侧列表
    m_song_list_ctrl.SetExtendedStyle(m_song_list_ctrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
    //CRect rc_song_list;
    //m_song_list_ctrl.GetWindowRect(rc_song_list);
    m_song_list_ctrl.InsertColumn(0, CCommon::LoadText(IDS_TITLE), LVCFMT_LEFT, theApp.DPI(150));
    m_song_list_ctrl.InsertColumn(1, CCommon::LoadText(IDS_ARTIST), LVCFMT_LEFT, theApp.DPI(100));
    m_song_list_ctrl.InsertColumn(2, CCommon::LoadText(IDS_ALBUM), LVCFMT_LEFT, theApp.DPI(150));
    m_song_list_ctrl.InsertColumn(3, CCommon::LoadText(IDS_TRACK_NUM), LVCFMT_LEFT, theApp.DPI(60));
    m_song_list_ctrl.InsertColumn(4, CCommon::LoadText(IDS_GENRE), LVCFMT_LEFT, theApp.DPI(100));
    m_song_list_ctrl.InsertColumn(5, CCommon::LoadText(IDS_FILE_PATH), LVCFMT_LEFT, theApp.DPI(600));

    if (m_type == CMediaClassifier::CT_ARTIST)
        m_search_edit.SetCueBanner(CCommon::LoadText(IDS_SEARCH_ARTIST), TRUE);
    else if(m_type == CMediaClassifier::CT_ALBUM)
        m_search_edit.SetCueBanner(CCommon::LoadText(IDS_SEARCH_ALBUM), TRUE);
    else if (m_type == CMediaClassifier::CT_GENRE)
        m_search_edit.SetCueBanner(CCommon::LoadText(IDS_SEARCH_GENRE), TRUE);
    else if (m_type == CMediaClassifier::CT_YEAR)
        m_search_edit.SetCueBanner(CCommon::LoadText(IDS_SEARCH_YEAR), TRUE);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CMediaClassifyDlg::OnNMClickClassifyList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    ClassifyListClicked(pNMItemActivate->iItem);
    *pResult = 0;
}


void CMediaClassifyDlg::OnNMRClickClassifyList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    ClassifyListClicked(pNMItemActivate->iItem);
    m_selected_string = m_classify_list_ctrl.GetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem);

    //弹出右键菜单
    CMenu* pMenu = theApp.m_menu_set.m_media_lib_popup_menu.GetSubMenu(0);
    ASSERT(pMenu != nullptr);
    if (pMenu != nullptr)
    {
        m_classify_list_ctrl.ShowPopupMenu(pMenu, pNMItemActivate->iItem, this);
    }

    *pResult = 0;
}


void CMediaClassifyDlg::OnEnChangeMfceditbrowse1()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CTabDlg::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    CString str;
    m_search_edit.GetWindowText(str);
    QuickSearch(wstring(str));
    m_searched = !str.IsEmpty();
    ShowClassifyList();
    m_song_list_ctrl.DeleteAllItems();
    last_selected_index = -2;
}


afx_msg LRESULT CMediaClassifyDlg::OnSearchEditBtnClicked(WPARAM wParam, LPARAM lParam)
{
    //点击搜索框中的叉按钮时清除搜索结果
    if (m_searched)
    {
        //清除搜索结果
        m_searched = false;
        m_search_edit.SetWindowText(_T(""));
        ShowClassifyList();
        m_song_list_ctrl.DeleteAllItems();
    }
    return 0;
}


void CMediaClassifyDlg::OnNMClickSongList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    SongListClicked(pNMItemActivate->iItem);
    *pResult = 0;
}


void CMediaClassifyDlg::OnNMRClickSongList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    SongListClicked(pNMItemActivate->iItem);
    m_selected_string = m_song_list_ctrl.GetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem);

    //弹出右键菜单
    CMenu* pMenu = theApp.m_menu_set.m_media_lib_popup_menu.GetSubMenu(1);
    ASSERT(pMenu != nullptr);
    if (pMenu != nullptr)
    {
        m_song_list_ctrl.ShowPopupMenu(pMenu, pNMItemActivate->iItem, this);
    }
    
    *pResult = 0;
}


void CMediaClassifyDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    std::vector<wstring> files;
    GetSongsSelected(files);
    if(!files.empty())
    {
        CPlayer::GetInstance().OpenFilesInTempPlaylist(files);

        CTabDlg::OnOK();
        CWnd* pParent = GetParentWindow();
        if (pParent != nullptr)
            ::SendMessage(pParent->GetSafeHwnd(), WM_COMMAND, IDOK, 0);
    }
}


void CMediaClassifyDlg::OnPlayItem()
{
    // TODO: 在此添加命令处理程序代码
    OnOK();
}


void CMediaClassifyDlg::OnExploreOnline()
{
    // TODO: 在此添加命令处理程序代码
    AfxBeginThread(ViewOnlineThreadFunc, (void*)this);
}


void CMediaClassifyDlg::OnFormatConvert()
{
    // TODO: 在此添加命令处理程序代码
    std::vector<SongInfo> songs;
    GetSongsSelected(songs);
    CMusicPlayerCmdHelper cmd_helper(this);
    cmd_helper.FormatConvert(songs);
}


void CMediaClassifyDlg::OnExploreTrack()
{
    // TODO: 在此添加命令处理程序代码
    if (m_right_selected_item < 0)
        return;
    wstring file_path = m_song_list_ctrl.GetItemText(m_right_selected_item, COL_PATH).GetString();
    if (!file_path.empty())
    {
        CString str;
        str.Format(_T("/select,\"%s\""), file_path.c_str());
        ShellExecute(NULL, _T("open"), _T("explorer"), str, NULL, SW_SHOWNORMAL);

    }
}


void CMediaClassifyDlg::OnItemProperty()
{
    // TODO: 在此添加命令处理程序代码
    if (m_right_selected_item < 0)
        return;
    std::vector<SongInfo> songs;
    GetCurrentSongList(songs);
    CPropertyDlg propertyDlg(songs);
    propertyDlg.m_index = m_right_selected_item;
    propertyDlg.DoModal();
    if (propertyDlg.GetListRefresh())
        ShowSongList();
}


void CMediaClassifyDlg::OnInitMenu(CMenu* pMenu)
{
    CTabDlg::OnInitMenu(pMenu);

    bool select_valid;
    if (m_left_selected)
        select_valid = !m_left_selected_items.empty();
    else
        select_valid = !m_right_selected_items.empty();

    pMenu->EnableMenuItem(ID_PLAY_ITEM, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_PLAY_ITEM_IN_FOLDER_MODE, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_ADD_TO_DEFAULT_PLAYLIST, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_ADD_TO_MY_FAVOURITE, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    for (UINT id = ID_ADD_TO_MY_FAVOURITE + 1; id < ID_ADD_TO_MY_FAVOURITE + ADD_TO_PLAYLIST_MAX_SIZE; id++)
    {
        pMenu->EnableMenuItem(id, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    }
    pMenu->EnableMenuItem(ID_ADD_TO_NEW_PLAYLIST, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_ADD_TO_NEW_PALYLIST_AND_PLAY, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_EXPLORE_ONLINE, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_FORMAT_CONVERT, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_EXPLORE_TRACK, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_ITEM_PROPERTY, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));

    pMenu->SetDefaultItem(ID_PLAY_ITEM);

    // TODO: 在此处添加消息处理程序代码
}


BOOL CMediaClassifyDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    // TODO: 在此添加专用代码和/或调用基类
    WORD command = LOWORD(wParam);
    //响应播放列表右键菜单中的“添加到播放列表”
    if (command >= ID_ADD_TO_DEFAULT_PLAYLIST && command <= ID_ADD_TO_MY_FAVOURITE + ADD_TO_PLAYLIST_MAX_SIZE)
    {
        //获取选中的曲目的路径
        std::vector<SongInfo> selected_item_path;
        GetSongsSelected(selected_item_path);
        if (command == ID_ADD_TO_DEFAULT_PLAYLIST)      //添加到默认播放列表
        {
            std::wstring default_playlist_path = CPlayer::GetInstance().GetRecentPlaylist().m_default_playlist.path;
            CPlaylist playlist;
            playlist.LoadFromFile(default_playlist_path);
            playlist.AddFiles(selected_item_path);
            playlist.SaveToFile(default_playlist_path);

        }
        else if (command == ID_ADD_TO_MY_FAVOURITE)      //添加到“我喜欢”播放列表
        {
            std::wstring favourite_playlist_path = CPlayer::GetInstance().GetRecentPlaylist().m_favourite_playlist.path;
            CPlaylist playlist;
            playlist.LoadFromFile(favourite_playlist_path);
            playlist.AddFiles(selected_item_path);
            playlist.SaveToFile(favourite_playlist_path);
        }
        else        //添加到选中的播放列表
        {
            CString menu_string;
            theApp.m_menu_set.m_list_popup_menu.GetMenuString(command, menu_string, 0);
            if (!menu_string.IsEmpty())
            {
                wstring playlist_path = theApp.m_playlist_dir + menu_string.GetString() + PLAYLIST_EXTENSION;
                if (CCommon::FileExist(playlist_path))
                {
                    CPlaylist playlist;
                    playlist.LoadFromFile(playlist_path);
                    playlist.AddFiles(selected_item_path);
                    playlist.SaveToFile(playlist_path);
                }
                else
                {
                    MessageBox(CCommon::LoadText(IDS_ADD_TO_PLAYLIST_FAILED_WARNING), NULL, MB_ICONWARNING | MB_OK);
                }
            }
        }
    }

    return CTabDlg::OnCommand(wParam, lParam);
}


void CMediaClassifyDlg::OnAddToNewPlaylist()
{
    // TODO: 在此添加命令处理程序代码
    wstring playlist_path;
    _OnAddToNewPlaylist(playlist_path);
}


void CMediaClassifyDlg::OnAddToNewPalylistAndPlay()
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


void CMediaClassifyDlg::OnCancel()
{
    // TODO: 在此添加专用代码和/或调用基类

    CTabDlg::OnCancel();

    CWnd* pParent = GetParentWindow();
    if (pParent != nullptr)
        ::SendMessage(pParent->GetSafeHwnd(), WM_COMMAND, IDCANCEL, 0);
}


void CMediaClassifyDlg::OnNMDblclkClassifyList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    OnOK();
    *pResult = 0;
}


void CMediaClassifyDlg::OnNMDblclkSongList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    OnOK();
    *pResult = 0;
}


void CMediaClassifyDlg::OnHdnItemclickSongList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码

    if(phdr->hdr.hwndFrom == m_song_list_ctrl.GetHeaderCtrl()->GetSafeHwnd())
    {
        static bool ascending = false;
        ascending = !ascending;

        static int last_item = -1;
        if (last_item != phdr->iItem)
        {
            last_item = phdr->iItem;
            ascending = true;
        }

        //对右侧列表排序
        if (m_left_selected_items.size() == 1)  //仅当左侧列表选中了一项时处理
        {
            auto iter = m_classifer.GetMeidaList().find(wstring(m_classify_selected));
            if (iter != m_classifer.GetMeidaList().end())
            {
                switch (phdr->iItem)
                {
                case CMediaClassifyDlg::COL_TITLE:
                    std::sort(iter->second.begin(), iter->second.end(), [](const SongInfo& a, const SongInfo& b) { if (ascending) return a.title < b.title; else return a.title > b.title; });
                    ShowSongList(false);
                    break;
                case CMediaClassifyDlg::COL_ARTIST:
                    std::sort(iter->second.begin(), iter->second.end(), [](const SongInfo& a, const SongInfo& b) { if (ascending) return a.artist < b.artist; else return a.artist > b.artist; });
                    ShowSongList(false);
                    break;
                case CMediaClassifyDlg::COL_ALBUM:
                    std::sort(iter->second.begin(), iter->second.end(), [](const SongInfo& a, const SongInfo& b) { if (ascending) return a.album < b.album; else return a.album > b.album; });
                    ShowSongList(false);
                    break;
                case CMediaClassifyDlg::COL_TRACK:
                    std::sort(iter->second.begin(), iter->second.end(), [](const SongInfo& a, const SongInfo& b) { if (ascending) return a.track < b.track; else return a.track > b.track; });
                    ShowSongList(false);
                    break;
                case CMediaClassifyDlg::COL_GENRE:
                    std::sort(iter->second.begin(), iter->second.end(), [](const SongInfo& a, const SongInfo& b) { if (ascending) return a.genre < b.genre; else return a.genre > b.genre; });
                    ShowSongList(false);
                    break;
                case CMediaClassifyDlg::COL_PATH:
                    std::sort(iter->second.begin(), iter->second.end(), [](const SongInfo& a, const SongInfo& b) { if (ascending) return a.file_path < b.file_path; else return a.file_path > b.file_path; });
                    ShowSongList(false);
                    break;
                default:
                    break;
                }
            }
        }
    }

    *pResult = 0;
}


void CMediaClassifyDlg::OnPlayItemInFolderMode()
{
    // TODO: 在此添加命令处理程序代码

    if (m_right_selected_item >= 0)
    {
        std::wstring file_path = m_song_list_ctrl.GetItemText(m_right_selected_item, COL_PATH);
        CPlayer::GetInstance().OpenAFile(file_path, true);

        CTabDlg::OnOK();
        CWnd* pParent = GetParentWindow();
        if (pParent != nullptr)
            ::SendMessage(pParent->GetSafeHwnd(), WM_COMMAND, IDOK, 0);
    }
}


void CMediaClassifyDlg::OnCopyText()
{
    // TODO: 在此添加命令处理程序代码
    if (!CCommon::CopyStringToClipboard(wstring(m_selected_string)))
        MessageBox(CCommon::LoadText(IDS_COPY_CLIPBOARD_FAILED), NULL, MB_ICONWARNING);
}


void CMediaClassifyDlg::OnSize(UINT nType, int cx, int cy)
{
    CTabDlg::OnSize(nType, cx, cy);

    // TODO: 在此处添加消息处理程序代码
    if (nType != SIZE_MINIMIZED && m_classify_list_ctrl.m_hWnd)
    {
        std::vector<int> width;
        CalculateClassifyListColumeWidth(width);
        for (size_t i{}; i < width.size(); i++)
            m_classify_list_ctrl.SetColumnWidth(i, width[i]);
    }
}
