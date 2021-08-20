// LyricsWindow.cpp : 实现文件
//

#include "stdafx.h"
#include "LyricsWindow.h"

// CLyricsWindow

const Gdiplus::REAL TRANSLATE_FONT_SIZE_FACTOR = 0.88f;		//歌词翻译文本大小占歌词文本大小的比例

IMPLEMENT_DYNAMIC(CLyricsWindow, CWnd)

CLyricsWindow::CLyricsWindow()
{
	HDC hDC=::GetDC(NULL);
	m_hCacheDC=::CreateCompatibleDC(hDC);
	::ReleaseDC(NULL,hDC);
	//---------------------------------
	m_nHighlight=NULL ; //高亮歌词的百分比 0--100
	m_TextGradientMode=LyricsGradientMode_Two ; //普通歌词渐变模式
	m_pTextPen=NULL ; //普通歌词边框画笔
	m_HighlightGradientMode=LyricsGradientMode_Two ; //高亮歌词渐变模式
	m_pHighlightPen=NULL ; //高亮歌词边框画笔
	m_pShadowBrush=NULL ; //阴影画刷,GDIPlus画刷
	m_nShadowOffset=1 ; //阴影偏移
	m_pFont=NULL ; //GDIPlus字体
	m_FontStyle=NULL ;
	m_FontSize=NULL ;
	m_pTextFormat=NULL;
	//---------------------------------
	m_pFontFamily=new Gdiplus::FontFamily();
	m_pTextFormat=new Gdiplus::StringFormat();
	m_pTextFormat->SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);//不换行
	m_pTextFormat->SetAlignment(Gdiplus::StringAlignmentCenter); //置水平对齐方式
	m_pTextFormat->SetLineAlignment(Gdiplus::StringAlignmentNear); //置垂直对齐方式
	//---------------------------------
	//SetLyricsFont(L"微软雅黑", 40, Gdiplus::FontStyle::FontStyleRegular);
	//SetLyricsColor(Gdiplus::Color::Red,Gdiplus::Color(255,172,0),LyricsGradientMode_Three);
	//SetLyricsBorder(Gdiplus::Color::Black,1);
	SetLyricsShadow(Gdiplus::Color(150,0,0,0),1);
	//SetHighlightColor(Gdiplus::Color(255,100,26),Gdiplus::Color(255,255,0),LyricsGradientMode_Three);
	//SetHighlightBorder(Gdiplus::Color::Black,1);

}

CLyricsWindow::~CLyricsWindow()
{
	if(m_pTextPen){
		delete m_pTextPen;
		m_pTextPen=NULL;
	}
	if(m_pHighlightPen){
		delete m_pHighlightPen;
		m_pHighlightPen=NULL;
	}
	if(m_pShadowBrush){
		delete m_pShadowBrush;
		m_pShadowBrush=NULL;
	}
	if(m_pFontFamily){
		delete m_pFontFamily;
		m_pFontFamily=NULL;
	}
	if(m_pTextFormat){
		delete m_pTextFormat;
		m_pTextFormat=NULL;
	}
	if(m_pFont){
		delete m_pFont;
		m_pFont=NULL;
	}
}


BEGIN_MESSAGE_MAP(CLyricsWindow, CWnd)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



