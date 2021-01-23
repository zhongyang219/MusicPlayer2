// ScintillaEditView.cpp: 实现文件
//

#include "stdafx.h"
#include "ScintillaEditView.h"
#include "Common.h"


// CScintillaEditView

IMPLEMENT_DYNCREATE(CScintillaEditView, CView)

CScintillaEditView::CScintillaEditView()
{

}

CScintillaEditView::~CScintillaEditView()
{
}

BEGIN_MESSAGE_MAP(CScintillaEditView, CView)
    ON_WM_PAINT()
    ON_WM_RBUTTONUP()
END_MESSAGE_MAP()


// CScintillaEditView 绘图

void CScintillaEditView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO:  在此添加绘制代码
}


// CScintillaEditView 诊断

#ifdef _DEBUG
void CScintillaEditView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CScintillaEditView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif
#endif //_DEBUG


void CScintillaEditView::SetText(const wstring& text)
{
    m_change_notification_enable = false;       //确保正在执行SetText时不响应文本改变消息
    bool is_read_onle = IsReadOnly();
    //执行设置文件前，如果编辑器的只读的，则取消只读
    if (is_read_onle)
        SetReadOnly(false);
    int size = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), text.size(), NULL, 0, NULL, NULL);
    if (size > 0)
    {
        char* str = new char[size + 1];
        WideCharToMultiByte(CP_UTF8, 0, text.c_str(), text.size(), str, size, NULL, NULL);
        SendMessage(SCI_SETTEXT, size, (LPARAM)str);
        delete[] str;
    }
    else
    {
        SendMessage(SCI_SETTEXT, 0, (LPARAM)"");
    }
    //恢复只读状态
    if (is_read_onle)
        SetReadOnly(true);
    m_change_notification_enable = true;
}

void CScintillaEditView::GetText(wstring& text)
{
    text.clear();
    int size{};
    const wchar_t* str_unicode = GetText(size);
    if (size == 0)
        return;
    text.assign(str_unicode, size);
    delete[] str_unicode;
}

const wchar_t* CScintillaEditView::GetText(int& size)
{
    auto length = SendMessage(SCI_GETLENGTH);
    char* buf = new char[length + 1];
    SendMessage(SCI_GETTEXT, length + 1, reinterpret_cast<LPARAM>(buf));

    size = MultiByteToWideChar(CP_UTF8, 0, buf, length, NULL, 0);
    if (size <= 0) return nullptr;
    wchar_t* str_unicode = new wchar_t[size + 1];
    MultiByteToWideChar(CP_UTF8, 0, buf, length, str_unicode, size);
    //text.assign(str_unicode, size);
    //delete[] str_unicode;
    delete[] buf;
    return str_unicode;
}

const char * CScintillaEditView::GetTextUtf8(int & size)
{
    size = SendMessage(SCI_GETLENGTH);
    char* buf = new char[size + 1];
    SendMessage(SCI_GETTEXT, size + 1, reinterpret_cast<LPARAM>(buf));
    return buf;
}

void CScintillaEditView::SetFontFace(const wchar_t* font_face)
{
    string str_font_face = CCommon::UnicodeToStr(font_face, CodeType::UTF8_NO_BOM);
    SendMessage(SCI_STYLESETFONT, STYLE_DEFAULT, (sptr_t)str_font_face.c_str());
}

void CScintillaEditView::SetFontSize(int font_size)
{
    SendMessage(SCI_STYLESETSIZE, STYLE_DEFAULT, font_size);
}

void CScintillaEditView::SetTabSize(int tab_size)
{
    SendMessage(SCI_SETTABWIDTH, tab_size);
}

void CScintillaEditView::SetSel(int start, int end, const wstring& edit_str)
{
    int byte_start = CharactorPosToBytePos(start, edit_str.c_str(), edit_str.size());
    int byte_end = CharactorPosToBytePos(end, edit_str.c_str(), edit_str.size());
    int length = SendMessage(SCI_GETLENGTH);
    if (start >= static_cast<int>(edit_str.size()))
        byte_start = length;
    if (end >= static_cast<int>(edit_str.size()))
        byte_end = length;
    SendMessage(SCI_SETSEL, byte_start, byte_end);
}

void CScintillaEditView::GetSel(int & start, int & end)
{
    int byte_start = SendMessage(SCI_GETANCHOR);
    int byte_end = SendMessage(SCI_GETCURRENTPOS);
    if (byte_end < byte_start)
        std::swap(byte_start, byte_end);
    int size{};
    const char* str = GetTextUtf8(size);
    start = BytePosToCharactorPos(byte_start, str, size);
    end = BytePosToCharactorPos(byte_end, str, size);
    delete[] str;
}

void CScintillaEditView::SetBackgroundColor(COLORREF color)
{
    m_background_color = color;
    //SendMessage(SCI_STYLESETBACK, STYLE_DEFAULT, m_background_color);
}

