// TagFromFileNameDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "TagModeSelectDlg.h"
#include "MusicPlayer2.h"


// CTagModeSelectDlg 对话框

IMPLEMENT_DYNAMIC(CTagModeSelectDlg, CDialog)

static const vector<wstring> default_formular{ FORMULAR_ARTIST L" - " FORMULAR_TITLE, FORMULAR_TITLE L" - " FORMULAR_ARTIST, FORMULAR_TRACK L"." FORMULAR_ARTIST L" - " FORMULAR_TITLE };

CTagModeSelectDlg::CTagModeSelectDlg(wstring title, bool original_str_disable, CWnd* pParent /*=nullptr*/)
    : CDialog(IDD_TAG_MODE_SELECT_DIALOG, pParent), m_title_str{ title }, m_original_str_disable{ original_str_disable }
{
    ASSERT(!m_title_str.empty());   // 没有默认值，需要一个有效的标题
}

CTagModeSelectDlg::~CTagModeSelectDlg()
{
}

void CTagModeSelectDlg::GetTagFromFileName(const wstring& file_name, SongInfo& song_info)
{
    GetTagFromFileName(m_formular_selected, file_name, song_info);
}

void CTagModeSelectDlg::GetTagFromFileName(const wstring& formular, const wstring& file_name, SongInfo& song_info)
{
    wstring str_format = formular;
    std::map<size_t, wstring> identifiers;    //保存标识符，int为标识符在formualr中的索引

    //查找每个标识符的位置，并保存在identifers中，FORMULAR_ORIGINAL不参与推测
    const vector<wstring> FORMULARS{ FORMULAR_TITLE, FORMULAR_ARTIST, FORMULAR_ALBUM, FORMULAR_TRACK, FORMULAR_YEAR, FORMULAR_GENRE, FORMULAR_COMMENT };
    for (const auto& f : FORMULARS)
    {
        size_t index = str_format.find(f);
        if (index != wstring::npos)
        {
            identifiers[index] = f;
        }
    }

    const wchar_t* SPLITER = L"|";

    //将标识符全部替换成|
    for (const auto& item : identifiers)
    {
        CCommon::StringReplace(str_format, item.second.c_str(), SPLITER);
    }
    //取得分割符
    vector<wstring> seprators;
    CCommon::StringSplit(str_format, SPLITER, seprators, true, false);

    //用分割符分割文件名
    vector<wstring> results;
    CCommon::StringSplitWithSeparators(file_name, seprators, results);

    //获取分割结果
    if (results.empty())
    {
        song_info.title = file_name;
    }
    else
    {
        size_t index{};
        for (const auto& item : identifiers)
        {
            if (index < results.size())
            {
                wstring result = results[index];
                if (item.second == FORMULAR_TITLE)
                    song_info.title = result;
                else if (item.second == FORMULAR_ARTIST)
                    song_info.artist = result;
                else if (item.second == FORMULAR_ALBUM)
                    song_info.album = result;
                else if (item.second == FORMULAR_TRACK)
                    song_info.track = _wtoi(result.c_str());
                else if (item.second == FORMULAR_YEAR)
                    song_info.SetYear(result.c_str());
                else if (item.second == FORMULAR_GENRE)
                    song_info.genre = result;
                else if (item.second == FORMULAR_COMMENT)
                    song_info.comment = result;
            }
            index++;
        }
    }
}

wstring CTagModeSelectDlg::FileNameFromTag(const SongInfo& song_info)
{
    return FileNameFromTag(m_formular_selected, song_info);
}

wstring CTagModeSelectDlg::FileNameFromTag(wstring formular, const SongInfo& song_info)
{
    vector<pair<wstring, wstring>> replacements = {
        { FORMULAR_TITLE, song_info.GetTitle() },
        { FORMULAR_ARTIST, song_info.GetArtist() },
        { FORMULAR_ALBUM, song_info.GetAlbum() },
        { FORMULAR_TRACK, std::to_wstring(song_info.track) },
        { FORMULAR_GENRE, song_info.GetGenre() },
        { FORMULAR_YEAR, song_info.GetYear() },
        { FORMULAR_COMMENT, song_info.comment },
        { FORMULAR_ORIGINAL, CFilePathHelper(song_info.file_path).GetFileNameWithoutExtension() }
    };
    // 这里的先查找出所有格式字符串再替换是为了彻底规避普通的替换被来自song_info的字符串干扰（虽然几乎不可能发生）
    std::map<size_t, size_t> find_result;   // 键是pos，值是replacements的索引
    size_t item_index{};
    for (const auto& item : replacements)
    {
        size_t pos = 0;
        while ((pos = formular.find(item.first, pos)) != wstring::npos)
        {
            find_result.emplace(pos, item_index);   // 这里的pos一定不会重复（除非FORMULAR_*有问题）
            pos += item.first.size();
        }
        ++item_index;
    }
    // 逆序遍历以免需要修改first中存储的pos
    for (auto riter = find_result.rbegin(); riter != find_result.rend(); ++riter)
    {
        formular.replace(riter->first, replacements[riter->second].first.size(), replacements[riter->second].second);
    }
    CCommon::FileNameNormalize(formular);
    // 这里给出的文件名可能太长，调用方可能需要截断（不应在这里进行）
    return formular;
}