BOOL CLyricsWindow::Create(int nHeight)
{
	return CLyricsWindow::Create(_T("CometLyricsWindow"), -1, nHeight);
}
BOOL CLyricsWindow::Create(LPCTSTR lpszClassName)
{
	return CLyricsWindow::Create(lpszClassName,-1,-1);
}
BOOL CLyricsWindow::Create(LPCTSTR lpszClassName,int nWidth,int nHeight)
{
	if(!RegisterWndClass(lpszClassName))
	{
		TRACE("Class　Registration　Failedn");
	}

	//--------------------------------------------
	//取出桌面工作区域
	RECT rcWork;
	SystemParametersInfo (SPI_GETWORKAREA,NULL,&rcWork,NULL);
	int nWorkWidth=rcWork.right-rcWork.left;
	int nWorkHeight=rcWork.bottom-rcWork.top;
	//未传递宽度、高度参数时设置个默认值
	if(nWidth<0)nWidth=nWorkWidth*2/3;      //默认宽度为桌面宽度的2/3
	if(nHeight<0)nHeight=150;
	//设置左边、顶边位置,让窗口在屏幕下方
	int x=rcWork.left+( (nWorkWidth-nWidth)/2 );
	int y=rcWork.bottom-nHeight;
	//--------------------------------------------
	DWORD dwStyle=WS_POPUP|WS_VISIBLE|WS_THICKFRAME;
	DWORD dwExStyle=WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_LAYERED;
    BOOL rtn = CWnd::CreateEx(dwExStyle, lpszClassName, NULL, dwStyle, x, y, nWidth, nHeight, NULL, NULL);

    return rtn;
}
BOOL CLyricsWindow::RegisterWndClass(LPCTSTR lpszClassName)
{
	HINSTANCE hInstance=AfxGetInstanceHandle();
	WNDCLASSEX wndcls;
	memset(&wndcls,0,sizeof(WNDCLASSEX));
	wndcls.cbSize=sizeof(WNDCLASSEX);
	if(GetClassInfoEx(hInstance,lpszClassName,&wndcls))
	{
		return TRUE;
	}
	if(GetClassInfoEx(NULL,lpszClassName,&wndcls))
	{
		return TRUE;
	}

	wndcls.style=CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW;
	wndcls.lpfnWndProc=::DefWindowProc;
	wndcls.hInstance=hInstance;
	wndcls.hIcon=NULL;
	wndcls.hCursor=::LoadCursor(NULL,IDC_ARROW);
	wndcls.hbrBackground=(HBRUSH)(COLOR_BTNFACE+1);
	wndcls.lpszMenuName=NULL;
	wndcls.lpszClassName=lpszClassName;
	if(!RegisterClassEx(&wndcls))
	{
		return FALSE;
	}
	return TRUE;
}


//更新歌词(歌词文本,高亮进度百分比)
void CLyricsWindow::UpdateLyrics(LPCTSTR lpszLyrics,int nHighlight)
{
    m_lpszLyrics = lpszLyrics;
    UpdateLyrics(nHighlight);
}
//更新高亮进度(高亮进度百分比)
void CLyricsWindow::UpdateLyrics(int nHighlight)
{
	m_nHighlight=nHighlight;
	if(m_nHighlight<0)
		m_nHighlight=0;
	if(m_nHighlight>1000)
		m_nHighlight=1000;
	Draw();
}

void CLyricsWindow::UpdateLyricTranslate(LPCTSTR lpszLyricTranslate)
{
	m_strTranslate = lpszLyricTranslate;
}

