// LyricRelateDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "Player.h"
#include "LyricRelateDlg.h"
#include "SongInfoHelper.h"
#include "MusicPlayerCmdHelper.h"
#include "SongDataManager.h"
#include "FilterHelper.h"
#include "CommonDialogMgr.h"


// CLyricRelateDlg 对话框

IMPLEMENT_DYNAMIC(CLyricRelateDlg, CBaseDialog)

CLyricRelateDlg::CLyricRelateDlg(CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_LYRIC_RELATE_DIALOG, pParent)
{

}

CLyricRelateDlg::~CLyricRelateDlg()
{
}

void CLyricRelateDlg::ShowSearchResult()
{
    m_result_list.DeleteAllItems();
    m_result_list.SetHightItem(-1);
    wstring current_lyric_file{ CPlayer::GetInstance().GetCurrentSongInfo().lyric_file };
    AddListRow(current_lyric_file);
    for (const auto& lyric_file : m_search_result)
    {
        if (current_lyric_file != lyric_file)
            AddListRow(lyric_file);
    }
}

void CLyricRelateDlg::AddListRow(const wstring& lyric_path)
{
    if (lyric_path.empty() || lyric_path == NO_LYRIC_STR)
        return;

    CFilePathHelper helper(lyric_path);
    int index = m_result_list.GetItemCount();
    m_result_list.InsertItem(index, std::to_wstring(index + 1).c_str());
    m_result_list.SetItemText(index, 1, helper.GetFileName().c_str());
    m_result_list.SetItemText(index, 2, helper.GetDir().c_str());
    bool is_related{ lyric_path == CPlayer::GetInstance().GetCurrentSongInfo().lyric_file };
    if(is_related)
    {
        m_result_list.SetItemText(index, 3, theApp.m_str_table.LoadText(L"TXT_LRC_RELATE_IS_RELATED_YES").c_str());
        m_result_list.SetHightItem(index);
    }
}

wstring CLyricRelateDlg::GetListRow(int index)
{
    if(index >= 0 && index < m_result_list.GetItemCount())
    {
        CString file_name, file_path;
        file_name = m_result_list.GetItemText(index, 1);
        file_path = m_result_list.GetItemText(index, 2);
        return wstring(file_path + file_name);
    }
    else
    {
        return wstring();
    }
}

void CLyricRelateDlg::SearchLyrics()
{
    bool fuzzy_match{ m_fuzzy_match_chk.GetCheck() != 0 };
    CString lyric_name;
    GetDlgItemText(IDC_LYRIC_NAME_EDIT, lyric_name);
    CMusicPlayerCmdHelper helper;
    helper.SearchLyricFiles(wstring(lyric_name), CPlayer::GetInstance().GetCurrentDir(), m_search_result, fuzzy_match);
}

void CLyricRelateDlg::EnableControls(bool enable)
{
    CWnd* pWnd = GetDlgItem(IDC_DELETE_FILE_BUTTON);
    if (pWnd != nullptr)
        pWnd->EnableWindow(enable);
    pWnd = GetDlgItem(IDOK);
    if (pWnd != nullptr)
        pWnd->EnableWindow(enable);
}

CString CLyricRelateDlg::GetDialogName() const
{
    return _T("LyricRelateDlg");
}