void CScintillaEditView::SetReadOnly(bool read_only)
{
    SendMessage(SCI_SETREADONLY, read_only);
}

bool CScintillaEditView::IsReadOnly()
{
    return (SendMessage(SCI_GETREADONLY) != 0);
}

void CScintillaEditView::Undo()
{
    SendMessage(SCI_UNDO);
}

void CScintillaEditView::Redo()
{
    SendMessage(SCI_REDO);
}

void CScintillaEditView::Cut()
{
    SendMessage(SCI_CUT);
}

void CScintillaEditView::Copy()
{
    SendMessage(SCI_COPY);
}

void CScintillaEditView::Paste()
{
    SendMessage(SCI_PASTE);
}

void CScintillaEditView::SelectAll()
{
    SendMessage(SCI_SELECTALL);
}

void CScintillaEditView::EmptyUndoBuffer()
{
    SendMessage(SCI_EMPTYUNDOBUFFER);
}

void CScintillaEditView::SetWordWrap(bool word_wrap)
{
    SendMessage(SCI_SETWRAPMODE, word_wrap ? SC_WRAP_WORD : SC_WRAP_NONE);
}


bool CScintillaEditView::IsEditChangeNotificationEnable()
{
    return m_change_notification_enable;
}

bool CScintillaEditView::CanUndo()
{
    return (SendMessage(SCI_CANUNDO) != 0);
}

bool CScintillaEditView::CanRedo()
{
    return (SendMessage(SCI_CANREDO) != 0);
}

bool CScintillaEditView::CanPaste()
{
    return (SendMessage(SCI_CANPASTE) != 0);
}

bool CScintillaEditView::IsSelectionEmpty()
{
    int anchor = SendMessage(SCI_GETANCHOR);
    int current_pos = SendMessage(SCI_GETCURRENTPOS);
    return anchor == current_pos;
}

bool CScintillaEditView::IsModified()
{
    return (SendMessage(SCI_GETMODIFY) != 0);
}

void CScintillaEditView::SetSavePoint()
{
    SendMessage(SCI_SETSAVEPOINT);
}

void CScintillaEditView::SetLineNumberWidth(int width)
{
    m_line_number_width = width;
}

void CScintillaEditView::ShowLineNumber(bool show)
{
    if (show)
        SendMessage(SCI_SETMARGINWIDTHN, SCINTILLA_MARGIN_LINENUMBER, m_line_number_width);
    else
        SendMessage(SCI_SETMARGINWIDTHN, SCINTILLA_MARGIN_LINENUMBER, 0);
}

void CScintillaEditView::SetLineNumberColor(COLORREF color)
{
    //SendMessage(SCI_STYLESETFORE, STYLE_LINENUMBER, color);
    m_line_number_color = color;
}

int CScintillaEditView::GetZoom()
{
    return SendMessage(SCI_GETZOOM);
}

void CScintillaEditView::SetZoom(int zoom)
{
    SendMessage(SCI_SETZOOM, zoom);
}

void CScintillaEditView::SetEolMode(eEolMode eolMode)
{
    int mode = 0;
    switch (eolMode)
    {
    case EOL_CRLF:
        mode = SC_EOL_CRLF;
        break;
    case EOL_CR:
        mode = SC_EOL_CR;
        break;
    case EOL_LF:
        mode = SC_EOL_LF;
        break;
    default:
        break;
    }
    SendMessage(SCI_SETEOLMODE, mode);
}

CScintillaEditView::eEolMode CScintillaEditView::GetEolMode()
{
    int mode = SendMessage(SCI_GETEOLMODE);
    switch (mode)
    {
    case SC_EOL_CR:
        return EOL_CR;
    case SC_EOL_LF:
        return EOL_LF;
    default:
        return EOL_CRLF;
        break;
    }
}

void CScintillaEditView::ConvertEolMode(eEolMode eolMode)
{
    int mode = 0;
    switch (eolMode)
    {
    case EOL_CRLF:
        mode = SC_EOL_CRLF;
        break;
    case EOL_CR:
        mode = SC_EOL_CR;
        break;
    case EOL_LF:
        mode = SC_EOL_LF;
        break;
    default:
        break;
    }
    SendMessage(SCI_CONVERTEOLS, mode);
}

void CScintillaEditView::SetViewEol(bool show)
{
    SendMessage(SCI_SETVIEWEOL, show);
}

int CScintillaEditView::GetFirstVisibleLine()
{
    return SendMessage(SCI_GETFIRSTVISIBLELINE);
}

void CScintillaEditView::SetFirstVisibleLine(int line)
{
    SendMessage(SCI_SETFIRSTVISIBLELINE, line);
}

void CScintillaEditView::SetLexer(int lexer)
{
    SendMessage(SCI_SETLEXER, lexer);
}