//重画歌词窗口
void CLyricsWindow::Draw()
{
	//CRect rcWindow;
	GetWindowRect(m_rcWindow);
	m_nWidth= m_rcWindow.Width();
	m_nHeight= m_rcWindow.Height();
    CRect rcClient;
    GetClientRect(rcClient);
    m_frameSize.cx = (m_rcWindow.Width() - rcClient.Width()) / 2;
    m_frameSize.cy = (m_rcWindow.Height() - rcClient.Height()) / 2;

	//----------------------------------
	BITMAPINFO bitmapinfo;
	bitmapinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapinfo.bmiHeader.biBitCount = 32;
	bitmapinfo.bmiHeader.biHeight = m_nHeight;
	bitmapinfo.bmiHeader.biWidth = m_nWidth;
	bitmapinfo.bmiHeader.biPlanes = 1;
	bitmapinfo.bmiHeader.biCompression=BI_RGB;
	bitmapinfo.bmiHeader.biXPelsPerMeter=0;
	bitmapinfo.bmiHeader.biYPelsPerMeter=0;
	bitmapinfo.bmiHeader.biClrUsed=0;
	bitmapinfo.bmiHeader.biClrImportant=0;
	bitmapinfo.bmiHeader.biSizeImage = bitmapinfo.bmiHeader.biWidth * bitmapinfo.bmiHeader.biHeight * bitmapinfo.bmiHeader.biBitCount / 8;
	HBITMAP hBitmap=CreateDIBSection (m_hCacheDC,&bitmapinfo, 0,NULL, 0, 0);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject (m_hCacheDC,hBitmap);
	//----------------------------------
	Gdiplus::Graphics* pGraphics=new Gdiplus::Graphics(m_hCacheDC);
	pGraphics->SetSmoothingMode (Gdiplus::SmoothingModeAntiAlias);
	pGraphics->SetTextRenderingHint (Gdiplus::TextRenderingHintAntiAlias);

    PreDrawLyric(pGraphics);
    bool bDrawTranslate = m_bShowTranslate && !m_strTranslate.IsEmpty();
    if (m_bDoubleLine && !m_strNextLyric.IsEmpty() && !bDrawTranslate)
        DrawLyricsDoubleLine(pGraphics);
    else
        DrawLyrics(pGraphics);
    AfterDrawLyric(pGraphics);

	delete pGraphics;
	//----------------------------------
	//设置透明窗口
	CPoint DestPt(0,0);
	CSize psize(m_nWidth,m_nHeight);
	BLENDFUNCTION blendFunc32bpp;
	blendFunc32bpp.AlphaFormat = AC_SRC_ALPHA;
	blendFunc32bpp.BlendFlags = 0;
	blendFunc32bpp.BlendOp = AC_SRC_OVER;
	blendFunc32bpp.SourceConstantAlpha = m_alpha;
	HDC hDC=::GetDC(m_hWnd);
	::UpdateLayeredWindow(m_hWnd,hDC,NULL,&psize,m_hCacheDC,&DestPt,0,&blendFunc32bpp,ULW_ALPHA);
	//----------------------------------
	//释放资源
	::SelectObject (m_hCacheDC,hOldBitmap);
	::DeleteObject(hBitmap);
	::ReleaseDC(m_hWnd,hDC);
}

