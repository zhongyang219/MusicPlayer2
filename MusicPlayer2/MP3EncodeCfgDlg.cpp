// MP3EncodeCfgDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "MP3EncodeCfgDlg.h"


// CMP3EncodeCfgDlg 对话框

IMPLEMENT_DYNAMIC(CMP3EncodeCfgDlg, CBaseDialog)

CMP3EncodeCfgDlg::CMP3EncodeCfgDlg(CWnd* pParent /*=nullptr*/)
    : CBaseDialog(IDD_MP3_ENCODE_CFG_DIALOG, pParent)
{

}

CMP3EncodeCfgDlg::~CMP3EncodeCfgDlg()
{
}

void CMP3EncodeCfgDlg::EncodeParaToCmdline(MP3EncodePara& para)
{
    if (para.user_define_para)
        return;
    wchar_t buff[32];
    switch (para.encode_type)
    {
    case 0:
        swprintf_s(buff, L"-b %d -h", para.cbr_bitrate);
        para.cmd_para = buff;
        break;
    case 1:
        swprintf_s(buff, L"--abr %d -h", para.abr_bitrate);
        para.cmd_para = buff;
        break;
    case 2:
        swprintf_s(buff, L"-V %d -h", para.vbr_quality);
        para.cmd_para = buff;
        break;
    default:
        break;
    }
    if (para.joint_stereo)
        para.cmd_para += L" -m j";
}

void CMP3EncodeCfgDlg::SetControlState()
{
    EnableDlgCtrl(IDC_CBR_RADIO, !m_encode_para.user_define_para);
    EnableDlgCtrl(IDC_ABR_RADIO, !m_encode_para.user_define_para);
    EnableDlgCtrl(IDC_VBR_RADIO, !m_encode_para.user_define_para);
    EnableDlgCtrl(IDC_CBR_RATE_COMBO, !m_encode_para.user_define_para && m_encode_para.encode_type == 0);
    EnableDlgCtrl(IDC_ABR_RATE_COMBO, !m_encode_para.user_define_para && m_encode_para.encode_type == 1);
    EnableDlgCtrl(IDC_SLIDER1, !m_encode_para.user_define_para && m_encode_para.encode_type == 2);
    EnableDlgCtrl(IDC_JOINT_STEREO_CHECK, !m_encode_para.user_define_para);
    m_encode_cmdline_edit.SetReadOnly(!m_encode_para.user_define_para);
    SetDlgItemTextW(IDC_ENCODE_CMDLINE_EDIT, m_encode_para.cmd_para.c_str());
}

CString CMP3EncodeCfgDlg::GetDialogName() const
{
    return L"Mp3EncodeOptDlg";
}

