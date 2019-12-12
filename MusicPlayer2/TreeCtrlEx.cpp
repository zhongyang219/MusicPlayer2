// TreeCtrlEx.cpp: 实现文件
//

#include "stdafx.h"
#include "TreeCtrlEx.h"


// CTreeCtrlEx

IMPLEMENT_DYNAMIC(CTreeCtrlEx, CTreeCtrl)

CTreeCtrlEx::CTreeCtrlEx()
{

}

CTreeCtrlEx::~CTreeCtrlEx()
{
}

void CTreeCtrlEx::InsertPath(CString path, HTREEITEM hRoot)
{
    HTREEITEM hRootItem = InsertItem(path, hRoot);
    _InsertPath(path, hRootItem);
}

CString CTreeCtrlEx::GetItemPath(HTREEITEM hItem)
{
    CString strPath;
    strPath = GetItemText(hItem);
    while (hItem != NULL)
    {
        hItem = GetNextItem(hItem, TVGN_PARENT);
        CString strParent = GetItemText(hItem);
        if(!strParent.IsEmpty())
            strPath = strParent + _T("\\") + strPath;
    }
    strPath += _T('\\');
    return strPath;
}

void CTreeCtrlEx::_InsertPath(CString path, HTREEITEM hRoot)
{
    CFileFind nFindFile;
    CString str = L"";
    CString nPicFileName = L"";
    BOOL IsExist = FALSE;
    HTREEITEM hSubItem;
    nPicFileName.Format(L"%s\\*.*", path);
    IsExist = nFindFile.FindFile(nPicFileName);
    while (IsExist)
    {
        IsExist = nFindFile.FindNextFile();
        if (nFindFile.IsDots())
            continue;
        nPicFileName = nFindFile.GetFileName();
        //路径
        if (nFindFile.IsDirectory())
        {
            hSubItem = InsertItem(nPicFileName, hRoot);
            _InsertPath(nFindFile.GetFilePath(), hSubItem);
        }
        //else
        //{
        //    //文件
        //    str = nPicFileName.Right(4);
        //    if (!str.CompareNoCase(_T(".jpg")) || !str.CompareNoCase(_T(".tif")))
        //    {
        //        InsertItem(nPicFileName, hRoot);
        //    }
        //}
    }
    nFindFile.Close();
}


BEGIN_MESSAGE_MAP(CTreeCtrlEx, CTreeCtrl)
END_MESSAGE_MAP()