void CLyricsWindow::DrawLyricText(Gdiplus::Graphics* pGraphics, LPCTSTR strText, Gdiplus::RectF rect, bool bDrawHighlight, bool bDrawTranslate)
{
	Gdiplus::REAL fontSize = bDrawTranslate ? m_FontSize * TRANSLATE_FONT_SIZE_FACTOR : m_FontSize;
	if (fontSize < 1)
		fontSize = m_FontSize;

    Gdiplus::REAL textWidth = rect.Width;
    Gdiplus::REAL highlighWidth = rect.Width * m_nHighlight / 1000;

    if (!bDrawHighlight && !bDrawTranslate)
    {
        if (rect.X < 0)
            rect.X = 0;
    }
    else
    {
        //如果文本宽度大于控件宽度，就要根据分割的位置滚动文本
        if (textWidth > m_nWidth)
        {
            //如果分割的位置（歌词进度）剩下的宽度已经小于控件宽度的一半，此时使文本右侧和控件右侧对齐
            if (textWidth - highlighWidth < m_nWidth / 2)
            {
                rect.X = m_nWidth - textWidth;
            }
            //分割位置剩下的宽度还没有到小于控件宽度的一半，但是分割位置的宽度已经大于控件宽度的一半时，需要移动文本使分割位置正好在控件的中间
            else if (highlighWidth > m_nWidth / 2)
            {
                rect.X = m_nWidth / 2 - highlighWidth;
            }
            //分割位置还不到控件宽度的一半时，使文本左侧和控件左侧对齐
            else
            {
                rect.X = 0;
            }
        }
    }

	//-----------------------------------------------------------
	//画出阴影
	if (m_pShadowBrush) {
		Gdiplus::RectF layoutRect(0, 0, 0, 0);
		layoutRect = rect;
		layoutRect.X = layoutRect.X + m_nShadowOffset;
		layoutRect.Y = layoutRect.Y + m_nShadowOffset;
		Gdiplus::GraphicsPath* pShadowPath = new Gdiplus::GraphicsPath(Gdiplus::FillModeAlternate);//创建路径
		pShadowPath->AddString(strText, -1, m_pFontFamily, m_FontStyle, fontSize, layoutRect, m_pTextFormat); //把文字加入路径
		pGraphics->FillPath(m_pShadowBrush, pShadowPath);//填充路径
		delete pShadowPath; //销毁路径
	}

	//-----------------------------------------------------------
	//画出歌词
	Gdiplus::GraphicsPath* pStringPath = new Gdiplus::GraphicsPath(Gdiplus::FillModeAlternate);//创建路径
	pStringPath->AddString(strText, -1, m_pFontFamily, m_FontStyle, fontSize, rect, m_pTextFormat); //把文字加入路径
	if (m_pTextPen) {
		pGraphics->DrawPath(m_pTextPen, pStringPath);//画路径,文字边框
	}
	Gdiplus::Brush* pBrush = CreateGradientBrush(m_TextGradientMode, m_TextColor1, m_TextColor2, rect);
	pGraphics->FillPath(pBrush, pStringPath);//填充路径
	delete pBrush;//销毁画刷
	if(bDrawHighlight)
		DrawHighlightLyrics(pGraphics, pStringPath, rect);
	delete pStringPath; //销毁路径
}

//绘制歌词
void CLyricsWindow::DrawLyrics(Gdiplus::Graphics* pGraphics)
{
    int lyricHeight = m_nHeight - m_toobar_height;
	//先取出文字宽度和高度
	Gdiplus::RectF layoutRect(0,0,0,0);
	Gdiplus::RectF boundingBox;
	pGraphics->MeasureString (m_lpszLyrics, -1, m_pFont,layoutRect, m_pTextFormat,&boundingBox, 0, 0);
    boundingBox.Width += 1;     //测量到的文本宽度加1，以防止出现使用某些字体时，最后一个字符无法显示的问题
	//计算歌词画出的位置
	Gdiplus::RectF dstRect;		//文字的矩形
	Gdiplus::RectF transRect;	//翻译文本的矩形
    bool bDrawTranslate = m_bShowTranslate && !m_strTranslate.IsEmpty();
	if(!bDrawTranslate)
	{
        switch (m_alignment)
        {
        case Alignment::LEFT:
            dstRect = Gdiplus::RectF(0, m_toobar_height + (lyricHeight - boundingBox.Height) / 2, boundingBox.Width, boundingBox.Height);
            break;
        case Alignment::RIGHT:
            dstRect = Gdiplus::RectF(m_nWidth - boundingBox.Width, m_toobar_height + (lyricHeight - boundingBox.Height) / 2, boundingBox.Width, boundingBox.Height);
            break;
        //居中
        default:
            dstRect = Gdiplus::RectF((m_nWidth - boundingBox.Width) / 2, m_toobar_height + (lyricHeight - boundingBox.Height) / 2, boundingBox.Width, boundingBox.Height);
        }
    }
	else
	{
		Gdiplus::RectF transBoundingBox;
		pGraphics->MeasureString(m_strTranslate, -1, m_pFont, layoutRect, m_pTextFormat, &transBoundingBox, 0, 0);
        transBoundingBox.Width += 1;     //测量到的文本宽度加1，以防止出现使用某些字体时，最后一个字符无法显示的问题
        Gdiplus::REAL translateHeight = transBoundingBox.Height * TRANSLATE_FONT_SIZE_FACTOR;
		Gdiplus::REAL translateWidth = transBoundingBox.Width * TRANSLATE_FONT_SIZE_FACTOR;
		Gdiplus::REAL gapHeight = boundingBox.Height * 0.2f;	//歌词和翻译之间的间隙
		Gdiplus::REAL height = boundingBox.Height + gapHeight + translateHeight;
        switch (m_alignment)
        {
        case Alignment::LEFT:
            dstRect = Gdiplus::RectF(0, m_toobar_height + (lyricHeight - height) / 2, boundingBox.Width, boundingBox.Height);
            transRect = Gdiplus::RectF(0, dstRect.GetBottom() + gapHeight, translateWidth, translateHeight);
            break;
        case Alignment::RIGHT:
            dstRect = Gdiplus::RectF((m_nWidth - boundingBox.Width), m_toobar_height + (lyricHeight - height) / 2, boundingBox.Width, boundingBox.Height);
            transRect = Gdiplus::RectF((m_nWidth - translateWidth), dstRect.GetBottom() + gapHeight, translateWidth, translateHeight);
            break;
        default:
		    dstRect = Gdiplus::RectF((m_nWidth - boundingBox.Width) / 2, m_toobar_height + (lyricHeight - height) / 2, boundingBox.Width, boundingBox.Height);
		    transRect = Gdiplus::RectF((m_nWidth - translateWidth) / 2, dstRect.GetBottom() + gapHeight, translateWidth, translateHeight);
            break;
        }
	}

	DrawLyricText(pGraphics, m_lpszLyrics, dstRect, m_lyric_karaoke_disp);
	if (bDrawTranslate)
		DrawLyricText(pGraphics, m_strTranslate, transRect, false, true);
}

