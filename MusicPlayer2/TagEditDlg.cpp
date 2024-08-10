// TagEditDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "TagEditDlg.h"
#include "AudioCommon.h"


// CTagEditDlg 对话框

IMPLEMENT_DYNAMIC(CTagEditDlg, CBaseDialog)

CTagEditDlg::CTagEditDlg(vector<SongInfo>& file_list, int index, CWnd* pParent /*=nullptr*/)
    : CBaseDialog(IDD_TAG_EDIT_DIALOG, pParent), m_file_list{ file_list }, m_index{ index }
{

}

CTagEditDlg::~CTagEditDlg()
{
}

void CTagEditDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_GENRE_COMBO, m_genre_combo);
}

void CTagEditDlg::ShowInfo()
{
    if (m_index < 0 || m_index >= static_cast<int>(m_file_list.size()))
        return;
    const SongInfo& item{ m_file_list[m_index] };
    SetDlgItemTextW(IDC_PATH_EDIT, item.file_path.c_str());
    SetDlgItemTextW(IDC_TITEL_EDIT, item.title.c_str());
    SetDlgItemTextW(IDC_ARTIST_EDIT, item.artist.c_str());
    SetDlgItemTextW(IDC_ALBUM_EDIT, item.album.c_str());
    SetDlgItemTextW(IDC_TRACK_EDIT, CAudioCommon::TrackToString(item.track));
    SetDlgItemTextW(IDC_YEAR_EDIT, item.get_year().c_str());
    SetDlgItemTextW(IDC_GENRE_COMBO, item.genre.c_str());
    SetDlgItemTextW(IDC_COMMENT_EDIT, item.comment.c_str());
    CString info_str;
    info_str.Format(_T("%d/%d"), m_index + 1, m_file_list.size());
    SetDlgItemTextW(IDC_ITEM_STATIC, info_str);
}

CString CTagEditDlg::GetDialogName() const
{
    return L"EditAudioTagDlg";
}

