// CSelectPlaylist.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "CSelectPlaylist.h"
#include "afxdialogex.h"


// CSelectPlaylist 对话框

IMPLEMENT_DYNAMIC(CSelectPlaylist, CTabDlg)

CSelectPlaylist::CSelectPlaylist(CWnd* pParent /*=nullptr*/)
	: CTabDlg(IDD_SELECT_PLAYLIST_DIALOG, pParent)
{

}

CSelectPlaylist::~CSelectPlaylist()
{
}

void CSelectPlaylist::DoDataExchange(CDataExchange* pDX)
{
	CTabDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSelectPlaylist, CTabDlg)
END_MESSAGE_MAP()


// CSelectPlaylist 消息处理程序
