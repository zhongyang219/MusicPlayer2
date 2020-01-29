// CMediaLibDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "CMediaLibDlg.h"
#include "afxdialogex.h"


// CMediaLibDlg 对话框

IMPLEMENT_DYNAMIC(CMediaLibDlg, CDialog)

CMediaLibDlg::CMediaLibDlg(int cur_tab, CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_MEDIA_LIB_DIALOG, pParent), m_init_tab(cur_tab)
{
}

CMediaLibDlg::~CMediaLibDlg()
{
    SaveConfig();
}

void CMediaLibDlg::SaveConfig() const
{
    CIniHelper ini{ theApp.m_config_path };
    ini.WriteInt(L"media_lib", L"width", m_window_size.cx);
    ini.WriteInt(L"media_lib", L"height", m_window_size.cy);
    ini.Save();
}

void CMediaLibDlg::LoadConfig()
{
    CIniHelper ini{ theApp.m_config_path };
    m_window_size.cx = ini.GetInt(L"media_lib", L"width", -1);
    m_window_size.cy = ini.GetInt(L"media_lib", L"height", -1);
}

void CMediaLibDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TAB1, m_tab_ctrl);
}


BEGIN_MESSAGE_MAP(CMediaLibDlg, CDialog)
    ON_BN_CLICKED(IDC_PLAY_SELECTED, &CMediaLibDlg::OnBnClickedPlaySelected)
    ON_WM_GETMINMAXINFO()
    ON_MESSAGE(WM_PLAY_SELECTED_BTN_ENABLE, &CMediaLibDlg::OnPlaySelectedBtnEnable)
    ON_WM_ERASEBKGND()
    ON_BN_CLICKED(IDC_MEDIA_LIB_SETTINGS_BTN, &CMediaLibDlg::OnBnClickedMediaLibSettingsBtn)
    ON_WM_SIZE()
END_MESSAGE_MAP()


// CMediaLibDlg 消息处理程序


void CMediaLibDlg::OnCancel()
{
    // TODO: 在此添加专用代码和/或调用基类
    DestroyWindow();

    //CDialog::OnCancel();
}


BOOL CMediaLibDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    ModifyStyle(0, WS_CLIPCHILDREN);

    SetIcon(AfxGetApp()->LoadIcon(IDI_MEDIA_LIB_D), FALSE);		// 设置小图标

    //创建子对话框
    m_path_dlg.Create(IDD_SET_PATH_DIALOG);
    m_playlist_dlg.Create(IDD_SELECT_PLAYLIST_DIALOG);
    m_artist_dlg.Create(IDD_MEDIA_CLASSIFY_DIALOG);
    m_album_dlg.Create(IDD_MEDIA_CLASSIFY_DIALOG);
    m_genre_dlg.Create(IDD_MEDIA_CLASSIFY_DIALOG);
    m_year_dlg.Create(IDD_MEDIA_CLASSIFY_DIALOG);
	m_all_media_dlg.Create(IDD_ALL_MEDIA_DIALOG);
	m_recent_media_dlg.Create(IDD_ALL_MEDIA_DIALOG);
    m_folder_explore_dlg.Create(IDD_FOLDER_EXPLORE_DIALOG);

    m_tab_ctrl.SetItemSize(CSize(theApp.DPI(60), theApp.DPI(24)));
    m_tab_ctrl.AdjustTabWindowSize();

    //为每个标签添加图标
    CImageList ImageList;
    ImageList.Create(theApp.DPI(16), theApp.DPI(16), ILC_COLOR32 | ILC_MASK, 2, 2);
    ImageList.Add(theApp.m_icon_set.select_folder.GetIcon(true));		//文件夹
    ImageList.Add(theApp.m_icon_set.show_playlist.GetIcon(true));		//播放列表
    ImageList.Add(theApp.m_icon_set.artist.GetIcon(true));				//艺术家
    ImageList.Add(theApp.m_icon_set.album.GetIcon(true));				//唱片集
    ImageList.Add(theApp.m_icon_set.genre.GetIcon(true));				//流派
    ImageList.Add(theApp.m_icon_set.year.GetIcon(true));				//年份
    ImageList.Add(theApp.m_icon_set.media_lib.GetIcon(true));			//所有曲目
    ImageList.Add(theApp.m_icon_set.recent_songs.GetIcon(true));		//最近播放
    ImageList.Add(theApp.m_icon_set.folder_explore.GetIcon(true));		//文件夹浏览
    m_tab_ctrl.SetImageList(&ImageList);
    ImageList.Detach();

    //添加对话框
    m_tab_ctrl.AddWindow(&m_path_dlg, CCommon::LoadText(IDS_FOLDER));
    m_tab_ctrl.AddWindow(&m_playlist_dlg, CCommon::LoadText(IDS_PLAYLIST));
    m_tab_ctrl.AddWindow(&m_artist_dlg, CCommon::LoadText(IDS_ARTIST));
    m_tab_ctrl.AddWindow(&m_album_dlg, CCommon::LoadText(IDS_ALBUM));
    m_tab_ctrl.AddWindow(&m_genre_dlg, CCommon::LoadText(IDS_GENRE));
    m_tab_ctrl.AddWindow(&m_year_dlg, CCommon::LoadText(IDS_YEAR));
	m_tab_ctrl.AddWindow(&m_all_media_dlg, CCommon::LoadText(IDS_ALL_TRACKS));
	m_tab_ctrl.AddWindow(&m_recent_media_dlg, CCommon::LoadText(IDS_RECENT_PLAYED));
    m_tab_ctrl.AddWindow(&m_folder_explore_dlg, CCommon::LoadText(IDS_FOLDER_EXPLORE));
    m_tab_ctrl.SetCurTab(m_init_tab);

    //获取初始时窗口的大小
    CRect rect;
    GetWindowRect(rect);
    m_min_size.cx = rect.Width();
    m_min_size.cy = rect.Height();

    OnPlaySelectedBtnEnable(0, 0);

    LoadConfig();
    if (m_window_size.cx > 0 && m_window_size.cy > 0)
    {
        SetWindowPos(nullptr, 0, 0, m_window_size.cx, m_window_size.cy, SWP_NOMOVE | SWP_NOZORDER);
    }
    CenterWindow(GetParent());

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CMediaLibDlg::OnBnClickedPlaySelected()
{
    CTabDlg* current_tab = dynamic_cast<CTabDlg*>(m_tab_ctrl.GetCurrentTab());
    if (current_tab != nullptr)
        current_tab->OnOK();
    //if (current_tab == &m_path_dlg)
    //    m_path_dlg.OnOK();
    //else if (current_tab == &m_playlist_dlg)
    //    m_playlist_dlg.OnOK();
    //else if(c)
}


void CMediaLibDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    //限制窗口最小大小
    lpMMI->ptMinTrackSize.x = m_min_size.cx;		//设置最小宽度
    lpMMI->ptMinTrackSize.y = m_min_size.cy;		//设置最小高度

    CDialog::OnGetMinMaxInfo(lpMMI);
}


afx_msg LRESULT CMediaLibDlg::OnPlaySelectedBtnEnable(WPARAM wParam, LPARAM lParam)
{
    bool enable = (wParam != 0);
    CWnd* pBtn = GetDlgItem(IDC_PLAY_SELECTED);
    if (pBtn != nullptr)
        pBtn->EnableWindow(enable);

    return 0;
}


BOOL CMediaLibDlg::OnEraseBkgnd(CDC* pDC)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    ////重绘背景时设置剪辑区域为窗口区域减去tab控件的区域
    //CRect rc_tab;
    //m_tab_ctrl.GetWindowRect(rc_tab);
    //ScreenToClient(rc_tab);
    //CRgn tab_rgn;       //tab控件区域
    //tab_rgn.CreateRectRgnIndirect(rc_tab);
    //CRect rc_client;
    //GetClientRect(rc_client);
    //CRgn draw_rgn;      //需要重绘的区域
    //draw_rgn.CreateRectRgnIndirect(rc_client);
    //draw_rgn.CombineRgn(&draw_rgn, &tab_rgn, RGN_DIFF);
    //pDC->SelectClipRgn(&draw_rgn);
    return CDialog::OnEraseBkgnd(pDC);
}


void CMediaLibDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类

    CDialog::OnOK();
    DestroyWindow();

}


void CMediaLibDlg::OnBnClickedMediaLibSettingsBtn()
{
    // TODO: 在此添加控件通知处理程序代码
    theApp.m_pMainWnd->SendMessage(WM_OPTION_SETTINGS, 4, LPARAM(this));
}


void CMediaLibDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    // TODO: 在此处添加消息处理程序代码
    if (nType != SIZE_MINIMIZED && nType != SIZE_MAXIMIZED)
    {
        CRect rect;
        GetWindowRect(&rect);
        m_window_size = rect.Size();
    }
}
