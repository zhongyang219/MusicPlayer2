// CListenTimeStatisticsDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "CListenTimeStatisticsDlg.h"
#include "SongDataManager.h"
#include "FilterHelper.h"
#include "AudioCommon.h"


// CListenTimeStatisticsDlg 对话框

IMPLEMENT_DYNAMIC(CListenTimeStatisticsDlg, CBaseDialog)

CListenTimeStatisticsDlg::CListenTimeStatisticsDlg(CWnd* pParent /*=nullptr*/)
    : CBaseDialog(IDD_LISTEN_TIME_STATISTICS_DLG, pParent)
{

}

CListenTimeStatisticsDlg::~CListenTimeStatisticsDlg()
{
}

CString CListenTimeStatisticsDlg::GetDialogName() const
{
    return L"ListenTimeStatisticsDlg";
}

bool CListenTimeStatisticsDlg::InitializeControls()
{
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TITLE_LISTEN_TIME");
    SetWindowTextW(temp.c_str());
    // IDC_LIST1
    temp = theApp.m_str_table.LoadText(L"TXT_LISTEN_TIME_EXPORT");
    SetDlgItemTextW(IDC_EXPORT_BUTTON, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_LISTEN_TIME_CLEAR");
    SetDlgItemTextW(IDC_CLEAR_BUTTON, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_CLOSE");
    SetDlgItemTextW(IDCANCEL, temp.c_str());

    RepositionTextBasedControls({
        { CtrlTextInfo::L2, IDC_EXPORT_BUTTON, CtrlTextInfo::W32 },
        { CtrlTextInfo::L1, IDC_CLEAR_BUTTON, CtrlTextInfo::W32 },
        { CtrlTextInfo::R1, IDCANCEL, CtrlTextInfo::W32 }
        });
    return true;
}

void CListenTimeStatisticsDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
}


void CListenTimeStatisticsDlg::ShowData(bool size_changed)
{
	//将vector中的数据显示到列表中
    if(size_changed)
        m_list_ctrl.DeleteAllItems();

	int index = 0;
	for (const auto& data : m_data_list)
	{
        if (size_changed)
            m_list_ctrl.InsertItem(index, std::to_wstring(index + 1).c_str());
        else
            m_list_ctrl.SetItemText(index, COL_INDEX, std::to_wstring(index + 1).c_str());

		m_list_ctrl.SetItemText(index, COL_TRACK, data.name.c_str());
		m_list_ctrl.SetItemText(index, COL_PATH, data.path.c_str());
		m_list_ctrl.SetItemText(index, COL_TOTAL_TIME, data.total_time.toString3().c_str());
		m_list_ctrl.SetItemText(index, COL_LENGTH, data.length.toString().c_str());
        CString str;
		str.Format(_T("%.1f"), data.times);
		if (str.Right(2) == _T(".0"))
			str = str.Left(str.GetLength() - 2);

		m_list_ctrl.SetItemText(index, COL_TIMES, str);
        // 这里之前设置当前播放高亮，意义不大，去掉了
		index++;
	}
}


CListenTimeStatisticsDlg::ListItem CListenTimeStatisticsDlg::SongInfoToListItem(const SongInfo& song)
{
    ListItem list_item;
    if (song.IsTitleEmpty() || CAudioCommon::GetAudioTypeByFileName(song.file_path) == AU_MIDI)
    {
        list_item.name = song.GetFileName().c_str();
    }
    else
    {
        list_item.name = (song.GetArtist() + L" - " + song.GetTitle()).c_str();
    }

    list_item.path = song.file_path;
    list_item.total_time = Time(song.listen_time * 1000);
    list_item.length = song.length();
    list_item.times = static_cast<double>(song.listen_time) / song.length().toInt() * 1000;
    return list_item;
}

BEGIN_MESSAGE_MAP(CListenTimeStatisticsDlg, CBaseDialog)
	ON_BN_CLICKED(IDC_EXPORT_BUTTON, &CListenTimeStatisticsDlg::OnBnClickedExportButton)
	ON_WM_GETMINMAXINFO()
    ON_BN_CLICKED(IDC_CLEAR_BUTTON, &CListenTimeStatisticsDlg::OnBnClickedClearButton)
    ON_NOTIFY(HDN_ITEMCLICK, 0, &CListenTimeStatisticsDlg::OnHdnItemclickList1)
END_MESSAGE_MAP()


// CListenTimeStatisticsDlg 消息处理程序


BOOL CListenTimeStatisticsDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	//初始化列表
    CRect rect;
    m_list_ctrl.GetWindowRect(rect);
	int width[6];
	width[0] = theApp.DPI(40);
	width[1] = theApp.DPI(150);
	width[3] = width[5] = theApp.DPI(60);
	width[4] = theApp.DPI(50);
	width[2] = rect.Width() - width[1] - width[3] - width[4] - width[5] - width[0] - theApp.DPI(20) - 1;
    m_list_ctrl.SetExtendedStyle(m_list_ctrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
    m_list_ctrl.InsertColumn(0, theApp.m_str_table.LoadText(L"TXT_SERIAL_NUMBER").c_str(), LVCFMT_LEFT, width[0]);
    m_list_ctrl.InsertColumn(1, theApp.m_str_table.LoadText(L"TXT_TRACK").c_str(), LVCFMT_LEFT, width[1]);
    m_list_ctrl.InsertColumn(2, theApp.m_str_table.LoadText(L"TXT_PATH").c_str(), LVCFMT_LEFT, width[2]);
    m_list_ctrl.InsertColumn(3, theApp.m_str_table.LoadText(L"TXT_LISTEN_TIME_TOTAL_TIME").c_str(), LVCFMT_LEFT, width[3]);
    m_list_ctrl.InsertColumn(4, theApp.m_str_table.LoadText(L"TXT_LENGTH").c_str(), LVCFMT_LEFT, width[4]);
    m_list_ctrl.InsertColumn(5, theApp.m_str_table.LoadText(L"TXT_LISTEN_TIME_TOTAL_COUNT").c_str(), LVCFMT_LEFT, width[5]);

    //获取数据
    //从所有歌曲信息中查找累计听的时间超过指定时间的曲目添加到vector
    CSongDataManager::GetInstance().GetSongData([&](const CSongDataManager::SongDataMap& song_data_map)
        {
            for (const auto& data : song_data_map)
            {
                SongInfo song{ data.second };
                if (song.listen_time >= 20 && song.length() > 0)
                {
                    song.file_path = data.first.path;
                    m_data_list.push_back(SongInfoToListItem(song));
                }
            }
        });

    //先按累计收听时间从大到小排序
    std::sort(m_data_list.begin(), m_data_list.end(), [](const ListItem& a, const ListItem& b)
    {
        return a.total_time > b.total_time;
    });

    //再按累计次数从大到小排序，并确保累计次数相同的项目按累计时间从大到小排序
    std::stable_sort(m_data_list.begin(), m_data_list.end(), [](const ListItem& a, const ListItem& b)
    {
        double times_a, times_b;
        times_a = CCommon::DoubleRound(a.times, 1);
        times_b = CCommon::DoubleRound(b.times, 1);
        return times_a > times_b;
    });

    //将数据显示到列表中
	ShowData();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CListenTimeStatisticsDlg::OnBnClickedExportButton()
{
	// TODO: 在此添加控件通知处理程序代码

	//弹出保存对话框

    wstring filter = FilterHelper::GetListenTimeFilter();
    wstring file_name = theApp.m_str_table.LoadText(L"TITLE_LISTEN_TIME");
	CString str_cur_date;
	SYSTEMTIME cur_time;
	GetLocalTime(&cur_time);
	str_cur_date.Format(_T("_%.4d-%.2d-%.2d"), cur_time.wYear, cur_time.wMonth, cur_time.wDay);
	file_name += str_cur_date;
    CCommon::FileNameNormalize(file_name);

	// 构造保存文件对话框
    CFileDialog fileDlg(FALSE, _T("csv"), file_name.c_str(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter.c_str(), this);
	// 显示保存文件对话框
	if (IDOK == fileDlg.DoModal())
	{
        //生成导出的csv文本
        std::wstringstream wss;
        wss << theApp.m_str_table.LoadText(L"TXT_SERIAL_NUMBER") << L','
            << theApp.m_str_table.LoadText(L"TXT_TRACK") << L','
            << theApp.m_str_table.LoadText(L"TXT_PATH") << L','
            << theApp.m_str_table.LoadText(L"TXT_LISTEN_TIME_TOTAL_TIME") << L','
            << theApp.m_str_table.LoadText(L"TXT_LENGTH") << L','
            << theApp.m_str_table.LoadText(L"TXT_LISTEN_TIME_TOTAL_COUNT") << L'\n';

        int list_size = m_list_ctrl.GetItemCount();
        for (int i = 0; i < list_size; i++)
        {
            const int COLUMN = 6;
            for (int j = 0; j < COLUMN; j++)
            {
                CString item_text{ m_list_ctrl.GetItemText(i, j).GetString() };
                CCommon::StringCsvNormalize(item_text);
                wss << item_text.GetString();
                if (j == COLUMN - 1)
                    wss << L'\n';
                else
                    wss << L',';
            }
        }

        ofstream out_put{ fileDlg.GetPathName().GetString() };
        out_put << CCommon::UnicodeToStr(wss.str(), CodeType::UTF8);
        out_put.close();
    }
}


void CListenTimeStatisticsDlg::OnBnClickedClearButton()
{
    // TODO: 在此添加控件通知处理程序代码
    const wstring& info = theApp.m_str_table.LoadText(L"MSG_LISTEN_TIME_CLEAR_WARNING");
    if (MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OKCANCEL) == IDOK)
    {
        CSongDataManager::GetInstance().ClearPlayTime();
        m_list_ctrl.DeleteAllItems();
    }
}


void CListenTimeStatisticsDlg::OnHdnItemclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    if (phdr->hdr.hwndFrom == m_list_ctrl.GetHeaderCtrl()->GetSafeHwnd())
    {
        static bool ascending = true;
        ascending = !ascending;

        static int last_item = -1;
        if (last_item != phdr->iItem)
        {
            last_item = phdr->iItem;
            ascending = false;
        }

        //对列表排序
        switch (phdr->iItem)
        {
        case COL_INDEX:
            break;
        case COL_TRACK:
            std::sort(m_data_list.begin(), m_data_list.end(), [](const ListItem& a, const ListItem& b) { if (ascending) return CCommon::StringCompareInLocalLanguage(a.name, b.name) < 0; else return CCommon::StringCompareInLocalLanguage(a.name, b.name) > 0; });
            ShowData(false);
            break;
        case COL_PATH:
            std::sort(m_data_list.begin(), m_data_list.end(), [](const ListItem& a, const ListItem& b) { if (ascending) return CCommon::StringCompareInLocalLanguage(a.path, b.path) < 0; else return CCommon::StringCompareInLocalLanguage(a.path, b.path) > 0; });
            ShowData(false);
            break;
        case COL_TOTAL_TIME:
            std::sort(m_data_list.begin(), m_data_list.end(), [](const ListItem& a, const ListItem& b) { if (ascending) return a.total_time < b.total_time; else return a.total_time > b.total_time; });
            ShowData(false);
            break;
        case COL_LENGTH:
            std::sort(m_data_list.begin(), m_data_list.end(), [](const ListItem& a, const ListItem& b) { if (ascending) return a.length < b.length; else return a.length > b.length; });
            ShowData(false);
            break;
        case COL_TIMES:
            std::sort(m_data_list.begin(), m_data_list.end(), [](const ListItem& a, const ListItem& b) { if (ascending) return a.times < b.times; else return a.times > b.times; });
            ShowData(false);
            break;
        default:
            break;
        }
    }
    *pResult = 0;
}
