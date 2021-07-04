// LyricEditDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "LyricEditDlg.h"
#include "afxdialogex.h"
#include "WIC.h"


// CLyricEditDlg 对话框

IMPLEMENT_DYNAMIC(CLyricEditDlg, CBaseDialog)

CLyricEditDlg::CLyricEditDlg(CWnd* pParent /*=NULL*/)
	: CBaseDialog(IDD_LYRIC_EDIT_DIALOG, pParent)
{

}

CLyricEditDlg::~CLyricEditDlg()
{
}

void CLyricEditDlg::OpreateTag(TagOpreation operation)
{
	int start, end;			//光标选中的起始的结束位置
	int tag_index;		//要操作的时间标签的位置
	m_view->GetSel(start, end);
	tag_index = m_lyric_string.rfind(L"\r\n", start - 1);	//从光标位置向前查找\r\n的位置
	if (tag_index == string::npos || start == 0)
		tag_index = 0;									//如果没有找到，则插入点的位置是最前面
	else
		tag_index += 2;

	Time time_tag{ CPlayer::GetInstance().GetCurrentPosition() };		//获取当前播放时间
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

    {
        CScintillaEditView::KeepCurrentLine keep_cur_line(m_view);
        m_view->SetText(m_lyric_string);
    }
	if (operation != TagOpreation::DELETE_)
        m_view->SetSel(next_index, next_index, m_lyric_string);
	else
        m_view->SetSel(tag_index, tag_index, m_lyric_string);
    m_view->SetFocus();
	m_modified = true;
	UpdateStatusbarInfo();
}

bool CLyricEditDlg::SaveLyric(wstring path, CodeType code_type)
{
    const SongInfo& song{ CPlayer::GetInstance().GetCurrentSongInfo() };
    bool lyric_write_support = CAudioTag::IsFileTypeLyricWriteSupport(CFilePathHelper(CPlayer::GetInstance().GetCurrentFilePath()).GetFileExtension());
    if (m_inner_lyric && lyric_write_support)
    {
        //写入内嵌歌词
        bool saved{};
        SongInfo song_info{ song };
        CAudioTag audio_tag(song_info);
        CPlayer::ReOpen reopen(true);
        saved = audio_tag.WriteAudioLyric(m_lyric_string);
        if (saved)
        {
            m_modified = false;
            m_lyric_saved = true;
            UpdateStatusbarInfo();
        }
        else
        {
            MessageBox(CCommon::LoadText(IDS_LYRIC_SAVE_FAILED), NULL, MB_ICONWARNING | MB_OK);
        }
        return saved;
    }
    else
    {
        //写入歌词文件
	    if (path.empty())		//如果保存时传递的路径的空字符串，则将保存路径设置为当前歌曲所在路径
	    {

		    if (!song.is_cue)
		    {
			    m_lyric_path = CPlayer::GetInstance().GetCurrentDir() + CPlayer::GetInstance().GetFileName();
			    int index = m_lyric_path.rfind(L'.');
			    m_lyric_path = m_lyric_path.substr(0, index);
		    }
		    else
		    {
			    m_lyric_path = CPlayer::GetInstance().GetCurrentDir() + song.artist + L" - " + song.title;
		    }
		    m_lyric_path += L".lrc";
		    m_original_lyric_path = m_lyric_path;
            SetLyricPathEditText();
		    path = m_lyric_path;
	    }
	    bool char_connot_convert;
	    string lyric_str = CCommon::UnicodeToStr(m_lyric_string, code_type, &char_connot_convert);
	    if (char_connot_convert)	//当文件中包含Unicode字符时，询问用户是否要选择一个Unicode编码格式再保存
	    {
            CString info{ CCommon::LoadText(IDS_STRING103) };                                   //从string table载入字符串
		    if (MessageBox(info, NULL, MB_OKCANCEL | MB_ICONWARNING) != IDOK) return false;		//如果用户点击了取消按钮，则返回false
	    }
	    ofstream out_put{ path, std::ios::binary };
        bool failed = out_put.fail();
	    out_put << lyric_str;
        out_put.close();
        if (!failed)
        {
            m_modified = false;
            m_lyric_saved = true;
            UpdateStatusbarInfo();
        }
        else
        {
            MessageBox(CCommon::LoadText(IDS_LYRIC_SAVE_FAILED), NULL, MB_ICONWARNING | MB_OK);
        }
        return !failed;
    }
}

