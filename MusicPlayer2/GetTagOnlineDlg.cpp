#include "stdafx.h"
#include "GetTagOnlineDlg.h"
#include "Resource.h"
#include "Player.h"
#include "MusicPlayer2.h"

CGetTagOnlineDlg::CGetTagOnlineDlg(const SongInfo& song, CWnd * pParent)
    : CCoverDownloadDlg(pParent), m_song_info(song)
{
}

CGetTagOnlineDlg::~CGetTagOnlineDlg()
{
}

CString CGetTagOnlineDlg::GetDialogName() const
{
    return _T("GetTagOnlineDlg");
}

void CGetTagOnlineDlg::OnBnClickedDownloadSelected()
{
    //MessageBox(_T("Downloan button clicked!"));
    if (m_item_selected < 0 || m_item_selected >= static_cast<int>(m_down_list.size()))
        return;
    m_item_info = m_down_list[m_item_selected];
    CPlayer::GetInstance().SetRelatedSongID(m_item_info.id);		//将选中项目的歌曲ID关联到歌曲

    if (m_pParentWnd != nullptr)
    {
        m_pParentWnd->SendMessage(WM_PORPERTY_ONLINE_INFO_ACQUIRED, (WPARAM)&m_item_info);
        OnCancel();
    }
}

SongInfo CGetTagOnlineDlg::GetSongInfo() const
{
    return m_song_info;
}


BOOL CGetTagOnlineDlg::OnInitDialog()
{
    CCoverDownloadDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
    SetIcon(theApp.m_icon_set.info.GetIcon(true), FALSE);
    
    SetWindowText(CCommon::LoadText(IDS_GET_TAG_ONLINE_TITLE));

    CWnd* pGetInfoBtn = GetDlgItem(IDC_DOWNLOAD_SELECTED);
    if (pGetInfoBtn != nullptr)
        pGetInfoBtn->SetWindowText(CCommon::LoadText(IDS_GET_THIS));

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}
