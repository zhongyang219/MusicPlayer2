// ListCtrlEx.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "Player.h"
#include "PlayListCtrl.h"
#include "SongInfoHelper.h"

// CPlayListCtrl

IMPLEMENT_DYNAMIC(CPlayListCtrl, CListCtrlEx)

//通过构造函数参数传递列表中所有文件的信息的引用
CPlayListCtrl::CPlayListCtrl(const vector<SongInfo>& all_song_info) :m_all_song_info{ all_song_info }
{
    m_toolTip.CreateEx(this, TTS_ALWAYSTIP | TTS_NOPREFIX, WS_EX_TRANSPARENT);
}

CPlayListCtrl::~CPlayListCtrl()
{
}

void CPlayListCtrl::ShowPlaylist(DisplayFormat display_format, bool search_result)
{
    m_searched = search_result;
    m_list_data.clear();
    if (m_all_song_info.size() == 1 && m_all_song_info[0].file_path.empty())
    {
        DeleteAllItems();
        return;
    }

    if (!search_result)		//显示所有曲目
    {
        int index{};
        for (const auto& song : m_all_song_info)
        {
            CListCtrlEx::RowData row_data;
            row_data[0] = std::to_wstring(index + 1);
            row_data[1] = CSongInfoHelper::GetDisplayStr(song, display_format);
            row_data[2] = song.length().toString();
            m_list_data.push_back(std::move(row_data));
            index++;
        }
    }
    else		//只显示搜索结果的曲目
    {
        if (m_search_result.empty())
        {
            CListCtrlEx::RowData row_data;
            row_data[0] = wstring();
            row_data[1] = theApp.m_str_table.LoadText(L"TXT_PLAYLIST_CTRL_NO_RESULT_TO_SHOW");
            m_list_data.push_back(std::move(row_data));

        }
        else
        {
            for (int index : m_search_result)
            {
                CListCtrlEx::RowData row_data;
                row_data[0] = std::to_wstring(index + 1);
                row_data[1] = CSongInfoHelper::GetDisplayStr(m_all_song_info[index], display_format);
                row_data[2] = m_all_song_info[index].length().toString();
                m_list_data.push_back(std::move(row_data));
            }
        }
    }
    SetListData(&m_list_data);
}

void CPlayListCtrl::QuickSearch(const wstring & key_word)
{
    m_search_result.clear();
    if (key_word.empty())
        return;
    for (size_t i{}; i < m_all_song_info.size(); i++)
    {
        if (theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(key_word, m_all_song_info[i].GetFileName())
            || theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(key_word, m_all_song_info[i].title)
            || theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(key_word, m_all_song_info[i].artist)
            || theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(key_word, m_all_song_info[i].album))
            m_search_result.push_back(i);
    }
}

void CPlayListCtrl::GetItemSelectedSearched(vector<int>& item_selected)
{
    item_selected.clear();
    POSITION pos = GetFirstSelectedItemPosition();
    if (pos != NULL)
    {
        while (pos)
        {
            int nItem = GetNextSelectedItem(pos);
            CString str;
            str = GetItemText(nItem, 0);
            item_selected.push_back(_ttoi(str) - 1);
        }
    }
}

void CPlayListCtrl::ShowPopupMenu(CMenu* pMenu, int item_index, CWnd* pWnd)
{
    m_toolTip.Pop();
    CListCtrlEx::ShowPopupMenu(pMenu, item_index, pWnd);
}

void CPlayListCtrl::AdjustColumnWidth()
{
    vector<int> width;
    CalculateColumeWidth(width);

    for (size_t i{}; i<width.size(); i++)
        SetColumnWidth(i, width[i]);
}


bool CPlayListCtrl::SetRowHeight(int height)
{
    //调用基类CListCtrlEx::SetRowHeight函数，指定了列表左侧的空白宽度
    return CListCtrlEx::SetRowHeight(height, theApp.DPI(6) - 4);
}

BEGIN_MESSAGE_MAP(CPlayListCtrl, CListCtrlEx)
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, CPlayListCtrl::OnNMCustomdraw)
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONDOWN()
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CListCtrlEx::OnNMCustomdraw)
END_MESSAGE_MAP()


// CPlayListCtrl 消息处理程序

