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
#include "AddToPlaylistDlg.h"
#include "SongDataManager.h"


// CMediaClassifyDlg 对话框

IMPLEMENT_DYNAMIC(CMediaClassifyDlg, CMediaLibTabDlg)

CMediaClassifyDlg::CMediaClassifyDlg(CMediaClassifier::ClassificationType type, CWnd* pParent /*=nullptr*/)
    : CMediaLibTabDlg(IDD_MEDIA_CLASSIFY_DIALOG, pParent), m_type(type),
    m_classifer(type, theApp.m_media_lib_setting_data.hide_only_one_classification)
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
                    song_list.push_back(item.file_path);
                }
            }
        }
    }
    else
    {
        //auto iter = media_list.find(wstring(m_classify_selected));
        //if (iter != media_list.end())
        //{
        //    for (int index : m_right_selected_items)
        //    {
        //        if (index >= 0 && index < iter->second.size())
        //            song_list.push_back(iter->second[index].file_path);
        //    }
        //}

        for (int index : m_right_selected_items)
        {
            wstring file_path = m_song_list_ctrl.GetItemText(index, COL_PATH).GetString();
            song_list.push_back(file_path);
        }
    }
}


//void CMediaClassifyDlg::GetCurrentSongList(std::vector<SongInfo>& song_list) const
//{
//    auto& media_list{ m_searched ? m_search_result : m_classifer.GetMeidaList() };
//    auto iter = media_list.find(wstring(m_classify_selected));
//    if (iter != media_list.end())
//    {
//        song_list = iter->second;
//    }
//}

void CMediaClassifyDlg::RefreshData()
{
    m_classifer.SetHideOnlyOneClassification(theApp.m_media_lib_setting_data.hide_only_one_classification);
    if (m_initialized)
    {
        m_classifer.ClassifyMedia();
        ShowClassifyList();
    }
}

bool CMediaClassifyDlg::SetLeftListSel(const wstring& item)
{
    int list_size = static_cast<int>(m_list_data_left.size());
    //遍历左侧列表，寻找匹配匹配的项目
    for (int i = 0; i < list_size; i++)
    {
        wstring name = m_list_data_left[i][0];
        if (CCommon::StringCompareNoCase(name, item))
        {
            m_classify_list_ctrl.SetCurSel(i);
            ClassifyListClicked(i);
            return true;
        }
    }
    //如果在媒体库中设置了“将只有一项的分类归到其他类中”，则要寻找的项目可能在“其他”类中
    int other_index{ -1 };              //项目在“其他”类中的索引
    //在“其他”类中寻找
    auto iter = m_classifer.GetMeidaList().find(STR_OTHER_CLASSIFY_TYPE);
    if (iter != m_classifer.GetMeidaList().end())
    {
        int i = 0;
        for (const auto& song : iter->second)
        {
            if (m_type == CMediaClassifier::CT_ARTIST)
            {
                vector<wstring> artist_list;
                song.GetArtistList(artist_list);
                if (CCommon::IsItemInVector(artist_list, item))
                {
                    other_index = i;
                    break;
                }
            }
            else if (m_type == CMediaClassifier::CT_ALBUM)
            {
                if (item == song.GetAlbum())
                {
                    other_index = i;
                    break;
                }
            }
            i++;
        }
    }

    if (other_index >= 0)
    {
        m_classify_list_ctrl.SetCurSel(m_classify_list_ctrl.GetItemCount() - 1);        //在左侧列表中选中“其他”类
        ClassifyListClicked(m_classify_list_ctrl.GetItemCount() - 1);
        m_song_list_ctrl.SetCurSel(other_index);
        return true;
    }
    return false;
}

void CMediaClassifyDlg::RefreshSongList()
{
    ShowSongList();
}

