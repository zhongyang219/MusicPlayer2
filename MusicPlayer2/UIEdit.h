#pragma once
#include "EditEx.h"

// CUIEdit

namespace UiElement
{
	class EditControl;
}

class CUIEdit : public CEditEx
{
	DECLARE_DYNAMIC(CUIEdit)

public:
	CUIEdit(UiElement::EditControl* ui_edit);
	virtual ~CUIEdit();

	friend class UiElement::EditControl;

protected:
	DECLARE_MESSAGE_MAP()

private:
	UiElement::EditControl* m_ui_edit{};
public:
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	afx_msg void OnEnChange();
};


