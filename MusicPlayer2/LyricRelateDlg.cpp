// LyricRelateDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "LyricRelateDlg.h"
#include "MusicPlayerCmdHelper.h"


// CLyricRelateDlg 对话框

IMPLEMENT_DYNAMIC(CLyricRelateDlg, CDialog)

CLyricRelateDlg::CLyricRelateDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_LYRIC_RELATE_DIALOG, pParent)
{

}

CLyricRelateDlg::~CLyricRelateDlg()
{
}

void CLyricRelateDlg::ShowSearchResult()
{
    m_result_list.DeleteAllItems();
    m_result_list.SetHightItem(-1);
    //wstring current_lyric_file{ CPlayer::GetInstance().GetCurrentSongInfo().lyric_file };
    //AddListRow(current_lyric_file);
    for (const auto& lyric_file : m_search_result)
    {
        //if (current_lyric_file != lyric_file)
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
        m_result_list.SetItemText(index, 3, CCommon::LoadText(IDS_YES));
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

void CLyricRelateDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_FUZZY_MATCH_CHECK, m_fuzzy_match_chk);
    DDX_Control(pDX, IDC_SEARCH_RESULT_LIST, m_result_list);
}


BEGIN_MESSAGE_MAP(CLyricRelateDlg, CDialog)
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
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    SetIcon(theApp.m_icon_set.app.GetIcon(), FALSE);

    CRect rect;
    GetWindowRect(rect);
    m_min_size.cx = rect.Width();
    m_min_size.cy = rect.Height();


    wstring lyric_name{ CFilePathHelper(CPlayer::GetInstance().GetCurrentFilePath()).GetFileNameWithoutExtension() };
    SetDlgItemText(IDC_LYRIC_NAME_EDIT, lyric_name.c_str());

    EnableControls(false);

    //初始化列表控件
    m_result_list.InsertColumn(0, CCommon::LoadText(IDS_NUMBER), LVCFMT_LEFT, theApp.DPI(40));
    m_result_list.InsertColumn(1, CCommon::LoadText(IDS_FILE_NAME), LVCFMT_LEFT, theApp.DPI(160));
    m_result_list.InsertColumn(2, CCommon::LoadText(IDS_FILE_PATH), LVCFMT_LEFT, theApp.DPI(200));
    m_result_list.InsertColumn(3, CCommon::LoadText(IDS_IS_RELATED), LVCFMT_LEFT, theApp.DPI(60));

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
    CString szFilter = CCommon::LoadText(IDS_LYRIC_FILE_FILTER);
    //构造打开文件对话框
    CFileDialog fileDlg(TRUE, _T("lrc"), NULL, 0, szFilter, this);
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
    CCommon::DeleteAFile(m_hWnd, lyric_file);
    SearchLyrics();
    ShowSearchResult();
}


void CLyricRelateDlg::OnBnClickedDonotRelateButton()
{
    // TODO: 在此添加控件通知处理程序代码
    CPlayer::GetInstance().ClearLyric();		//清除歌词
    SongInfo& song_info{ theApp.GetSongInfoRef(CPlayer::GetInstance().GetCurrentFilePath()) };
    song_info.lyric_file = NO_LYRIC_STR;       //将该歌曲设置为不关联歌词
    song_info.SetNoOnlineLyric(true);
    //ShowSearchResult();
    OnCancel();
}


void CLyricRelateDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类

    int index_selected = m_result_list.GetCurSel();
    wstring related_lyric = GetListRow(index_selected);
    CPlayer::GetInstance().IniLyrics(related_lyric);

    CDialog::OnOK();
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


void CLyricRelateDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    lpMMI->ptMinTrackSize.x = m_min_size.cx;
    lpMMI->ptMinTrackSize.y = m_min_size.cy;

    CDialog::OnGetMinMaxInfo(lpMMI);
}