void CMediaClassifyDlg::ShowClassifyList()
{
    CWaitCursor wait_cursor;
    auto& media_list{ m_searched ? m_search_result : m_classifer.GetMeidaList() };
    m_list_data_left.clear();
    for (const auto& item : media_list)
    {
        if (item.first == STR_OTHER_CLASSIFY_TYPE)       //跳过“其他”分类
            continue;

        wstring item_name = item.first;
        if (item_name.empty())
        {
            item_name = m_default_str.GetString();
        }

        CListCtrlEx::RowData row_data;
        row_data[0] = item_name;
        row_data[1] = std::to_wstring(item.second.size());
        m_list_data_left.push_back(std::move(row_data));
    }

    if (m_type == CMediaClassifier::CT_BITRATE)
    {
        //如果类型是比特率，则对其进行自定义排序
        std::sort(m_list_data_left.begin(), m_list_data_left.end(), [](const CListCtrlEx::RowData& a, const CListCtrlEx::RowData& b)
            {
                wstring str_a = a.at(0);
                wstring str_b = b.at(0);

                auto normalize_string = [](wstring& str)
                {
                    if (!str.empty() && (str[0] < L'0' || str[0] > L'9'))
                        str = str.substr(1);
                    if (str.size() < 5)
                        str = wstring(5 - str.size(), L'0') + str;
                };

                normalize_string(str_a);
                normalize_string(str_b);
                return str_a < str_b;
            });
    }

    //将“其他”分类放到列表的最后面
    auto iter = media_list.find(STR_OTHER_CLASSIFY_TYPE);
    if (iter != media_list.end())
    {
        CString item_name = CCommon::LoadText(_T("<"), IDS_OTHER, _T(">"));
        CListCtrlEx::RowData row_data;
        row_data[0] = wstring(item_name);
        row_data[1] = std::to_wstring(iter->second.size());
        m_list_data_left.push_back(std::move(row_data));
    }
    m_classify_list_ctrl.SetListData(&m_list_data_left);
}

void CMediaClassifyDlg::ShowSongList()
{
    CWaitCursor wait_cursor;
    auto& media_list{ m_searched ? m_search_result : m_classifer.GetMeidaList() };

    m_list_data.clear();
    for (int index : m_left_selected_items)
    {
        CString str_selected = GetClassifyListSelectedString(index);

        auto iter = media_list.find(wstring(str_selected));
        if (iter != media_list.end())
        {
            for (const auto& item : iter->second)
            {
                const SongInfo song{ CSongDataManager::GetInstance().GetSongInfo(item.file_path) };
                CListCtrlEx::RowData row_data;
                row_data[COL_TITLE] = song.GetTitle();
                row_data[COL_ARTIST] = song.GetArtist();
                row_data[COL_ALBUM] = song.GetAlbum();
                std::wstring track_str;
                if (song.track != 0)
                    track_str = std::to_wstring(song.track);
                row_data[COL_TRACK] = track_str;
                row_data[COL_GENRE] = song.GetGenre();
                row_data[COL_BITRATE] = (song.bitrate == 0 ? L"-" : std::to_wstring(song.bitrate));
                row_data[COL_PATH] = song.file_path;
                m_list_data.push_back(std::move(row_data));
            }
        }
    }

    m_song_list_ctrl.SetListData(&m_list_data);
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
    m_right_selected_item = -1;           // 点击左侧列表时清空右侧列表选中项
    m_right_selected_items.clear();
    m_song_list_ctrl.SelectNone();

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
    else if (m_type == CMediaClassifier::CT_ALBUM)
        return IsItemMatchKeyWord(song.album, key_word);
    else if (m_type == CMediaClassifier::CT_GENRE)
        return IsItemMatchKeyWord(song.genre, key_word);
    else if (m_type == CMediaClassifier::CT_YEAR)
        return IsItemMatchKeyWord(song.get_year(), key_word);
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
    if (!m_initialized)
    {
        CWaitCursor wait_cursor;
        m_classifer.ClassifyMedia();
        ShowClassifyList();
        m_initialized = true;
    }
}