void CLyricEditDlg::UpdateStatusbarInfo()
{
	CString str;
	//显示字符数
	str = CCommon::LoadTextFormat(IDS_CHARACTER_TOTAL, { m_lyric_string.size() });
	m_status_bar.SetText(str, 0, 0);

	//显示是否修改
	m_status_bar.SetText(m_modified ? CCommon::LoadText(IDS_MODIFIED) : CCommon::LoadText(IDS_UNMODIFIED), 1, 0);

	//显示编码格式
	str = CCommon::LoadText(IDS_ENCODE_FORMAT, _T(": "));
	switch (m_code_type)
	{
	case CodeType::ANSI: str += _T("ANSI"); break;
	case CodeType::UTF8: str += _T("UTF8"); break;
	case CodeType::UTF8_NO_BOM: str += CCommon::LoadText(IDS_UTF8_NO_BOM); break;
	case CodeType::UTF16LE: str += _T("UTF16LE"); break;
	case CodeType::UTF16BE: str += _T("UTF16BE"); break;
	}
	m_status_bar.SetText(str, 2, 0);
}

void CLyricEditDlg::StatusBarSetParts(int width)
{
	int nParts[3] = { width - theApp.DPI(240), width - theApp.DPI(160), -1 }; //分割尺寸
	m_status_bar.SetParts(3, nParts); //分割状态栏
}

void CLyricEditDlg::OpenLyric(const wchar_t * path)
{
	m_lyric_path = path;
	CLyrics lyrics{ m_lyric_path };					//打开文件
	m_lyric_string = lyrics.GetLyricsString();
	m_code_type = lyrics.GetCodeType();
    m_inner_lyric = false;
}

bool CLyricEditDlg::SaveInquiry()
{
    if (m_modified)
    {
        int rtn = MessageBox(CCommon::LoadText(IDS_LYRIC_SAVE_INRUARY), NULL, MB_YESNOCANCEL | MB_ICONQUESTION);
        switch (rtn)
        {
        case IDYES: if (!SaveLyric(m_lyric_path.c_str(), m_code_type)) return false;
        case IDNO: m_modified = false; break;
        default: return false;
        }
    }
    return true;
}

void CLyricEditDlg::SetLyricPathEditText()
{
    if (m_inner_lyric)
        SetDlgItemText(IDC_LYRIC_PATH_EDIT2, CCommon::LoadText(IDS_INNER_LYRIC));
    else
        SetDlgItemText(IDC_LYRIC_PATH_EDIT2, m_lyric_path.c_str());
}

CRect CLyricEditDlg::CalculateEditCtrlRect()
{
    CRect edit_rect;
    GetClientRect(edit_rect);

    CRect path_edit_rect;
    GetDlgItem(IDC_LYRIC_PATH_EDIT2)->GetWindowRect(path_edit_rect);
    ScreenToClient(path_edit_rect);

    edit_rect.top = path_edit_rect.bottom + theApp.DPI(4);
    //edit_rect.left += MARGIN;
    //edit_rect.right -= MARGIN;
    edit_rect.bottom -= (STATUSBAR_HEIGHT + theApp.DPI(4));
    return edit_rect;
}

CString CLyricEditDlg::GetDialogName() const
{
    return _T("LyricEditDlg");
}

void CLyricEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLyricEditDlg, CBaseDialog)
	//ON_BN_CLICKED(IDC_INSERT_TAG_BUTTON, &CLyricEditDlg::OnBnClickedInsertTagButton)
	//ON_BN_CLICKED(IDC_REPLACE_TAG_BUTTON, &CLyricEditDlg::OnBnClickedReplaceTagButton)
	//ON_BN_CLICKED(IDC_DELETE_TAG__BUTTON, &CLyricEditDlg::OnBnClickedDeleteTag)
	//ON_BN_CLICKED(IDC_SAVE_LYRIC_BUTTON, &CLyricEditDlg::OnBnClickedSaveLyricButton)
	//ON_BN_CLICKED(IDC_SAVE_AS_BUTTON5, &CLyricEditDlg::OnBnClickedSaveAsButton5)
	ON_WM_DESTROY()
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
    ON_COMMAND(ID_LRYIC_MERGE_SAME_TIME_TAG, &CLyricEditDlg::OnLryicMergeSameTimeTag)
    ON_COMMAND(ID_LYRIC_SWAP_TEXT_AND_TRANSLATION, &CLyricEditDlg::OnLyricSwapTextAndTranslation)
    ON_COMMAND(ID_LYRIC_TIME_TAG_FORWARD, &CLyricEditDlg::OnLyricTimeTagForward)
    ON_COMMAND(ID_LYRIC_TIME_TAG_DELAY, &CLyricEditDlg::OnLyricTimeTagDelay)
END_MESSAGE_MAP()


// CLyricEditDlg 消息处理程序


