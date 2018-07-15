// LyricEditDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "LyricEditDlg.h"
#include "afxdialogex.h"


// CLyricEditDlg 对话框

IMPLEMENT_DYNAMIC(CLyricEditDlg, CDialog)

CLyricEditDlg::CLyricEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_LYRIC_EDIT_DIALOG, pParent)
{

}

CLyricEditDlg::~CLyricEditDlg()
{
}

void CLyricEditDlg::OpreateTag(TagOpreation operation)
{
	int start, end;			//光标选中的起始的结束位置
	int tag_index;		//要操作的时间标签的位置
	m_lyric_edit.GetSel(start, end);
	tag_index = m_lyric_string.rfind(L"\r\n", start - 1);	//从光标位置向前查找\r\n的位置
	if (tag_index == string::npos)
		tag_index = 0;									//如果没有找到，则插入点的位置是最前面
	else
		tag_index += 2;

	Time time_tag{ theApp.m_player.GetCurrentPosition() };		//获取当前播放时间
	wchar_t time_tag_str[16];
	swprintf_s(time_tag_str, L"[%.2d:%.2d.%.2d]", time_tag.min, time_tag.sec, time_tag.msec / 10);

	int index2;		//当前时间标签的结束位置
	int tag_length;		//当前时间标签的长度
	int tag_index2;		//光标所在处时间标签开始的位置
	index2 = m_lyric_string.find(L']', tag_index);
	tag_length = index2 - tag_index + 1;
	switch (operation)
	{
	case TagOpreation::INSERT:			//插入时间标签（在光标所在行的最左边插入）
		m_lyric_string.insert(tag_index, time_tag_str);
		break;
	case TagOpreation::REPLACE:			//替换时间标签（替换光标所在行的左边的标签）
		m_lyric_string.replace(tag_index,tag_length, time_tag_str, wcslen(time_tag_str));
		break;
	case TagOpreation::DELETE_:			//删除时间标签（删除光标所在处的标签）
		tag_index2 = m_lyric_string.rfind(L'[', start);
		if (tag_index2 < tag_index) tag_index2 = tag_index;
		index2 = m_lyric_string.find(L']', tag_index2);
		tag_length = index2 - tag_index2 + 1;
		m_lyric_string.erase(tag_index2, tag_length);
		break;
	}
	

	int next_index;			//下一行的起始位置
	next_index = m_lyric_string.find(L"\r\n", start);
	if (next_index == string::npos)
	{
		if (operation != TagOpreation::DELETE_)
			m_lyric_string += L"\r\n";				//如果没有找到，说明当前已经是最后一行了，在末尾加上换行符
		next_index = m_lyric_string.size() - 1;
	}
	else
	{
		next_index += 2;
	}

	m_lyric_edit.SetWindowText(m_lyric_string.c_str());
	if (operation != TagOpreation::DELETE_)
		m_lyric_edit.SetSel(next_index, next_index);
	else
		m_lyric_edit.SetSel(tag_index, tag_index);
	m_lyric_edit.SetFocus();
	m_modified = true;
	UpdateStatusbarInfo();
}

bool CLyricEditDlg::SaveLyric(const wchar_t * path, CodeType code_type)
{
	if (path[0] == L'\0')		//如果保存时传递的路径的空字符串，则将保存路径设置为当前歌曲所在路径
	{
		m_lyric_path = theApp.m_player.GetCurrentDir() + theApp.m_player.GetFileName();
		int index = m_lyric_path.rfind(L'.');
		m_lyric_path = m_lyric_path.substr(0, index);
		m_lyric_path += L".lrc";
		m_original_lyric_path = m_lyric_path;
		SetDlgItemText(IDC_LYRIC_PATH_EDIT2, m_lyric_path.c_str());
		path = m_lyric_path.c_str();
	}
	bool char_connot_convert;
	string lyric_str = CCommon::UnicodeToStr(m_lyric_string, code_type, &char_connot_convert);
	if (char_connot_convert)	//当文件中包含Unicode字符时，询问用户是否要选择一个Unicode编码格式再保存
	{
		CString info;
		info.LoadString(IDS_STRING103);		//从string table载入字符串
		if (MessageBox(info, NULL, MB_OKCANCEL | MB_ICONWARNING) != IDOK) return false;		//如果用户点击了取消按钮，则返回false
	}
	ofstream out_put{ path, std::ios::binary };
	out_put << lyric_str;
	m_modified = false;
	m_lyric_saved = true;
	UpdateStatusbarInfo();
	return true;
}

