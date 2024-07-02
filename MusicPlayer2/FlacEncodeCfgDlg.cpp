// FlacEncodeCfgDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "FlacEncodeCfgDlg.h"


// CFlacEncodeCfgDlg 对话框

IMPLEMENT_DYNAMIC(CFlacEncodeCfgDlg, CDialog)

CFlacEncodeCfgDlg::CFlacEncodeCfgDlg(CWnd* pParent /*=nullptr*/)
    : CBaseDialog(IDD_FLAC_ENCODE_CFG_DIALOG, pParent)
{

}

CFlacEncodeCfgDlg::~CFlacEncodeCfgDlg()
{
}

CString CFlacEncodeCfgDlg::GetDialogName() const
{
    return CString();
}

bool CFlacEncodeCfgDlg::InitializeControls()
{
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TITLE_ENCODE_OPT_FLAC");
    SetWindowTextW(temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_ENCODE_OPT_FLAC_CMP_LEVEL");
    SetDlgItemTextW(IDC_TXT_ENCODE_OPT_FLAC_CMP_LEVEL_STATIC, temp.c_str());
    // IDC_SLIDER1
    // IDC_COMP_LEVEL_STATIC
    temp = theApp.m_str_table.LoadText(L"TXT_ENCODE_OPT_FLAC_CMP_LEVEL_LOW");
    SetDlgItemTextW(IDC_TXT_ENCODE_OPT_FLAC_CMP_LEVEL_LOW_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_ENCODE_OPT_FLAC_CMP_LEVEL_HIGH");
    SetDlgItemTextW(IDC_TXT_ENCODE_OPT_FLAC_CMP_LEVEL_HIGH_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_ENCODE_OPT_FLAC_SPECIFY_PARA");
    SetDlgItemTextW(IDC_SPECIFY_PARA_CHECK, temp.c_str());
    // IDC_EDIT1
    // IDOK
    // IDCANCEL

    RepositionTextBasedControls({
        { CtrlTextInfo::L1, IDC_SPECIFY_PARA_CHECK, CtrlTextInfo::W16 },
        { CtrlTextInfo::C0, IDC_EDIT1 }
        }, CtrlTextInfo::W128);
    RepositionTextBasedControls({
        { CtrlTextInfo::R1, IDOK, CtrlTextInfo::W32 },
        { CtrlTextInfo::R2, IDCANCEL, CtrlTextInfo::W32 }
        });
    return true;
}

void CFlacEncodeCfgDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SLIDER1, m_comp_level_slider);
}

void CFlacEncodeCfgDlg::SetInfoText()
{
    wstring comp_level = std::to_wstring(m_encode_para.compression_level);
    SetDlgItemTextW(IDC_COMP_LEVEL_STATIC, comp_level.c_str());

    //设置编码参数
    if (!m_encode_para.user_define_para)
        m_encode_para.cmd_para = L"-" + comp_level;
    SetDlgItemTextW(IDC_EDIT1, m_encode_para.cmd_para.c_str());
}

void CFlacEncodeCfgDlg::EnableControl()
{
    EnableDlgCtrl(IDC_SLIDER1, !m_encode_para.user_define_para);
    EnableDlgCtrl(IDC_EDIT1, m_encode_para.user_define_para);
}


BEGIN_MESSAGE_MAP(CFlacEncodeCfgDlg, CBaseDialog)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CFlacEncodeCfgDlg::OnNMCustomdrawSlider1)
    ON_BN_CLICKED(IDC_SPECIFY_PARA_CHECK, &CFlacEncodeCfgDlg::OnBnClickedSpecifyParaCheck)
END_MESSAGE_MAP()


// CFlacEncodeCfgDlg 消息处理程序


BOOL CFlacEncodeCfgDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    m_comp_level_slider.SetRange(0, 8);
    m_comp_level_slider.SetPos(m_encode_para.compression_level);
    SetInfoText();
    CheckDlgButton(IDC_SPECIFY_PARA_CHECK, m_encode_para.user_define_para);
    EnableControl();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CFlacEncodeCfgDlg::OnNMCustomdrawSlider1(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    m_encode_para.compression_level = m_comp_level_slider.GetPos();
    SetInfoText();
    *pResult = 0;
}


void CFlacEncodeCfgDlg::OnBnClickedSpecifyParaCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_encode_para.user_define_para = (IsDlgButtonChecked(IDC_SPECIFY_PARA_CHECK) != 0);
    SetInfoText();
    EnableControl();
}


void CFlacEncodeCfgDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    CString str;
    GetDlgItemText(IDC_EDIT1, str);
    m_encode_para.cmd_para = str;

    CBaseDialog::OnOK();
}
