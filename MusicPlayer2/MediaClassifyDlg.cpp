// MediaClassfyDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "MediaClassifyDlg.h"
#include "Playlist.h"
#include "InputDlg.h"
#include "MusicPlayerCmdHelper.h"
#include "PropertyDlg.h"
#include "AddToPlaylistDlg.h"
#include "SongDataManager.h"
#include "Player.h"


// CMediaClassifyDlg 对话框

IMPLEMENT_DYNAMIC(CMediaClassifyDlg, CMediaLibTabDlg)

CMediaClassifyDlg::CMediaClassifyDlg(CMediaClassifier::ClassificationType type, CWnd* pParent /*=nullptr*/)
    : CMediaLibTabDlg(IDD_MEDIA_CLASSIFY_DIALOG, pParent), m_type(type),
    m_classifer(type, theApp.m_media_lib_setting_data.hide_only_one_classification)
{
    if (m_type == CMediaClassifier::CT_ARTIST)
        m_default_str = theApp.m_str_table.LoadText(L"TXT_EMPTY_ARTIST");
    else if (m_type == CMediaClassifier::CT_ALBUM)
        m_default_str = theApp.m_str_table.LoadText(L"TXT_EMPTY_ALBUM");
    else if (m_type == CMediaClassifier::CT_GENRE)
        m_default_str = theApp.m_str_table.LoadText(L"TXT_EMPTY_GENRE");
    else if (m_type == CMediaClassifier::CT_YEAR)
        m_default_str = theApp.m_str_table.LoadText(L"TXT_EMPTY_YEAR");
}

CMediaClassifyDlg::~CMediaClassifyDlg()
{
}

void CMediaClassifyDlg::RefreshData()
{
    m_classifer.SetHideOnlyOneClassification(theApp.m_media_lib_setting_data.hide_only_one_classification);
    if (m_initialized)
    {
        m_classifer.ClassifyMedia();
        ShowClassifyList();
    }
}

bool CMediaClassifyDlg::SetLeftListSel(wstring item)
{
    int list_size = static_cast<int>(m_list_data_left.size());
    item = CMediaLibPlaylistMgr::GetMediaLibItemDisplayName(m_type, item);  // 将空item转换为用于显示的<未知xxx>
    //遍历左侧列表，寻找匹配匹配的项目
    for (int i = 0; i < list_size; i++)
    {
        const wstring& name = m_list_data_left[i][0];
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
            item_name = m_default_str;
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
        CListCtrlEx::RowData row_data;
        row_data[0] = theApp.m_str_table.LoadText(L"TXT_CLASSIFY_OTHER");
        row_data[1] = std::to_wstring(iter->second.size());
        m_list_data_left.push_back(std::move(row_data));
    }
    m_classify_list_ctrl.SetListData(&m_list_data_left);
}

void CMediaClassifyDlg::ShowSongList()
{
    CWaitCursor wait_cursor;
    auto& media_list{ m_searched ? m_search_result : m_classifer.GetMeidaList() };

    m_list_data_right.clear();
    m_right_items.clear();
    int highlight_item{ -1 };
    int right_index{ 0 };
    for (int index : m_left_selected_items)
    {
        CString str_selected = GetClassifyListSelectedString(index);

        auto iter = media_list.find(wstring(str_selected));
        if (iter != media_list.end())
        {
            for (const auto& item : iter->second)
            {
                const SongInfo& song{ CSongDataManager::GetInstance().GetSongInfo3(item) };
                m_right_items.push_back(song);  // 更新显示列表同时存储一份右侧列表SongInfo

                //判断正在播放项
                if (song.IsSameSong(CPlayer::GetInstance().GetCurrentSongInfo()))
                    highlight_item = right_index;

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
                m_list_data_right.push_back(std::move(row_data));

                right_index++;
            }
        }
    }

    m_song_list_ctrl.SetListData(&m_list_data_right);
    if (CPlayer::GetInstance().IsMediaLibMode())
    {
        m_song_list_ctrl.SetHightItem(highlight_item);
        m_song_list_ctrl.EnsureVisible(highlight_item, FALSE);
    }
}

