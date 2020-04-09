#pragma once
class CFolderBrowserDlg
{
public:
	CFolderBrowserDlg(HWND hParent);
	~CFolderBrowserDlg();

	CString GetPathName() { return m_path; }
	void SetInfo(const CString& info) { m_info = info; }
	int DoModal();

protected:
	HWND m_hParent;
	CString m_path;
	CString m_info;
};

