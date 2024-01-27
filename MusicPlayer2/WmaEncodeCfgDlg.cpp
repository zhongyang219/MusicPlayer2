// CWmaEncodeCfgDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "WmaEncodeCfgDlg.h"


// CWmaEncodeCfgDlg 对话框

IMPLEMENT_DYNAMIC(CWmaEncodeCfgDlg, CBaseDialog)

CWmaEncodeCfgDlg::CWmaEncodeCfgDlg(CWnd* pParent /*=nullptr*/)
    : CBaseDialog(IDD_WMA_ENCODE_CFG_DIALOG, pParent)
{

}

CWmaEncodeCfgDlg::~CWmaEncodeCfgDlg()
{
}

void CWmaEncodeCfgDlg::SetControlEnable()
{
    m_bitrate_combo.EnableWindow(m_encode_para.cbr);
    m_vbr_quality_combo.EnableWindow(!m_encode_para.cbr);
}

CString CWmaEncodeCfgDlg::GetDialogName() const
{
    return L"WmaEncodeOptDlg";
}

bool CWmaEncodeCfgDlg::InitializeControls()
{
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TITLE_ENCODE_OPT_WMA");
    SetWindowTextW(temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_ENCODE_OPT_WMA_CBR");
    SetDlgItemTextW(IDC_CBR_RADIO, temp.c_str());
    // IDC_BITRATE_COMBO
    temp = theApp.m_str_table.LoadText(L"TXT_ENCODE_OPT_WMA_VBR");
    SetDlgItemTextW(IDC_CBR_RADIO2, temp.c_str());
    // IDC_VBR_QUALITY_COMBO
    // IDOK
    // IDCANCEL

    RepositionTextBasedControls({
        { CtrlTextInfo::R1, IDOK, CtrlTextInfo::W32 },
        { CtrlTextInfo::R2, IDCANCEL, CtrlTextInfo::W32 }
        });
    return true;
}

void CWmaEncodeCfgDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BITRATE_COMBO, m_bitrate_combo);
    DDX_Control(pDX, IDC_VBR_QUALITY_COMBO, m_vbr_quality_combo);
}


BEGIN_MESSAGE_MAP(CWmaEncodeCfgDlg, CBaseDialog)
    ON_CBN_SELCHANGE(IDC_BITRATE_COMBO, &CWmaEncodeCfgDlg::OnCbnSelchangeBitrateCombo)
    ON_CBN_SELCHANGE(IDC_VBR_QUALITY_COMBO, &CWmaEncodeCfgDlg::OnCbnSelchangeVbrQualityCombo)
    ON_BN_CLICKED(IDC_CBR_RADIO, &CWmaEncodeCfgDlg::OnBnClickedCbrRadio)
    ON_BN_CLICKED(IDC_CBR_RADIO2, &CWmaEncodeCfgDlg::OnBnClickedCbrRadio2)
END_MESSAGE_MAP()


// CWmaEncodeCfgDlg 消息处理程序


BOOL CWmaEncodeCfgDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    m_bitrate_list.emplace_back(L"16 kbps", 16);
    m_bitrate_list.emplace_back(L"24 kbps", 24);
    m_bitrate_list.emplace_back(L"32 kbps", 32);
    m_bitrate_list.emplace_back(L"48 kbps", 48);
    m_bitrate_list.emplace_back(L"64 kbps", 64);
    m_bitrate_list.emplace_back(L"80 kbps", 80);
    m_bitrate_list.emplace_back(L"96 kbps", 96);
    m_bitrate_list.emplace_back(L"128 kbps", 128);
    m_bitrate_list.emplace_back(L"160 kbps", 160);
    m_bitrate_list.emplace_back(L"192 kbps", 192);
    m_bitrate_list.emplace_back(L"224 kbps", 224);
    m_bitrate_list.emplace_back(L"256 kbps", 256);
    m_bitrate_list.emplace_back(L"320 kbps", 320);
    m_quality_list.emplace_back(L"10 %", 10);
    m_quality_list.emplace_back(L"25 %", 25);
    m_quality_list.emplace_back(L"50 %", 50);
    m_quality_list.emplace_back(L"75 %", 75);
    m_quality_list.emplace_back(L"90 %", 90);
    m_quality_list.emplace_back(L"98 %", 98);

    if (m_encode_para.cbr)
        CheckDlgButton(IDC_CBR_RADIO, TRUE);
    else
        CheckDlgButton(IDC_CBR_RADIO2, TRUE);

    int index{};
    for (size_t i{}; i < m_bitrate_list.size(); ++i)
    {
        m_bitrate_combo.AddString(m_bitrate_list[i].first.c_str());
        if (m_bitrate_list[i].second >= m_encode_para.cbr_bitrate)
            index = i;      // 选择最接近的项目
    }
    m_bitrate_combo.SetCurSel(index);
    for (size_t i{}; i < m_quality_list.size(); ++i)
    {
        m_vbr_quality_combo.AddString(m_quality_list[i].first.c_str());
        if (m_quality_list[i].second >= m_encode_para.vbr_quality)
            index = i;      // 选择最接近的项目
    }
    m_vbr_quality_combo.SetCurSel(index);

    SetControlEnable();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CWmaEncodeCfgDlg::OnCbnSelchangeBitrateCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    int index = m_bitrate_combo.GetCurSel();
    if (index >= 0 && index < static_cast<int>(m_bitrate_list.size()))
        m_encode_para.cbr_bitrate = m_bitrate_list[index].second;
}


void CWmaEncodeCfgDlg::OnCbnSelchangeVbrQualityCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    int index = m_vbr_quality_combo.GetCurSel();
    if (index >= 0 && index < static_cast<int>(m_quality_list.size()))
        m_encode_para.vbr_quality = m_quality_list[index].second;
}


void CWmaEncodeCfgDlg::OnBnClickedCbrRadio()
{
    // TODO: 在此添加控件通知处理程序代码
    m_encode_para.cbr = true;
    SetControlEnable();
}


void CWmaEncodeCfgDlg::OnBnClickedCbrRadio2()
{
    // TODO: 在此添加控件通知处理程序代码
    m_encode_para.cbr = false;
    SetControlEnable();
}