void CLyricEditDlg::UpdateStatusbarInfo()
{
	CString str;
	//显示字符数
	str.Format(_T("共%d个字符"), m_lyric_string.size());
	m_status_bar.SetText(str, 0, 0);

	//显示是否修改
	m_status_bar.SetText(m_modified ? _T("已修改") : _T("未修改"), 1, 0);

	//显示编码格式
	str = _T("编码格式：");
	switch (m_code_type)
	{
	case CodeType::ANSI: str += _T("ANSI"); break;
	case CodeType::UTF8: str += _T("UTF8"); break;
	case CodeType::UTF8_NO_BOM: str += _T("UTF8无BOM"); break;
	case CodeType::UTF16: str += _T("UTF16"); break;
	}
	m_status_bar.SetText(str, 2, 0);
}

void CLyricEditDlg::StatusBarSetParts(int width)
{
	int nParts[3] = { width - DPI(220), width - DPI(160), -1 }; //分割尺寸
	m_status_bar.SetParts(3, nParts); //分割状态栏
}

void CLyricEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_lyric_edit);
}


BEGIN_MESSAGE_MAP(CLyricEditDlg, CDialog)
	//ON_BN_CLICKED(IDC_INSERT_TAG_BUTTON, &CLyricEditDlg::OnBnClickedInsertTagButton)
	//ON_BN_CLICKED(IDC_REPLACE_TAG_BUTTON, &CLyricEditDlg::OnBnClickedReplaceTagButton)
	//ON_BN_CLICKED(IDC_DELETE_TAG__BUTTON, &CLyricEditDlg::OnBnClickedDeleteTag)
	//ON_BN_CLICKED(IDC_SAVE_LYRIC_BUTTON, &CLyricEditDlg::OnBnClickedSaveLyricButton)
	//ON_BN_CLICKED(IDC_SAVE_AS_BUTTON5, &CLyricEditDlg::OnBnClickedSaveAsButton5)
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_EDIT1, &CLyricEditDlg::OnEnChangeEdit1)
	ON_WM_CLOSE()
	//ON_BN_CLICKED(IDC_OPEN_LYRIC_BUTTON, &CLyricEditDlg::OnBnClickedOpenLyricButton)
	ON_COMMAND(ID_LYRIC_OPEN, &CLyricEditDlg::OnLyricOpen)
	ON_COMMAND(ID_LYRIC_SAVE, &CLyricEditDlg::OnLyricSave)
	ON_COMMAND(ID_LYRIC_SAVE_AS, &CLyricEditDlg::OnLyricSaveAs)
	ON_COMMAND(ID_LYRIC_FIND, &CLyricEditDlg::OnLyricFind)
	ON_COMMAND(ID_LYRIC_REPLACE, &CLyricEditDlg::OnLyricReplace)
	ON_REGISTERED_MESSAGE(WM_FINDREPLACE, &CLyricEditDlg::OnFindReplace)
	ON_COMMAND(ID_FIND_NEXT, &CLyricEditDlg::OnFindNext)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_COMMAND(ID_LE_TRANSLATE_TO_SIMPLIFIED_CHINESE, &CLyricEditDlg::OnLeTranslateToSimplifiedChinese)
	ON_COMMAND(ID_LE_TRANSLATE_TO_TRANDITIONAL_CHINESE, &CLyricEditDlg::OnLeTranslateToTranditionalChinese)
	ON_COMMAND(ID_LYRIC_INSERT_TAG, &CLyricEditDlg::OnLyricInsertTag)
	ON_COMMAND(ID_LYRIC_REPLACE_TAG, &CLyricEditDlg::OnLyricReplaceTag)
	ON_COMMAND(ID_LYRIC_DELETE_TAG, &CLyricEditDlg::OnLyricDeleteTag)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipText)
