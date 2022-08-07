// FlacEncodeCfgDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "afxdialogex.h"
#include "FlacEncodeCfgDlg.h"


// CFlacEncodeCfgDlg 对话框

IMPLEMENT_DYNAMIC(CFlacEncodeCfgDlg, CDialog)

CFlacEncodeCfgDlg::CFlacEncodeCfgDlg(CWnd* pParent /*=nullptr*/)
    : CDialog(IDD_FLAC_ENCODE_CFG_DIALOG, pParent)
{

}

CFlacEncodeCfgDlg::~CFlacEncodeCfgDlg()
{
}

void CFlacEncodeCfgDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SLIDER1, m_comp_level_slider);
}

void CFlacEncodeCfgDlg::SetInfoText()
{
    CString comp_level;
    comp_level.Format(_T("%d"), m_encode_para.compression_level);
    SetDlgItemText(IDC_COMP_LEVEL_STATIC, comp_level);

    //设置编码参数
    CString str_cmd_para = _T("-") + comp_level;
    m_encode_para.cmd_para = str_cmd_para.GetString();

    SetDlgItemText(IDC_EDIT1, str_cmd_para);
}

void CFlacEncodeCfgDlg::EnableControl()
{
    CWnd* edit_control{ GetDlgItem(IDC_EDIT1) };
    if (edit_control != nullptr)
        edit_control->EnableWindow(m_encode_para.user_define_para);
}


BEGIN_MESSAGE_MAP(CFlacEncodeCfgDlg, CDialog)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CFlacEncodeCfgDlg::OnNMCustomdrawSlider1)
    ON_BN_CLICKED(IDC_SPECIFY_PARA_CHECK, &CFlacEncodeCfgDlg::OnBnClickedSpecifyParaCheck)
END_MESSAGE_MAP()


// CFlacEncodeCfgDlg 消息处理程序


BOOL CFlacEncodeCfgDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

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
    EnableControl();
}


void CFlacEncodeCfgDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    CString str;
    GetDlgItemText(IDC_EDIT1, str);
    m_encode_para.cmd_para = str;

    CDialog::OnOK();
}