void CScintillaEditView::SetKeywords(int id, const char* keywords)
{
    SendMessage(SCI_SETKEYWORDS, id, (sptr_t)keywords);
}

void CScintillaEditView::SetSyntaxColor(int id, COLORREF color)
{
    SendMessage(SCI_STYLESETFORE, id, color);
}

void CScintillaEditView::SetSyntaxFontStyle(int id, bool bold, bool italic)
{
    SendMessage(SCI_STYLESETBOLD, id, bold);
    SendMessage(SCI_STYLESETITALIC, id, italic);
}

void CScintillaEditView::SetLexerNormalText()
{
    SetLexer(SCLEX_NULL);
    SendMessage(SCI_STYLESETFORE, STYLE_DEFAULT, RGB(0, 0, 0));
    SendMessage(SCI_STYLESETBACK, STYLE_DEFAULT, m_background_color);
    SendMessage(SCI_STYLESETBOLD, STYLE_DEFAULT, 0);
    SendMessage(SCI_STYLESETBOLD, STYLE_DEFAULT, 0);
    SendMessage(SCI_STYLECLEARALL);

    SendMessage(SCI_STYLESETFORE, STYLE_LINENUMBER, m_line_number_color);
}

CScintillaEditView::eEolMode CScintillaEditView::JudgeEolMode(const wstring& str)
{
    size_t index = str.find(L"\r\n");
    if (index != wstring::npos)
        return EOL_CRLF;

    index = str.find(L'\n');
    if (index != wstring::npos)
        return EOL_LF;
    
    index = str.find(L'\r');
    if (index != wstring::npos)
        return EOL_CR;

    return EOL_CRLF;

}

int CScintillaEditView::CharactorPosToBytePos(int pos, const wchar_t * str, size_t size)
{
    if (pos >= size)
        return size;
    else
        return WideCharToMultiByte(CP_UTF8, 0, str, pos, NULL, 0, NULL, NULL);
}

int CScintillaEditView::BytePosToCharactorPos(int pos, const char * str, size_t size)
{
    if (pos >= size)
        return size;
    else
        return MultiByteToWideChar(CP_UTF8, 0, str, pos, NULL, 0);
}


void CScintillaEditView::SetContextMenu(CMenu* pMenu, CWnd* pMenuOwner)
{
    if (pMenu != nullptr)
    {
        m_pMenu = pMenu;
        m_pContextMenuOwner = pMenuOwner;
        SendMessage(SCI_USEPOPUP, SC_POPUP_NEVER);
    }

}

void CScintillaEditView::SetLexerLyric(ColorTable theme_color)
{
    //设置LRC歌词的语法解析
    SetLexerNormalText();
    SetLexer(SCLEX_LYRIC);
    SetKeywords(0, "ar ti al by id");
    SetSyntaxColor(SCE_LYRIC_TIMETAG, theme_color.dark1_5);
    SetSyntaxColor(SCE_LYRIC_TIME_TAG_KEYWORD, theme_color.dark1_5);
    SetSyntaxFontStyle(SCE_LYRIC_TIME_TAG_KEYWORD, true, false);
    SetSyntaxColor(SCE_LYRIC_SEPARATOR, theme_color.light1);
    SetSyntaxColor(SCE_LYRIC_TRANSLATION, theme_color.dark2_5);

    //设置当前行背景色
    SendMessage(SCI_SETCARETLINEVISIBLE, TRUE);
    SendMessage(SCI_SETCARETLINEBACK, theme_color.light3);
}

// CScintillaEditView 消息处理程序


BOOL CScintillaEditView::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: 在此添加专用代码和/或调用基类
    cs.lpszClass = _T("Scintilla");


    return CView::PreCreateWindow(cs);
}


void CScintillaEditView::OnPaint()
{
    //CPaintDC dc(this); // device context for painting
                       // TODO: 在此处添加消息处理程序代码
                       // 不为绘图消息调用 CView::OnPaint()
    Default();
}


void CScintillaEditView::PreSubclassWindow()
{
    // TODO: 在此添加专用代码和/或调用基类

    CView::PreSubclassWindow();
}


void CScintillaEditView::OnInitialUpdate()
{
    CView::OnInitialUpdate();

    // TODO: 在此添加专用代码和/或调用基类
    SendMessage(SCI_SETCODEPAGE, SC_CP_UTF8);       //总是使用Unicode
    SendMessage(SCI_SETMARGINTYPEN, SCINTILLA_MARGIN_LINENUMBER, SC_MARGIN_NUMBER);

    SendMessage(SCI_SETSCROLLWIDTH, 100);
    SendMessage(SCI_SETSCROLLWIDTHTRACKING, TRUE);
}


void CScintillaEditView::OnRButtonUp(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    if (m_pMenu != nullptr)
    {
        CPoint point1;
        GetCursorPos(&point1);
        m_pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, m_pContextMenuOwner);
    }


    CView::OnRButtonUp(nFlags, point);
}
