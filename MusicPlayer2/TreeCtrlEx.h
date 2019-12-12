#pragma once


// CTreeCtrlEx

class CTreeCtrlEx : public CTreeCtrl
{
	DECLARE_DYNAMIC(CTreeCtrlEx)

public:
	CTreeCtrlEx();
	virtual ~CTreeCtrlEx();

public:
    void InsertPath(CString path, HTREEITEM hRoot);
    CString GetItemPath(HTREEITEM hItem);

private:
    void _InsertPath(CString path, HTREEITEM hRoot);

protected:
	DECLARE_MESSAGE_MAP()
};


