// MediaLibStatisticsDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "MediaLibStatisticsDlg.h"
#include "afxdialogex.h"
#include "SongDataManager.h"
#include "MediaLibHelper.h"

// CMediaLibStatisticsDlg 对话框

IMPLEMENT_DYNAMIC(CMediaLibStatisticsDlg, CBaseDialog)

CMediaLibStatisticsDlg::CMediaLibStatisticsDlg(CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_SELECT_ITEM_DIALOG, pParent)
{

}

CMediaLibStatisticsDlg::~CMediaLibStatisticsDlg()
{
}

void CMediaLibStatisticsDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
}


CString CMediaLibStatisticsDlg::GetDialogName() const
{
    return _T("MediaLibStatisticsDlg");
}

BEGIN_MESSAGE_MAP(CMediaLibStatisticsDlg, CBaseDialog)
END_MESSAGE_MAP()


// CMediaLibStatisticsDlg 消息处理程序


BOOL CMediaLibStatisticsDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    
    SetWindowText(CCommon::LoadText(IDS_MEDIALIB_STATISTICS));
    SetIcon(theApp.m_icon_set.info.GetIcon(true), FALSE);

    //初始化控件
    CWnd* ok_btn{ GetDlgItem(IDOK) };
    if (ok_btn != nullptr)
        ok_btn->ShowWindow(SW_HIDE);
    SetDlgItemText(IDCANCEL, CCommon::LoadText(IDS_CLOSE));

    //初始化列表
    CRect rect;
    m_list_ctrl.GetWindowRect(rect);
    m_list_ctrl.SetExtendedStyle(m_list_ctrl.GetExtendedStyle() | LVS_EX_GRIDLINES);
    int width0 = rect.Width() / 2;
    int width1 = rect.Width() - width0 - theApp.DPI(20) - 1;
    m_list_ctrl.InsertColumn(0, CCommon::LoadText(IDS_ITEM), LVCFMT_LEFT, width0);
    m_list_ctrl.InsertColumn(1, CCommon::LoadText(IDS_VLAUE), LVCFMT_LEFT, width1);

    //插入列
    m_list_ctrl.InsertItem(RI_ARTIST, CCommon::LoadText(IDS_ARTIST));   //艺术家
    m_list_ctrl.InsertItem(RI_ALBUM, CCommon::LoadText(IDS_ALBUM));     //唱片
    m_list_ctrl.InsertItem(RI_GENRE, CCommon::LoadText(IDS_GENRE));     //流派
    m_list_ctrl.InsertItem(RI_TOTAL, CCommon::LoadText(IDS_TOTAL_TRACKS));      //曲目总数
    m_list_ctrl.InsertItem(RI_PLAYED, CCommon::LoadText(IDS_TRACKS_PLAYED));    //播放过的曲目数

    //设置数值
    std::set<std::wstring, StringComparerNoCase> artist_set;
    std::set<std::wstring, StringComparerNoCase> album_set;
    std::set<std::wstring, StringComparerNoCase> genre_set;
    int played_num{};
    for (const auto& item : CSongDataManager::GetInstance().GetSongData())
    {
        artist_set.emplace(item.second.artist);
        album_set.emplace(item.second.album);
        genre_set.emplace(item.second.genre);
        if (item.second.last_played_time > 0)
            played_num++;
    }
    m_list_ctrl.SetItemText(RI_ARTIST, 1, std::to_wstring(artist_set.size()).c_str());
    m_list_ctrl.SetItemText(RI_ALBUM, 1, std::to_wstring(album_set.size()).c_str());
    m_list_ctrl.SetItemText(RI_GENRE, 1, std::to_wstring(genre_set.size()).c_str());
    m_list_ctrl.SetItemText(RI_TOTAL, 1, std::to_wstring(CSongDataManager::GetInstance().GetSongData().size()).c_str());
    m_list_ctrl.SetItemText(RI_PLAYED, 1, std::to_wstring(played_num).c_str());

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}
