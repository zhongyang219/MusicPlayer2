// AlbumCoverInfoDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "AlbumCoverInfoDlg.h"


// CAlbumCoverInfoDlg 对话框

IMPLEMENT_DYNAMIC(CAlbumCoverInfoDlg, CBaseDialog)

CAlbumCoverInfoDlg::CAlbumCoverInfoDlg(CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_ADD_TO_PLAYLIST_DIALOG, pParent)
{

}

CAlbumCoverInfoDlg::~CAlbumCoverInfoDlg()
{
}

CString CAlbumCoverInfoDlg::GetDialogName() const
{
    return _T("AlbumCoverInfoDlg");
}

void CAlbumCoverInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
}


BEGIN_MESSAGE_MAP(CAlbumCoverInfoDlg, CBaseDialog)
END_MESSAGE_MAP()


// CAlbumCoverInfoDlg 消息处理程序


BOOL CAlbumCoverInfoDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    SetWindowText(CCommon::LoadText(IDS_ALBUM_COVER_INFO));
    SetIcon(theApp.m_icon_set.album_cover, FALSE);

    CWnd* pWnd = GetDlgItem(IDOK);
    if (pWnd != nullptr)
        pWnd->ShowWindow(SW_HIDE);

    //初始化列表
    CRect rect;
    m_list_ctrl.GetWindowRect(rect);
    m_list_ctrl.SetExtendedStyle(m_list_ctrl.GetExtendedStyle() | LVS_EX_GRIDLINES);
    int width0 = theApp.DPI(85);
    int width1 = rect.Width() - width0 - theApp.DPI(20) - 1;
    m_list_ctrl.InsertColumn(0, CCommon::LoadText(IDS_ITEM), LVCFMT_LEFT, width0);
    m_list_ctrl.InsertColumn(1, CCommon::LoadText(IDS_VLAUE), LVCFMT_LEFT, width1);

    //添加数据
    //封面路径
    m_list_ctrl.InsertItem(0, CCommon::LoadText(IDS_PATH));
    if (CPlayer::GetInstance().AlbumCoverExist())
        m_list_ctrl.SetItemText(0, 1, CPlayer::GetInstance().IsInnerCover() ? CCommon::LoadText(_T("<"), IDS_INNER_ALBUM_COVER, L">").GetString() : CPlayer::GetInstance().GetAlbumCoverPath().c_str());
    //封面类型
    m_list_ctrl.InsertItem(1, CCommon::LoadText(IDS_FORMAT));
    if (CPlayer::GetInstance().AlbumCoverExist())
    {
        wstring cover_type;
        if (CPlayer::GetInstance().IsInnerCover())
        {
            switch (CPlayer::GetInstance().GetAlbumCoverType())
            {
            case 0:
                cover_type = L"jpg";
                break;
            case 1:
                cover_type = L"png";
                break;
            case 2:
                cover_type = L"gif";
                break;
            case 3:
                cover_type = L"bmp";
                break;
            default:
                break;
            }
        }
        else
        {
            cover_type = CFilePathHelper(CPlayer::GetInstance().GetAlbumCoverPath()).GetFileExtension();
        }
        m_list_ctrl.SetItemText(1, 1, cover_type.c_str());
    }

    AlbumCoverInfo album_cover_info{ CPlayer::GetInstance().GetAlbumCoverInfo() };
    //宽度
    m_list_ctrl.InsertItem(2, CCommon::LoadText(IDS_WIDTH));
    if(CPlayer::GetInstance().AlbumCoverExist())
        m_list_ctrl.SetItemText(2, 1, std::to_wstring(album_cover_info.width).c_str());

    //高度
    m_list_ctrl.InsertItem(3, CCommon::LoadText(IDS_HEIGHT));
    if (CPlayer::GetInstance().AlbumCoverExist())
        m_list_ctrl.SetItemText(3, 1, std::to_wstring(album_cover_info.height).c_str());

    //每像素位数
    m_list_ctrl.InsertItem(4, CCommon::LoadText(IDS_BPP));
    if (CPlayer::GetInstance().AlbumCoverExist())
        m_list_ctrl.SetItemText(4, 1, std::to_wstring(album_cover_info.bpp).c_str());

    //文件大小
    m_list_ctrl.InsertItem(5, CCommon::LoadText(IDS_FILE_SIZE));
    if (CPlayer::GetInstance().AlbumCoverExist())
    {
        size_t file_size = CCommon::GetFileSize(CPlayer::GetInstance().GetAlbumCoverPath());
        m_list_ctrl.SetItemText(5, 1, CCommon::DataSizeToString(file_size));
    }

    //已压缩尺寸过大的专辑封面
    m_list_ctrl.InsertItem(6, CCommon::LoadText(IDS_ALBUM_COVER_COMPRESSED));
    if (CPlayer::GetInstance().AlbumCoverExist())
    {
        m_list_ctrl.SetItemText(6, 1, (album_cover_info.size_exceed ? CCommon::LoadText(IDS_YES) : CCommon::LoadText(IDS_NO)));
    }

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}