void CLyricsWindow::DrawLyricsDoubleLine(Gdiplus::Graphics* pGraphics)
{
    int lyricHeight = m_nHeight - m_toobar_height;
    static bool bSwap = false;
    if (m_lyricChangeFlag)      //如果歌词发生了改变，则交换当前歌词和下一句歌词的位置
        bSwap = !bSwap;
    //先取出文字宽度和高度
    Gdiplus::RectF layoutRect(0, 0, 0, 0);
    Gdiplus::RectF boundingBox;
    pGraphics->MeasureString(m_lpszLyrics, -1, m_pFont, layoutRect, m_pTextFormat, &boundingBox, 0, 0);
    boundingBox.Width += 1;     //测量到的文本宽度加1，以防止出现使用某些字体时，最后一个字符无法显示的问题
    Gdiplus::RectF nextBoundingBox;
    pGraphics->MeasureString(m_strNextLyric, -1, m_pFont, layoutRect, m_pTextFormat, &nextBoundingBox, 0, 0);
    nextBoundingBox.Width += 1; //测量到的文本宽度加1，以防止出现使用某些字体时，最后一个字符无法显示的问题
    //计算歌词画出的位置
    Gdiplus::RectF dstRect;		//文字的矩形
    Gdiplus::RectF nextRect;	//下一句文本的矩形

    dstRect = Gdiplus::RectF(0, m_toobar_height + (lyricHeight / 2 - boundingBox.Height) / 2, boundingBox.Width, boundingBox.Height);
    nextRect = Gdiplus::RectF(m_nWidth - nextBoundingBox.Width, dstRect.Y + lyricHeight / 2, nextBoundingBox.Width, nextBoundingBox.Height);

    if (bSwap)
    {
        std::swap(dstRect.Y, nextRect.Y);
        nextRect.X = 0;
        dstRect.X = m_nWidth - dstRect.Width;
    }

    DrawLyricText(pGraphics, m_lpszLyrics, dstRect, true);
    DrawLyricText(pGraphics, m_strNextLyric, nextRect, false);
}

