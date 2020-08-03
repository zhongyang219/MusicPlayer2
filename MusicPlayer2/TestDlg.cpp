// TestDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "TestDlg.h"
#include "afxdialogex.h"

// CTestDlg 对话框

IMPLEMENT_DYNAMIC(CTestDlg, CDialog)

CTestDlg::CTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_TEST_DIALOG, pParent)
{

}

CTestDlg::~CTestDlg()
{
    SAFE_DELETE(m_pImage);
}

void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TEST_TOOLBAR, m_toolbar);
    DDX_Control(pDX, IDC_TEST_PROGRESS_BAR, m_progress_bar);
    DDX_Control(pDX, IDC_MFCEDITBROWSE1, m_browse_edit);
}


BEGIN_MESSAGE_MAP(CTestDlg, CDialog)
    ON_WM_TIMER()
    ON_WM_PAINT()
END_MESSAGE_MAP()


// CTestDlg 消息处理程序


BOOL CTestDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    m_toolbar.SetIconSize(theApp.DPI(20));
    m_toolbar.AddToolButton(theApp.m_icon_set.media_lib, _T("添加"), _T("打开文件"), ID_SET_PATH, false);
    m_toolbar.AddToolButton(theApp.m_icon_set.setting, _T("添加"), _T("测试文本"), (UINT)0, true);
    m_toolbar.AddToolButton(theApp.m_icon_set.eq, _T("删除"), _T("测试文本1"), (UINT)0, true);
    m_toolbar.AddToolButton(theApp.m_icon_set.menu, _T("菜单"), _T("显示菜单"), theApp.m_menu_set.m_main_menu.GetSubMenu(0), true);
    m_toolbar.AddToolButton(theApp.m_icon_set.edit, nullptr, _T("显示菜单"), nullptr, true);

    //进度条
    m_progress_bar.SetProgress(18);
    m_progress_bar.SetBarCount(10);

    SetTimer(82373, 80, NULL);

    CString szFilter = CCommon::LoadText(IDS_SOUND_FONT_FILTER);
    m_browse_edit.EnableFileBrowseButton(_T("SF2"), szFilter);

    m_image.Load(_T("D:\\Temp\\Desktop\\AlbumCover - 曲婉婷 - Love Birds.jpg"));
    int width = m_image.GetWidth();
    int height = m_image.GetHeight();
    m_pImage = new Gdiplus::Image(L"D:\\Temp\\Desktop\\AlbumCover - 曲婉婷 - Love Birds.jpg");

    //m_pImage = CCommon::GetPngImageResource(IDB_DEFAULT_ALBUM_COVER);

    //if (m_image.GetBPP() == 32) //确认该图像包含Alpha通道
    //{
    //    for (int i = 0; i < m_image.GetWidth(); i++)
    //    {
    //        for (int j = 0; j < m_image.GetHeight(); j++)
    //        {
    //            byte *pByte = (byte *)m_image.GetPixelAddress(i, j);
    //            pByte[0] = pByte[0] * pByte[3] / 255;
    //            pByte[1] = pByte[1] * pByte[3] / 255;
    //            pByte[2] = pByte[2] * pByte[3] / 255;
    //        }
    //    }
    //}

    //bitmap.LoadBitmap(IDB_CORTANA_WHITE);

    //CDrawCommon::SaveBitmap(bitmap, L"D:\\Temp\\test_before.bmp");

    //CDrawCommon::CopyBitmap(bitmap_copy, bitmap);

    //CDrawCommon::SaveBitmap(bitmap_copy, L"D:\\Temp\\test_after.bmp");


    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CTestDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (nIDEvent == 82373)
    {
        m_timer_cnt+=4;
        m_progress_bar.SetProgress(m_timer_cnt % 100);
    }

    CDialog::OnTimer(nIDEvent);
}


void CTestDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
                       // TODO: 在此处添加消息处理程序代码
                       // 不为绘图消息调用 CDialog::OnPaint()

    const int START_X{ theApp.DPI(16) };
    const int START_Y{ theApp.DPI(120) };

    CRect img_rect{ CPoint(START_X, START_Y), CSize(theApp.DPI(250), theApp.DPI(150)) };
    CDrawCommon draw;
    draw.Create(&dc, this);
    //draw.DrawImage(m_image, img_rect.TopLeft(), img_rect.Size(), CDrawCommon::StretchMode::FIT);

    //Gdiplus::Bitmap bm(m_image, NULL);
    //Gdiplus::CachedBitmap cb(&bm, draw.GetGraphics());
    //draw.GetGraphics()->DrawCachedBitmap(&cb, START_X, START_Y);

    //draw.DrawRectOutLine(img_rect, RGB(0, 0, 0), 1, false);
    //draw.DrawBitmap(m_image, CPoint(START_X, START_Y), CSize(theApp.DPI(200), theApp.DPI(200)), CDrawCommon::StretchMode::FIT);


    //m_image.StretchBlt(dc.GetSafeHdc(), START_X, START_Y, theApp.DPI(200), theApp.DPI(200), SRCCOPY);
    //m_image.Draw(dc.GetSafeHdc(), img_rect, Gdiplus::InterpolationMode::InterpolationModeHighQuality);

    //m_image.AlphaBlend(dc.GetSafeHdc(), START_X, START_Y, theApp.DPI(200), theApp.DPI(200), 0, 0, m_image.GetWidth(), m_image.GetHeight());

    //draw.DrawBitmap(bitmap_copy, img_rect.TopLeft(), img_rect.Size(), CDrawCommon::StretchMode::FIT);

    draw.DrawRoundRect(img_rect, RGB(78, 176, 255), theApp.DPI(4), 190);

}
