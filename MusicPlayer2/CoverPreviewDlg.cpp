// CoverPreviewDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "afxdialogex.h"
#include "CoverPreviewDlg.h"

// CCoverPreviewDlg 对话框

IMPLEMENT_DYNAMIC(CCoverPreviewDlg, CBaseDialog)

CCoverPreviewDlg::CCoverPreviewDlg(std::wstring& file_path, CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_COVER_PREVIEW_DIALOG, pParent)
{
	m_cover_img.Load(file_path.c_str());
}

CCoverPreviewDlg::~CCoverPreviewDlg()
{
}

void CCoverPreviewDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCoverPreviewDlg, CBaseDialog)
	ON_WM_PAINT()
    ON_WM_SIZE()
END_MESSAGE_MAP()


// CCoverPreviewDlg 消息处理程序

BOOL CCoverPreviewDlg::OnInitDialog()
{
	CBaseDialog::OnInitDialog();

    SetIcon(IconMgr::IT_Album_Cover, FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CCoverPreviewDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CBaseDialog::OnPaint()

    //计算专辑封面的绘图区域
    CRect rect;
    GetClientRect(rect);

    CRect rect_btn;
    ::GetWindowRect(GetDlgItem(IDCANCEL)->GetSafeHwnd(), rect_btn);
    ScreenToClient(rect_btn);

    rect.bottom = rect_btn.top;
    rect.DeflateRect(theApp.DPI(16), theApp.DPI(16));

    if (!m_cover_img.IsNull())        //有专辑封面时绘制专辑封面
    {
        CImage& img{ m_cover_img };
        if (img.GetWidth() < rect.Width() && img.GetHeight() < rect.Height())       //如果专辑封面图片大小小于绘图区域，则将绘图区域改为图片大小
        {
            CRect rect_img;
            rect_img.left = rect.left + (rect.Width() - img.GetWidth()) / 2;
            rect_img.top = rect.top + (rect.Height() - img.GetHeight()) / 2;
            rect_img.right = rect_img.left + img.GetWidth();
            rect_img.bottom = rect_img.top + img.GetHeight();
            rect = rect_img;
        }
        CDrawCommon draw;
        draw.Create(&dc);
        draw.DrawImage(img, rect.TopLeft(), rect.Size(), CDrawCommon::StretchMode::FIT);
    }
    else        //否则以灰色填充
    {
        dc.FillSolidRect(rect, RGB(210, 210, 210));
    }

}

CString CCoverPreviewDlg::GetDialogName() const
{
    return _T("CoverPreviewDlg");
}

bool CCoverPreviewDlg::InitializeControls()
{
    SetWindowText(theApp.m_str_table.LoadText(L"TITLE_COVER_PREVIEW").c_str());
    SetDlgControlText(IDCANCEL, L"TXT_CLOSE");

    return false;
}

void CCoverPreviewDlg::OnSize(UINT nType, int cx, int cy)
{
    CBaseDialog::OnSize(nType, cx, cy);

    if (cx > 0 && cy > 0)
    {
        Invalidate();
    }
}