END_MESSAGE_MAP()


// CLyricEditDlg 消息处理程序


BOOL CLyricEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CenterWindow();

	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);		// 设置小图标
	//获取歌词信息
	m_lyric_string = theApp.m_player.m_Lyrics.GetLyricsString();
	m_lyric_path = theApp.m_player.m_Lyrics.GetPathName();
	m_original_lyric_path = m_lyric_path;
	m_code_type = theApp.m_player.m_Lyrics.GetCodeType();
	m_current_song_name = theApp.m_player.GetFileName();

	//初始化编辑区字体
	m_font.CreatePointFont(100, _T("微软雅黑"));
	m_lyric_edit.SetFont(&m_font);

	m_lyric_edit.SetWindowText(m_lyric_string.c_str());
	SetDlgItemText(IDC_LYRIC_PATH_EDIT2, m_lyric_path.c_str());

	////初始化提示信息
	//m_Mytip.Create(this, TTS_ALWAYSTIP);
	//m_Mytip.AddTool(GetDlgItem(IDC_INSERT_TAG_BUTTON), _T("在光标所在行的最左边插入一个时间标签，快捷键：F8"));
	//m_Mytip.AddTool(GetDlgItem(IDC_REPLACE_TAG_BUTTON), _T("替换光标所在行最左边的时间标签，快捷键：F9"));
	//m_Mytip.AddTool(GetDlgItem(IDC_DELETE_TAG__BUTTON), _T("删除光标处的时间标签，快捷键：Ctrl+Del"));
	//m_Mytip.AddTool(GetDlgItem(IDC_SAVE_LYRIC_BUTTON), _T("快捷键：Ctrl+S"));
	//m_Mytip.AddTool(GetDlgItem(ID_PLAY_PAUSE), _T("快捷键：Ctrl+P"));
	//m_Mytip.AddTool(GetDlgItem(ID_REW), _T("快捷键：Ctrl+←"));
	//m_Mytip.AddTool(GetDlgItem(ID_FF), _T("快捷键：Ctrl+→"));

	////获取初始时窗口的大小
	//CRect rect;
	//GetWindowRect(rect);
	//m_min_size.cx = rect.Width();
	//m_min_size.cy = rect.Height();

	//初始化工具栏
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE/* | CBRS_TOP*/ | CBRS_ALIGN_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_TOP
		/*| CBRS_GRIPPER*/ | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_LYRIC_EDIT_TOOLBAR))  //指定工具栏ID号
	{
		TRACE0("Failed to create toolbar/n");
		return -1;      // fail to create
	}
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	CImageList ImageList;
	ImageList.Create(DPI(20), DPI(20), ILC_COLOR32 | ILC_MASK, 2, 2);

	//通过ImageList对象加载图标作为工具栏的图标
	//添加图标
	ImageList.Add(AfxGetApp()->LoadIcon(IDI_ADD_TAG));
	ImageList.Add(AfxGetApp()->LoadIcon(IDI_REPLACE_TAG));
	ImageList.Add(AfxGetApp()->LoadIcon(IDI_DELETE_TAG));
	ImageList.Add(AfxGetApp()->LoadIcon(IDI_SAVE));
	ImageList.Add(AfxGetApp()->LoadIcon(IDI_PLAY_PAUSE));
	ImageList.Add(AfxGetApp()->LoadIcon(IDI_REW));
	ImageList.Add(AfxGetApp()->LoadIcon(IDI_FF));
	ImageList.Add(AfxGetApp()->LoadIcon(IDI_FIND));
	ImageList.Add(AfxGetApp()->LoadIcon(IDI_REPLACE));
	m_wndToolBar.GetToolBarCtrl().SetImageList(&ImageList);
	ImageList.Detach();

	//设置工具栏高度
	CRect rect1;
	m_wndToolBar.GetClientRect(rect1);
	rect1.bottom = rect1.top + TOOLBAR_HEIGHT;
	m_wndToolBar.MoveWindow(rect1);

	//初始化状态栏
	CRect rect;
	GetClientRect(&rect);
	rect.top = rect.bottom - DPI(20);
	m_status_bar.Create(WS_VISIBLE | CBRS_BOTTOM, rect, this, 3);

	StatusBarSetParts(rect.Width());
	UpdateStatusbarInfo();

	m_dlg_exist = true;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CLyricEditDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	DestroyWindow();

	//CDialog::OnCancel();
}


