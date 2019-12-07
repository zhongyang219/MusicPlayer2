// MediaClassfyDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "MediaClassifyDlg.h"
#include "afxdialogex.h"
#include "Playlist.h"


// CMediaClassifyDlg 对话框

IMPLEMENT_DYNAMIC(CMediaClassifyDlg, CTabDlg)

CMediaClassifyDlg::CMediaClassifyDlg(CMediaClassifier::ClassificationType type, CWnd* pParent /*=nullptr*/)
	: CTabDlg(IDD_MEDIA_CLASSIFY_DIALOG, pParent), m_type(type), 
    m_classifer(type == CMediaClassifier::CT_ARTIST ? theApp.m_artist_classifer : theApp.m_album_classifer)
{
    if (m_type == CMediaClassifier::CT_ARTIST)
        m_default_str = CCommon::LoadText(IDS_DEFAULT_ARTIST);
    else if (m_type == CMediaClassifier::CT_ALBUM)
        m_default_str = CCommon::LoadText(IDS_DEFAULT_ALBUM);
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
        //
        auto iter = media_list.find(wstring(m_classify_selected));
        if (iter != media_list.end())
        {
            for (int index : m_right_selected_items)
            {
                if(index >= 0 && index < iter->second.size())
                    song_list.push_back(iter->second[index].file_path);
            }
        }
    }
}

void CMediaClassifyDlg::ShowClassifyList()
{
    auto& media_list{ m_searched ? m_search_result : m_classifer.GetMeidaList() };
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
        m_classify_list_ctrl.InsertItem(index, item_name);
        m_classify_list_ctrl.SetItemText(index, 1, std::to_wstring(item.second.size()).c_str());
        index++;
    }

    //将“其他”分类放到列表的最后面
    auto iter = media_list.find(STR_OTHER_CLASSIFY_TYPE);
    if (iter != media_list.end())
    {
        m_classify_list_ctrl.InsertItem(index, CCommon::LoadText(_T("<"), IDS_OTHER, _T(">")));
        m_classify_list_ctrl.SetItemText(index, 1, std::to_wstring(iter->second.size()).c_str());
    }
}

void CMediaClassifyDlg::ShowSongList()
{
    auto& media_list{ m_searched ? m_search_result : m_classifer.GetMeidaList() };
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
                m_song_list_ctrl.InsertItem(item_index, item.GetTitle().c_str());
                m_song_list_ctrl.SetItemText(item_index, 1, item.GetArtist().c_str());
                m_song_list_ctrl.SetItemText(item_index, 2, item.GetAlbum().c_str());
                m_song_list_ctrl.SetItemText(item_index, 3, item.file_path.c_str());
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
    if (index < 0)
    {
        SetButtonsEnable(false);
        return;
    }
    m_classify_list_ctrl.GetItemSelected(m_left_selected_items);    //获取选中的项目

    CString str_selected = GetClassifyListSelectedString(index);
    static size_t last_selected_count = 0;
    if (last_selected_index != index || last_selected_count != m_left_selected_items.size())
    {
        m_classify_selected = str_selected;
        ShowSongList();
        last_selected_index = index;
        last_selected_count = m_left_selected_items.size();
    }

    m_left_selected = true;
    SetButtonsEnable(/*(index >= 0 && index < m_classify_list_ctrl.GetItemCount()) ||*/ !m_left_selected_items.empty());

}

void CMediaClassifyDlg::SongListClicked(int index)
{
    m_song_list_ctrl.GetItemSelected(m_right_selected_items);
    m_left_selected = false;
    SetButtonsEnable(/*(index >=0 && index < m_song_list_ctrl.GetItemCount()) ||*/ !m_right_selected_items.empty());
}

bool CMediaClassifyDlg::IsItemMatchKeyWord(const SongInfo& song, const wstring& key_word)
{
    if (m_type == CMediaClassifier::CT_ARTIST)
        return IsItemMatchKeyWord(song.artist, key_word);
    else if(m_type == CMediaClassifier::CT_ALBUM)
        return IsItemMatchKeyWord(song.album, key_word);
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
END_MESSAGE_MAP()


// CMediaClassifyDlg 消息处理程序


BOOL CMediaClassifyDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    //初始化左侧列表
    m_classify_list_ctrl.SetExtendedStyle(m_classify_list_ctrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
    CRect rc_classify_list;
    m_classify_list_ctrl.GetWindowRect(rc_classify_list);
    CString title_name;
    if (m_type == CMediaClassifier::CT_ARTIST)
        title_name = CCommon::LoadText(IDS_ARTIST);
    else if (m_type == CMediaClassifier::CT_ALBUM)
        title_name = CCommon::LoadText(IDS_ALBUM);
    int width0, width1;
    width1 = theApp.DPI(50);
    width0 = rc_classify_list.Width() - width1 - theApp.DPI(20) - 1;
    m_classify_list_ctrl.InsertColumn(0, title_name, LVCFMT_LEFT, width0);
    m_classify_list_ctrl.InsertColumn(1, CCommon::LoadText(IDS_TRACK_TOTAL_NUM), LVCFMT_LEFT, width1);
    ShowClassifyList();

    //初始化右侧列表
    m_song_list_ctrl.SetExtendedStyle(m_classify_list_ctrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
    //CRect rc_song_list;
    //m_song_list_ctrl.GetWindowRect(rc_song_list);
    m_song_list_ctrl.InsertColumn(0, CCommon::LoadText(IDS_TITLE), LVCFMT_LEFT, theApp.DPI(150));
    m_song_list_ctrl.InsertColumn(1, CCommon::LoadText(IDS_ARTIST), LVCFMT_LEFT, theApp.DPI(100));
    m_song_list_ctrl.InsertColumn(2, CCommon::LoadText(IDS_ALBUM), LVCFMT_LEFT, theApp.DPI(150));
    m_song_list_ctrl.InsertColumn(3, CCommon::LoadText(IDS_FILE_PATH), LVCFMT_LEFT, theApp.DPI(300));

    if (m_type == CMediaClassifier::CT_ARTIST)
        m_search_edit.SetCueBanner(CCommon::LoadText(IDS_SEARCH_ARTIST), TRUE);
    else if(m_type == CMediaClassifier::CT_ALBUM)
        m_search_edit.SetCueBanner(CCommon::LoadText(IDS_SEARCH_ALBUM), TRUE);

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
