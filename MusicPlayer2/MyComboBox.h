#pragma once
class CMyComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CMyComboBox)

public:
	CMyComboBox();
	virtual ~CMyComboBox();

	void SetReadOnly(bool read_only = true);
	void SetEditReadOnly(bool read_only = true);
    CEdit* GetEditCtrl();

protected:
	bool m_read_only{ false };
	CRect m_arrow_rect;		//下拉箭头的矩形区域

public:
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