bool CMediaClassifyDlg::_OnAddToNewPlaylist(std::wstring& playlist_path)
{
    std::wstring default_name;
    //如果选中了左侧列表，则添加到新建播放列表时名称自动填上选中项的名称
    if (m_classify_selected != STR_OTHER_CLASSIFY_TYPE)
        default_name = m_classify_selected;

    auto getSongList = [&](std::vector<SongInfo>& song_list)
    {
        CMediaLibTabDlg::GetSongsSelected(song_list);
    };
    CMusicPlayerCmdHelper cmd_helper(this);
    return cmd_helper.OnAddToNewPlaylist(getSongList, playlist_path, default_name);
}

void CMediaClassifyDlg::CalculateClassifyListColumeWidth(std::vector<int>& width)
{
    CRect rect;
    m_classify_list_ctrl.GetWindowRect(rect);
    width.resize(2);

    width[1] = theApp.DPI(50);
    width[0] = rect.Width() - width[1] - theApp.DPI(20) - 1;
}

const CListCtrlEx& CMediaClassifyDlg::GetSongListCtrl() const
{
    return m_song_list_ctrl;
}

int CMediaClassifyDlg::GetItemSelected() const
{
    return m_right_selected_item;
}

const vector<int>& CMediaClassifyDlg::GetItemsSelected() const
{
    return m_right_selected_items;
}

void CMediaClassifyDlg::AfterDeleteFromDisk(const std::vector<SongInfo>& files)
{
    //删除成功，则刷新列表
    m_classifer.RemoveFiles(files);
    ShowSongList();
}

int CMediaClassifyDlg::GetPathColIndex() const
{
    return COL_PATH;
}

wstring CMediaClassifyDlg::GetSelectedString() const
{
    return wstring(m_selected_string);
}

void CMediaClassifyDlg::DoDataExchange(CDataExchange* pDX)
{
    CMediaLibTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CLASSIFY_LIST, m_classify_list_ctrl);
    DDX_Control(pDX, IDC_SONG_LIST, m_song_list_ctrl);
    DDX_Control(pDX, IDC_MFCEDITBROWSE1, m_search_edit);
}


BEGIN_MESSAGE_MAP(CMediaClassifyDlg, CMediaLibTabDlg)
    ON_NOTIFY(NM_CLICK, IDC_CLASSIFY_LIST, &CMediaClassifyDlg::OnNMClickClassifyList)
    ON_NOTIFY(NM_RCLICK, IDC_CLASSIFY_LIST, &CMediaClassifyDlg::OnNMRClickClassifyList)
    ON_EN_CHANGE(IDC_MFCEDITBROWSE1, &CMediaClassifyDlg::OnEnChangeMfceditbrowse1)
    ON_MESSAGE(WM_SEARCH_EDIT_BTN_CLICKED, &CMediaClassifyDlg::OnSearchEditBtnClicked)
    ON_NOTIFY(NM_CLICK, IDC_SONG_LIST, &CMediaClassifyDlg::OnNMClickSongList)
    ON_NOTIFY(NM_RCLICK, IDC_SONG_LIST, &CMediaClassifyDlg::OnNMRClickSongList)
    ON_NOTIFY(NM_DBLCLK, IDC_CLASSIFY_LIST, &CMediaClassifyDlg::OnNMDblclkClassifyList)
    ON_NOTIFY(NM_DBLCLK, IDC_SONG_LIST, &CMediaClassifyDlg::OnNMDblclkSongList)
    ON_NOTIFY(HDN_ITEMCLICK, 0, &CMediaClassifyDlg::OnHdnItemclickSongList)
    ON_WM_SIZE()
    ON_WM_DESTROY()
    ON_COMMAND(ID_DELETE_FROM_DISK, &CMediaClassifyDlg::OnDeleteFromDisk)
