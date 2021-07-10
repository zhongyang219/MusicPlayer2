// SelectItemDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "EditStringListDlg.h"
#include "afxdialogex.h"


// CEditStringListDlg 对话框

#define IDC_ITEM_EDITBOX 1991

IMPLEMENT_DYNAMIC(CEditStringListDlg, CBaseDialog)

CEditStringListDlg::CEditStringListDlg(vector<wstring>& items, CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_SELECT_ITEM_DIALOG, pParent), m_items(items)
{

}

CEditStringListDlg::~CEditStringListDlg()
{
}

void CEditStringListDlg::SetTitle(LPCTSTR title)
{
    m_title = title;
}

void CEditStringListDlg::SetDlgIcon(HICON icon)
{
    m_icon = icon;
}


void CEditStringListDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
}


CString CEditStringListDlg::GetDialogName() const
{
    return _T("SelectItemDlg");
}

void CEditStringListDlg::Edit(int row)
{
    CString last_row_text = m_list_ctrl.GetItemText(m_list_ctrl.GetItemCount() - 1, 0);
    if (row < 0)    //如果编辑的行超出了范围
    {
        if (!last_row_text.IsEmpty())       //最后一行不为空，则直接添加一个新行
            m_list_ctrl.AddString(_T(""));
        row = m_list_ctrl.GetItemCount() - 1;
    }
    m_list_ctrl.EnsureVisible(row, FALSE);				//编辑一行时确保该行可见
    m_editing = true;

    m_edit_row = row;
    CRect item_rect;
    m_list_ctrl.GetSubItemRect(row, 0, LVIR_LABEL, item_rect);	//取得子项的矩形

    CRect list_ctrl_rect;       //列表控件在窗口中的位置
    m_list_ctrl.GetWindowRect(list_ctrl_rect);
    ScreenToClient(list_ctrl_rect);

    item_rect.OffsetRect(list_ctrl_rect.left, list_ctrl_rect.top);

    CString text = m_list_ctrl.GetItemText(row, 0);		//取得子项的内容

    m_item_edit.SetWindowText(text);		//将子项的内容显示到编辑框中
    m_item_edit.ShowWindow(SW_SHOW);		//显示编辑框
    m_item_edit.MoveWindow(item_rect);		//将编辑框移动到子项上面，覆盖在子项上
    m_item_edit.SetFocus();					//使编辑框取得焦点
    m_item_edit.SetSel(0, -1);
}

void CEditStringListDlg::EndEdit()
{
    if (m_editing)
    {
        if (m_edit_row >= 0 && m_edit_row < m_list_ctrl.GetItemCount())
        {
            CString str;
            m_item_edit.GetWindowText(str);	//取得编辑框的内容
            m_list_ctrl.SetItemText(m_edit_row, 0, str);	//将该内容更新到CListCtrl中
        }
        m_item_edit.ShowWindow(SW_HIDE);//隐藏编辑框
        m_editing = false;
    }
}

BEGIN_MESSAGE_MAP(CEditStringListDlg, CBaseDialog)
    ON_EN_KILLFOCUS(IDC_ITEM_EDITBOX, &CEditStringListDlg::OnEnKillfocusEdit1)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CEditStringListDlg::OnNMDblclkList1)
    ON_NOTIFY(LVN_BEGINSCROLL, IDC_LIST1, &CEditStringListDlg::OnLvnBeginScrollList1)
END_MESSAGE_MAP()


// CEditStringListDlg 消息处理程序


BOOL CEditStringListDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    SetWindowText(m_title);
    if (m_icon == NULL)
        SetIcon(theApp.m_icon_set.app.GetIcon(), FALSE);
    else
        SetIcon(m_icon, FALSE);

    for (const auto& str : m_items)
        m_list_ctrl.AddString(str.c_str());

    m_item_edit.Create(WS_BORDER | ES_AUTOHSCROLL, CRect(), this, IDC_ITEM_EDITBOX);
    m_item_edit.SetFont(GetFont());


    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}

void CEditStringListDlg::OnEnKillfocusEdit1()
{
    //当文本编辑控件控件失去焦点时响应
    EndEdit();
}


void CEditStringListDlg::OnNMDblclkList1(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    //双击时编辑
    Edit(pNMItemActivate->iItem);
    *pResult = 0;
}


void CEditStringListDlg::OnLvnBeginScrollList1(NMHDR* pNMHDR, LRESULT* pResult)
{
    // 此功能要求 Internet Explorer 5.5 或更高版本。
    // 符号 _WIN32_IE 必须是 >= 0x0560。
    LPNMLVSCROLL pStateChanged = reinterpret_cast<LPNMLVSCROLL>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    EndEdit();
    *pResult = 0;
}


void CEditStringListDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    m_items.clear();
    int item_count = m_list_ctrl.GetItemCount();
    for (int i = 0; i < item_count; i++)
    {
        m_items.push_back(m_list_ctrl.GetItemText(i, 0).GetString());
    }

    CBaseDialog::OnOK();
}
