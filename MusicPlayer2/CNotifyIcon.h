#pragma once
class CNotifyIcon
{
public:
	CNotifyIcon();
	~CNotifyIcon();
	void Init(HICON hIcon);
    void SetIcon(HICON hIcon);

	void AddNotifyIcon();
	void DeleteNotifyIcon();
	void SetIconToolTip(LPCTSTR strTip);

	void OnNotifyIcon(UINT msgId, HWND hMiniMode);

private:
	NOTIFYICONDATA m_ntIcon;
	CString m_tool_tip_str;
};