//绘制高亮歌词
void CLyricsWindow::DrawHighlightLyrics(Gdiplus::Graphics* pGraphics,Gdiplus::GraphicsPath* pPath, Gdiplus::RectF& dstRect)
{
	if(m_nHighlight<=0)return;
	Gdiplus::Region* pRegion=NULL;
	if(m_nHighlight<1000){
		Gdiplus::RectF CliptRect(dstRect);
		CliptRect.Width=CliptRect.Width * m_nHighlight / 1000;
		pRegion=new Gdiplus::Region(CliptRect);
		pGraphics->SetClip(pRegion, Gdiplus::CombineModeReplace);
	}
	//--------------------------------------------
	if(m_pHighlightPen){
		pGraphics->DrawPath (m_pHighlightPen,pPath);//画路径,文字边框
	}
	Gdiplus::Brush* pBrush = CreateGradientBrush(m_HighlightGradientMode, m_HighlightColor1,m_HighlightColor2,dstRect);
	pGraphics->FillPath (pBrush,pPath);//填充路径
	delete pBrush;//销毁画刷
	//--------------------------------------------
	if(pRegion){
		pGraphics->ResetClip();
		delete pRegion;
	}
}

//创建渐变画刷
Gdiplus::Brush* CLyricsWindow::CreateGradientBrush(LyricsGradientMode TextGradientMode,Gdiplus::Color& Color1,Gdiplus::Color& Color2, Gdiplus::RectF& dstRect)
{
	Gdiplus::PointF pt1;
	Gdiplus::PointF pt2;
	Gdiplus::Brush* pBrush=NULL;
	switch (TextGradientMode)
	{
	case LyricsGradientMode_Two://两色渐变
		{
			Gdiplus::PointF point1(dstRect.X,dstRect.Y);
			Gdiplus::PointF point2(dstRect.X,dstRect.Y+dstRect.Height);
			pBrush=new Gdiplus::LinearGradientBrush(point1,point2,Color1,Color2);
			((Gdiplus::LinearGradientBrush*)pBrush)->SetWrapMode(Gdiplus::WrapModeTileFlipXY);
			break;
		}

	case LyricsGradientMode_Three://三色渐变
		{
			Gdiplus::PointF point1(dstRect.X,dstRect.Y);
			Gdiplus::PointF point2(dstRect.X,dstRect.Y+dstRect.Height/2);
			pBrush=new Gdiplus::LinearGradientBrush(point1,point2,Color1,Color2);
			((Gdiplus::LinearGradientBrush*)pBrush)->SetWrapMode(Gdiplus::WrapModeTileFlipXY);
			break;
		}

	default://无渐变
		{
			pBrush=new Gdiplus::SolidBrush(Color1);
			break;
		}
	}
	return pBrush;
}