CString CMediaClassifyDlg::GetClassifyListSelectedString(int index) const
{
    CString str_selected = m_classify_list_ctrl.GetItemText(index, 0);
    if (str_selected == m_default_str.c_str())
        str_selected.Empty();
    if (str_selected == theApp.m_str_table.LoadText(L"TXT_CLASSIFY_OTHER").c_str())
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
    return theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(key_word, str);
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

        //设置左侧列表默认选中项
        if (CPlayer::GetInstance().IsMediaLibMode())
            SetLeftListSel(CPlayer::GetInstance().GetMedialibItemName());

        m_initialized = true;
    }
}

wstring CMediaClassifyDlg::GetNewPlaylistName() const
{
    std::wstring default_name;
    //如果选中了左侧列表，则添加到新建播放列表时名称自动填上选中项的名称
    if (m_classify_selected != STR_OTHER_CLASSIFY_TYPE)
        default_name = m_classify_selected;
    CCommon::FileNameNormalize(default_name);
    return default_name;
}

CMediaClassifier::ClassificationType CMediaClassifyDlg::GetClassificationType() const
{
    return m_type;
}

std::wstring CMediaClassifyDlg::GetClassificationItemName() const
{
    if (m_left_selected_items.size() > 1)    // 左侧列表有多个选中项时返回<其他>
        return STR_OTHER_CLASSIFY_TYPE;
    return m_classify_selected;
}

void CMediaClassifyDlg::CalculateClassifyListColumeWidth(std::vector<int>& width)
{
    CRect rect;
    m_classify_list_ctrl.GetWindowRect(rect);
    width.resize(2);

    width[1] = theApp.DPI(50);
    width[0] = rect.Width() - width[1] - theApp.DPI(20) - 1;
}