bool CMP3EncodeCfgDlg::InitializeControls()
{
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TITLE_ENCODE_OPT_MP3");
    SetWindowTextW(temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_ENCODE_OPT_MP3_CBR");
    SetDlgItemTextW(IDC_CBR_RADIO, temp.c_str());
    // IDC_CBR_RATE_COMBO
    temp = theApp.m_str_table.LoadText(L"TXT_ENCODE_OPT_MP3_ABR");
    SetDlgItemTextW(IDC_ABR_RADIO, temp.c_str());
    // IDC_ABR_RATE_COMBO
    temp = theApp.m_str_table.LoadText(L"TXT_ENCODE_OPT_MP3_VBR");
    SetDlgItemTextW(IDC_VBR_RADIO, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_ENCODE_OPT_MP3_VBR_QUALITY_SEL");
    SetDlgItemTextW(IDC_TXT_ENCODE_OPT_MP3_VBR_QUALITY_SEL_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_ENCODE_OPT_MP3_VBR_QUALITY_LOW");
    SetDlgItemTextW(IDC_TXT_ENCODE_OPT_MP3_VBR_QUALITY_LOW_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_ENCODE_OPT_MP3_VBR_QUALITY_HIGH");
    SetDlgItemTextW(IDC_TXT_ENCODE_OPT_MP3_VBR_QUALITY_HIGH_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_ENCODE_OPT_MP3_JOINT_STEREO");
    SetDlgItemTextW(IDC_JOINT_STEREO_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_ENCODE_OPT_MP3_SPECIFY_PARA");
    SetDlgItemTextW(IDC_USER_CHECK, temp.c_str());
    // IDC_ENCODE_CMDLINE_EDIT

    RepositionTextBasedControls({
        { CtrlTextInfo::L1, IDC_USER_CHECK, CtrlTextInfo::W16 },
        { CtrlTextInfo::C0, IDC_ENCODE_CMDLINE_EDIT }
        });
    RepositionTextBasedControls({
        { CtrlTextInfo::R1, IDOK, CtrlTextInfo::W32 },
        { CtrlTextInfo::R2, IDCANCEL, CtrlTextInfo::W32 }
        });
    return true;
}

void CMP3EncodeCfgDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CBR_RATE_COMBO, m_cbr_rate_combo);
    DDX_Control(pDX, IDC_ABR_RATE_COMBO, m_abr_rate_combo);
    DDX_Control(pDX, IDC_SLIDER1, m_vbr_qua_sld);
    DDX_Control(pDX, IDC_ENCODE_CMDLINE_EDIT, m_encode_cmdline_edit);
}


BEGIN_MESSAGE_MAP(CMP3EncodeCfgDlg, CBaseDialog)
    ON_CBN_SELCHANGE(IDC_CBR_RATE_COMBO, &CMP3EncodeCfgDlg::OnCbnSelchangeCbrRateCombo)
    ON_CBN_SELCHANGE(IDC_ABR_RATE_COMBO, &CMP3EncodeCfgDlg::OnCbnSelchangeAbrRateCombo)
    ON_BN_CLICKED(IDC_CBR_RADIO, &CMP3EncodeCfgDlg::OnBnClickedCbrRadio)
    ON_BN_CLICKED(IDC_ABR_RADIO, &CMP3EncodeCfgDlg::OnBnClickedAbrRadio)
    ON_BN_CLICKED(IDC_VBR_RADIO, &CMP3EncodeCfgDlg::OnBnClickedVbrRadio)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CMP3EncodeCfgDlg::OnNMCustomdrawSlider1)
    ON_BN_CLICKED(IDC_JOINT_STEREO_CHECK, &CMP3EncodeCfgDlg::OnBnClickedJointStereoCheck)
    ON_BN_CLICKED(IDC_USER_CHECK, &CMP3EncodeCfgDlg::OnBnClickedUserRadio)
END_MESSAGE_MAP()


// CMP3EncodeCfgDlg 消息处理程序


BOOL CMP3EncodeCfgDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    m_bitrate_list.emplace_back(L"32 kbps", 32);
    m_bitrate_list.emplace_back(L"40 kbps", 40);
    m_bitrate_list.emplace_back(L"64 kbps", 64);
    m_bitrate_list.emplace_back(L"80 kbps", 80);
    m_bitrate_list.emplace_back(L"96 kbps", 96);
    m_bitrate_list.emplace_back(L"112 kbps", 112);
    m_bitrate_list.emplace_back(L"128 kbps", 128);
    m_bitrate_list.emplace_back(L"160 kbps", 160);
    m_bitrate_list.emplace_back(L"192 kbps", 192);
    m_bitrate_list.emplace_back(L"224 kbps", 224);
    m_bitrate_list.emplace_back(L"256 kbps", 256);
    m_bitrate_list.emplace_back(L"320 kbps", 320);

    int index_cbr{}, index_abr{};
    for (size_t i{}; i < m_bitrate_list.size(); ++i)
    {
        const auto& item = m_bitrate_list[i];
        if (item.second == m_encode_para.cbr_bitrate)
            index_cbr = i;
        if (item.second == m_encode_para.abr_bitrate)
            index_abr = i;
        m_cbr_rate_combo.AddString(item.first.c_str());
        m_abr_rate_combo.AddString(item.first.c_str());
    }
    m_cbr_rate_combo.SetCurSel(index_cbr);
    m_abr_rate_combo.SetCurSel(index_abr);

    switch (m_encode_para.encode_type)
    {
    case 0: CheckDlgButton(IDC_CBR_RADIO, TRUE); break;
    case 1: CheckDlgButton(IDC_ABR_RADIO, TRUE); break;
    case 2: CheckDlgButton(IDC_VBR_RADIO, TRUE); break;
    default: break;
    }

    m_vbr_qua_sld.SetRange(0, 9);
    m_vbr_qua_sld.SetPos(9 - m_encode_para.vbr_quality);

    CheckDlgButton(IDC_JOINT_STEREO_CHECK, m_encode_para.joint_stereo);
    CheckDlgButton(IDC_USER_CHECK, m_encode_para.user_define_para);

    EncodeParaToCmdline(m_encode_para);
    SetControlState();
    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CMP3EncodeCfgDlg::OnCbnSelchangeCbrRateCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    int sel = m_cbr_rate_combo.GetCurSel();
    if (sel >= 0 && sel < static_cast<int>(m_bitrate_list.size()))
        m_encode_para.cbr_bitrate = m_bitrate_list[sel].second;
    EncodeParaToCmdline(m_encode_para);
    SetControlState();
}


void CMP3EncodeCfgDlg::OnCbnSelchangeAbrRateCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    int sel = m_abr_rate_combo.GetCurSel();
    if (sel >= 0 && sel < static_cast<int>(m_bitrate_list.size()))
        m_encode_para.abr_bitrate = m_bitrate_list[sel].second;
    EncodeParaToCmdline(m_encode_para);
    SetControlState();
}


void CMP3EncodeCfgDlg::OnBnClickedCbrRadio()
{
    // TODO: 在此添加控件通知处理程序代码
    m_encode_para.encode_type = 0;
    EncodeParaToCmdline(m_encode_para);
    SetControlState();
}


void CMP3EncodeCfgDlg::OnBnClickedAbrRadio()
{
    // TODO: 在此添加控件通知处理程序代码
    m_encode_para.encode_type = 1;
    EncodeParaToCmdline(m_encode_para);
    SetControlState();
}


void CMP3EncodeCfgDlg::OnBnClickedVbrRadio()
{
    // TODO: 在此添加控件通知处理程序代码
    m_encode_para.encode_type = 2;
    EncodeParaToCmdline(m_encode_para);
    SetControlState();
}


void CMP3EncodeCfgDlg::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    m_encode_para.vbr_quality = 9 - m_vbr_qua_sld.GetPos();
    EncodeParaToCmdline(m_encode_para);
    SetControlState();
    *pResult = 0;
}


void CMP3EncodeCfgDlg::OnBnClickedJointStereoCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_encode_para.joint_stereo = (IsDlgButtonChecked(IDC_JOINT_STEREO_CHECK) != 0);
    EncodeParaToCmdline(m_encode_para);
    SetControlState();
}


void CMP3EncodeCfgDlg::OnBnClickedUserRadio()
{
    // TODO: 在此添加控件通知处理程序代码
    m_encode_para.user_define_para = (IsDlgButtonChecked(IDC_USER_CHECK) != 0);
    EncodeParaToCmdline(m_encode_para);
    SetControlState();
}


void CMP3EncodeCfgDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    CString str;
    m_encode_cmdline_edit.GetWindowTextW(str);
    m_encode_para.cmd_para = str;

    CBaseDialog::OnOK();
}