END_MESSAGE_MAP()


// CMediaClassifyDlg 消息处理程序


BOOL CMediaClassifyDlg::OnInitDialog()
{
    CMediaLibTabDlg::OnInitDialog();

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
    else if (m_type == CMediaClassifier::CT_TYPE)
        title_name = CCommon::LoadText(IDS_FILE_TYPE);
    else if (m_type == CMediaClassifier::CT_BITRATE)
        title_name = CCommon::LoadText(IDS_BITRATE);
    else if (m_type == CMediaClassifier::CT_RATING)
        title_name = CCommon::LoadText(IDS_RATING);
    CRect rc_classify_list;
    m_classify_list_ctrl.GetWindowRect(rc_classify_list);
    std::vector<int> width;
    CalculateClassifyListColumeWidth(width);
    m_classify_list_ctrl.InsertColumn(0, title_name, LVCFMT_LEFT, width[0]);
    m_classify_list_ctrl.InsertColumn(1, CCommon::LoadText(IDS_TRACK_TOTAL_NUM), LVCFMT_LEFT, width[1]);
    //ShowClassifyList();

    //初始化右侧列表
    m_song_list_ctrl.SetExtendedStyle(m_song_list_ctrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
    //CRect rc_song_list;
    //m_song_list_ctrl.GetWindowRect(rc_song_list);
    m_song_list_ctrl.InsertColumn(0, CCommon::LoadText(IDS_TITLE), LVCFMT_LEFT, theApp.DPI(150));
    m_song_list_ctrl.InsertColumn(1, CCommon::LoadText(IDS_ARTIST), LVCFMT_LEFT, theApp.DPI(100));
    m_song_list_ctrl.InsertColumn(2, CCommon::LoadText(IDS_ALBUM), LVCFMT_LEFT, theApp.DPI(150));
    m_song_list_ctrl.InsertColumn(3, CCommon::LoadText(IDS_TRACK_NUM), LVCFMT_LEFT, theApp.DPI(60));
    m_song_list_ctrl.InsertColumn(4, CCommon::LoadText(IDS_GENRE), LVCFMT_LEFT, theApp.DPI(100));
    m_song_list_ctrl.InsertColumn(5, CCommon::LoadText(IDS_BITRATE), LVCFMT_LEFT, theApp.DPI(60));
    m_song_list_ctrl.InsertColumn(6, CCommon::LoadText(IDS_FILE_PATH), LVCFMT_LEFT, theApp.DPI(600));
    m_song_list_ctrl.SetCtrlAEnable(true);

    if (m_type == CMediaClassifier::CT_ARTIST)
        m_search_edit.SetCueBanner(CCommon::LoadText(IDS_SEARCH_ARTIST), TRUE);
    else if (m_type == CMediaClassifier::CT_ALBUM)
        m_search_edit.SetCueBanner(CCommon::LoadText(IDS_SEARCH_ALBUM), TRUE);
    else if (m_type == CMediaClassifier::CT_GENRE)
        m_search_edit.SetCueBanner(CCommon::LoadText(IDS_SEARCH_GENRE), TRUE);
    else if (m_type == CMediaClassifier::CT_YEAR)
        m_search_edit.SetCueBanner(CCommon::LoadText(IDS_SEARCH_YEAR), TRUE);
    else if (m_type == CMediaClassifier::CT_TYPE)
        m_search_edit.SetCueBanner(CCommon::LoadText(IDS_SEARCH_FILE_TYPE), TRUE);
    else if (m_type == CMediaClassifier::CT_BITRATE)
        m_search_edit.SetCueBanner(CCommon::LoadText(IDS_SEARCH_BITRATE), TRUE);
    else if (m_type == CMediaClassifier::CT_RATING)
        m_search_edit.EnableWindow(FALSE);
    else
        m_search_edit.SetCueBanner(CCommon::LoadText(IDS_SEARCH_HERE), TRUE);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CMediaClassifyDlg::OnNMClickClassifyList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    ClassifyListClicked(pNMItemActivate->iItem);
    *pResult = 0;
}


void CMediaClassifyDlg::OnNMRClickClassifyList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    ClassifyListClicked(pNMItemActivate->iItem);
    m_selected_string = m_classify_list_ctrl.GetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem);

    if (!m_left_selected_items.empty())
    {
        //弹出右键菜单
        CMenu* pMenu = theApp.m_menu_set.m_media_lib_popup_menu.GetSubMenu(0);
        ASSERT(pMenu != nullptr);
        if (pMenu != nullptr)
        {
            m_classify_list_ctrl.ShowPopupMenu(pMenu, pNMItemActivate->iItem, this);
        }
    }

    *pResult = 0;
}


