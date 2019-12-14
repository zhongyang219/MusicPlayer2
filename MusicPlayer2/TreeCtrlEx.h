#pragma once
#include <functional>


// CTreeCtrlEx

class CTreeCtrlEx : public CTreeCtrl
{
	DECLARE_DYNAMIC(CTreeCtrlEx)

public:
	CTreeCtrlEx();
	virtual ~CTreeCtrlEx();

public:
    void InsertPath(CString path, HTREEITEM hRoot);

    //向树中插入一个文件夹结构
    //path: 文件夹的根目录
    //hRoot: 要插入的根目录在树控件中的位置
    //is_path_show: 一个函数对象，用于判断一个文件夹是否需要显示
    void InsertPath(CString path, HTREEITEM hRoot, std::function<bool(const CString&)> is_path_show);
    CString GetItemPath(HTREEITEM hItem);

private:
    void _InsertPath(CString path, HTREEITEM hRoot, std::function<bool(const CString&)> is_path_show = [](const CString&) {return true; });

protected:
	DECLARE_MESSAGE_MAP()
};


