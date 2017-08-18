// ListCtrlEx.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "PlayListCtrl.h"


// CPlayListCtrl

IMPLEMENT_DYNAMIC(CPlayListCtrl, CListCtrl)

//通过构造函数参数传递列表中所有文件的信息的引用
CPlayListCtrl::CPlayListCtrl(const vector<SongInfo>& all_song_info) :m_all_song_info{ all_song_info }
{
	m_toolTip.Create(this, TTS_ALWAYSTIP | TTS_NOPREFIX);
}

CPlayListCtrl::~CPlayListCtrl()
{
}

void CPlayListCtrl::SetColor(COLORREF TextColor, COLORREF TextBkColor, COLORREF selected_color, COLORREF other_text_color)
{
	m_text_color = TextColor;
	m_back_color = TextBkColor;
	m_selected_color = selected_color;
	m_other_text_color = other_text_color;
	DWORD itemCount = GetItemCount();
	this->RedrawItems(0, itemCount - 1);
}

wstring CPlayListCtrl::GetDisplayStr(const SongInfo & song_info, DisplayFormat display_format)
{
	switch (display_format)
	{
	case DF_FILE_NAME:		//显示为文件名
		return song_info.file_name;
	case DF_TITLE:			//显示为歌曲标题
		if (song_info.title == DEFAULT_TITLE)	//如果获取不到歌曲标题，就显示文件名
			return song_info.file_name;
		else
			return song_info.title;
	case DF_ARTIST_TITLE:	//显示为艺术家 - 标题
		if (song_info.title == DEFAULT_TITLE && song_info.artist == DEFAULT_ARTIST)		//如果标题和艺术家都获取不到，就显示文件名
			return song_info.file_name;
		else
			return (song_info.artist + _T(" - ") + song_info.title);
	case DF_TITLE_ARTIST:	//显示为标题 - 艺术家
		if (song_info.title == DEFAULT_TITLE && song_info.artist == DEFAULT_ARTIST)		//如果标题和艺术家都获取不到，就显示文件名
			return song_info.file_name;
		else
			return (song_info.title + _T(" - ") + song_info.artist);
	default:
		return song_info.file_name;
	}
}

void CPlayListCtrl::ShowPlaylist(DisplayFormat display_format)
{
	int item_num_before = GetItemCount();
	int item_num_after = theApp.m_player.GetSongNum();
	//如果当前列表中项目的数量小于原来的，则直接清空原来列表中所有的项目，重新添加
	if (item_num_after < item_num_before)
	{
		DeleteAllItems();
		item_num_before = 0;
	}
	CString str;
	for (int i{}; i < item_num_after; i++)
	{
		if (i >= item_num_before)	//如果当前列表中的项目数量大于之前的数量，则需要在不够时插入新的项目
		{
			str.Format(_T("%u"), i + 1);
			InsertItem(i, str);
		}
		SetItemText(i, 1, (GetDisplayStr(m_all_song_info[i], display_format)).c_str());

		SetItemText(i, 2, theApp.m_player.GetAllSongLength(i).time2str().c_str());
	}
}


BEGIN_MESSAGE_MAP(CPlayListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, CPlayListCtrl::OnNMCustomdraw)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()


// CPlayListCtrl 消息处理程序

void CPlayListCtrl::OnNMCustomdraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = CDRF_DODEFAULT;
	LPNMLVCUSTOMDRAW lplvdr = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	NMCUSTOMDRAW& nmcd = lplvdr->nmcd;
	switch (lplvdr->nmcd.dwDrawStage)	//判断状态   
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT:			//如果为画ITEM之前就要进行颜色的改变
		//当选中行又是高亮行时
		if (GetItemState(nmcd.dwItemSpec, LVIS_SELECTED) == LVIS_SELECTED && nmcd.dwItemSpec == m_hight_item)
		{
			SetItemState(nmcd.dwItemSpec, 0, LVIS_SELECTED);
			lplvdr->clrText = m_back_color;
			lplvdr->clrTextBk = m_selected_color;
		}
		//选中行颜色改变
		else if (GetItemState(nmcd.dwItemSpec, LVIS_SELECTED) == LVIS_SELECTED/*pLVCD->nmcd.uItemState & CDIS_SELECTED*/)
		{
			SetItemState(nmcd.dwItemSpec, 0, LVIS_SELECTED);
			lplvdr->clrText = RGB(255,255,255);
			lplvdr->clrTextBk = m_other_text_color;
		}
		//else if (MapItemColor.Lookup(nmcd.dwItemSpec, tb))
			// 根据在 SetItemColor(DWORD iItem, COLORREF color) 设置的   
			// ITEM号和COLORREF 在摸板中查找，然后进行颜色赋值。
		else if(nmcd.dwItemSpec==m_hight_item)
		{
			lplvdr->clrText = m_text_color;
			lplvdr->clrTextBk = m_back_color;
		}
		else
		{
			lplvdr->clrText = GetSysColor(COLOR_WINDOWTEXT);
			lplvdr->clrTextBk = m_background_color;
		}
		*pResult = CDRF_DODEFAULT;
		break;
	}
}




void CPlayListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//如果开启文本提示
	if (m_bEnableTips)
	{
		CString str_tip;
		LVHITTESTINFO lvhti;

		// 判断鼠标当前所在的位置(行, 列)
		lvhti.pt = point;
		SubItemHitTest(&lvhti);

		//如果鼠标移动到另一行, 则进行处理; 否则, 不做处理
		if (lvhti.iItem != m_nItem)
		{
			// 保存当前鼠标所在的行
			m_nItem = lvhti.iItem;
			if (m_nItem >= m_all_song_info.size()) return;

			// 如果鼠标移动到一个合法的行，则显示新的提示信息，否则不显示提示
			if (m_nItem != -1)
			{
				CString dis_str = GetItemText(m_nItem, 1);
				int strWidth = GetStringWidth(dis_str) + DPI(10);		//获取要显示当前字符串的最小宽度
				int columnWidth = GetColumnWidth(1);	//获取鼠标指向列的宽度
				if (columnWidth < strWidth)		//当单元格内的的字符无法显示完全时在提示的第1行显示单元格内文本
				{
					str_tip += dis_str;
					str_tip += _T("\r\n");
				}

				str_tip += _T("文件名：");
				str_tip += m_all_song_info[m_nItem].file_name.c_str();
				str_tip += _T("\r\n");

				str_tip += _T("标题：");
				str_tip += m_all_song_info[m_nItem].title.c_str();
				str_tip += _T("\r\n");

				str_tip += _T("艺术家：");
				str_tip += m_all_song_info[m_nItem].artist.c_str();
				str_tip += _T("\r\n");

				str_tip += _T("唱片集：");
				str_tip += m_all_song_info[m_nItem].album.c_str();
				str_tip += _T("\r\n");

				CString str_bitrate;
				str_bitrate.Format(_T("比特率：%dkbps"), m_all_song_info[m_nItem].bitrate);
				str_tip += str_bitrate;

				m_toolTip.SetMaxTipWidth(DPI(400));		//设置提示信息的宽度，以支持提示换行

				m_toolTip.AddTool(this, str_tip);
				m_toolTip.Pop();			// 显示提示框
			}
			else
			{
				m_toolTip.AddTool(this, _T(""));
				m_toolTip.Pop();
			}
		}
	}
	CListCtrl::OnMouseMove(nFlags, point);
}


BOOL CPlayListCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (m_toolTip.GetSafeHwnd())
	{
		m_toolTip.RelayEvent(pMsg);
	}

	return CListCtrl::PreTranslateMessage(pMsg);
}


void CPlayListCtrl::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	//CWindowDC dc(this);
	//HDC hDC = dc.GetSafeHdc();
	//m_dpi = GetDeviceCaps(hDC, LOGPIXELSY);

	//将提示信息设为置顶
	m_toolTip.SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	//初始化播放列表
	SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	int width0, width1, width2;
	width0 = DPI(40);
	width2 = DPI(50);
	CRect rect;
	GetWindowRect(rect);
	width1 = rect.Width() - width0 - width2 - DPI(21);
	InsertColumn(0, _T("序号"), LVCFMT_LEFT, width0);		//插入第1列
	InsertColumn(1, _T("曲目"), LVCFMT_LEFT, width1);		//插入第2列
	InsertColumn(2, _T("长度"), LVCFMT_LEFT, width2);		//插入第3列
	EnableTip();

	this->SetBkColor(m_background_color);

	CListCtrl::PreSubclassWindow();
}


void CPlayListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	this->SetFocus();
	CListCtrl::OnLButtonDown(nFlags, point);
}


void CPlayListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	this->SetFocus();
	CListCtrl::OnRButtonDown(nFlags, point);
}
