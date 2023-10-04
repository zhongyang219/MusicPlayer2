#pragma once
#include "CListBoxEnhanced.h"

class CEditableListBox :
    public CListBoxEnhanced
{
    DECLARE_DYNAMIC(CEditableListBox)
public:
    CEditableListBox();
    ~CEditableListBox();

    void Edit(int row);			//编辑指定行

protected:
    CEdit m_item_edit;
    int m_edit_row{};
    bool m_editing{};

protected:
    void EndEdit();

    DECLARE_MESSAGE_MAP()
    virtual void PreSubclassWindow();
    afx_msg void OnEnKillfocusEdit1();

public:
    afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnBeginScroll(NMHDR *pNMHDR, LRESULT *pResult);
};

