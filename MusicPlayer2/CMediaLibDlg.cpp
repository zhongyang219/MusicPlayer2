// CMediaLibDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "CMediaLibDlg.h"
#include "afxdialogex.h"
#include "MediaLibStatisticsDlg.h"

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

void CMediaLibDlg::SetCurTab(int tab)
{
    m_tab_ctrl.SetCurTab(tab);
}

bool CMediaLibDlg::NavigateToItem(const wstring& item)
{
    CMediaClassifyDlg* pCurTab = dynamic_cast<CMediaClassifyDlg*>(m_tab_ctrl.GetCurrentTab());
    if (pCurTab != nullptr)
    {
        return pCurTab->SetLeftListSel(item);
    }
    return false;
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
    ON_BN_CLICKED(IDC_STATISTICS_INFO_BUTTON, &CMediaLibDlg::OnBnClickedStatisticsInfoButton)
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

    SetIcon(theApp.m_icon_set.media_lib.GetIcon(true), FALSE);
    SetIcon(AfxGetApp()->LoadIcon(IDI_MEDIA_LIB_D), TRUE);

    CButton* close_btn = (CButton*)(GetDlgItem(IDCANCEL));
    if (close_btn != nullptr)
        close_btn->SetIcon(theApp.m_icon_set.close.GetIcon(true));
    CButton* play_btn = (CButton*)(GetDlgItem(IDC_PLAY_SELECTED));
    if (play_btn != nullptr)
        play_btn->SetIcon(theApp.m_icon_set.play_new.GetIcon(true));
    CButton* setting_btn = (CButton*)(GetDlgItem(IDC_MEDIA_LIB_SETTINGS_BTN));
    if (setting_btn != nullptr)
        setting_btn->SetIcon(theApp.m_icon_set.setting.GetIcon(true));
    CButton* statistics_btn = (CButton*)(GetDlgItem(IDC_STATISTICS_INFO_BUTTON));
    if (statistics_btn != nullptr)
        statistics_btn->SetIcon(theApp.m_icon_set.info.GetIcon(true));

    //为每个标签添加图标
    CImageList ImageList;
    ImageList.Create(theApp.DPI(16), theApp.DPI(16), ILC_COLOR32 | ILC_MASK, 2, 2);

    //创建子对话框
    //文件夹
    m_path_dlg.Create(IDD_SET_PATH_DIALOG);
    ImageList.Add(theApp.m_icon_set.select_folder.GetIcon(true));
    m_tab_ctrl.AddWindow(&m_path_dlg, CCommon::LoadText(IDS_FOLDER));

    //播放列表
    m_playlist_dlg.Create(IDD_SELECT_PLAYLIST_DIALOG);
    ImageList.Add(theApp.m_icon_set.show_playlist.GetIcon(true));
    m_tab_ctrl.AddWindow(&m_playlist_dlg, CCommon::LoadText(IDS_PLAYLIST));

    //艺术家
    if ((theApp.m_media_lib_setting_data.display_item & MLDI_ARTIST) || (m_tab_show_force & MLDI_ARTIST))
    {
        m_artist_dlg.Create(IDD_MEDIA_CLASSIFY_DIALOG);
        ImageList.Add(theApp.m_icon_set.artist.GetIcon(true));
        m_tab_ctrl.AddWindow(&m_artist_dlg, CCommon::LoadText(IDS_ARTIST));
    }
    //唱片集
    if ((theApp.m_media_lib_setting_data.display_item & MLDI_ALBUM) || (m_tab_show_force & MLDI_ALBUM))
    {
        m_album_dlg.Create(IDD_MEDIA_CLASSIFY_DIALOG);
        ImageList.Add(theApp.m_icon_set.album.GetIcon(true));
        m_tab_ctrl.AddWindow(&m_album_dlg, CCommon::LoadText(IDS_ALBUM));
    }
    //流派
    if (theApp.m_media_lib_setting_data.display_item & MLDI_GENRE)
    {
        m_genre_dlg.Create(IDD_MEDIA_CLASSIFY_DIALOG);
        ImageList.Add(theApp.m_icon_set.genre.GetIcon(true));
        m_tab_ctrl.AddWindow(&m_genre_dlg, CCommon::LoadText(IDS_GENRE));
    }
    //年份
    if (theApp.m_media_lib_setting_data.display_item & MLDI_YEAR)
    {
        m_year_dlg.Create(IDD_MEDIA_CLASSIFY_DIALOG);
        ImageList.Add(theApp.m_icon_set.year.GetIcon(true));
        m_tab_ctrl.AddWindow(&m_year_dlg, CCommon::LoadText(IDS_YEAR));
    }
    //类型
    if (theApp.m_media_lib_setting_data.display_item & MLDI_TYPE)
    {
        m_type_dlg.Create(IDD_MEDIA_CLASSIFY_DIALOG);
        ImageList.Add(theApp.m_icon_set.file_relate);
        m_tab_ctrl.AddWindow(&m_type_dlg, CCommon::LoadText(IDS_FILE_TYPE));
    }
    //比特率
    if (theApp.m_media_lib_setting_data.display_item & MLDI_BITRATE)
    {
        m_bitrate_dlg.Create(IDD_MEDIA_CLASSIFY_DIALOG);
        ImageList.Add(theApp.m_icon_set.convert);
        m_tab_ctrl.AddWindow(&m_bitrate_dlg, CCommon::LoadText(IDS_BITRATE));
    }
    //分级
    if (theApp.m_media_lib_setting_data.display_item & MLDI_RATING)
    {
        m_rating_dlg.Create(IDD_MEDIA_CLASSIFY_DIALOG);
        ImageList.Add(theApp.m_icon_set.star);
        m_tab_ctrl.AddWindow(&m_rating_dlg, CCommon::LoadText(IDS_RATING));
    }
    //所有曲目
    if (theApp.m_media_lib_setting_data.display_item & MLDI_ALL)
    {
        m_all_media_dlg.Create(IDD_ALL_MEDIA_DIALOG);
        ImageList.Add(theApp.m_icon_set.media_lib.GetIcon(true));
        m_tab_ctrl.AddWindow(&m_all_media_dlg, CCommon::LoadText(IDS_ALL_TRACKS));
    }
    //最近播放
    if (theApp.m_media_lib_setting_data.display_item & MLDI_RECENT)
    {
        m_recent_media_dlg.Create(IDD_ALL_MEDIA_DIALOG);
        ImageList.Add(theApp.m_icon_set.recent_songs.GetIcon(true));
        m_tab_ctrl.AddWindow(&m_recent_media_dlg, CCommon::LoadText(IDS_RECENT_PLAYED));
    }
    //文件夹浏览
    if (theApp.m_media_lib_setting_data.display_item & MLDI_FOLDER_EXPLORE)
    {
        m_folder_explore_dlg.Create(IDD_FOLDER_EXPLORE_DIALOG);
        ImageList.Add(theApp.m_icon_set.folder_explore.GetIcon(true));
        m_tab_ctrl.AddWindow(&m_folder_explore_dlg, CCommon::LoadText(IDS_FOLDER_EXPLORE));
    }

    m_tab_ctrl.SetItemSize(CSize(theApp.DPI(60), theApp.DPI(24)));
    m_tab_ctrl.AdjustTabWindowSize();

    m_tab_ctrl.SetImageList(&ImageList);
    ImageList.Detach();

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

    m_path_dlg.AdjustColumnWidth();
    m_playlist_dlg.AdjustColumnWidth();

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


void CMediaLibDlg::OnBnClickedStatisticsInfoButton()
{
    // TODO: 在此添加控件通知处理程序代码
    CMediaLibStatisticsDlg dlg;
    dlg.DoModal();
}