//void CLyricEditDlg::OnBnClickedInsertTagButton()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	OpreateTag(TagOpreation::INSERT);
//}
//
//
//void CLyricEditDlg::OnBnClickedReplaceTagButton()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	OpreateTag(TagOpreation::REPLACE);
//}
//
//
//void CLyricEditDlg::OnBnClickedDeleteTag()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	OpreateTag(TagOpreation::DELETE_);
//}


//void CLyricEditDlg::OnBnClickedSaveLyricButton()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	OnLyricSave();
//}


//void CLyricEditDlg::OnBnClickedSaveAsButton5()
//{
//	// TODO: 在此添加控件通知处理程序代码
//}


void CLyricEditDlg::OnDestroy()
{
	// TODO: 在此处添加消息处理程序代码
	CDialog::OnDestroy();
	m_dlg_exist = false;
	if (m_current_song_name == theApp.m_player.GetFileName() && m_lyric_saved)		//关闭歌词编辑窗口时如果正在播放的歌曲没有变，且执行过保存操作，就重新初始化歌词
		theApp.m_player.IniLyrics(m_original_lyric_path);
}


void CLyricEditDlg::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString lyric_str;
	m_lyric_edit.GetWindowText(lyric_str);
	m_lyric_string = lyric_str;
	m_modified = true;
	UpdateStatusbarInfo();
}


BOOL CLyricEditDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	//屏蔽按回车键和ESC键退出
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
			return TRUE;
		if (pMsg->wParam == VK_F8)
		{
			OpreateTag(TagOpreation::INSERT);
			return TRUE;
		}
		if (pMsg->wParam == VK_F9)
		{
			OpreateTag(TagOpreation::REPLACE);
			return TRUE;
		}
		if ((GetKeyState(VK_CONTROL) & 0x80) && pMsg->wParam == VK_DELETE)
		{
			OpreateTag(TagOpreation::DELETE_);
			return TRUE;
		}
		if ((GetKeyState(VK_CONTROL) & 0x80) && pMsg->wParam == 'S')
		{
			OnLyricSave();
			return TRUE;
		}
		if ((GetKeyState(VK_CONTROL) & 0x80) && pMsg->wParam == 'P')
		{
			SendMessage(WM_COMMAND, ID_PLAY_PAUSE);
			return TRUE;
		}
		if ((GetKeyState(VK_CONTROL) & 0x80) && pMsg->wParam == VK_LEFT)
		{
			SendMessage(WM_COMMAND, ID_REW);
			return TRUE;
		}
		if ((GetKeyState(VK_CONTROL) & 0x80) && pMsg->wParam == VK_RIGHT)
		{
			SendMessage(WM_COMMAND, ID_FF);
			return TRUE;
		}
		if ((GetKeyState(VK_CONTROL) & 0x80) && pMsg->wParam == 'F')
		{
			OnLyricFind();
			return TRUE;
		}
		if ((GetKeyState(VK_CONTROL) & 0x80) && pMsg->wParam == 'H')
		{
			OnLyricReplace();
			return TRUE;
		}
		if (pMsg->wParam == VK_F3)
		{
			OnFindNext();
			return TRUE;
		}
	}
	//if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	//	return TRUE;
	//if (pMsg->message == WM_MOUSEMOVE)
	//	m_Mytip.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}


void CLyricEditDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_modified)
	{
		int rtn = MessageBox(_T("歌词已更改，是否要保存？"), NULL, MB_YESNOCANCEL | MB_ICONWARNING);
		switch (rtn)
		{
		case IDYES: SaveLyric(m_lyric_path.c_str(), m_code_type);
		case IDNO: m_modified = false; break;
		default: return;
		}
	}

	CDialog::OnClose();
}