BOOL CLyricEditDlg::OnInitDialog()
{
    CBaseDialog::SetMinSize(theApp.DPI(300), theApp.DPI(300));
	CBaseDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CenterWindow();

    SetIcon(theApp.m_icon_set.edit.GetIcon(true), FALSE);
    SetIcon(AfxGetApp()->LoadIcon(IDI_EDIT_D), TRUE);

    m_view = (CScintillaEditView*)RUNTIME_CLASS(CScintillaEditView)->CreateObject();
    m_view->Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, CalculateEditCtrlRect(), this, 3000);
    m_view->OnInitialUpdate();
    m_view->ShowWindow(SW_SHOW);

	//获取歌词信息
	//m_lyric_string = CPlayer::GetInstance().m_Lyrics.GetLyricsString();
	//m_lyric_path = CPlayer::GetInstance().m_Lyrics.GetPathName();
	if (CPlayer::GetInstance().IsInnerLyric())
	{
		CLyrics lyrics = CPlayer::GetInstance().m_Lyrics;
		m_lyric_string = lyrics.GetLyricsString();
		m_code_type = lyrics.GetCodeType();
        m_inner_lyric = true;
	}
	else
	{
		OpenLyric(CPlayer::GetInstance().m_Lyrics.GetPathName().c_str());
        m_inner_lyric = false;
	}
	m_original_lyric_path = m_lyric_path;
	//m_code_type = CPlayer::GetInstance().m_Lyrics.GetCodeType();
	m_current_song_name = CPlayer::GetInstance().GetFileName();

	//初始化编辑区字体
    m_view->SetFontFace(CCommon::LoadText(IDS_DEFAULT_FONT));
    m_view->SetFontSize(10);

    m_view->SetText(m_lyric_string);
    m_view->EmptyUndoBuffer();

    m_view->SetLexerLyric(theApp.m_app_setting_data.theme_color);

    SetLyricPathEditText();

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

    //为菜单添加图标
    CMenu* pMenu = GetMenu();
    if (pMenu != nullptr)
    {
        CMenuIcon::AddIconToMenuItem(pMenu->GetSafeHmenu(), ID_LYRIC_OPEN, FALSE, theApp.m_icon_set.music);
        CMenuIcon::AddIconToMenuItem(pMenu->GetSafeHmenu(), ID_LYRIC_SAVE, FALSE, theApp.m_icon_set.save_new);
        CMenuIcon::AddIconToMenuItem(pMenu->GetSafeHmenu(), ID_LYRIC_SAVE_AS, FALSE, theApp.m_icon_set.save_as);
        CMenuIcon::AddIconToMenuItem(pMenu->GetSafeHmenu(), IDCANCEL, FALSE, theApp.m_icon_set.exit);
        CMenuIcon::AddIconToMenuItem(pMenu->GetSafeHmenu(), ID_LYRIC_INSERT_TAG, FALSE, theApp.m_icon_set.exit);

        HICON icon_add_tag = CDrawCommon::LoadIconResource(IDI_ADD_TAG, theApp.DPI(16), theApp.DPI(16));
        HICON icon_replace_tag = CDrawCommon::LoadIconResource(IDI_REPLACE_TAG, theApp.DPI(16), theApp.DPI(16));
        HICON icon_delete_tag = CDrawCommon::LoadIconResource(IDI_DELETE_TAG, theApp.DPI(16), theApp.DPI(16));
        HICON icon_find = CDrawCommon::LoadIconResource(IDI_FIND, theApp.DPI(16), theApp.DPI(16));
        HICON icon_replace = CDrawCommon::LoadIconResource(IDI_REPLACE, theApp.DPI(16), theApp.DPI(16));
        CMenuIcon::AddIconToMenuItem(pMenu->GetSafeHmenu(), ID_LYRIC_INSERT_TAG, FALSE, icon_add_tag);
        CMenuIcon::AddIconToMenuItem(pMenu->GetSafeHmenu(), ID_LYRIC_REPLACE_TAG, FALSE, icon_replace_tag);
        CMenuIcon::AddIconToMenuItem(pMenu->GetSafeHmenu(), ID_LYRIC_DELETE_TAG, FALSE, icon_delete_tag);
        CMenuIcon::AddIconToMenuItem(pMenu->GetSafeHmenu(), ID_LYRIC_FIND, FALSE, icon_find);
        CMenuIcon::AddIconToMenuItem(pMenu->GetSafeHmenu(), ID_LYRIC_REPLACE, FALSE, icon_replace);
        DestroyIcon(icon_add_tag);
        DestroyIcon(icon_replace_tag);
        DestroyIcon(icon_delete_tag);
        DestroyIcon(icon_find);
        DestroyIcon(icon_replace);

        CMenuIcon::AddIconToMenuItem(pMenu->GetSafeHmenu(), ID_PLAY_PAUSE, FALSE, theApp.m_icon_set.play_pause);
        CMenuIcon::AddIconToMenuItem(pMenu->GetSafeHmenu(), ID_REW, FALSE, theApp.m_icon_set.rew_new);
        CMenuIcon::AddIconToMenuItem(pMenu->GetSafeHmenu(), ID_FF, FALSE, theApp.m_icon_set.ff_new);
    }

	//初始化工具栏
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE/* | CBRS_TOP*/ | CBRS_ALIGN_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_TOP
		/*| CBRS_GRIPPER*/ | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_LYRIC_EDIT_TOOLBAR))  //指定工具栏ID号
	{
		TRACE0("Failed to create toolbar/n");
		return -1;      // fail to create
	}
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
    int icon_size = theApp.DPI(20);
    if (icon_size < 32)
        icon_size = CCommon::IconSizeNormalize(icon_size);
	CImageList ImageList;
	ImageList.Create(icon_size, icon_size, ILC_COLOR32 | ILC_MASK, 2, 2);

	//通过ImageList对象加载图标作为工具栏的图标
	//添加图标
	ImageList.Add(CDrawCommon::LoadIconResource(IDI_ADD_TAG, icon_size, icon_size));
	ImageList.Add(CDrawCommon::LoadIconResource(IDI_REPLACE_TAG, icon_size, icon_size));
	ImageList.Add(CDrawCommon::LoadIconResource(IDI_DELETE_TAG, icon_size, icon_size));
	ImageList.Add(CDrawCommon::LoadIconResource(IDI_SAVE, icon_size, icon_size));
	ImageList.Add(CDrawCommon::LoadIconResource(IDI_PLAY_PAUSE, icon_size, icon_size));
	ImageList.Add(CDrawCommon::LoadIconResource(IDI_REW, icon_size, icon_size));
	ImageList.Add(CDrawCommon::LoadIconResource(IDI_FF, icon_size, icon_size));
	ImageList.Add(CDrawCommon::LoadIconResource(IDI_FIND, icon_size, icon_size));
	ImageList.Add(CDrawCommon::LoadIconResource(IDI_REPLACE, icon_size, icon_size));
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
	rect.top = rect.bottom - theApp.DPI(20);
	m_status_bar.Create(WS_VISIBLE | CBRS_BOTTOM, rect, this, 3);

	StatusBarSetParts(rect.Width());
	UpdateStatusbarInfo();

	m_dlg_exist = true;

    m_view->SetFocus();
	return FALSE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CLyricEditDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
    if (!SaveInquiry())
        return;

	DestroyWindow();

	//CBaseDialog::OnCancel();
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
	CBaseDialog::OnDestroy();
	m_dlg_exist = false;
	if (m_current_song_name == CPlayer::GetInstance().GetFileName() && m_lyric_saved)		//关闭歌词编辑窗口时如果正在播放的歌曲没有变，且执行过保存操作，就重新初始化歌词
    {
        if (CPlayer::GetInstance().IsInnerLyric())
        {
            CPlayer::GetInstance().IniLyrics();
        }
        else
        {
            CPlayer::GetInstance().IniLyrics(m_original_lyric_path);
        }
    }
}


