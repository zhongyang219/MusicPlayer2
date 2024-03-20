// OggEncodeCfgDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "OggEncodeCfgDlg.h"


// COggEncodeCfgDlg 对话框

IMPLEMENT_DYNAMIC(COggEncodeCfgDlg, CBaseDialog)

COggEncodeCfgDlg::COggEncodeCfgDlg(CWnd* pParent /*=nullptr*/)
    : CBaseDialog(IDD_OGG_ENCODE_CFG_DIALOG, pParent)
{

}

COggEncodeCfgDlg::~COggEncodeCfgDlg()
{
}

void COggEncodeCfgDlg::SetInfoText()
{
    int rate{};
    switch (m_encode_para.encode_quality)
    {
    case -1: rate = 45; break;
    case 0: rate = 64; break;
    case 1: rate = 80; break;
    case 2: rate = 96; break;
    case 3: rate = 112; break;
    case 4: rate = 128; break;
    case 5: rate = 160; break;
    case 6: rate = 192; break;
    case 7: rate = 224; break;
    case 8: rate = 256; break;
    case 9: rate = 320; break;
    case 10: rate = 500; break;
    }
    // wstring info = theApp.m_str_table.LoadTextFormat(L"TXT_ENCODE_OPT_OGG_QUALITY_INFO", { m_encode_para.encode_quality, rate });
    std::wstringstream wss;
    wss << m_encode_para.encode_quality << L" (" << rate << L" kbps)";
    wstring info = wss.str();
    SetDlgItemTextW(IDC_INFO_STATIC, info.c_str());
}

CString COggEncodeCfgDlg::GetDialogName() const
{
    return L"OggEncodeOptDlg";
}

bool COggEncodeCfgDlg::InitializeControls()
{
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TITLE_ENCODE_OPT_OGG");
    SetWindowTextW(temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_ENCODE_OPT_OGG_ENCODE_QUALITY_SEL");
    SetDlgItemTextW(IDC_TXT_ENCODE_OPT_OGG_ENCODE_QUALITY_SEL_STATIC, temp.c_str());
    // IDC_QUALITY_SLIDER
    // IDC_INFO_STATIC
    temp = theApp.m_str_table.LoadText(L"TXT_ENCODE_OPT_OGG_ENCODE_QUALITY_LOW");
    SetDlgItemTextW(IDC_TXT_ENCODE_OPT_OGG_ENCODE_QUALITY_LOW_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_ENCODE_OPT_OGG_ENCODE_QUALITY_HIGH");
    SetDlgItemTextW(IDC_TXT_ENCODE_OPT_OGG_ENCODE_QUALITY_HIGH_STATIC, temp.c_str());

    RepositionTextBasedControls({
        { CtrlTextInfo::R1, IDOK, CtrlTextInfo::W32 },
        { CtrlTextInfo::R2, IDCANCEL, CtrlTextInfo::W32 }
        });
    return true;
}

void COggEncodeCfgDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_QUALITY_SLIDER, m_quality_sld);
}


BEGIN_MESSAGE_MAP(COggEncodeCfgDlg, CBaseDialog)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_QUALITY_SLIDER, &COggEncodeCfgDlg::OnNMCustomdrawQualitySlider)
END_MESSAGE_MAP()


// COggEncodeCfgDlg 消息处理程序


BOOL COggEncodeCfgDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    m_quality_sld.SetRange(-1, 10);
    m_quality_sld.SetPos(m_encode_para.encode_quality);
    SetInfoText();
    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void COggEncodeCfgDlg::OnNMCustomdrawQualitySlider(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    m_encode_para.encode_quality = m_quality_sld.GetPos();
    SetInfoText();
    *pResult = 0;
}