//void CLyricEditDlg::OnBnClickedOpenLyricButton()
//{
//	// TODO: 在此添加控件通知处理程序代码
//}


void CLyricEditDlg::OnLyricOpen()
{
	// TODO: 在此添加命令处理程序代码
	if (m_modified)
	{
		int rtn = MessageBox(_T("歌词已更改，是否要保存？"), NULL, MB_YESNOCANCEL | MB_ICONWARNING);
		switch (rtn)
		{
		case IDYES: SaveLyric(m_lyric_path.c_str(), m_code_type); m_modified = false; break;
		case IDNO: break;
		default: return;
		}
	}

	//设置过滤器
	LPCTSTR szFilter = _T("lrc歌词文件(*.lrc)|*.lrc|文本文件(*.txt)|*.txt|所有文件(*.*)|*.*||");
	//构造打开文件对话框
	CFileDialog fileDlg(TRUE, _T("txt"), NULL, 0, szFilter, this);
	//显示打开文件对话框
	if (IDOK == fileDlg.DoModal())
	{
		m_lyric_path = fileDlg.GetPathName();	//获取打开的文件路径
		SetDlgItemText(IDC_LYRIC_PATH_EDIT2, m_lyric_path.c_str());
		CLyrics lyrics{ m_lyric_path };					//打开文件
		m_lyric_string = lyrics.GetLyricsString();
		m_code_type = lyrics.GetCodeType();
		m_lyric_edit.SetWindowText(m_lyric_string.c_str());
	}
}


void CLyricEditDlg::OnLyricSave()
{
	// TODO: 在此添加命令处理程序代码
	if (m_modified)
		SaveLyric(m_lyric_path.c_str(), m_code_type);
}


void CLyricEditDlg::OnLyricSaveAs()
{
	// TODO: 在此添加命令处理程序代码
	//设置过滤器
	const wchar_t* szFilter = _T("lrc歌词文件(*.lrc)|*.lrc|文本文件(*.txt)|*.txt|所有文件(*.*)|*.*||");
	//构造保存文件对话框
	CFileDialog fileDlg(FALSE, _T("txt"), m_lyric_path.c_str(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	//为“另存为”对话框添加一个组合选择框
	fileDlg.AddComboBox(IDC_SAVE_COMBO_BOX);
	//为组合选择框添加项目
	fileDlg.AddControlItem(IDC_SAVE_COMBO_BOX, 0, _T("以ANSI格式保存"));
	fileDlg.AddControlItem(IDC_SAVE_COMBO_BOX, 1, _T("以UTF-8格式保存"));
	//为组合选择框设置默认选中的项目
	DWORD default_selected{ 0 };
	if (m_code_type == CodeType::UTF8 || m_code_type == CodeType::UTF8_NO_BOM)
		default_selected = 1;
	fileDlg.SetSelectedControlItem(IDC_SAVE_COMBO_BOX, default_selected);

	//显示保存文件对话框
	if (IDOK == fileDlg.DoModal())
	{
		DWORD selected_item;
		fileDlg.GetSelectedControlItem(IDC_SAVE_COMBO_BOX, selected_item);	//获取“编码格式”中选中的项目
		CodeType save_code{};
		switch (selected_item)
		{
		case 0: save_code = CodeType::ANSI; break;
		case 1: save_code = CodeType::UTF8; break;
		default: break;
		}
		SaveLyric(fileDlg.GetPathName().GetString(), save_code);
	}
}


void CLyricEditDlg::OnLyricFind()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pFindDlg == nullptr)
	{
		m_pFindDlg = new CFindReplaceDialog;
		m_pFindDlg->Create(TRUE, NULL, NULL, FR_DOWN | FR_HIDEWHOLEWORD | FR_HIDEMATCHCASE, this);
	}
	m_pFindDlg->ShowWindow(SW_SHOW);
	m_pFindDlg->SetActiveWindow();
}


void CLyricEditDlg::OnLyricReplace()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pReplaceDlg == nullptr)
	{
		m_pReplaceDlg = new CFindReplaceDialog;
		m_pReplaceDlg->Create(FALSE, NULL, NULL, FR_DOWN | FR_HIDEWHOLEWORD | FR_HIDEMATCHCASE, this);
	}
	m_pReplaceDlg->ShowWindow(SW_SHOW);
	m_pReplaceDlg->SetActiveWindow();
}