//void CLyricEditDlg::OnEnChangeEdit1()
//{
//	// TODO:  如果该控件是 RICHEDIT 控件，它将不
//	// 发送此通知，除非重写 CBaseDialog::OnInitDialog()
//	// 函数并调用 CRichEditCtrl().SetEventMask()，
//	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。
//
//	// TODO:  在此添加控件通知处理程序代码
//	CString lyric_str;
//	m_lyric_edit.GetWindowText(lyric_str);
//	m_lyric_string = lyric_str;
//	m_modified = true;
//	UpdateStatusbarInfo();
//}


BOOL CLyricEditDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

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

	return CBaseDialog::PreTranslateMessage(pMsg);
}


void CLyricEditDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
    if (!SaveInquiry())
        return;

	CBaseDialog::OnClose();
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
		int rtn = MessageBox(CCommon::LoadText(IDS_LYRIC_SAVE_INRUARY), NULL, MB_YESNOCANCEL | MB_ICONQUESTION);
		switch (rtn)
		{
		case IDYES:
            if (!SaveLyric(m_lyric_path.c_str(), m_code_type))
                return;
            m_modified = false;
            break;
		case IDNO:
            break;
		default:
            return;
		}
	}

	//设置过滤器
	CString szFilter = CCommon::LoadText(IDS_LYRIC_FILE_FILTER);
	//构造打开文件对话框
	CFileDialog fileDlg(TRUE, _T("txt"), NULL, 0, szFilter, this);
	//显示打开文件对话框
	if (IDOK == fileDlg.DoModal())
	{
		//m_lyric_path = fileDlg.GetPathName();	//获取打开的文件路径
		OpenLyric(fileDlg.GetPathName());
		SetDlgItemText(IDC_LYRIC_PATH_EDIT2, m_lyric_path.c_str());
        m_view->SetText(m_lyric_string);
        m_view->EmptyUndoBuffer();
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
	//构造保存文件对话框
    CString default_path = m_lyric_path.c_str();
    if (m_inner_lyric)
        default_path = CFilePathHelper(CPlayer::GetInstance().GetCurrentFilePath()).ReplaceFileExtension(L"lrc").c_str();
	CFileDialog fileDlg(FALSE, _T("txt"), default_path, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, CCommon::LoadText(IDS_LYRIC_FILE_FILTER), this);
	//为“另存为”对话框添加一个组合选择框
	fileDlg.AddComboBox(IDC_SAVE_COMBO_BOX);
	//为组合选择框添加项目
	fileDlg.AddControlItem(IDC_SAVE_COMBO_BOX, 0, CCommon::LoadText(IDS_SAVE_AS_ANSI));
	fileDlg.AddControlItem(IDC_SAVE_COMBO_BOX, 1, CCommon::LoadText(IDS_SAVE_AS_UTF8));
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
                {
                    CScintillaEditView::KeepCurrentLine keep_cur_line(m_view);
                    m_view->SetText(m_lyric_string);
                }
                m_modified = true;
				UpdateStatusbarInfo();
				OnFindNext();
                m_view->SetSel(m_find_index, m_find_index + m_replace_str.size(), m_lyric_string);	//选中替换的字符串
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
				int index_offset = m_replace_str.size() - m_find_str.size();
				if (index_offset < 0)
					index_offset = 0;
				m_find_index = m_lyric_string.find(m_find_str, m_find_index + index_offset + 1);	//查找字符串
				if (m_find_index == string::npos)
					break;
				m_lyric_string.replace(m_find_index, m_find_str.size(), m_replace_str.c_str(), m_replace_str.size());	//替换找到的字符串
				replace_count++;
			}
            {
                CScintillaEditView::KeepCurrentLine keep_cur_line(m_view);
                m_view->SetText(m_lyric_string);
            }
            m_modified = true;
			UpdateStatusbarInfo();
			if (replace_count != 0)
			{
				CString info;
				info = CCommon::LoadTextFormat(IDS_REPLACE_COMPLETE_INFO, { replace_count });
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
		info = CCommon::LoadTextFormat(IDS_CONNOT_FIND_STRING, { m_find_str });
		MessageBox(info, NULL, MB_OK | MB_ICONINFORMATION);
		m_find_flag = false;
	}
	else
	{
        m_view->SetSel(m_find_index, m_find_index + m_find_str.size(), m_lyric_string);		//选中找到的字符串
		SetActiveWindow();		//将编辑器窗口设为活动窗口
        m_view->SetFocus();
		m_find_flag = true;
	}
}