bool CTagEditDlg::InitializeControls()
{
    // 这里部分使用 IDD_PROPERTY_DIALOG / IDD_PROPERTY_PARENT_DIALOG 的字符串
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TITLE_TAG_EDIT");
    SetWindowTextW(temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_TAG_EDIT_PATH");
    SetDlgItemTextW(IDC_TXT_TAG_EDIT_PATH_STATIC, temp.c_str());
    // IDC_PATH_EDIT
    temp = theApp.m_str_table.LoadText(L"TXT_PROPERTY_DLG_TAG_TITLE");
    SetDlgItemTextW(IDC_TXT_PROPERTY_DLG_TAG_TITLE_STATIC, temp.c_str());
    // IDC_TITEL_EDIT
    temp = theApp.m_str_table.LoadText(L"TXT_PROPERTY_DLG_TAG_ARTIST");
    SetDlgItemTextW(IDC_TXT_PROPERTY_DLG_TAG_ARTIST_STATIC, temp.c_str());
    // IDC_ARTIST_EDIT
    temp = theApp.m_str_table.LoadText(L"TXT_PROPERTY_DLG_TAG_ALBUM");
    SetDlgItemTextW(IDC_TXT_PROPERTY_DLG_TAG_ALBUM_STATIC, temp.c_str());
    // IDC_ALBUM_EDIT
    temp = theApp.m_str_table.LoadText(L"TXT_PROPERTY_DLG_TAG_TRACK");
    SetDlgItemTextW(IDC_TXT_PROPERTY_DLG_TAG_TRACK_STATIC, temp.c_str());
    // IDC_TRACK_EDIT
    temp = theApp.m_str_table.LoadText(L"TXT_PROPERTY_DLG_TAG_YEAR");
    SetDlgItemTextW(IDC_TXT_PROPERTY_DLG_TAG_YEAR_STATIC, temp.c_str());
    // IDC_YEAR_EDIT
    temp = theApp.m_str_table.LoadText(L"TXT_PROPERTY_DLG_TAG_GENRE");
    SetDlgItemTextW(IDC_TXT_PROPERTY_DLG_TAG_GENRE_STATIC, temp.c_str());
    // IDC_GENRE_COMBO
    temp = theApp.m_str_table.LoadText(L"TXT_PROPERTY_DLG_TAG_COMMENT");
    SetDlgItemTextW(IDC_TXT_PROPERTY_DLG_TAG_COMMENT_STATIC, temp.c_str());
    // IDC_COMMENT_EDIT
    temp = theApp.m_str_table.LoadText(L"TXT_PROPERTY_PARENT_PREVIOUS");
    SetDlgItemTextW(IDC_PREVIOUS_BUTTON, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_PROPERTY_PARENT_NEXT");
    SetDlgItemTextW(IDC_NEXT_BUTTON, temp.c_str());
    temp = L"0/0";
    SetDlgItemTextW(IDC_ITEM_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_TAG_EDIT_SAVE");
    SetDlgItemTextW(IDC_SAVE_BUTTON, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_CLOSE");
    SetDlgItemTextW(IDCANCEL, temp.c_str());

    RepositionTextBasedControls({
        { CtrlTextInfo::L1, IDC_TXT_TAG_EDIT_PATH_STATIC },
        { CtrlTextInfo::C0, IDC_PATH_EDIT },
        { CtrlTextInfo::L1, IDC_TXT_PROPERTY_DLG_TAG_TITLE_STATIC },
        { CtrlTextInfo::C0, IDC_TITEL_EDIT },
        { CtrlTextInfo::L1, IDC_TXT_PROPERTY_DLG_TAG_ARTIST_STATIC },
        { CtrlTextInfo::C0, IDC_ARTIST_EDIT },
        { CtrlTextInfo::L1, IDC_TXT_PROPERTY_DLG_TAG_ALBUM_STATIC },
        { CtrlTextInfo::C0, IDC_ALBUM_EDIT },
        { CtrlTextInfo::L1, IDC_TXT_PROPERTY_DLG_TAG_TRACK_STATIC },
        { CtrlTextInfo::C0, IDC_TRACK_EDIT },
        { CtrlTextInfo::L1, IDC_TXT_PROPERTY_DLG_TAG_GENRE_STATIC },
        { CtrlTextInfo::C0, IDC_GENRE_COMBO },
        { CtrlTextInfo::L1, IDC_TXT_PROPERTY_DLG_TAG_COMMENT_STATIC },
        { CtrlTextInfo::C0, IDC_COMMENT_EDIT }
        }, CtrlTextInfo::W32);
    RepositionTextBasedControls({
        { CtrlTextInfo::L1, IDC_TXT_PROPERTY_DLG_TAG_YEAR_STATIC },
        { CtrlTextInfo::C0, IDC_YEAR_EDIT }
        }, CtrlTextInfo::W40);
    RepositionTextBasedControls({
        { CtrlTextInfo::L2, IDC_PREVIOUS_BUTTON, CtrlTextInfo::W32 },
        { CtrlTextInfo::L1, IDC_NEXT_BUTTON, CtrlTextInfo::W32 },
        { CtrlTextInfo::C0, IDC_ITEM_STATIC },
        { CtrlTextInfo::R1, IDC_SAVE_BUTTON, CtrlTextInfo::W32 },
        { CtrlTextInfo::R2, IDCANCEL, CtrlTextInfo::W32 }
        });
    return true;
}


BEGIN_MESSAGE_MAP(CTagEditDlg, CBaseDialog)
    ON_BN_CLICKED(IDC_PREVIOUS_BUTTON, &CTagEditDlg::OnBnClickedPreviousButton)
    ON_BN_CLICKED(IDC_NEXT_BUTTON, &CTagEditDlg::OnBnClickedNextButton)
    ON_BN_CLICKED(IDC_SAVE_BUTTON, &CTagEditDlg::OnBnClickedSaveButton)
END_MESSAGE_MAP()


// CTagEditDlg 消息处理程序


BOOL CTagEditDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    SetIcon(IconMgr::IconType::IT_Edit, FALSE);
    SetButtonIcon(IDC_PREVIOUS_BUTTON, IconMgr::IconType::IT_Triangle_Left);
    SetButtonIcon(IDC_NEXT_BUTTON, IconMgr::IconType::IT_Triangle_Right);
    SetButtonIcon(IDC_SAVE_BUTTON, IconMgr::IconType::IT_Save);

    //初始化流派列表
    CAudioCommon::EmulateGenre([&](const wstring& genre_str)
    {
        m_genre_combo.AddString(genre_str.c_str());
    }, true);
    CRect rect;
    m_genre_combo.SetMinVisibleItems(15);       //设置下拉列表的高度

    ShowInfo();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CTagEditDlg::OnBnClickedPreviousButton()
{
    // TODO: 在此添加控件通知处理程序代码
    if (m_file_list.size() <= 1)
        return;
    m_index--;
    if (m_index < 0) m_index = m_file_list.size() - 1;
    if (m_index < 0) m_index = 0;
    ShowInfo();
}


void CTagEditDlg::OnBnClickedNextButton()
{
    // TODO: 在此添加控件通知处理程序代码
    if (m_file_list.size() <= 1)
        return;
    m_index++;
    if (m_index >= static_cast<int>(m_file_list.size())) m_index = 0;
    ShowInfo();
}


void CTagEditDlg::OnBnClickedSaveButton()
{
    // TODO: 在此添加控件通知处理程序代码
    if (m_index < 0 || m_index >= static_cast<int>(m_file_list.size()))
        return;
    SongInfo& item{ m_file_list[m_index] };
    CString temp;
    GetDlgItemTextW(IDC_TITEL_EDIT, temp);
    item.title = temp;
    GetDlgItemTextW(IDC_ARTIST_EDIT, temp);
    item.artist = temp;
    GetDlgItemTextW(IDC_ALBUM_EDIT, temp);
    item.album = temp;
    GetDlgItemTextW(IDC_TRACK_EDIT, temp);
    item.track = _ttoi(temp);
    GetDlgItemTextW(IDC_YEAR_EDIT, temp);
    item.SetYear(temp);
    GetDlgItemTextW(IDC_GENRE_COMBO, temp);
    item.genre = temp;
    GetDlgItemTextW(IDC_COMMENT_EDIT, temp);
    item.comment = temp;
}