void CTagModeSelectDlg::SaveConfig() const
{
    CIniHelper ini(theApp.m_config_path);
    ini.WriteBool(L"tag_edit", L"insert_when_clicked", IsInsertWhenClicked());
    ini.WriteStringList(L"tag_edit", L"default_formular", m_default_formular);
    ini.Save();
}

void CTagModeSelectDlg::LoadConfig()
{
    CIniHelper ini(theApp.m_config_path);
    m_insert_when_clicked = ini.GetBool(L"tag_edit", L"insert_when_clicked", true);
    ini.GetStringList(L"tag_edit", L"default_formular", m_default_formular, default_formular);
}

wstring CTagModeSelectDlg::InitComboList()
{
    wstring first_item;
    m_format_combo.ResetContent();
    for (const auto& formular : m_default_formular)
    {
        //如果隐藏了“原文件名”按钮，则不在下拉列表中添加含有“原文件名”的项
        if (m_original_str_disable)
        {
            if (formular.find(FORMULAR_ORIGINAL) != std::wstring::npos)
                continue;
        }
        if (first_item.empty())
            first_item = formular;
        m_format_combo.AddString(formular.c_str());
    }
    m_format_combo.AddString(theApp.m_str_table.LoadText(L"TXT_TAG_SEL_CLEAR_HISTORY").c_str());
    return first_item;
}

void CTagModeSelectDlg::InsertTag(const wchar_t* tag)
{
    CString str;
    m_format_combo.GetWindowText(str);
    str += tag;
    m_format_combo.SetWindowText(str);
}

void CTagModeSelectDlg::SetInsertWhenClicked(bool insert)
{
    if (insert)
        CheckDlgButton(IDC_INSERT_RADIO, TRUE);
    else
        CheckDlgButton(IDC_COPY_RADIO, TRUE);
}

bool CTagModeSelectDlg::IsInsertWhenClicked() const
{
    return (IsDlgButtonChecked(IDC_INSERT_RADIO) != 0);
}

bool CTagModeSelectDlg::IsStringContainsFormular(const wstring& str)
{
    const vector<wstring> formular_list{ FORMULAR_TITLE, FORMULAR_ARTIST, FORMULAR_ALBUM, FORMULAR_TRACK, FORMULAR_GENRE, FORMULAR_YEAR, FORMULAR_COMMENT, FORMULAR_ORIGINAL };
    for (const auto& formular : formular_list)
    {
        if (str.find(formular) != wstring::npos)
            return true;
    }
    return false;
}

void CTagModeSelectDlg::InsertFormular(const wstring& str_formular)
{
    if (!IsStringContainsFormular(str_formular))
        return;

    //向m_default_formular中插入一项
    auto iter = std::find(m_default_formular.begin(), m_default_formular.end(), str_formular);
    if (iter == m_default_formular.begin())     //如果要插入的就在第一项，则直接返回
        return;

    if (iter != m_default_formular.end())       //如果列表中包含要插入的项，就将其和第一项交换
    {
        std::swap(*iter, *m_default_formular.begin());
    }
    else        //列表中没有包含要插入的项，就将其插入到最前面
    {
        m_default_formular.insert(m_default_formular.begin(), str_formular);
    }
}

void CTagModeSelectDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO1, m_format_combo);
}


BEGIN_MESSAGE_MAP(CTagModeSelectDlg, CDialog)
    ON_BN_CLICKED(IDC_TITLE_BUTTON, &CTagModeSelectDlg::OnBnClickedTitleButton)
    ON_BN_CLICKED(IDC_ARTIST_BUTTON, &CTagModeSelectDlg::OnBnClickedArtistButton)
    ON_BN_CLICKED(IDC_ALBUM_BUTTON, &CTagModeSelectDlg::OnBnClickedAlbumButton)
    ON_BN_CLICKED(IDC_TRACK_BUTTON, &CTagModeSelectDlg::OnBnClickedTrackButton)
    ON_BN_CLICKED(IDC_YEAR_BUTTON, &CTagModeSelectDlg::OnBnClickedYearButton)
    ON_BN_CLICKED(IDC_GENRE_BUTTON, &CTagModeSelectDlg::OnBnClickedGenreButton)
    ON_BN_CLICKED(IDC_COMMENT_BUTTON, &CTagModeSelectDlg::OnBnClickedCommentButton)
    ON_BN_CLICKED(IDC_ORIGINAL_BUTTON, &CTagModeSelectDlg::OnBnClickedOriginalButton)
    ON_CBN_SELCHANGE(IDC_COMBO1, &CTagModeSelectDlg::OnCbnSelchangeCombo1)
    ON_WM_DESTROY()