bool CLyricRelateDlg::InitializeControls()
{
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TITLE_LRC_RELATE");
    SetWindowTextW(temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_LRC_RELATE_LRC_TITLE");
    SetDlgItemTextW(IDC_TXT_LRC_RELATE_LRC_TITLE_STATIC, temp.c_str());
    // IDC_LYRIC_NAME_EDIT
    temp = theApp.m_str_table.LoadText(L"TXT_LRC_RELATE_FUZZY_MATCH");
    SetDlgItemTextW(IDC_FUZZY_MATCH_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_LRC_RELATE_SEARCH_LOCAL");
    SetDlgItemTextW(IDC_LOCAL_SEARCH_BUTTON, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_LRC_RELATE_SEARCH_RESULT");
    SetDlgItemTextW(IDC_TXT_LRC_RELATE_SEARCH_RESULT_STATIC, temp.c_str());
    // IDC_SEARCH_RESULT_LIST
    temp = theApp.m_str_table.LoadText(L"TXT_LRC_RELATE_BROWSE");
    SetDlgItemTextW(IDC_BROWSE_BUTTON1, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_LRC_RELATE_DEL_FILE");
    SetDlgItemTextW(IDC_DELETE_FILE_BUTTON, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_LRC_RELATE_UNLINK");
    SetDlgItemTextW(IDC_DONOT_RELATE_BUTTON, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_LRC_RELATE_SEL_LINK");
    SetDlgItemTextW(IDOK, temp.c_str());
    // IDCANCEL

    RepositionTextBasedControls({
        { CtrlTextInfo::L1, IDC_TXT_LRC_RELATE_LRC_TITLE_STATIC },
        { CtrlTextInfo::C0, IDC_LYRIC_NAME_EDIT },
        { CtrlTextInfo::R1, IDC_FUZZY_MATCH_CHECK, CtrlTextInfo::W16 },
        { CtrlTextInfo::R2, IDC_LOCAL_SEARCH_BUTTON, CtrlTextInfo::W32 }
        }, CtrlTextInfo::W128);
    RepositionTextBasedControls({
        { CtrlTextInfo::C0, IDC_SEARCH_RESULT_LIST },
        { CtrlTextInfo::R1, IDC_BROWSE_BUTTON1, CtrlTextInfo::W32 },
        { CtrlTextInfo::R1, IDC_DELETE_FILE_BUTTON, CtrlTextInfo::W32 }
        }, CtrlTextInfo::W256);
    RepositionTextBasedControls({
        { CtrlTextInfo::L1, IDC_DONOT_RELATE_BUTTON, CtrlTextInfo::W32 },
        { CtrlTextInfo::R1, IDOK, CtrlTextInfo::W32 },
        { CtrlTextInfo::R2, IDCANCEL, CtrlTextInfo::W32 }
        });
    return true;
}

void CLyricRelateDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_FUZZY_MATCH_CHECK, m_fuzzy_match_chk);
    DDX_Control(pDX, IDC_SEARCH_RESULT_LIST, m_result_list);
}


BEGIN_MESSAGE_MAP(CLyricRelateDlg, CBaseDialog)
    ON_BN_CLICKED(IDC_LOCAL_SEARCH_BUTTON, &CLyricRelateDlg::OnBnClickedLocalSearchButton)
    ON_BN_CLICKED(IDC_BROWSE_BUTTON1, &CLyricRelateDlg::OnBnClickedBrowseButton1)
    ON_BN_CLICKED(IDC_DELETE_FILE_BUTTON, &CLyricRelateDlg::OnBnClickedDeleteFileButton)
    ON_BN_CLICKED(IDC_DONOT_RELATE_BUTTON, &CLyricRelateDlg::OnBnClickedDonotRelateButton)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_SEARCH_RESULT_LIST, &CLyricRelateDlg::OnLvnItemchangedSearchResultList)
    ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// CLyricRelateDlg 消息处理程序


BOOL CLyricRelateDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    SetIcon(IconMgr::IconType::IT_Lyric, FALSE);
    SetButtonIcon(IDC_LOCAL_SEARCH_BUTTON, IconMgr::IconType::IT_Find);
    SetButtonIcon(IDC_BROWSE_BUTTON1, IconMgr::IconType::IT_Folder_Explore);
    SetButtonIcon(IDC_DELETE_FILE_BUTTON, IconMgr::IconType::IT_Cancel);
    SetButtonIcon(IDC_DONOT_RELATE_BUTTON, IconMgr::IconType::IT_Unlink);

    wstring lyric_name;
    if (CPlayer::GetInstance().GetCurrentSongInfo().is_cue || CPlayer::GetInstance().IsOsuFile())
        lyric_name = CSongInfoHelper::GetDisplayStr(CPlayer::GetInstance().GetCurrentSongInfo(), DF_ARTIST_TITLE);
    else
        lyric_name = CFilePathHelper(CPlayer::GetInstance().GetCurrentFilePath()).GetFileNameWithoutExtension();
    SetDlgItemText(IDC_LYRIC_NAME_EDIT, lyric_name.c_str());

    EnableControls(false);

    //初始化列表控件
    CRect rect;
    m_result_list.GetWindowRect(rect);
    int width[4]{};
    width[0] = theApp.DPI(40);
    width[1] = rect.Width() * 3 / 10;
    width[2] = rect.Width() * 2 / 5;
    width[3] = rect.Width() - width[0] - width[1] - width[2] - theApp.DPI(20) - 1;
    m_result_list.InsertColumn(0, theApp.m_str_table.LoadText(L"TXT_SERIAL_NUMBER").c_str(), LVCFMT_LEFT, width[0]);
    m_result_list.InsertColumn(1, theApp.m_str_table.LoadText(L"TXT_FILE_NAME").c_str(), LVCFMT_LEFT, width[1]);
    m_result_list.InsertColumn(2, theApp.m_str_table.LoadText(L"TXT_FILE_PATH").c_str(), LVCFMT_LEFT, width[2]);
    m_result_list.InsertColumn(3, theApp.m_str_table.LoadText(L"TXT_LRC_RELATE_IS_RELATED").c_str(), LVCFMT_LEFT, width[3]);

    SearchLyrics();
    ShowSearchResult();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CLyricRelateDlg::OnBnClickedLocalSearchButton()
{
    // TODO: 在此添加控件通知处理程序代码
    SearchLyrics();
    ShowSearchResult();
}


void CLyricRelateDlg::OnBnClickedBrowseButton1()
{
    // TODO: 在此添加控件通知处理程序代码
    wstring filter = FilterHelper::GetLyricFileFilter();
    //构造打开文件对话框
    CFileDialog fileDlg(TRUE, NULL, NULL, 0, filter.c_str(), this);
    //显示打开文件对话框
    if (IDOK == fileDlg.DoModal())
    {
        CPlayer::GetInstance().IniLyrics(wstring(fileDlg.GetPathName()));
        OnCancel();
    }
}


void CLyricRelateDlg::OnBnClickedDeleteFileButton()
{
    // TODO: 在此添加控件通知处理程序代码
    int index_selected = m_result_list.GetCurSel();
    wstring lyric_file = GetListRow(index_selected);
    CommonDialogMgr::DeleteAFile(m_hWnd, lyric_file);
    if (lyric_file == CPlayer::GetInstance().GetCurrentSongInfo().lyric_file)
    {
        //如果删除的是正在显示的歌词，则将其清除
        CPlayer::GetInstance().ClearLyric();
    }
    SearchLyrics();
    ShowSearchResult();
}


void CLyricRelateDlg::OnBnClickedDonotRelateButton()
{
    // TODO: 在此添加控件通知处理程序代码
    CPlayer::GetInstance().ClearLyric();        //清除歌词
    SongInfo song_info{ CSongDataManager::GetInstance().GetSongInfo3(CPlayer::GetInstance().GetCurrentSongInfo()) };
    song_info.lyric_file = NO_LYRIC_STR;        //将该歌曲设置为不关联歌词
    song_info.SetNoOnlineLyric(true);
    CSongDataManager::GetInstance().AddItem(song_info);
    //ShowSearchResult();
    OnCancel();
}


void CLyricRelateDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类

    int index_selected = m_result_list.GetCurSel();
    wstring related_lyric = GetListRow(index_selected);
    CPlayer::GetInstance().IniLyrics(related_lyric);

    CBaseDialog::OnOK();
}


void CLyricRelateDlg::OnLvnItemchangedSearchResultList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    int index = m_result_list.GetCurSel();
    bool selected_valid = (index >= 0 && index < m_result_list.GetItemCount());
    EnableControls(selected_valid);
    *pResult = 0;
}