void CPlayListCtrl::CalculateColumeWidth(vector<int>& width)
{
    width.resize(3);

    width[0] = theApp.DPI(40);
    width[2] = theApp.DPI(50);
    CRect rect;
    GetWindowRect(rect);
    width[1] = rect.Width() - width[0] - width[2] - theApp.DPI(20) - 1;
}

void CPlayListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    //如果开启文本提示
    if (theApp.m_media_lib_setting_data.show_playlist_tooltip)
    {
        LVHITTESTINFO lvhti;

        // 判断鼠标当前所在的位置(行, 列)
        lvhti.pt = point;
        SubItemHitTest(&lvhti);

        //如果鼠标移动到另一行, 则进行处理; 否则, 不做处理
        if (lvhti.iItem != m_nItem)
        {
            // 保存当前鼠标所在的行
            m_nItem = lvhti.iItem;

            // 如果鼠标移动到一个合法的行，则显示新的提示信息，否则不显示提示
            if (m_nItem >= 0 && m_nItem < static_cast<int>(m_all_song_info.size()) && !m_dragging)
            {
                int song_index;
                if (!m_searched)
                {
                    song_index = m_nItem;
                }
                else
                {
                    CString str = GetItemText(m_nItem, 0);
                    song_index = _ttoi(str) - 1;
                }
                if (song_index < 0 || song_index >= static_cast<int>(m_all_song_info.size()))
                    return;

                CString dis_str = GetItemText(m_nItem, 1);
                int strWidth = GetStringWidth(dis_str) + theApp.DPI(10);		//获取要显示当前字符串的最小宽度
                int columnWidth = GetColumnWidth(1);	//获取鼠标指向列的宽度
                bool show_title = (columnWidth < strWidth);		//当单元格内的的字符无法显示完全时在提示的第1行显示单元格内文本
                bool show_full_path = (!CPlayer::GetInstance().IsFolderMode() || CPlayer::GetInstance().IsContainSubFolder());
                CString str_tip = CSongInfoHelper::GetPlaylistItemToolTip(m_all_song_info[song_index], show_title, show_full_path).c_str();

                m_toolTip.SetMaxTipWidth(theApp.DPI(400));		//设置提示信息的宽度，以支持提示换行

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
    else
    {
        m_toolTip.AddTool(this, _T(""));
        m_toolTip.Pop();
    }
    CListCtrlEx::OnMouseMove(nFlags, point);
}


BOOL CPlayListCtrl::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (m_toolTip.GetSafeHwnd() && pMsg->message == WM_MOUSEMOVE)
    {
        m_toolTip.RelayEvent(pMsg);
    }

    return CListCtrlEx::PreTranslateMessage(pMsg);
}


void CPlayListCtrl::PreSubclassWindow()
{
    // TODO: 在此添加专用代码和/或调用基类
    //CWindowDC dc(this);
    //HDC hDC = dc.GetSafeHdc();
    //m_dpi = GetDeviceCaps(hDC, LOGPIXELSY);

    CListCtrlEx::PreSubclassWindow();

    //将提示信息设为置顶
    m_toolTip.SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    //初始化播放列表
    DWORD style = GetExtendedStyle();
    style = (style | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
    style &= ~LVS_EX_LABELTIP;      //播放列表控件使用自己的鼠标提示，因此不需要LVS_EX_LABELTIP样式
    SetExtendedStyle(style);
    vector<int> width;
    CalculateColumeWidth(width);
    InsertColumn(0, theApp.m_str_table.LoadText(L"TXT_SERIAL_NUMBER").c_str(), LVCFMT_LEFT, width[0]);
    InsertColumn(1, theApp.m_str_table.LoadText(L"TXT_TRACK").c_str(), LVCFMT_LEFT, width[1]);
    InsertColumn(2, theApp.m_str_table.LoadText(L"TXT_LENGTH").c_str(), LVCFMT_LEFT, width[2]);
    SetCtrlAEnable(true);

    SetRowHeight(theApp.DPI(theApp.m_media_lib_setting_data.playlist_item_height));

}


void CPlayListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
    *pResult = CDRF_DODEFAULT;
    LPNMLVCUSTOMDRAW lplvdr = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
    NMCUSTOMDRAW& nmcd = lplvdr->nmcd;
    static bool this_item_select = false;
    switch (lplvdr->nmcd.dwDrawStage)	//判断状态
    {
    case CDDS_PREPAINT:
        *pResult = CDRF_NOTIFYITEMDRAW;
        break;
    case CDDS_ITEMPREPAINT:			//如果为画ITEM之前就要进行颜色的改变
        if (IsWindowEnabled())
        {
            this_item_select = false;
            int highlight_item{ m_highlight_item };
            if (m_searched && m_search_result.size() == 0)		//如果播放列表处于搜索状态且没有搜索结果
            {
                if (GetItemState(nmcd.dwItemSpec, LVIS_SELECTED) == LVIS_SELECTED)	//不允许选中行
                    SetItemState(nmcd.dwItemSpec, 0, LVIS_SELECTED);
                lplvdr->clrText = GRAY(140);
                lplvdr->clrTextBk = GRAY(255);
            }
            else
            {
                if (!m_searched || m_search_result.size() == m_all_song_info.size())	//当播放列表不处理搜索状态，或搜索结果数量等于播放列表中曲目数量时
                {
                    highlight_item = m_highlight_item;
                }
                else		//如果播放列表处于搜索状态，则高亮项目应该为搜索结果的索引
                {
                    auto iter = std::find(m_search_result.begin(), m_search_result.end(), m_highlight_item);
                    if (iter == m_search_result.end())
                        highlight_item = -1;
                    else
                        highlight_item = iter - m_search_result.begin();
                }
                //当选中行又是高亮行时设置颜色
                if (GetItemState(nmcd.dwItemSpec, LVIS_SELECTED) == LVIS_SELECTED && nmcd.dwItemSpec == highlight_item)
                {
                    this_item_select = true;
                    //SetItemState(nmcd.dwItemSpec, 0, LVIS_SELECTED);
                    lplvdr->clrText = m_theme_color.light3;
                    lplvdr->clrTextBk = m_theme_color.dark1;
                }
                //设置选中行的颜色
                else if (GetItemState(nmcd.dwItemSpec, LVIS_SELECTED) == LVIS_SELECTED)
                {
                    this_item_select = true;
                    //SetItemState(nmcd.dwItemSpec, 0, LVIS_SELECTED);
                    lplvdr->clrText = m_theme_color.dark3;
                    lplvdr->clrTextBk = m_theme_color.light2;
                }
                //设置高亮行的颜色
                else if (nmcd.dwItemSpec == highlight_item)
                {
                    lplvdr->clrText = m_theme_color.dark2;
                    //lplvdr->clrText = 0;
                    lplvdr->clrTextBk = m_theme_color.light3;
                }
                //设置偶数行的颜色
                else if (nmcd.dwItemSpec % 2 == 0)
                {
                    lplvdr->clrText = CColorConvert::m_gray_color.dark3;
                    lplvdr->clrTextBk = CColorConvert::m_gray_color.light3;
                }
                //设置奇数行的颜色
                else
                {
                    lplvdr->clrText = CColorConvert::m_gray_color.dark3;
                    lplvdr->clrTextBk = CColorConvert::m_gray_color.light4;
                }
            }

            //用背景色填充单元格，以去掉每行前面的空白
            CRect rect = nmcd.rc;
            CDC* pDC = CDC::FromHandle(nmcd.hdc);		//获取绘图DC
            COLORREF left_color{};
            left_color = lplvdr->clrTextBk;
            pDC->FillSolidRect(rect, left_color);
            //如果是高亮（正在播放）行，则在左侧绘制一个表示高亮的矩形
            bool is_search_result_empty = (m_searched && m_search_result.empty());
            if (nmcd.dwItemSpec == highlight_item && !is_search_result_empty)
            {
                CRect highlight_rect = rect;
                highlight_rect.right = highlight_rect.left + theApp.DPI(4);
                int hight = rect.Height() * 7 / 10;
                highlight_rect.top += (rect.Height() - hight) / 2;
                highlight_rect.bottom = highlight_rect.top + hight;
                pDC->FillSolidRect(highlight_rect, m_theme_color.dark1);
            }
        }
        else		//当控件被禁用时，显示文本设为灰色
        {
            lplvdr->clrText = GRAY(140);
            lplvdr->clrTextBk = GRAY(240);
        }
        *pResult = CDRF_NOTIFYPOSTPAINT;
        break;
    case CDDS_ITEMPOSTPAINT:
        if (this_item_select)
            SetItemState(nmcd.dwItemSpec, 0xFF, LVIS_SELECTED);
        //*pResult = CDRF_DODEFAULT;
        break;
    }
}
