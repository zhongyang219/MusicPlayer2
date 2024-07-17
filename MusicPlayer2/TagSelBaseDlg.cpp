// TagModeSelectBaseDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "TagSelBaseDlg.h"
#include "FilePathHelper.h"
#include "IniHelper.h"


// CTagSelBaseDlg 对话框

IMPLEMENT_DYNAMIC(CTagSelBaseDlg, CBaseDialog)

const wstring CTagSelBaseDlg::FORMULAR_TITLE = L"%(Title)";
const wstring CTagSelBaseDlg::FORMULAR_ARTIST = L"%(Artist)";
const wstring CTagSelBaseDlg::FORMULAR_ALBUM = L"%(Album)";
const wstring CTagSelBaseDlg::FORMULAR_TRACK = L"%(Track)";
const wstring CTagSelBaseDlg::FORMULAR_YEAR = L"%(Year)";
const wstring CTagSelBaseDlg::FORMULAR_GENRE = L"%(Genre)";
const wstring CTagSelBaseDlg::FORMULAR_COMMENT = L"%(Comment)";
const wstring CTagSelBaseDlg::FORMULAR_ORIGINAL = L"%(Original)";
const vector<wstring> CTagSelBaseDlg::default_formular{ FORMULAR_ARTIST + L" - " + FORMULAR_TITLE, FORMULAR_TITLE + L" - " + FORMULAR_ARTIST, FORMULAR_TRACK + L"." + FORMULAR_ARTIST + L" - " + FORMULAR_TITLE };

CTagSelBaseDlg::CTagSelBaseDlg(bool original_str_disable, CWnd* pParent /*=nullptr*/)
    : CBaseDialog(IDD_TAG_MODE_SELECT_DIALOG, pParent), m_original_str_disable{ original_str_disable }
{

}

CTagSelBaseDlg::~CTagSelBaseDlg()
{
}

void CTagSelBaseDlg::GetTagFromFileName(const wstring& formular, const wstring& file_name, SongInfo& song_info)
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
        CCommon::StringReplace(str_format, item.second, SPLITER);
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

