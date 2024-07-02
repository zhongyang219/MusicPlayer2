// CBrowseEdit.cpp: 实现文件
//

#include "stdafx.h"
#include "BrowseEdit.h"
#include "MusicPlayer2.h"
#include "DrawCommon.h"
#include "FileNameFormDlg.h"
#include "EditStringListDlg.h"

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
    drawer.Create(drawDoubleBuffer.GetMemDC(), GetFont());
    CRect rc_draw{ rect };
    rc_draw.MoveToXY(0, 0);

    //使用圆角风格时，先填充背景色，再画按钮
    if (theApp.m_app_setting_data.button_round_corners)
    {
        COLORREF back_color;
        bool is_read_only = (GetStyle() & ES_READONLY) != 0;
        if (is_read_only || !IsWindowEnabled())
            back_color = GetSysColor(COLOR_3DFACE);
        else
            back_color = GetSysColor(COLOR_WINDOW);
        drawer.GetDC()->FillSolidRect(rc_draw, back_color);
    }

    COLORREF btn_color;
    if (bIsButtonPressed)
        btn_color = m_theme_color.light1_5;
    else if (bIsButtonHot)
        btn_color = m_theme_color.light2_5;
    else
        btn_color = CColorConvert::m_gray_color.light3;
    if (theApp.m_app_setting_data.button_round_corners)
    {
        CRect rc_btn{ rc_draw };
        rc_btn.DeflateRect(theApp.DPI(1), theApp.DPI(1));
        drawer.DrawRoundRect(rc_btn, btn_color, theApp.DPI(3));
    }
    else
    {
        drawer.GetDC()->FillSolidRect(rc_draw, btn_color);
    }

    IconMgr::IconType icon_type = IconMgr::IconType::IT_Folder;
    if (m_Mode == BrowseMode_Default)
        icon_type = IconMgr::IconType::IT_Edit;

    HICON hIcon = theApp.m_icon_mgr.GetHICON(icon_type, IconMgr::IconStyle::IS_OutlinedDark, IconMgr::IconSize::IS_DPI_16);
    CSize icon_size = theApp.m_icon_mgr.GetIconSize(IconMgr::IconSize::IS_DPI_16);
    CPoint icon_top_left;
    icon_top_left.x = rc_draw.left + theApp.DPI(4);
    icon_top_left.y = rc_draw.top + (rc_draw.Height() - icon_size.cy) / 2;
    drawer.DrawIcon(hIcon, icon_top_left, icon_size);

    CRect rc_text = rc_draw;
    rc_text.left += theApp.DPI(20);
    COLORREF text_color = CColorConvert::m_gray_color.dark4;
    if (!IsWindowEnabled())
        text_color = CColorConvert::m_gray_color.dark1;
    drawer.DrawWindowText(rc_text, m_btn_str.c_str(), text_color, Alignment::CENTER, true);
}


void CBrowseEdit::OnChangeLayout()
{
    ASSERT_VALID(this);
    ENSURE(GetSafeHwnd() != NULL);

    int btn_width;
    if (m_Mode == BrowseMode_Default)
        m_btn_str = theApp.m_str_table.LoadText(L"TXT_BROWSE_EDIT_EDIT");
    else
        m_btn_str = theApp.m_str_table.LoadText(L"TXT_BROWSE_EDIT_BROWSE");

    //如果编辑框的宽度小于一定值，则不显示按钮后面的文本
    CRect rect_client;
    GetClientRect(rect_client);
    if (rect_client.Width() < theApp.DPI(120))
    {
        m_btn_str = L"";
        btn_width = theApp.DPI(24);
    }
    else
    {
        CDC* pDC = GetDC();     // OnChangeLayout仅在设置浏览模式时被调用，不需要保持DC
        CDrawCommon drawer;
        drawer.Create(pDC, GetFont());
        btn_width = drawer.GetTextExtent(m_btn_str.c_str()).cx + theApp.DPI(28);
        ReleaseDC(pDC);
    }
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
    //浏览文件夹
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

    //浏览文件
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

    //自定义浏览模式
    case BrowseMode_Default:
    {
        switch (m_browse_mode)
        {
        case CBrowseEdit::EditBrowseMode::RENAME:
        {
            CString strFile;
            GetWindowText(strFile);
            // 这里无视m_poopup_dlg_title设置的对话框标题，如果有必要再加
            CFileNameFormDlg dlg;
            dlg.SetInitInsertFormular(strFile.GetString());

            if (dlg.DoModal() == IDOK && strFile != dlg.GetFormularSelected().c_str())
            {
                SetWindowText(dlg.GetFormularSelected().c_str());
                SetModify(TRUE);
                OnAfterUpdate();
            }

            if (GetParent() != NULL)
            {
                GetParent()->RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
            }
        }
            break;
        case CBrowseEdit::EditBrowseMode::LIST:
        {
            //列表模式下，将编辑框的文本以逗号分隔后以列表的形式编辑
            CString strFile;
            GetWindowText(strFile);
            vector<wstring> items;
            CCommon::StringSplit(wstring(strFile), L',', items);

            CEditStringListDlg dlg(items);
            dlg.SetTitle(m_poopup_dlg_title);
            if (dlg.DoModal() == IDOK)
            {
                strFile = CCommon::StringMerge(items, L',').c_str();
                SetWindowText(strFile);
                SetModify(TRUE);
                OnAfterUpdate();
            }

            if (GetParent() != NULL)
            {
                GetParent()->RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
            }
        }
            break;
        case CBrowseEdit::EditBrowseMode::LIST2:
        {
            // 列表模式2 下，将编辑框的文本用""包裹后以逗号分隔后以列表的形式编辑
            CString strFile;
            GetWindowText(strFile);
            vector<wstring> items;
            CCommon::SplitStringList(items, wstring(strFile));

            CEditStringListDlg dlg(items);
            dlg.SetTitle(m_poopup_dlg_title);
            if (dlg.DoModal() == IDOK)
            {
                strFile = CCommon::MergeStringList(items).c_str();
                SetWindowText(strFile);
                SetModify(TRUE);
                OnAfterUpdate();
            }

            if (GetParent() != NULL)
            {
                GetParent()->RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
            }
        }
            break;
        default:
            ASSERT(false);
            break;

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

void CBrowseEdit::SetEditBrowseMode(EditBrowseMode browse_mode)
{
    m_browse_mode = browse_mode;
    EnableBrowseButton(TRUE);       //将基类的编辑模式改成BrowseMode_Default
}

void CBrowseEdit::SetPopupDlgTitle(const wstring& popup_dlg_title)
{
    m_poopup_dlg_title = popup_dlg_title.c_str();
}

BEGIN_MESSAGE_MAP(CBrowseEdit, CMFCEditBrowseCtrl)
    ON_WM_NCLBUTTONDOWN()
    ON_MESSAGE(WM_TABLET_QUERYSYSTEMGESTURESTATUS, &CBrowseEdit::OnTabletQuerysystemgesturestatus)
END_MESSAGE_MAP()


void CBrowseEdit::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
    if (HTCAPTION == nHitTest)
    {
        return;
    }
    CMFCEditBrowseCtrl::OnNcLButtonDown(nHitTest, point);
}


afx_msg LRESULT CBrowseEdit::OnTabletQuerysystemgesturestatus(WPARAM wParam, LPARAM lParam)
{
    return 0;
}