void CMediaClassifyDlg::OnEnChangeMfceditbrowse1()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CMediaLibTabDlg::OnInitDialog()
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


void CMediaClassifyDlg::OnNMClickSongList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    SongListClicked(pNMItemActivate->iItem);
    *pResult = 0;
}


void CMediaClassifyDlg::OnNMRClickSongList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    SongListClicked(pNMItemActivate->iItem);
    m_selected_string = m_song_list_ctrl.GetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem);

    if (!m_right_selected_items.empty())
    {
        //弹出右键菜单
        CMenu* pMenu = theApp.m_menu_set.m_media_lib_popup_menu.GetSubMenu(1);
        ASSERT(pMenu != nullptr);
        if (pMenu != nullptr)
        {
            m_song_list_ctrl.ShowPopupMenu(pMenu, pNMItemActivate->iItem, this);
        }
    }

    *pResult = 0;
}


//void CMediaClassifyDlg::OnInitMenu(CMenu* pMenu)
//{
//    CMediaLibTabDlg::OnInitMenu(pMenu);
//
//    //bool select_valid;
//    //if (m_left_selected)
//    //    select_valid = !m_left_selected_items.empty();
//    //else
//    //    select_valid = !m_right_selected_items.empty();
//
//    //pMenu->EnableMenuItem(ID_PLAY_ITEM, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
//    //pMenu->EnableMenuItem(ID_PLAY_ITEM_IN_FOLDER_MODE, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
//    //pMenu->EnableMenuItem(ID_ADD_TO_DEFAULT_PLAYLIST, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
//    //pMenu->EnableMenuItem(ID_ADD_TO_MY_FAVOURITE, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
//    //for (UINT id = ID_ADD_TO_MY_FAVOURITE + 1; id < ID_ADD_TO_MY_FAVOURITE + ADD_TO_PLAYLIST_MAX_SIZE; id++)
//    //{
//    //    pMenu->EnableMenuItem(id, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
//    //}
//    //pMenu->EnableMenuItem(ID_ADD_TO_NEW_PLAYLIST, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
//    //pMenu->EnableMenuItem(ID_ADD_TO_NEW_PALYLIST_AND_PLAY, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
//    //pMenu->EnableMenuItem(ID_EXPLORE_ONLINE, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
//    //pMenu->EnableMenuItem(ID_FORMAT_CONVERT, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
//    //pMenu->EnableMenuItem(ID_EXPLORE_TRACK, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
//    //pMenu->EnableMenuItem(ID_ITEM_PROPERTY, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
//
//    pMenu->SetDefaultItem(ID_PLAY_ITEM);
//
//    // TODO: 在此处添加消息处理程序代码
//}




void CMediaClassifyDlg::OnNMDblclkClassifyList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    OnOK();
    *pResult = 0;
}


void CMediaClassifyDlg::OnNMDblclkSongList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    OnOK();
    *pResult = 0;
}