wstring CTagSelBaseDlg::FileNameFromTag(wstring formular, const SongInfo& song_info)
{
    vector<pair<wstring, wstring>> replacements = {
        { FORMULAR_TITLE, song_info.GetTitle() },
        { FORMULAR_ARTIST, song_info.GetFirstArtist() },
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

void CTagSelBaseDlg::SaveConfig() const
{
    CIniHelper ini(theApp.m_config_path);
    ini.WriteBool(L"tag_edit", L"insert_when_clicked", m_insert_when_clicked);
    ini.WriteStringList(L"tag_edit", L"default_formular", m_default_formular);
    ini.Save();
}

void CTagSelBaseDlg::LoadConfig()
{
    CIniHelper ini(theApp.m_config_path);
    m_insert_when_clicked = ini.GetBool(L"tag_edit", L"insert_when_clicked", true);
    ini.GetStringList(L"tag_edit", L"default_formular", m_default_formular, default_formular);
}

void CTagSelBaseDlg::InitComboList()
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
    if (m_format_combo.GetCount() == 0) // 如果为空那么添加一个作为默认
        m_format_combo.AddString(default_formular[0].c_str());
    m_format_combo.AddString(theApp.m_str_table.LoadText(L"TXT_TAG_SEL_CLEAR_HISTORY").c_str());
    m_format_combo.SetCurSel(0);
}

bool CTagSelBaseDlg::IsStringContainsFormular(const wstring& str) const
{
    const vector<wstring> formular_list{ FORMULAR_TITLE, FORMULAR_ARTIST, FORMULAR_ALBUM, FORMULAR_TRACK, FORMULAR_GENRE, FORMULAR_YEAR, FORMULAR_COMMENT, FORMULAR_ORIGINAL };
    for (const auto& formular : formular_list)
    {
        if (str.find(formular) != wstring::npos)
            return true;
    }
    return false;
}

void CTagSelBaseDlg::InsertFormular(const wstring& str_formular)
{
    if (str_formular.empty())
        return;
    //向m_default_formular中插入一项
    auto iter = std::find(m_default_formular.begin(), m_default_formular.end(), str_formular);
    if (iter != m_default_formular.end())   // 如果列表中包含要插入的项，将其移动到第一项
        std::rotate(m_default_formular.begin(), iter, iter + 1);
    else                                    // 列表中没有包含要插入的项（或列表为空），就将其插入到最前面
        m_default_formular.insert(m_default_formular.begin(), str_formular);
}

void CTagSelBaseDlg::OnTagBtnClicked(const wstring& tag)
{
    if (m_insert_when_clicked)
    {
        CString str;
        m_format_combo.GetWindowText(str);
        str += tag.c_str();
        m_format_combo.SetWindowText(str);
    }
    else
        CCommon::CopyStringToClipboard(tag);
}

bool CTagSelBaseDlg::InitializeControls()
{
    // IDC_INFO_STATIC
    // IDC_COMBO1
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TXT_PROPERTY_DLG_TAG_TITLE");
    SetDlgItemTextW(IDC_TXT_PROPERTY_DLG_TAG_TITLE_STATIC, temp.c_str());
    SetDlgItemTextW(IDC_TITLE_BUTTON, FORMULAR_TITLE.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_PROPERTY_DLG_TAG_ARTIST");
    SetDlgItemTextW(IDC_TXT_PROPERTY_DLG_TAG_ARTIST_STATIC, temp.c_str());
    SetDlgItemTextW(IDC_ARTIST_BUTTON, FORMULAR_ARTIST.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_PROPERTY_DLG_TAG_ALBUM");
    SetDlgItemTextW(IDC_TXT_PROPERTY_DLG_TAG_ALBUM_STATIC, temp.c_str());
    SetDlgItemTextW(IDC_ALBUM_BUTTON, FORMULAR_ALBUM.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_PROPERTY_DLG_TAG_TRACK");
    SetDlgItemTextW(IDC_TXT_PROPERTY_DLG_TAG_TRACK_STATIC, temp.c_str());
    SetDlgItemTextW(IDC_TRACK_BUTTON, FORMULAR_TRACK.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_PROPERTY_DLG_TAG_YEAR");
    SetDlgItemTextW(IDC_TXT_PROPERTY_DLG_TAG_YEAR_STATIC, temp.c_str());
    SetDlgItemTextW(IDC_YEAR_BUTTON, FORMULAR_YEAR.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_PROPERTY_DLG_TAG_GENRE");
    SetDlgItemTextW(IDC_TXT_PROPERTY_DLG_TAG_GENRE_STATIC, temp.c_str());
    SetDlgItemTextW(IDC_GENRE_BUTTON, FORMULAR_GENRE.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_PROPERTY_DLG_TAG_COMMENT");
    SetDlgItemTextW(IDC_TXT_PROPERTY_DLG_TAG_COMMENT_STATIC, temp.c_str());
    SetDlgItemTextW(IDC_COMMENT_BUTTON, FORMULAR_COMMENT.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_TAG_SEL_ORIGINAL");
    SetDlgItemTextW(IDC_ORIGINAL_STATIC, temp.c_str());
    SetDlgItemTextW(IDC_ORIGINAL_BUTTON, FORMULAR_ORIGINAL.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_TAG_SEL_CLICK_BTN_SEL");
    SetDlgItemTextW(IDC_TXT_TAG_SEL_CLICK_BTN_SEL_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_TAG_SEL_CLICK_BTN_INS");
    SetDlgItemTextW(IDC_INSERT_RADIO, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_TAG_SEL_CLICK_BTN_COPY");
    SetDlgItemTextW(IDC_COPY_RADIO, temp.c_str());
    // IDOK
    // IDCANCEL

    RepositionTextBasedControls({
        { CtrlTextInfo::L1, IDC_TXT_PROPERTY_DLG_TAG_TITLE_STATIC },
        { CtrlTextInfo::C0, IDC_TITLE_BUTTON },
        { CtrlTextInfo::L1, IDC_TXT_PROPERTY_DLG_TAG_ALBUM_STATIC },
        { CtrlTextInfo::C0, IDC_ALBUM_BUTTON },
        { CtrlTextInfo::L1, IDC_TXT_PROPERTY_DLG_TAG_YEAR_STATIC },
        { CtrlTextInfo::C0, IDC_YEAR_BUTTON },
        { CtrlTextInfo::L1, IDC_TXT_PROPERTY_DLG_TAG_COMMENT_STATIC },
        { CtrlTextInfo::C0, IDC_COMMENT_BUTTON }
        }, CtrlTextInfo::W96);
    RepositionTextBasedControls({
        { CtrlTextInfo::L1, IDC_TXT_PROPERTY_DLG_TAG_ARTIST_STATIC },
        { CtrlTextInfo::C0, IDC_ARTIST_BUTTON },
        { CtrlTextInfo::L1, IDC_TXT_PROPERTY_DLG_TAG_TRACK_STATIC },
        { CtrlTextInfo::C0, IDC_TRACK_BUTTON },
        { CtrlTextInfo::L1, IDC_TXT_PROPERTY_DLG_TAG_GENRE_STATIC },
        { CtrlTextInfo::C0, IDC_GENRE_BUTTON },
        { CtrlTextInfo::L1, IDC_ORIGINAL_STATIC },
        { CtrlTextInfo::C0, IDC_ORIGINAL_BUTTON }
        }, CtrlTextInfo::W96);
    RepositionTextBasedControls({
        { CtrlTextInfo::L3, IDC_TXT_TAG_SEL_CLICK_BTN_SEL_STATIC },
        { CtrlTextInfo::L2, IDC_INSERT_RADIO, CtrlTextInfo::W16 },
        { CtrlTextInfo::L1, IDC_COPY_RADIO, CtrlTextInfo::W16 }
        });
    RepositionTextBasedControls({
        { CtrlTextInfo::R1, IDOK, CtrlTextInfo::W32 },
        { CtrlTextInfo::R2, IDCANCEL, CtrlTextInfo::W32 }
        });
    return true;
}

void CTagSelBaseDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO1, m_format_combo);
}


BEGIN_MESSAGE_MAP(CTagSelBaseDlg, CBaseDialog)
    ON_BN_CLICKED(IDC_TITLE_BUTTON, &CTagSelBaseDlg::OnBnClickedTitleButton)
    ON_BN_CLICKED(IDC_ARTIST_BUTTON, &CTagSelBaseDlg::OnBnClickedArtistButton)
    ON_BN_CLICKED(IDC_ALBUM_BUTTON, &CTagSelBaseDlg::OnBnClickedAlbumButton)
    ON_BN_CLICKED(IDC_TRACK_BUTTON, &CTagSelBaseDlg::OnBnClickedTrackButton)
    ON_BN_CLICKED(IDC_YEAR_BUTTON, &CTagSelBaseDlg::OnBnClickedYearButton)
    ON_BN_CLICKED(IDC_GENRE_BUTTON, &CTagSelBaseDlg::OnBnClickedGenreButton)
    ON_BN_CLICKED(IDC_COMMENT_BUTTON, &CTagSelBaseDlg::OnBnClickedCommentButton)
    ON_BN_CLICKED(IDC_ORIGINAL_BUTTON, &CTagSelBaseDlg::OnBnClickedOriginalButton)
    ON_CBN_SELCHANGE(IDC_COMBO1, &CTagSelBaseDlg::OnCbnSelchangeCombo1)
    ON_BN_CLICKED(IDC_INSERT_RADIO, &CTagSelBaseDlg::OnBnClickedInsertRadio)
    ON_BN_CLICKED(IDC_COPY_RADIO, &CTagSelBaseDlg::OnBnClickedCopyRadio)
    ON_WM_DESTROY()
END_MESSAGE_MAP()


// CTagModeSelectDlg 消息处理程序


BOOL CTagSelBaseDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    LoadConfig();

    if (m_insert_when_clicked)
        CheckDlgButton(IDC_INSERT_RADIO, TRUE);
    else
        CheckDlgButton(IDC_COPY_RADIO, TRUE);

    InsertFormular(m_init_insert_formular);
    InitComboList();

    if (m_original_str_disable)
    {
        ShowDlgCtrl(IDC_ORIGINAL_STATIC, false);
        ShowDlgCtrl(IDC_ORIGINAL_BUTTON, false);
    }

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


void CTagSelBaseDlg::OnBnClickedTitleButton()
{
    OnTagBtnClicked(FORMULAR_TITLE);
}


void CTagSelBaseDlg::OnBnClickedArtistButton()
{
    OnTagBtnClicked(FORMULAR_ARTIST);
}


void CTagSelBaseDlg::OnBnClickedAlbumButton()
{
    OnTagBtnClicked(FORMULAR_ALBUM);
}


void CTagSelBaseDlg::OnBnClickedTrackButton()
{
    OnTagBtnClicked(FORMULAR_TRACK);
}


void CTagSelBaseDlg::OnBnClickedYearButton()
{
    OnTagBtnClicked(FORMULAR_YEAR);
}


void CTagSelBaseDlg::OnBnClickedGenreButton()
{
    OnTagBtnClicked(FORMULAR_GENRE);
}


void CTagSelBaseDlg::OnBnClickedCommentButton()
{
    OnTagBtnClicked(FORMULAR_COMMENT);
}


void CTagSelBaseDlg::OnBnClickedOriginalButton()
{
    OnTagBtnClicked(FORMULAR_ORIGINAL);
}


void CTagSelBaseDlg::OnCbnSelchangeCombo1()
{
    // TODO: 在此添加控件通知处理程序代码
    int cur_sel = m_format_combo.GetCurSel();
    if (cur_sel == m_format_combo.GetCount() - 1)
    {
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_TAG_SEL_CLEAR_HISTORY_INQUIRY");
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


void CTagSelBaseDlg::OnBnClickedInsertRadio()
{
    // TODO: 在此添加控件通知处理程序代码
    m_insert_when_clicked = true;
}


void CTagSelBaseDlg::OnBnClickedCopyRadio()
{
    // TODO: 在此添加控件通知处理程序代码
    m_insert_when_clicked = false;
}


void CTagSelBaseDlg::OnDestroy()
{
    CBaseDialog::OnDestroy();

    // TODO: 在此处添加消息处理程序代码
    CString str;
    m_format_combo.GetWindowText(str);
    m_formular_selected = str.GetString();
    // 不保存没有formular的项目(因为没有复用可能性)
    auto new_end = std::remove_if(m_default_formular.begin(), m_default_formular.end(),
        [this](const wstring& item) { return !IsStringContainsFormular(item); });
    m_default_formular.erase(new_end, m_default_formular.end());
    SaveConfig();
}


void CTagSelBaseDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    CString str;
    m_format_combo.GetWindowText(str);
    InsertFormular(str.GetString());

    CBaseDialog::OnOK();
}