//
//void CLyricEditDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	//限制窗口最小大小
//	lpMMI->ptMinTrackSize.x = theApp.DPI(300);		//设置最小宽度
//	lpMMI->ptMinTrackSize.y = theApp.DPI(300);		//设置最小高度
//
//	CBaseDialog::OnGetMinMaxInfo(lpMMI);
//}


void CLyricEditDlg::OnSize(UINT nType, int cx, int cy)
{
	CBaseDialog::OnSize(nType, cx, cy);

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

		if (m_view->GetSafeHwnd() != NULL)
		{
            m_view->MoveWindow(CalculateEditCtrlRect());
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
    {
        CScintillaEditView::KeepCurrentLine keep_cur_line(m_view);
        m_view->SetText(m_lyric_string);
    }
    m_modified = true;
	UpdateStatusbarInfo();
}


void CLyricEditDlg::OnLeTranslateToTranditionalChinese()
{
	// TODO: 在此添加命令处理程序代码
	m_lyric_string = CCommon::TranslateToTranditionalChinese(m_lyric_string);
    {
        CScintillaEditView::KeepCurrentLine keep_cur_line(m_view);
        m_view->SetText(m_lyric_string);
    }
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
	CString tipInfo;
	switch (nID)
	{
	case ID_LYRIC_INSERT_TAG:
		tipInfo = CCommon::LoadText(IDS_INSERT_TIME_TAG_TIP);
		break;
	case ID_LYRIC_REPLACE_TAG:
		tipInfo = CCommon::LoadText(IDS_REPLACE_TIME_TAG_TIP);
		break;
	case ID_LYRIC_DELETE_TAG:
		tipInfo = CCommon::LoadText(IDS_DELETE_TIME_TAG_TIP);
		break;
	case ID_LYRIC_SAVE:
		tipInfo = CCommon::LoadText(IDS_SAVE_TIP);
		break;
	case ID_PLAY_PAUSE:
		tipInfo = CCommon::LoadText(IDS_PLAY_PAUSE_TIP);
		break;
	case ID_REW:
		tipInfo = CCommon::LoadText(IDS_REWIND_TIP);
		break;
	case ID_FF:
		tipInfo = CCommon::LoadText(IDS_FAST_FOWARD_TIP);
		break;
	case ID_LYRIC_FIND:
		tipInfo = CCommon::LoadText(IDS_FIND_TIP);
		break;
	case ID_LYRIC_REPLACE:
		tipInfo = CCommon::LoadText(IDS_REPLACE_TIP);
		break;
	}

	CCommon::WStringCopy(pT->szText, 80, tipInfo.GetString());

	return 0;
}


void CLyricEditDlg::OnLryicMergeSameTimeTag()
{
    // TODO: 在此添加命令处理程序代码
    CLyrics lyrics;
    lyrics.LyricsFromRowString(m_lyric_string);
    lyrics.CombineSameTimeLyric();
    m_lyric_string = lyrics.GetLyricsString2();
    {
        CScintillaEditView::KeepCurrentLine keep_cur_line(m_view);
        m_view->SetText(m_lyric_string);
    }
    m_modified = true;
    UpdateStatusbarInfo();
}


void CLyricEditDlg::OnLyricSwapTextAndTranslation()
{
    // TODO: 在此添加命令处理程序代码
    CLyrics lyrics;
    lyrics.LyricsFromRowString(m_lyric_string);
    lyrics.SwapTextAndTranslation();
    m_lyric_string = lyrics.GetLyricsString2();
    {
        CScintillaEditView::KeepCurrentLine keep_cur_line(m_view);
        m_view->SetText(m_lyric_string);
    }
    m_modified = true;
    UpdateStatusbarInfo();
}


void CLyricEditDlg::OnLyricTimeTagForward()
{
    // TODO: 在此添加命令处理程序代码
    CLyrics lyrics;
    lyrics.LyricsFromRowString(m_lyric_string);
    lyrics.TimeTagForward();
    m_lyric_string = lyrics.GetLyricsString2();
    {
        CScintillaEditView::KeepCurrentLine keep_cur_line(m_view);
        m_view->SetText(m_lyric_string);
    }
    m_modified = true;
    UpdateStatusbarInfo();
}


void CLyricEditDlg::OnLyricTimeTagDelay()
{
    // TODO: 在此添加命令处理程序代码
    CLyrics lyrics;
    lyrics.LyricsFromRowString(m_lyric_string);
    lyrics.TimeTagDelay();
    m_lyric_string = lyrics.GetLyricsString2();
    {
        CScintillaEditView::KeepCurrentLine keep_cur_line(m_view);
        m_view->SetText(m_lyric_string);
    }
    m_modified = true;
    UpdateStatusbarInfo();
}


BOOL CLyricEditDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    // TODO: 在此添加专用代码和/或调用基类
    SCNotification *notification = reinterpret_cast<SCNotification *>(lParam);
    if (notification->nmhdr.hwndFrom == m_view->GetSafeHwnd())
    {
        //响应编辑器文本变化
        if (notification->nmhdr.code == SCN_MODIFIED && m_view->IsEditChangeNotificationEnable())
        {
            UINT marsk = (SC_MOD_DELETETEXT | SC_MOD_INSERTTEXT | SC_PERFORMED_UNDO | SC_PERFORMED_REDO);
            if ((notification->modificationType & marsk) != 0)
            {
                m_view->GetText(m_lyric_string);
                m_modified = true;
                UpdateStatusbarInfo();
            }
        }
    }

    return CBaseDialog::OnNotify(wParam, lParam, pResult);
}
