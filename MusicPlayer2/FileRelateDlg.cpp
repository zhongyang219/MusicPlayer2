// FileRelateDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "FileRelateDlg.h"
#include "afxdialogex.h"
#include "AudioCommon.h"
#include "RegFileRelate.h"
#include <set>
#include "Playlist.h"


// CFileRelateDlg 对话框

IMPLEMENT_DYNAMIC(CFileRelateDlg, CBaseDialog)

CFileRelateDlg::CFileRelateDlg(CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_FILE_RELATE_DIALOG, pParent)
{

}

CFileRelateDlg::~CFileRelateDlg()
{
}

CString CFileRelateDlg::GetDialogName() const
{
    return _T("FileRelateDlg");
}

void CFileRelateDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
}


void CFileRelateDlg::RefreshList()
{
    m_list_ctrl.DeleteAllItems();
    int index = 0;
    bool checked{ false }, unchecked{ false };
    CRegFileRelate reg_file;
    std::set<wstring> extensions;
    for (const auto& item : CAudioCommon::m_all_surpported_extensions)
        extensions.insert(item);
    std::vector<wstring> related_extensions;
    reg_file.GetAllRelatedExtensions(related_extensions);
    for (auto item : related_extensions)
    {
        if (!item.empty() && item[0] == L'.')
            item = item.substr(1);
        extensions.insert(item);
    }
    for (auto item : CPlaylistFile::m_surpported_playlist)
    {
        if (!item.empty() && item[0] == L'.')
            item = item.substr(1);
        extensions.insert(item);
    }

    for (const auto& item : extensions)
    {
        //if(item == L"cue")
        //    continue;
        m_list_ctrl.InsertItem(index, item.c_str());
        wstring description = CAudioCommon::GetAudioDescriptionByExtension(item);
        m_list_ctrl.SetItemText(index, 1, description.c_str());
        bool related = reg_file.IsFileTypeRelated(item.c_str());
        m_list_ctrl.SetCheck(index, related);
        if (related)
            checked = true;
        else
            unchecked = true;

        index++;
    }

    CButton* pBtn = (CButton*)GetDlgItem(IDC_SELECT_ALL_CHECK);
    if (pBtn != nullptr)
    {
        if (checked && !unchecked)
            pBtn->SetCheck(BST_CHECKED);
        else if (checked && unchecked)
            pBtn->SetCheck(BST_INDETERMINATE);
        else
            pBtn->SetCheck(BST_UNCHECKED);
    }
}

BEGIN_MESSAGE_MAP(CFileRelateDlg, CBaseDialog)
    ON_BN_CLICKED(IDC_SELECT_ALL_CHECK, &CFileRelateDlg::OnBnClickedSelectAllCheck)
    ON_BN_CLICKED(IDC_DEFAULT_BUTTON, &CFileRelateDlg::OnBnClickedDefaultButton)
END_MESSAGE_MAP()


// CFileRelateDlg 消息处理程序


BOOL CFileRelateDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    SetIcon(theApp.m_icon_set.file_relate, FALSE);
    
    CRect rect;
    m_list_ctrl.GetWindowRect(rect);

    m_list_ctrl.SetExtendedStyle(m_list_ctrl.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
    int width0 = theApp.DPI(120);
    int width1 = rect.Width() - width0 - theApp.DPI(20) - 1;
    m_list_ctrl.InsertColumn(0, CCommon::LoadText(IDS_FORMAT), LVCFMT_LEFT, width0);
    m_list_ctrl.InsertColumn(1,CCommon::LoadText(IDS_DESCRIPTION), LVCFMT_LEFT, width1);

    RefreshList();

    m_list_ctrl.SetRowHeight(theApp.DPI(22));
    m_list_ctrl.FillLeftSpaceAfterPaint(false);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CFileRelateDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    int list_count = m_list_ctrl.GetItemCount();
    for (int i = 0; i < list_count; i++)
    {
        bool checked = m_list_ctrl.GetCheck(i) != FALSE;
        CRegFileRelate reg_file;
        if (checked)
        {
            CString file_ext = m_list_ctrl.GetItemText(i, 0);
            wstring description = CAudioCommon::GetAudioDescriptionByExtension(wstring(file_ext));

            if (CPlaylistFile::IsPlaylistExt(wstring(file_ext)))
                reg_file.AddFileTypeRelate(file_ext, 66, false, description.c_str());
            else
                reg_file.AddFileTypeRelate(file_ext, 46, false, description.c_str());
        }
        else
        {
            reg_file.DeleteFileTypeRelate(m_list_ctrl.GetItemText(i, 0));
        }
    }

    CBaseDialog::OnOK();
}


void CFileRelateDlg::OnBnClickedSelectAllCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    CButton* pBtn = (CButton*)GetDlgItem(IDC_SELECT_ALL_CHECK);
    if (pBtn == nullptr)
        return;
    BOOL checked{ pBtn->GetCheck() };
    int list_count{ m_list_ctrl.GetItemCount() };
    for (int i = 0; i < list_count; i++)
        m_list_ctrl.SetCheck(i, checked);
}


void CFileRelateDlg::OnBnClickedDefaultButton()
{
    // TODO: 在此添加控件通知处理程序代码
    std::set<wstring> default_selected;
    for (auto item : CAudioCommon::m_all_surpported_extensions)
    {
        if (!item.empty() && item[0] == L'.')
            item = item.substr(1);
        if (item != L"mp1" && item != L"mp2" && item != L"mp4")
            default_selected.insert(item);
    }
    int list_count{ m_list_ctrl.GetItemCount() };
    for (int i = 0; i < list_count; i++)
    {
        wstring item_str = m_list_ctrl.GetItemText(i, 0).GetString();
        auto iter = default_selected.find(item_str);
        if (iter != default_selected.end())
            m_list_ctrl.SetCheck(i, TRUE);
        else
            m_list_ctrl.SetCheck(i, FALSE);
    }

}
