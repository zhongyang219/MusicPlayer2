#pragma once
class CNotifyIcon
{
public:
	CNotifyIcon();
	~CNotifyIcon();
	void Init(HICON hIcon);

	void AddNotifyIcon();
	void DeleteNotifyIcon();

	void SetIconToolTip(LPCTSTR strTip);

	LRESULT OnNotifyIcon(WPARAM wParam, LPARAM lParam);
private:
	NOTIFYICONDATA m_ntIcon;
	CString m_tool_tip_str;
	CMenu m_notify_menu;
};

