// CDonateDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "CDonateDlg.h"


// CDonateDlg 对话框

IMPLEMENT_DYNAMIC(CDonateDlg, CBaseDialog)

CDonateDlg::CDonateDlg(CWnd* pParent /*=nullptr*/)
    : CBaseDialog(IDD_DONATE_DIALOG, pParent)
{

}

CDonateDlg::~CDonateDlg()
{
}

CString CDonateDlg::GetDialogName() const
{
    return L"DonateDlg";
}

bool CDonateDlg::InitializeControls()
{
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TITLE_DONATE");
    SetWindowTextW(temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_DONATE_INFO");
    SetDlgItemTextW(IDC_DONATE_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_CLOSE");
    SetDlgItemTextW(IDCANCEL, temp.c_str());

    RepositionTextBasedControls({
        { CtrlTextInfo::R1, IDCANCEL, CtrlTextInfo::W32 }
        });
    return true;
}

void CDonateDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDonateDlg, CBaseDialog)
    ON_WM_PAINT()
END_MESSAGE_MAP()


// CDonateDlg 消息处理程序


BOOL CDonateDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化

    //计算两个二维码图片的位置
    CRect rect{};
    CWnd* pWnd = nullptr;
    pWnd = GetDlgItem(IDC_DONATE_STATIC);
    if (pWnd != nullptr)
    {
        pWnd->GetWindowRect(rect);
        ScreenToClient(rect);
    }

    CRect rc_pic_area;
    GetClientRect(rc_pic_area);
    rc_pic_area.top = rect.bottom + theApp.DPI(8);
    rc_pic_area.left += theApp.DPI(10);
    rc_pic_area.right -= theApp.DPI(10);
    pWnd = GetDlgItem(IDCANCEL);
    if (pWnd != nullptr)
    {
        pWnd->GetWindowRect(rect);
        ScreenToClient(rect);
    }
    rc_pic_area.bottom = rect.top - theApp.DPI(8);
    m_pic1_rect = rc_pic_area;
    m_pic1_rect.right = m_pic1_rect.left + (rc_pic_area.Width() / 2) - theApp.DPI(4);

    m_pic2_rect = rc_pic_area;
    m_pic2_rect.left = m_pic2_rect.right - (rc_pic_area.Width() / 2) + theApp.DPI(4);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CDonateDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
                       // TODO: 在此处添加消息处理程序代码
                       // 不为绘图消息调用 CDialogEx::OnPaint()

    CDrawCommon draw;
    draw.Create(&dc);
    draw.DrawBitmap(IDB_DONATE, m_pic1_rect.TopLeft(), m_pic1_rect.Size(), CDrawCommon::StretchMode::FIT);
    draw.DrawBitmap(IDB_DONATE_WECHAT, m_pic2_rect.TopLeft(), m_pic2_rect.Size(), CDrawCommon::StretchMode::FIT);
}