//设置歌词颜色
void CLyricsWindow::SetLyricsColor(Gdiplus::Color TextColor1)
{
	CLyricsWindow::SetLyricsColor(TextColor1,Gdiplus::Color::Black,LyricsGradientMode_None);
}
void CLyricsWindow::SetLyricsColor(Gdiplus::Color TextColor1,Gdiplus::Color TextColor2,LyricsGradientMode TextGradientMode)
{
	m_TextColor1=TextColor1;
	m_TextColor2=TextColor2;
	m_TextGradientMode=TextGradientMode;

}
//设置歌词边框
void CLyricsWindow::SetLyricsBorder(Gdiplus::Color BorderColor, Gdiplus::REAL BorderWidth)
{
	if(m_pTextPen){
		delete m_pTextPen;
		m_pTextPen=NULL;
	}
	if(BorderColor.GetA()>0 && BorderWidth>0)
		m_pTextPen=new Gdiplus::Pen(BorderColor,BorderWidth);
}
//设置高亮歌词颜色
void CLyricsWindow::SetHighlightColor(Gdiplus::Color TextColor1)
{
	CLyricsWindow::SetHighlightColor(TextColor1,Gdiplus::Color::Black,LyricsGradientMode_None);
}
void CLyricsWindow::SetHighlightColor(Gdiplus::Color TextColor1,Gdiplus::Color TextColor2,LyricsGradientMode TextGradientMode)
{
	m_HighlightColor1=TextColor1;
	m_HighlightColor2=TextColor2;
	m_HighlightGradientMode=TextGradientMode;

}
//设置高亮歌词边框
void CLyricsWindow::SetHighlightBorder(Gdiplus::Color BorderColor, Gdiplus::REAL BorderWidth)
{
	if(m_pHighlightPen){
		delete m_pHighlightPen;
		m_pHighlightPen=NULL;
	}
	if(BorderColor.GetA()>0 && BorderWidth>0)
		m_pHighlightPen=new Gdiplus::Pen(BorderColor,BorderWidth);
}
//设置歌词阴影
void CLyricsWindow::SetLyricsShadow(Gdiplus::Color ShadowColor,int nShadowOffset)
{
	if(m_pShadowBrush){
		delete m_pShadowBrush;
		m_pShadowBrush=NULL;
	}
	if(ShadowColor.GetA()>0 && nShadowOffset>0){
		m_nShadowOffset=nShadowOffset;
		m_pShadowBrush=new Gdiplus::SolidBrush(ShadowColor);
	}else{
		m_nShadowOffset=0;
	}
}
//设置歌词字体
void CLyricsWindow::SetLyricsFont(const WCHAR * familyName, Gdiplus::REAL emSize,INT style, Gdiplus::Unit unit)
{
	if(m_pFont){
		delete m_pFont;
		m_pFont=NULL;
	}
	Gdiplus::FontFamily family(familyName,NULL);
	Gdiplus::Status lastResult = family.GetLastStatus();
	if (lastResult != Gdiplus::Ok)
	{
		HFONT hFont=(HFONT)GetStockObject(DEFAULT_GUI_FONT);
		LOGFONTW lf;
		ZeroMemory(&lf,sizeof(LOGFONTW));
		GetObjectW(hFont,sizeof(LOGFONTW),&lf);
		Gdiplus::FontFamily family2(lf.lfFaceName,NULL);
		m_pFont=new Gdiplus::Font(&family2,emSize,style,unit);
	}else{
		m_pFont=new Gdiplus::Font(&family,emSize,style,unit);
	}
	 //----------------
	//保存一些字体属性,加入路径时要用到
	m_pFont->GetFamily (m_pFontFamily);
	m_FontSize=m_pFont->GetSize ();
	m_FontStyle=m_pFont->GetStyle ();



}

void CLyricsWindow::SetLyricDoubleLine(bool doubleLine)
{
	m_bDoubleLine = doubleLine;
}

void CLyricsWindow::SetNextLyric(LPCTSTR lpszNextLyric)
{
	m_strNextLyric = lpszNextLyric;
}

void CLyricsWindow::SetShowTranslate(bool showTranslate)
{
    m_bShowTranslate = showTranslate;
}

void CLyricsWindow::SetAlpha(int alpha)
{
    m_alpha = alpha;
}

const CString& CLyricsWindow::GetLyricStr() const
{
    return m_lpszLyrics;
}

void CLyricsWindow::SetLyricChangeFlag(bool bFlag)
{
    m_lyricChangeFlag = bFlag;
}

void CLyricsWindow::SetAlignment(Alignment alignment)
{
	m_alignment = alignment;
}

void CLyricsWindow::SetLyricKaraokeDisplay(bool karaoke_disp)
{
    m_lyric_karaoke_disp = karaoke_disp;
}

void CLyricsWindow::OnLButtonDown(UINT nFlags, CPoint point)
{

	CWnd::OnLButtonDown(nFlags, point);
	//ReleaseCapture();
	//SendMessage(WM_NCLBUTTONDOWN,HTCAPTION,NULL);
}