void CMediaClassifyDlg::GetSongsSelected(std::vector<SongInfo>& song_list) const
{
    if (m_left_selected)
    {
        //如果选中了左侧列表，则把选中分类下的所有曲目的路径添加到song_list中
        song_list.clear();
        auto& media_list{ m_searched ? m_search_result : m_classifer.GetMeidaList() };
        for (int index : m_left_selected_items)
        {
            wstring str_selected{ GetClassifyListSelectedString(index) };
            auto iter = media_list.find(str_selected);
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
        CMediaLibTabDlg::GetSongsSelected(song_list);
    }
}

const vector<SongInfo>& CMediaClassifyDlg::GetSongList() const
{
    return m_right_items;
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

wstring CMediaClassifyDlg::GetSelectedString() const
{
    return m_selected_string;
}

void CMediaClassifyDlg::DoDataExchange(CDataExchange* pDX)
{
    CMediaLibTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CLASSIFY_LIST, m_classify_list_ctrl);
    DDX_Control(pDX, IDC_SONG_LIST, m_song_list_ctrl);
    DDX_Control(pDX, IDC_MFCEDITBROWSE1, m_search_edit);
    DDX_Control(pDX, IDC_HSPLITER_STATIC, m_splitter_ctrl);
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
    ON_WM_INITMENU()
END_MESSAGE_MAP()


// CMediaClassifyDlg 消息处理程序


BOOL CMediaClassifyDlg::OnInitDialog()
{
    CMediaLibTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    //初始化左侧列表
    m_classify_list_ctrl.SetExtendedStyle(m_classify_list_ctrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
    wstring title_name;
    if (m_type == CMediaClassifier::CT_ARTIST)
        title_name = theApp.m_str_table.LoadText(L"TXT_ARTIST");
    else if (m_type == CMediaClassifier::CT_ALBUM)
        title_name = theApp.m_str_table.LoadText(L"TXT_ALBUM");
    else if (m_type == CMediaClassifier::CT_GENRE)
        title_name = theApp.m_str_table.LoadText(L"TXT_GENRE");
    else if (m_type == CMediaClassifier::CT_YEAR)
        title_name = theApp.m_str_table.LoadText(L"TXT_YEAR");
    else if (m_type == CMediaClassifier::CT_TYPE)
        title_name = theApp.m_str_table.LoadText(L"TXT_FILE_TYPE");
    else if (m_type == CMediaClassifier::CT_BITRATE)
        title_name = theApp.m_str_table.LoadText(L"TXT_BITRATE");
    else if (m_type == CMediaClassifier::CT_RATING)
        title_name = theApp.m_str_table.LoadText(L"TXT_RATING");
    CRect rc_classify_list;
    m_classify_list_ctrl.GetWindowRect(rc_classify_list);
    std::vector<int> width;
    CalculateClassifyListColumeWidth(width);
    m_classify_list_ctrl.InsertColumn(0, title_name.c_str(), LVCFMT_LEFT, width[0]);
    m_classify_list_ctrl.InsertColumn(1, theApp.m_str_table.LoadText(L"TXT_NUM_OF_TRACK").c_str(), LVCFMT_LEFT, width[1]);
    //ShowClassifyList();

    //初始化右侧列表
    m_song_list_ctrl.SetExtendedStyle(m_song_list_ctrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
    //CRect rc_song_list;
    //m_song_list_ctrl.GetWindowRect(rc_song_list);
    m_song_list_ctrl.InsertColumn(0, theApp.m_str_table.LoadText(L"TXT_TITLE").c_str(), LVCFMT_LEFT, theApp.DPI(150));
    m_song_list_ctrl.InsertColumn(1, theApp.m_str_table.LoadText(L"TXT_ARTIST").c_str(), LVCFMT_LEFT, theApp.DPI(100));
    m_song_list_ctrl.InsertColumn(2, theApp.m_str_table.LoadText(L"TXT_ALBUM").c_str(), LVCFMT_LEFT, theApp.DPI(150));
    m_song_list_ctrl.InsertColumn(3, theApp.m_str_table.LoadText(L"TXT_TRACK_NUM").c_str(), LVCFMT_LEFT, theApp.DPI(60));
    m_song_list_ctrl.InsertColumn(4, theApp.m_str_table.LoadText(L"TXT_GENRE").c_str(), LVCFMT_LEFT, theApp.DPI(100));
    m_song_list_ctrl.InsertColumn(5, theApp.m_str_table.LoadText(L"TXT_BITRATE").c_str(), LVCFMT_LEFT, theApp.DPI(60));
    m_song_list_ctrl.InsertColumn(6, theApp.m_str_table.LoadText(L"TXT_FILE_PATH").c_str(), LVCFMT_LEFT, theApp.DPI(600));
    m_song_list_ctrl.SetCtrlAEnable(true);

    if (m_type == CMediaClassifier::CT_ARTIST)
        m_search_edit.SetCueBanner(theApp.m_str_table.LoadText(L"TXT_SEARCH_PROMPT_ARTIST").c_str(), TRUE);
    else if (m_type == CMediaClassifier::CT_ALBUM)
        m_search_edit.SetCueBanner(theApp.m_str_table.LoadText(L"TXT_SEARCH_PROMPT_ALBUM").c_str(), TRUE);
    else if (m_type == CMediaClassifier::CT_GENRE)
        m_search_edit.SetCueBanner(theApp.m_str_table.LoadText(L"TXT_SEARCH_PROMPT_GENRE").c_str(), TRUE);
    else if (m_type == CMediaClassifier::CT_YEAR)
        m_search_edit.SetCueBanner(theApp.m_str_table.LoadText(L"TXT_SEARCH_PROMPT_YEAR").c_str(), TRUE);
    else if (m_type == CMediaClassifier::CT_TYPE)
        m_search_edit.SetCueBanner(theApp.m_str_table.LoadText(L"TXT_SEARCH_PROMPT_FILE_TYPE").c_str(), TRUE);
    else if (m_type == CMediaClassifier::CT_BITRATE)
        m_search_edit.SetCueBanner(theApp.m_str_table.LoadText(L"TXT_SEARCH_PROMPT_BITRATE").c_str(), TRUE);
    else if (m_type == CMediaClassifier::CT_RATING)
        m_search_edit.EnableWindow(FALSE);
    else
        m_search_edit.SetCueBanner(theApp.m_str_table.LoadText(L"TXT_SEARCH_PROMPT").c_str(), TRUE);

    //初始化分隔条
    m_splitter_ctrl.AttachCtrlAsLeftPane(IDC_CLASSIFY_LIST);
    m_splitter_ctrl.AttachCtrlAsLeftPane(IDC_MFCEDITBROWSE1);
    m_splitter_ctrl.AttachCtrlAsRightPane(IDC_SONG_LIST);

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
        CMenu* pMenu = theApp.m_menu_mgr.GetMenu(MenuMgr::LibLeftMenu);
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
        CMenu* pMenu = theApp.m_menu_mgr.GetMenu(MenuMgr::LibRightMenu);
        ASSERT(pMenu != nullptr);
        if (pMenu != nullptr)
        {
            m_song_list_ctrl.ShowPopupMenu(pMenu, pNMItemActivate->iItem, this);
        }
    }

    *pResult = 0;
}


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
            auto& media_list{ m_searched ? m_search_result : m_classifer.GetMeidaList() };
            auto iter = media_list.find(m_classify_selected);
            if (iter != media_list.end())
            {
                switch (phdr->iItem)
                {
                case CMediaClassifyDlg::COL_TITLE:
                    std::sort(iter->second.begin(), iter->second.end(), (ascending ? SongInfo::ByTitle : SongInfo::ByTitleDecending));
                    ShowSongList();
                    break;
                case CMediaClassifyDlg::COL_ARTIST:
                    std::sort(iter->second.begin(), iter->second.end(), (ascending ? SongInfo::ByArtist : SongInfo::ByArtistDecending));
                    ShowSongList();
                    break;
                case CMediaClassifyDlg::COL_ALBUM:
                    std::sort(iter->second.begin(), iter->second.end(), (ascending ? SongInfo::ByAlbum : SongInfo::ByAlbumDecending));
                    ShowSongList();
                    break;
                case CMediaClassifyDlg::COL_TRACK:
                    std::sort(iter->second.begin(), iter->second.end(), (ascending ? SongInfo::ByTrack : SongInfo::ByTrackDecending));
                    ShowSongList();
                    break;
                case CMediaClassifyDlg::COL_GENRE:
                    std::sort(iter->second.begin(), iter->second.end(), (ascending ? SongInfo::ByGenre : SongInfo::ByGenreDecending));
                    ShowSongList();
                    break;
                case CMediaClassifyDlg::COL_BITRATE:
                    std::sort(iter->second.begin(), iter->second.end(), (ascending ? SongInfo::ByBitrate : SongInfo::ByBitrateDecending));
                    ShowSongList();
                    break;
                case CMediaClassifyDlg::COL_PATH:
                    std::sort(iter->second.begin(), iter->second.end(), (ascending ? SongInfo::ByPath : SongInfo::ByPathDecending));
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


void CMediaClassifyDlg::OnInitMenu(CMenu* pMenu)
{
    CMediaLibTabDlg::OnInitMenu(pMenu);

    //设置“添加到播放列表”子菜单状态
    //未选中状态不会弹出右键菜单，因此“添加到播放列表”子菜单全部设置为可用状态
    for (UINT id = ID_ADD_TO_DEFAULT_PLAYLIST; id < ID_ADD_TO_MY_FAVOURITE + ADD_TO_PLAYLIST_MAX_SIZE + 1; id++)
    {
        pMenu->EnableMenuItem(id, MF_BYCOMMAND | MF_ENABLED);
    }
    pMenu->EnableMenuItem(ID_ADD_TO_NEW_PLAYLIST, MF_BYCOMMAND | MF_ENABLED);
    pMenu->EnableMenuItem(ID_ADD_TO_OTHER_PLAYLIST, MF_BYCOMMAND | MF_ENABLED);
}