END_MESSAGE_MAP()


// CTagModeSelectDlg 消息处理程序


BOOL CTagModeSelectDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    LoadConfig();

    SetWindowText(m_title_str.c_str());

    if (!m_init_insert_formular.empty())
    {
        InsertFormular(m_init_insert_formular);
    }

    SetInsertWhenClicked(m_insert_when_clicked);

    SetDlgItemText(IDC_TITLE_BUTTON, FORMULAR_TITLE);
    SetDlgItemText(IDC_ARTIST_BUTTON, FORMULAR_ARTIST);
    SetDlgItemText(IDC_ALBUM_BUTTON, FORMULAR_ALBUM);
    SetDlgItemText(IDC_TRACK_BUTTON, FORMULAR_TRACK);
    SetDlgItemText(IDC_GENRE_BUTTON, FORMULAR_GENRE);
    SetDlgItemText(IDC_YEAR_BUTTON, FORMULAR_YEAR);
    SetDlgItemText(IDC_COMMENT_BUTTON, FORMULAR_COMMENT);
    SetDlgItemText(IDC_ORIGINAL_BUTTON, FORMULAR_ORIGINAL);

    wstring first_item = InitComboList();
    if (!first_item.empty())
        m_format_combo.SetWindowText(first_item.c_str());

    if (m_original_str_disable)
    {
        GetDlgItem(IDC_ORIGINAL_STATIC)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_ORIGINAL_BUTTON)->ShowWindow(SW_HIDE);
    }

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CTagModeSelectDlg::OnBnClickedTitleButton()
{
    if (IsInsertWhenClicked())
        InsertTag(FORMULAR_TITLE);
    else
        CCommon::CopyStringToClipboard(FORMULAR_TITLE);
}


void CTagModeSelectDlg::OnBnClickedArtistButton()
{
    if (IsInsertWhenClicked())
        InsertTag(FORMULAR_ARTIST);
    else
        CCommon::CopyStringToClipboard(FORMULAR_ARTIST);
}


void CTagModeSelectDlg::OnBnClickedAlbumButton()
{
    if (IsInsertWhenClicked())
        InsertTag(FORMULAR_ALBUM);
    else
        CCommon::CopyStringToClipboard(FORMULAR_ALBUM);
}


void CTagModeSelectDlg::OnBnClickedTrackButton()
{
    if (IsInsertWhenClicked())
        InsertTag(FORMULAR_TRACK);
    else
        CCommon::CopyStringToClipboard(FORMULAR_TRACK);
}


void CTagModeSelectDlg::OnBnClickedYearButton()
{
    if (IsInsertWhenClicked())
        InsertTag(FORMULAR_YEAR);
    else
        CCommon::CopyStringToClipboard(FORMULAR_YEAR);
}


void CTagModeSelectDlg::OnBnClickedGenreButton()
{
    if (IsInsertWhenClicked())
        InsertTag(FORMULAR_GENRE);
    else
        CCommon::CopyStringToClipboard(FORMULAR_GENRE);
}


void CTagModeSelectDlg::OnBnClickedCommentButton()
{
    if (IsInsertWhenClicked())
        InsertTag(FORMULAR_COMMENT);
    else
        CCommon::CopyStringToClipboard(FORMULAR_COMMENT);
}


void CTagModeSelectDlg::OnBnClickedOriginalButton()
{
    if (IsInsertWhenClicked())
        InsertTag(FORMULAR_ORIGINAL);
    else
        CCommon::CopyStringToClipboard(FORMULAR_ORIGINAL);
}


void CTagModeSelectDlg::OnCbnSelchangeCombo1()
{
    // TODO: 在此添加控件通知处理程序代码
    int cur_sel = m_format_combo.GetCurSel();
    if (cur_sel == m_format_combo.GetCount() - 1)
    {
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_TAG_SEL_CLEAR_HISTORY_INQUARY");
        if (MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OKCANCEL) == IDOK)
        {
            m_default_formular = default_formular;
            InitComboList();
        }
        m_format_combo.SetCurSel(0);
    }
    else
    {
        CString str;
        m_format_combo.GetWindowText(str);
        InsertFormular(str.GetString());
    }
}


void CTagModeSelectDlg::OnDestroy()
{
    CDialog::OnDestroy();

    // TODO: 在此处添加消息处理程序代码
    CString str;
    m_format_combo.GetWindowText(str);
    m_formular_selected = str.GetString();
    SaveConfig();
}


void CTagModeSelectDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    CString str;
    m_format_combo.GetWindowText(str);
    InsertFormular(str.GetString());

    CDialog::OnOK();
}