void CMediaClassifyDlg::OnHdnItemclickSongList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码

    if (phdr->hdr.hwndFrom == m_song_list_ctrl.GetHeaderCtrl()->GetSafeHwnd())
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
                    std::sort(iter->second.begin(), iter->second.end(), [](const SongInfo& a, const SongInfo& b) { if (ascending) return CCommon::StringCompareInLocalLanguage(a.title, b.title) < 0; else return CCommon::StringCompareInLocalLanguage(a.title, b.title) > 0; });
                    ShowSongList();
                    break;
                case CMediaClassifyDlg::COL_ARTIST:
                    std::sort(iter->second.begin(), iter->second.end(), [](const SongInfo& a, const SongInfo& b) { if (ascending) return CCommon::StringCompareInLocalLanguage(a.artist, b.artist) < 0; else return CCommon::StringCompareInLocalLanguage(a.artist, b.artist) > 0; });
                    ShowSongList();
                    break;
                case CMediaClassifyDlg::COL_ALBUM:
                    std::sort(iter->second.begin(), iter->second.end(), [](const SongInfo& a, const SongInfo& b) { if (ascending) return CCommon::StringCompareInLocalLanguage(a.album, b.album) < 0; else return CCommon::StringCompareInLocalLanguage(a.album, b.album) > 0; });
                    ShowSongList();
                    break;
                case CMediaClassifyDlg::COL_TRACK:
                    std::sort(iter->second.begin(), iter->second.end(), [](const SongInfo& a, const SongInfo& b) { if (ascending) return a.track < b.track; else return a.track > b.track; });
                    ShowSongList();
                    break;
                case CMediaClassifyDlg::COL_GENRE:
                    std::sort(iter->second.begin(), iter->second.end(), [](const SongInfo& a, const SongInfo& b) { if (ascending) return CCommon::StringCompareInLocalLanguage(a.genre, b.genre) < 0; else return CCommon::StringCompareInLocalLanguage(a.genre, b.genre) > 0; });
                    ShowSongList();
                    break;
                case CMediaClassifyDlg::COL_BITRATE:
                    std::sort(iter->second.begin(), iter->second.end(), [](const SongInfo& a, const SongInfo& b) { if (ascending) return a.bitrate < b.bitrate; else return a.bitrate > b.bitrate; });
                    ShowSongList();
                    break;
                case CMediaClassifyDlg::COL_PATH:
                    std::sort(iter->second.begin(), iter->second.end(), [](const SongInfo& a, const SongInfo& b) { if (ascending) return CCommon::StringCompareInLocalLanguage(a.file_path, b.file_path) < 0; else return CCommon::StringCompareInLocalLanguage(a.file_path, b.file_path) > 0; });
                    ShowSongList();
                    break;
                default:
                    break;
                }
            }
        }
    }

    *pResult = 0;
}


void CMediaClassifyDlg::OnSize(UINT nType, int cx, int cy)
{
    CMediaLibTabDlg::OnSize(nType, cx, cy);

    // TODO: 在此处添加消息处理程序代码
    if (nType != SIZE_MINIMIZED && m_classify_list_ctrl.m_hWnd)
    {
        std::vector<int> width;
        CalculateClassifyListColumeWidth(width);
        for (size_t i{}; i < width.size(); i++)
            m_classify_list_ctrl.SetColumnWidth(i, width[i]);
    }
}


void CMediaClassifyDlg::OnDestroy()
{
    CMediaLibTabDlg::OnDestroy();

    // TODO: 在此处添加消息处理程序代码
    m_classifer.ClearResult();
}

//
//void CMediaClassifyDlg::OnDeleteFromDisk()
//{
//    // TODO: 在此添加命令处理程序代码
//    vector<SongInfo> songs_selected;
//    GetSongsSelected(songs_selected);
//    CMusicPlayerCmdHelper helper;
//    if (helper.DeleteSongsFromDisk(songs_selected))
//    {
//        //删除成功，则刷新列表
//        m_classifer.RemoveFiles(songs_selected);
//        ShowSongList();
//    }
//
//}
