// CBrowseEdit.cpp: 实现文件
//

#include "stdafx.h"
#include "BrowseEdit.h"
#include "MusicPlayer2.h"
#include "DrawCommon.h"

// CBrowseEdit

IMPLEMENT_DYNAMIC(CBrowseEdit, CMFCEditBrowseCtrl)

CBrowseEdit::CBrowseEdit()
    : m_theme_color(theApp.m_app_setting_data.theme_color)
{
}

CBrowseEdit::~CBrowseEdit()
{
}

void CBrowseEdit::OnDrawBrowseButton(CDC * pDC, CRect rect, BOOL bIsButtonPressed, BOOL bIsButtonHot)
{
    //使用双缓冲绘图
    CDrawDoubleBuffer drawDoubleBuffer(pDC, rect);

    CDrawCommon drawer;
    drawer.Create(drawDoubleBuffer.GetMemDC(), this);
    CRect rc_draw{ rect };
    rc_draw.MoveToXY(0, 0);

    COLORREF back_color;
    if (bIsButtonPressed)
        back_color = m_theme_color.light1_5;
    else if (bIsButtonHot)
        back_color = m_theme_color.light2_5;
    else
        back_color = CColorConvert::m_gray_color.light3;
    drawer.GetDC()->FillSolidRect(rc_draw, back_color);

    auto& icon = theApp.m_icon_set.select_folder;
    CSize icon_size = icon.GetSize();
    CPoint icon_top_left;
    icon_top_left.x = rc_draw.left + theApp.DPI(4);
    icon_top_left.y = rc_draw.top + (rc_draw.Height() - icon_size.cy) / 2;
    drawer.DrawIcon(icon.GetIcon(true), icon_top_left, icon_size);

    CRect rc_text = rc_draw;
    rc_text.left += theApp.DPI(20);
    COLORREF text_color = CColorConvert::m_gray_color.dark4;
    if (!IsWindowEnabled())
        text_color = CColorConvert::m_gray_color.dark1;
    drawer.DrawWindowText(rc_text, m_btn_str, text_color, Alignment::CENTER, true);
}


void CBrowseEdit::OnChangeLayout()
{
    ASSERT_VALID(this);
    ENSURE(GetSafeHwnd() != NULL);

    int btn_width;
    CDrawCommon drawer;
    drawer.Create(m_pDC, this);
    btn_width = drawer.GetTextExtent(m_btn_str).cx + theApp.DPI(28);
    m_nBrowseButtonWidth = max(btn_width, m_sizeImage.cx + 8);

    SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOZORDER | SWP_NOMOVE);

    if (m_Mode != BrowseMode_None)
    {
        GetWindowRect(m_rectBtn);
        m_rectBtn.left = m_rectBtn.right - m_nBrowseButtonWidth;

        ScreenToClient(&m_rectBtn);
    }
    else
    {
        m_rectBtn.SetRectEmpty();
    }

}

void CBrowseEdit::OnBrowse()
{
    ASSERT_VALID(this);
    ENSURE(GetSafeHwnd() != NULL);

    switch (m_Mode)
    {
    case BrowseMode_Folder:
    {
        CString strFolder;
        GetWindowText(strFolder);
        CFolderPickerDialog dlg(strFolder);
        if(!m_strBrowseFolderTitle.IsEmpty())
            dlg.m_ofn.lpstrTitle = m_strBrowseFolderTitle;
        if (dlg.DoModal() == IDOK)
        {
            CString strFolderNew = dlg.GetPathName();
            strFolderNew.AppendChar(_T('\\'));
            if(strFolderNew != strFolder)
            {
                SetWindowText(strFolderNew);
                SetModify(TRUE);
                OnAfterUpdate();
            }
        }
    }
        break;

    case BrowseMode_File:
    {
        CString strFile;
        GetWindowText(strFile);

        if (!strFile.IsEmpty())
        {
            TCHAR fname[_MAX_FNAME];

            _tsplitpath_s(strFile, NULL, 0, NULL, 0, fname, _MAX_FNAME, NULL, 0);

            CString strFileName = fname;
            strFileName.TrimLeft();
            strFileName.TrimRight();

            if (strFileName.IsEmpty())
            {
                strFile.Empty();
            }

            const CString strInvalidChars = _T("*?<>|");
            if (strFile.FindOneOf(strInvalidChars) >= 0)
            {
                strFile.Empty();
            }
        }

        CFileDialog dlg(TRUE, !m_strDefFileExt.IsEmpty() ? (LPCTSTR)m_strDefFileExt : (LPCTSTR)NULL, strFile, m_dwFileDialogFlags, !m_strFileFilter.IsEmpty() ? (LPCTSTR)m_strFileFilter : (LPCTSTR)NULL, NULL);
        if (dlg.DoModal() == IDOK && strFile != dlg.GetPathName())
        {
            SetWindowText(dlg.GetPathName());
            SetModify(TRUE);
            OnAfterUpdate();
        }

        if (GetParent() != NULL)
        {
            GetParent()->RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
        }
    }
    break;
    }

    SetFocus();
}

void CBrowseEdit::OnAfterUpdate()
{
    CWnd* pParent = GetParent();
    if (pParent != nullptr)
        pParent->SendMessage(WM_EDIT_BROWSE_CHANGED, 0, LPARAM(this));
}

BEGIN_MESSAGE_MAP(CBrowseEdit, CMFCEditBrowseCtrl)
    ON_WM_DESTROY()
END_MESSAGE_MAP()




void CBrowseEdit::PreSubclassWindow()
{
    // TODO: 在此添加专用代码和/或调用基类
    m_btn_str = CCommon::LoadText(IDS_BROWSE, _T("..."));
    m_pDC = GetDC();

    CMFCEditBrowseCtrl::PreSubclassWindow();
}


void CBrowseEdit::OnDestroy()
{
    CMFCEditBrowseCtrl::OnDestroy();

    // TODO: 在此处添加消息处理程序代码
    ReleaseDC(m_pDC);
}
