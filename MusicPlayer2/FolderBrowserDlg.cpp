#include "stdafx.h"
#include "FolderBrowserDlg.h"
#include "Common.h"
#include "resource.h"


CFolderBrowserDlg::CFolderBrowserDlg(HWND hParent)
	: m_hParent{ hParent }
{
}


CFolderBrowserDlg::~CFolderBrowserDlg()
{
}

int CFolderBrowserDlg::DoModal()
{
	TCHAR szPath[MAX_PATH];		//存放选择的目录路径
	CString str;

	BROWSEINFO bi;
	bi.hwndOwner = m_hParent;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szPath;
	bi.lpszTitle = m_info;
	bi.ulFlags = 0;
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;
	//弹出选择目录对话框
	browse:
	LPITEMIDLIST lp = SHBrowseForFolder(&bi);

	if (lp)
	{
		if (!SHGetPathFromIDList(lp, szPath))
		{
			AfxMessageBox(CCommon::LoadText(IDS_INVALID_DIR_WARNING), MB_ICONWARNING | MB_OK);
			goto browse;
		}
		m_path = szPath;
		return IDOK;
	}
	return IDCANCEL;
}