afx_msg LRESULT CLyricEditDlg::OnFindReplace(WPARAM wParam, LPARAM lParam)
{
	if (m_pFindDlg != nullptr)
	{
		m_find_str = m_pFindDlg->GetFindString();
		m_find_down = (m_pFindDlg->SearchDown() != 0);
		if (m_pFindDlg->FindNext())		//查找下一个时
		{
			OnFindNext();
		}
		if (m_pFindDlg->IsTerminating())	//关闭窗口时
		{
			//m_pFindDlg->DestroyWindow();
			m_pFindDlg = nullptr;
		}
	}
	//delete m_pFindDlg;

	if (m_pReplaceDlg != nullptr)
	{
		m_find_str = m_pReplaceDlg->GetFindString();
		m_replace_str = m_pReplaceDlg->GetReplaceString();
		if (m_pReplaceDlg->FindNext())		//查找下一个时
		{
			OnFindNext();
		}
		if (m_pReplaceDlg->ReplaceCurrent())	//替换当前时
		{
			if (m_find_flag)
			{
				m_lyric_string.replace(m_find_index, m_find_str.size(), m_replace_str.c_str(), m_replace_str.size());	//替换找到的字符串
				m_lyric_edit.SetWindowText(m_lyric_string.c_str());
				m_modified = true;
				UpdateStatusbarInfo();
				OnFindNext();
				m_lyric_edit.SetSel(m_find_index, m_find_index + m_find_str.size());	//选中替换的字符串
				SetActiveWindow();		//将编辑器窗口设置活动窗口
			}
			else
			{
				OnFindNext();
			}
		}
		if (m_pReplaceDlg->ReplaceAll())		//替换全部时
		{
			int replace_count{};	//统计替换的字符串的个数
			while (true)
			{
				m_find_index = m_lyric_string.find(m_find_str, m_find_index + 1);	//查找字符串
				if (m_find_index == string::npos)
					break;
				m_lyric_string.replace(m_find_index, m_find_str.size(), m_replace_str.c_str(), m_replace_str.size());	//替换找到的字符串
				replace_count++;
			}
			m_lyric_edit.SetWindowText(m_lyric_string.c_str());
			m_modified = true;
			UpdateStatusbarInfo();
			if (replace_count != 0)
			{
				CString info;
				info.Format(_T("替换完成，共替换%d个字符串。"), replace_count);
				MessageBox(info, NULL, MB_ICONINFORMATION);
			}
		}
		if (m_pReplaceDlg->IsTerminating())
		{
			m_pReplaceDlg = nullptr;
		}
	}
	return 0;
}


void CLyricEditDlg::OnFindNext()
{
	// TODO: 在此添加命令处理程序代码
	if (m_find_down)
		m_find_index = m_lyric_string.find(m_find_str, m_find_index + 1);	//向后查找
	else
		m_find_index = m_lyric_string.rfind(m_find_str, m_find_index - 1);	//向前查找
	if (m_find_index == string::npos)
	{
		CString info;
		info.Format(_T("找不到“%s”"), m_find_str.c_str());
		MessageBox(info, NULL, MB_OK | MB_ICONINFORMATION);
		m_find_flag = false;
	}
	else
	{
		m_lyric_edit.SetSel(m_find_index, m_find_index + m_find_str.size());		//选中找到的字符串
		SetActiveWindow();		//将编辑器窗口设为活动窗口
		m_lyric_edit.SetFocus();
		m_find_flag = true;
	}
}


void CLyricEditDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//限制窗口最小大小
	lpMMI->ptMinTrackSize.x = DPI(300);		//设置最小宽度
	lpMMI->ptMinTrackSize.y = DPI(300);		//设置最小高度

	CDialog::OnGetMinMaxInfo(lpMMI);
}


void CLyricEditDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	//窗口大小变化时调整状态栏的大小和位置
	if (nType != SIZE_MINIMIZED && m_status_bar.m_hWnd)
	{
		CRect rect;
		rect.right = cx;
		rect.bottom = cy;
		rect.top = rect.bottom - STATUSBAR_HEIGHT;
		m_status_bar.MoveWindow(rect);
		StatusBarSetParts(cx);
	}

	//调整窗口的大小和位置
	if (nType != SIZE_MINIMIZED)
	{
		CRect rect;
		CWnd* plyric_path_wnd{ GetDlgItem(IDC_LYRIC_PATH_EDIT2) };
		if (plyric_path_wnd != nullptr)
		{
			plyric_path_wnd->GetWindowRect(rect);
			ScreenToClient(&rect);
			rect.right = cx - MARGIN;
			plyric_path_wnd->MoveWindow(rect);
		}

		if (m_lyric_edit.m_hWnd != NULL)
		{
			m_lyric_edit.GetWindowRect(rect);
			ScreenToClient(&rect);
			rect.right = cx - MARGIN;
			rect.bottom = cy - STATUSBAR_HEIGHT - MARGIN;
			m_lyric_edit.MoveWindow(rect);
		}

		if (m_wndToolBar.m_hWnd != NULL)
		{
			rect.left = 0;
			rect.top = 0;
			rect.right = cx;
			rect.bottom = TOOLBAR_HEIGHT;
			m_wndToolBar.MoveWindow(rect);
		}
	}
}


void CLyricEditDlg::OnLeTranslateToSimplifiedChinese()
{
	// TODO: 在此添加命令处理程序代码
	m_lyric_string = CCommon::TranslateToSimplifiedChinese(m_lyric_string);
	m_lyric_edit.SetWindowText(m_lyric_string.c_str());
	m_modified = true;
	UpdateStatusbarInfo();
}


void CLyricEditDlg::OnLeTranslateToTranditionalChinese()
{
	// TODO: 在此添加命令处理程序代码
	m_lyric_string = CCommon::TranslateToTranditionalChinese(m_lyric_string);
	m_lyric_edit.SetWindowText(m_lyric_string.c_str());
	m_modified = true;
	UpdateStatusbarInfo();
}


void CLyricEditDlg::OnLyricInsertTag()
{
	// TODO: 在此添加命令处理程序代码
	OpreateTag(TagOpreation::INSERT);
}


void CLyricEditDlg::OnLyricReplaceTag()
{
	// TODO: 在此添加命令处理程序代码
	OpreateTag(TagOpreation::REPLACE);
}


void CLyricEditDlg::OnLyricDeleteTag()
{
	// TODO: 在此添加命令处理程序代码
	OpreateTag(TagOpreation::DELETE_);
}

BOOL CLyricEditDlg::OnToolTipText(UINT, NMHDR * pNMHDR, LRESULT * pResult)
{
	TOOLTIPTEXT* pT = (TOOLTIPTEXT*)pNMHDR; //将pNMHDR转换成TOOLTIPTEXT指针类型数据
	UINT nID = pNMHDR->idFrom;  //获取工具条上按钮的ID
	switch (nID)
	{
	case ID_LYRIC_INSERT_TAG:
		pT->lpszText = _T("插入时间标签 (F8)");
		break;
	case ID_LYRIC_REPLACE_TAG:
		pT->lpszText = _T("替换时间标签 (F9)");
		break;
	case ID_LYRIC_DELETE_TAG:
		pT->lpszText = _T("删除时间标签 (Ctrl+Del)");
		break;
	case ID_LYRIC_SAVE:
		pT->lpszText = _T("保存 (Ctrl+S)");
		break;
	case ID_PLAY_PAUSE:
		pT->lpszText = _T("播放/暂停 (Ctrl+P)");
		break;
	case ID_REW:
		pT->lpszText = _T("快退 (Ctrl+←)");
		break;
	case ID_FF:
		pT->lpszText = _T("快进 (Ctrl+→)");
		break;
	case ID_LYRIC_FIND:
		pT->lpszText = _T("查找 (Ctrl+F)");
		break;
	case ID_LYRIC_REPLACE:
		pT->lpszText = _T("替换 (Ctrl+H)");
		break;
	}
	return 0;
}
