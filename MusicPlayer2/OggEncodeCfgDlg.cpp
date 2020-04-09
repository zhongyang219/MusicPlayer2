// OggEncodeCfgDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "OggEncodeCfgDlg.h"
#include "afxdialogex.h"


// COggEncodeCfgDlg 对话框

IMPLEMENT_DYNAMIC(COggEncodeCfgDlg, CDialog)

COggEncodeCfgDlg::COggEncodeCfgDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_OGG_ENCODE_CFG_DIALOG, pParent)
{

}

COggEncodeCfgDlg::~COggEncodeCfgDlg()
{
}

void COggEncodeCfgDlg::SetInfoText()
{
	int rate;
	switch (m_encode_quality)
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
	CString info;
	info.Format(CCommon::LoadText(IDS_CURRENT_SELECTED, _T(": %d (%d kbps)")), m_encode_quality, rate);
	SetDlgItemText(IDC_INFO_STATIC, info);
}

void COggEncodeCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_QUALITY_SLIDER, m_quality_sld);
}


BEGIN_MESSAGE_MAP(COggEncodeCfgDlg, CDialog)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_QUALITY_SLIDER, &COggEncodeCfgDlg::OnNMCustomdrawQualitySlider)
END_MESSAGE_MAP()


// COggEncodeCfgDlg 消息处理程序


BOOL COggEncodeCfgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_quality_sld.SetRange(-1, 10);
	m_quality_sld.SetPos(m_encode_quality);
	SetInfoText();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void COggEncodeCfgDlg::OnNMCustomdrawQualitySlider(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_encode_quality = m_quality_sld.GetPos();
	SetInfoText();
	*pResult = 0;
}
