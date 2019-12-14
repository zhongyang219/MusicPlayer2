// TreeCtrlEx.cpp: 实现文件
//

#include "stdafx.h"
#include "TreeCtrlEx.h"
#include "MusicPlayer2.h"


// CTreeCtrlEx

std::map<CString, bool> CTreeCtrlEx::m_expand_state;

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

void CTreeCtrlEx::InsertPath(CString path, HTREEITEM hRoot, std::function<bool(const CString&)> is_path_show)
{
    HTREEITEM hRootItem = InsertItem(path, hRoot);
    _InsertPath(path, hRootItem, is_path_show);
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

bool CTreeCtrlEx::IsItemExpand(HTREEITEM hItem)
{
    return (GetItemState(hItem, TVIS_EXPANDED)&TVIS_EXPANDED) == TVIS_EXPANDED;
}

void CTreeCtrlEx::ExpandAll(HTREEITEM hItem)
{
    IterateItems(hItem, [&](HTREEITEM hChild)
    {
        Expand(hChild, TVE_EXPAND);
    });
}

void CTreeCtrlEx::ExpandAll()
{
    ExpandAll(NULL);
}

void CTreeCtrlEx::IterateItems(HTREEITEM hRoot, std::function<void(HTREEITEM)> func)
{
    HTREEITEM hChild, hNext;
    if(hRoot!=NULL)
        func(hRoot);

    hChild = GetNextItem(hRoot, TVGN_CHILD);
    if (hChild)
    {
        //遍历子节点  
        IterateItems(hChild, func);
        hNext = hChild;
        do
        {
            hNext = GetNextItem(hNext, TVGN_NEXT);
            if (hNext != NULL)
                IterateItems(hNext, func);
        } while (hNext != NULL);
    }
}

void CTreeCtrlEx::SaveExpandState()
{
    IterateItems(NULL, [&](HTREEITEM hItem)
    {
        bool expand = IsItemExpand(hItem);
        m_expand_state[GetItemPath(hItem)] = expand;
    });
}

void CTreeCtrlEx::SaveItemExpandState(HTREEITEM hItem, bool expand)
{
    CString item_path = GetItemPath(hItem);
    m_expand_state[item_path] = expand;
}

void CTreeCtrlEx::RestoreExpandState()
{
    IterateItems(NULL, [&](HTREEITEM hItem)
    {
        CString item_path = GetItemPath(hItem);
        auto iter = m_expand_state.find(item_path);
        if(iter != m_expand_state.end())
        {
            bool expand = iter->second;
            Expand(hItem, expand ? TVE_EXPAND : TVE_COLLAPSE);
        }
    });

}

void CTreeCtrlEx::_InsertPath(CString path, HTREEITEM hRoot, std::function<bool(const CString&)> is_path_show)
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
        if (nFindFile.IsDirectory() && is_path_show(nFindFile.GetFilePath()))
        {
            hSubItem = InsertItem(nPicFileName, hRoot);
            _InsertPath(nFindFile.GetFilePath(), hSubItem, is_path_show);
        }
    }
    nFindFile.Close();
}


BEGIN_MESSAGE_MAP(CTreeCtrlEx, CTreeCtrl)
END_MESSAGE_MAP()




void CTreeCtrlEx::PreSubclassWindow()
{
    // TODO: 在此添加专用代码和/或调用基类

    //设置图标
    CImageList image_list;
    image_list.Create(theApp.DPI(16), theApp.DPI(16), ILC_COLOR32 | ILC_MASK, 2, 2);
    image_list.Add(ExtractIcon(AfxGetApp()->m_hInstance, L"shell32.dll", 3));
    SetImageList(&image_list, TVSIL_NORMAL);
    image_list.Detach();

    CTreeCtrl::PreSubclassWindow();
}
