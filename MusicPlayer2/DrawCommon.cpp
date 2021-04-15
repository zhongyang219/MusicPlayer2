#include "stdafx.h"
#include "DrawCommon.h"
#include "GdiPlusTool.h"


CDrawCommon::CDrawCommon()
{
}

CDrawCommon::~CDrawCommon()
{
    if (m_auto_destory_graphics)
        SAFE_DELETE(m_pGraphics);
}

void CDrawCommon::Create(CDC* pDC, CWnd* pMainWnd)
{
    m_pDC = pDC;
    m_pMainWnd = pMainWnd;
    if (m_pMainWnd != nullptr)
        m_pfont = m_pMainWnd->GetFont();
    if (pDC != nullptr)
    {
        m_pGraphics = new Gdiplus::Graphics(pDC->GetSafeHdc());
        m_auto_destory_graphics = true;
    }
}

void CDrawCommon::Create(CDC* pDC, Gdiplus::Graphics* pGraphics, CWnd* pMainWnd)
{
    m_pDC = pDC;
    m_pMainWnd = pMainWnd;
    if (m_pMainWnd != nullptr)
        m_pfont = m_pMainWnd->GetFont();
    m_pGraphics = pGraphics;
    m_auto_destory_graphics = false;
}

//void CDrawCommon::SetBackColor(COLORREF back_color)
//{
//  m_backColor = back_color;
//}

CFont* CDrawCommon::SetFont(CFont* pfont)
{
    CFont* pOldFont = m_pfont;
    m_pfont = pfont;
    return pOldFont;
}

void CDrawCommon::SetDC(CDC* pDC)
{
    m_pDC = pDC;
    if (m_auto_destory_graphics)
        SAFE_DELETE(m_pGraphics);
    m_pGraphics = new Gdiplus::Graphics(pDC->GetSafeHdc());
    m_auto_destory_graphics = true;
}

void CDrawCommon::DrawWindowText(CRect rect, LPCTSTR lpszString, COLORREF color, Alignment align, bool no_clip_area, bool multi_line, bool default_right_align)
{
    if (m_pDC->GetSafeHdc() == NULL)
        return;
    m_pDC->SetTextColor(color);
    m_pDC->SetBkMode(TRANSPARENT);
    if (m_pfont != nullptr)
        m_pDC->SelectObject(m_pfont);
    //设置绘图的剪辑区域
    if (!no_clip_area)
    {
        SetDrawArea(m_pDC, rect);
    }
    CSize text_size = m_pDC->GetTextExtent(lpszString);
    //用背景色填充矩形区域
    //m_pDC->FillSolidRect(rect, m_backColor);
    UINT format{};
    switch (align)
    {
    case Alignment::RIGHT: format = DT_RIGHT; break;
    case Alignment::CENTER: format = DT_CENTER; break;
    }
    if (multi_line)
    {
        CRect text_rect{ rect };
        int height = m_pDC->DrawText(lpszString, text_rect, DT_CALCRECT | DT_CENTER | DT_EDITCONTROL | DT_WORDBREAK);
        if (height < rect.Height())
            rect.top += (rect.Height() - height) / 2;
        rect.bottom = rect.top + height;
        m_pDC->DrawText(lpszString, rect, format | DT_EDITCONTROL | DT_WORDBREAK | DT_NOPREFIX);
    }
    else
    {
        if (text_size.cx > rect.Width())        //如果文本宽度超过了矩形区域的宽度，设置了居中时左对齐
        {
            format = (default_right_align ? DT_RIGHT : 0);
        }
        m_pDC->DrawText(lpszString, rect, format | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    }
}

void CDrawCommon::DrawWindowText(CRect rect, LPCTSTR lpszString, COLORREF color1, COLORREF color2, int split, Alignment align, bool no_clip_area)
{
    if (m_pDC->GetSafeHdc() == NULL)
        return;
    if (split < 0) split = 0;
    if (split > 1000) split = 1000;
    m_pDC->SetBkMode(TRANSPARENT);
    if (m_pfont != nullptr)
        m_pDC->SelectObject(m_pfont);
    CSize text_size;    //文本的大小
    int text_top, text_left;        //输出文本的top和left位置
    //设置绘图的剪辑区域，防止文字输出超出控件区域
    if (!no_clip_area)
    {
        SetDrawArea(m_pDC, rect);
    }
    //获取文字的宽度和高度
    text_size = m_pDC->GetTextExtent(lpszString);
    //计算文字的起始坐标
    text_top = rect.top + (rect.Height() - text_size.cy) / 2;
    if (align == Alignment::CENTER)
        text_left = rect.left + (rect.Width() - text_size.cx) / 2;
    else if (align == Alignment::RIGHT)
        text_left = rect.left + (rect.Width() - text_size.cx);
    else
        text_left = rect.left;
    //计算背景文字和覆盖文字的矩形区域
    CRect text_rect{ CPoint{ text_left, text_top }, text_size };        //背景文字的区域
    CRect text_f_rect{ CPoint{ text_left, text_top }, CSize{ text_size.cx * split / 1000, text_size.cy } };     //覆盖文字的区域
                                                                                                                //如果文本宽度大于控件宽度，就要根据分割的位置滚动文本
    if (text_size.cx > rect.Width())
    {
        //如果分割的位置（歌词进度）剩下的宽度已经小于控件宽度的一半，此时使文本右侧和控件右侧对齐
        if (text_rect.Width() - text_f_rect.Width() < rect.Width() / 2)
        {
            text_rect.MoveToX(rect.left - (text_rect.Width() - rect.Width()));
            text_f_rect.MoveToX(text_rect.left);
        }
        //分割位置剩下的宽度还没有到小于控件宽度的一半，但是分割位置的宽度已经大于控件宽度的一半时，需要移动文本使分割位置正好在控件的中间
        else if (text_f_rect.Width() > rect.Width() / 2)
        {
            text_rect.MoveToX(rect.left - (text_f_rect.Width() - rect.Width() / 2));
            text_f_rect.MoveToX(text_rect.left);
        }
        //分割位置还不到控件宽度的一半时，使文本左侧和控件左侧对齐
        else
        {
            text_rect.MoveToX(rect.left);
            text_f_rect.MoveToX(rect.left);
        }
    }

    //用背景色填充矩形区域
    //m_pDC->FillSolidRect(rect, m_backColor);
    //输出文本
    m_pDC->SetTextColor(color2);
    m_pDC->DrawText(lpszString, text_rect, DT_SINGLELINE | DT_NOPREFIX);        //绘制背景文字
    if (color1 != color2)
    {
        m_pDC->SetTextColor(color1);
        m_pDC->DrawText(lpszString, text_f_rect, DT_SINGLELINE | DT_NOPREFIX);      //绘制覆盖文字
    }
}

void CDrawCommon::DrawScrollText(CRect rect, LPCTSTR lpszString, COLORREF color, double pixel, bool center, ScrollInfo& scroll_info, bool reset)
{
    if (m_pDC->GetSafeHdc() == NULL)
        return;
    //static int shift_cnt;     //移动的次数
    //static bool shift_dir;        //移动的方向，右移为false，左移为true
    //static int freez;         //当该变量大于0时，文本不滚动，直到小于等于0为止
    //static bool dir_changed{ false }; //如果方向发生了变化，则为true

    if (scroll_info.last_string.GetLength() != CString(lpszString).GetLength())      //当显示文本长度发生变化时，重置滚动位置
    {
        reset = true;
        scroll_info.last_string = lpszString;
    }

    if (reset)
    {
        scroll_info.shift_cnt = 0;
        scroll_info.shift_dir = false;
        scroll_info.freez = 20;
        scroll_info.dir_changed = false;
    }
    m_pDC->SetTextColor(color);
    m_pDC->SetBkMode(TRANSPARENT);
    if (m_pfont != nullptr)
        m_pDC->SelectObject(m_pfont);
    CSize text_size;    //文本的大小
    int text_top, text_left;        //输出文本的top和left位置
    //设置绘图的剪辑区域，防止文字输出超出控件区域
    SetDrawArea(m_pDC, rect);
    //获取文字的宽度和高度
    text_size = m_pDC->GetTextExtent(lpszString);
    //计算文字的起始坐标
    text_top = rect.top + (rect.Height() - text_size.cy) / 2;
    if (center)
        text_left = rect.left + (rect.Width() - text_size.cx) / 2;
    else
        text_left = rect.left;
    //计算文字的矩形区域
    CRect text_rect{ CPoint{ text_left, text_top }, text_size };
    //如果文本宽度大于控件宽度，就滚动文本
    if (text_size.cx > rect.Width())
    {
        text_rect.MoveToX(rect.left - scroll_info.shift_cnt * pixel);
        if ((text_rect.right < rect.right || text_rect.left > rect.left))       //移动到边界时换方向
        {
            if (!scroll_info.dir_changed)
            {
                scroll_info.shift_dir = !scroll_info.shift_dir;
                scroll_info.freez = 20;     //变换方向时稍微暂停滚动一段时间
            }
            scroll_info.dir_changed = true;
        }
        else
        {
            scroll_info.dir_changed = false;
        }
    }
    //用背景色填充矩形区域
    //m_pDC->FillSolidRect(rect, m_backColor);
    //输出文本
    m_pDC->DrawText(lpszString, text_rect, DT_SINGLELINE | DT_NOPREFIX);
    if (scroll_info.freez <= 0)     //当freez为0的时候才滚动
    {
        if (scroll_info.shift_dir)
            scroll_info.shift_cnt--;
        else
            scroll_info.shift_cnt++;
    }
    else
    {
        scroll_info.freez--;
    }
    if ((rect & text_rect).IsRectEmpty())       //如果文本矩形区域已经超出了绘图区域，则重置滚动
    {
        reset = true;
        scroll_info.shift_cnt = 0;
        scroll_info.freez = 20;
    }
}

void CDrawCommon::DrawScrollText2(CRect rect, LPCTSTR lpszString, COLORREF color, double pixel, bool center, ScrollInfo& scroll_info, bool reset)
{
    if (m_pDC->GetSafeHdc() == NULL)
        return;
    if (scroll_info.last_string.GetLength() != CString(lpszString).GetLength())      //当显示文本长度发生变化时，重置滚动位置
    {
        reset = true;
        scroll_info.last_string = lpszString;
    }

    if (reset)
    {
        scroll_info.shift_cnt = 0;
        scroll_info.freez = 20;
    }
    m_pDC->SetTextColor(color);
    m_pDC->SetBkMode(TRANSPARENT);
    if (m_pfont != nullptr)
        m_pDC->SelectObject(m_pfont);
    CSize text_size;    //文本的大小
    int text_top, text_left;        //输出文本的top和left位置
    //设置绘图的剪辑区域，防止文字输出超出控件区域
    SetDrawArea(m_pDC, rect);
    //获取文字的宽度和高度
    text_size = m_pDC->GetTextExtent(lpszString);
    //计算文字的起始坐标
    text_top = rect.top + (rect.Height() - text_size.cy) / 2;
    if (center)
        text_left = rect.left + (rect.Width() - text_size.cx) / 2;
    else
        text_left = rect.left;
    //计算文字的矩形区域
    CRect text_rect{ CPoint{ text_left, text_top }, text_size };
    //如果文本宽度大于控件宽度，就滚动文本
    if (text_size.cx > rect.Width())
    {
        text_rect.MoveToX(rect.left - scroll_info.shift_cnt * pixel);
        if ((text_rect.right < rect.right || text_rect.left > rect.left))       //移动超出边界时暂停滚动，freez从20开始递减
        {
            scroll_info.shift_cnt--;    //让文本往回移动一次，防止反复判断为超出边界
            text_rect.MoveToX(rect.left - scroll_info.shift_cnt * pixel);
            scroll_info.freez = 20;     //变换方向时稍微暂停滚动一段时间
        }
    }
    //用背景色填充矩形区域
    //m_pDC->FillSolidRect(rect, m_backColor);
    //输出文本
    m_pDC->DrawText(lpszString, text_rect, DT_SINGLELINE | DT_NOPREFIX);
    if (scroll_info.freez <= 0)     //当freez为0的时候才滚动
    {
        scroll_info.shift_cnt++;
    }
    else
    {
        scroll_info.freez--;
        if (scroll_info.freez == 10)        //当freez递减到一半时将文本复位
            scroll_info.shift_cnt = 0;
    }
    if ((rect & text_rect).IsRectEmpty())       //如果文本矩形区域已经超出了绘图区域，则重置滚动
    {
        reset = true;
        scroll_info.shift_cnt = 0;
        scroll_info.freez = 20;
    }
}

//void CDrawCommon::FillRect(CDC * pDC, CRect rect, COLORREF color)
//{
//  CBrush BGBrush, *pOldBrush;
//  BGBrush.CreateSolidBrush(color);
//  pOldBrush = pDC->SelectObject(&BGBrush);
//  pDC->FillRect(&rect, &BGBrush);
//  pDC->SelectObject(pOldBrush);
//  BGBrush.DeleteObject();
//}

void CDrawCommon::SetDrawArea(CDC* pDC, CRect rect)
{
    CRgn rgn;
    rgn.CreateRectRgnIndirect(rect);
    pDC->SelectClipRgn(&rgn);
}

void CDrawCommon::SetDrawArea(CRect rect)
{
    if (m_pDC->GetSafeHdc() == NULL)
        return;
    CRgn rgn;
    rgn.CreateRectRgnIndirect(rect);
    m_pDC->SelectClipRgn(&rgn);
}

void CDrawCommon::DrawBitmap(CBitmap& bitmap, CPoint start_point, CSize size, StretchMode stretch_mode, bool no_clip_area)
{
    if (m_pDC->GetSafeHdc() == NULL)
        return;
    CDC memDC;

    //获取图像实际大小
    BITMAP bm;
    GetObject(bitmap, sizeof(BITMAP), &bm);

    memDC.CreateCompatibleDC(m_pDC);
    memDC.SelectObject(&bitmap);
    // 以下两行避免图片失真
    m_pDC->SetStretchBltMode(HALFTONE);
    m_pDC->SetBrushOrg(0, 0);
    //CSize draw_size;
    ImageDrawAreaConvert(CSize(bm.bmWidth, bm.bmHeight), start_point, size, stretch_mode, no_clip_area);
    m_pDC->StretchBlt(start_point.x, start_point.y, size.cx, size.cy, &memDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
    memDC.DeleteDC();
}

void CDrawCommon::DrawBitmap(UINT bitmap_id, CPoint start_point, CSize size, StretchMode stretch_mode, bool no_clip_area)
{
    CBitmap bitmap;
    bitmap.LoadBitmap(bitmap_id);
    DrawBitmap(bitmap, start_point, size, stretch_mode, no_clip_area);
}

void CDrawCommon::DrawBitmap(HBITMAP hbitmap, CPoint start_point, CSize size, StretchMode stretch_mode, bool no_clip_area)
{
    CBitmap bitmap;
    if (!bitmap.Attach(hbitmap))
        return;
    DrawBitmap(bitmap, start_point, size, stretch_mode, no_clip_area);
    bitmap.Detach();
}

void CDrawCommon::DrawImage(const CImage& image, CPoint start_point, CSize size, StretchMode stretch_mode, bool no_clip_area)
{
    if (m_pDC->GetSafeHdc() == NULL)
        return;
    m_pGraphics->SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeHighQuality);
    if (!no_clip_area)
    {
        Gdiplus::Rect rect_clip = CGdiPlusTool::CRectToGdiplusRect(CRect(start_point, size));
        m_pGraphics->SetClip(rect_clip);
    }
    ImageDrawAreaConvert(CSize(image.GetWidth(), image.GetHeight()), start_point, size, stretch_mode, no_clip_area);
    Gdiplus::Bitmap bm(image, NULL);
    m_pGraphics->DrawImage(&bm, start_point.x, start_point.y, size.cx, size.cy);
    m_pGraphics->ResetClip();
}

void CDrawCommon::DrawImage(Gdiplus::Image* pImage, CPoint start_point, CSize size, StretchMode stretch_mode, bool no_clip_area)
{
    m_pGraphics->SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeHighQuality);
    if (!no_clip_area)
    {
        Gdiplus::Rect rect_clip = CGdiPlusTool::CRectToGdiplusRect(CRect(start_point, size));
        m_pGraphics->SetClip(rect_clip);
    }
    ImageDrawAreaConvert(CSize(pImage->GetWidth(), pImage->GetHeight()), start_point, size, stretch_mode, no_clip_area);
    m_pGraphics->DrawImage(pImage, start_point.x, start_point.y, size.cx, size.cy);
    m_pGraphics->ResetClip();
}

void CDrawCommon::DrawIcon(HICON hIcon, CPoint start_point, CSize size)
{
    if (m_pDC->GetSafeHdc() == NULL)
        return;
    if (size.cx == 0 || size.cy == 0)
        ::DrawIconEx(m_pDC->GetSafeHdc(), start_point.x, start_point.y, hIcon, 0, 0, 0, NULL, DI_NORMAL | DI_DEFAULTSIZE);
    else
        ::DrawIconEx(m_pDC->GetSafeHdc(), start_point.x, start_point.y, hIcon, size.cx, size.cy, 0, NULL, DI_NORMAL);
}

void CDrawCommon::FillRect(CRect rect, COLORREF color, bool no_clip_area)
{
    if (m_pDC->GetSafeHdc() == NULL)
        return;
    if (!no_clip_area)
        SetDrawArea(m_pDC, rect);
    m_pDC->FillSolidRect(rect, color);
}

void CDrawCommon::FillAlphaRect(CRect rect, COLORREF color, BYTE alpha, bool no_clip_area)
{
    if (m_pDC->GetSafeHdc() == NULL)
        return;
    if (alpha == 0)
        return;
    if (!no_clip_area)
        SetDrawArea(m_pDC, rect);
    if (alpha == 255)
    {
        FillRect(rect, color, no_clip_area);
        return;
    }
    CDC cdc;
    if (!cdc.CreateCompatibleDC(m_pDC))
        return;

    CBitmap bitmap, * pOldBitmap;
    bitmap.CreateCompatibleBitmap(m_pDC, rect.Width(), rect.Height());
    CRect src(rect);
    src.MoveToXY(0, 0);
    pOldBitmap = cdc.SelectObject(&bitmap);
    cdc.FillSolidRect(src, color); //透明色

    if (::AlphaBlend == 0)
    {
        m_pDC->BitBlt(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, &cdc, src.left, src.top, SRCINVERT);
    }
    else
    {
        BLENDFUNCTION bf;
        memset(&bf, 0, sizeof(bf));
        bf.SourceConstantAlpha = alpha; //透明程度//值越大越不透明
        bf.BlendOp = AC_SRC_OVER;
        ::AlphaBlend(m_pDC->GetSafeHdc(), rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
            cdc.GetSafeHdc(), src.left, src.top, src.right - src.left, src.bottom - src.top, bf);
    }
    cdc.SelectObject(pOldBitmap);
}

void CDrawCommon::DrawRectTopFrame(CRect rect, COLORREF color, int pilex)
{
    if (m_pDC->GetSafeHdc() == NULL)
        return;
    SetDrawArea(m_pDC, rect);
    CPen aPen, * pOldPen;
    aPen.CreatePen(PS_SOLID, pilex, color);
    pOldPen = m_pDC->SelectObject(&aPen);

    m_pDC->MoveTo(rect.TopLeft());
    m_pDC->LineTo(rect.right, rect.top);

    m_pDC->SelectObject(pOldPen);
    aPen.DeleteObject();
}

void CDrawCommon::DrawRectOutLine(CRect rect, COLORREF color, int width, bool dot_line)
{
    if (m_pDC->GetSafeHdc() == NULL)
        return;
    CPen aPen, * pOldPen;
    aPen.CreatePen((dot_line ? PS_DOT : PS_SOLID), width, color);
    pOldPen = m_pDC->SelectObject(&aPen);
    CBrush* pOldBrush{ dynamic_cast<CBrush*>(m_pDC->SelectStockObject(NULL_BRUSH)) };

    rect.DeflateRect(width / 2, width / 2);
    m_pDC->Rectangle(rect);
    m_pDC->SelectObject(pOldPen);
    m_pDC->SelectObject(pOldBrush);       // Restore the old brush
    aPen.DeleteObject();
}

void CDrawCommon::DrawRectFrame(CRect rect, COLORREF color, int width, BYTE alpha /*= 255*/)
{
    if (width < 1)
        width = 1;
    if (width > min(rect.Width(), rect.Height()) / 2)       //如果边框宽度超过矩形短边的一半，则直接填充两个矩形
    {
        FillAlphaRect(rect, color, alpha, true);
    }
    else        //通过绘制4个矩形来绘制矩形边框
    {
        CRect rect_top{ rect };
        rect_top.bottom = rect_top.top + width;
        FillAlphaRect(rect_top, color, alpha, true);

        CRect rect_bottom{ rect };
        rect_bottom.top = rect_bottom.bottom - width;
        FillAlphaRect(rect_bottom, color, alpha, true);

        CRect rect_left{ rect };
        rect_left.right = rect_left.left + width;
        rect_left.top += width;
        rect_left.bottom -= width;
        FillAlphaRect(rect_left, color, alpha, true);

        CRect rect_right{ rect };
        rect_right.left = rect_right.right - width;
        rect_right.top += width;
        rect_right.bottom -= width;
        FillAlphaRect(rect_right, color, alpha, true);
    }
}

void CDrawCommon::DrawLine(CPoint point1, CPoint point2, COLORREF color, int width, bool dot_line)
{
    if (m_pDC->GetSafeHdc() == NULL)
        return;
    CPen aPen, * pOldPen;
    aPen.CreatePen((dot_line ? PS_DOT : PS_SOLID), width, color);
    pOldPen = m_pDC->SelectObject(&aPen);
    CBrush* pOldBrush{ dynamic_cast<CBrush*>(m_pDC->SelectStockObject(NULL_BRUSH)) };

    m_pDC->MoveTo(point1);
    m_pDC->LineTo(point2);
    m_pDC->SelectObject(pOldPen);
    m_pDC->SelectObject(pOldBrush);       // Restore the old brush
    aPen.DeleteObject();
}

void CDrawCommon::DrawRoundRect(CRect rect, COLORREF color, int radius, BYTE alpha /*= 255*/)
{
    DrawRoundRect(CGdiPlusTool::CRectToGdiplusRect(rect), CGdiPlusTool::COLORREFToGdiplusColor(color, alpha), radius);
}

void CDrawCommon::DrawRoundRect(Gdiplus::Rect rect, Gdiplus::Color color, int radius)
{
    CRect rc{ CGdiPlusTool::GdiplusRectToCRect(rect) };
    rc.right--;
    rc.bottom--;
    //生成圆角矩形路径
    Gdiplus::GraphicsPath round_rect_path;
    CGdiPlusTool::CreateRoundRectPath(round_rect_path, rc, radius);

    m_pGraphics->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);      //设置抗锯齿
    m_pGraphics->FillPath(&Gdiplus::SolidBrush(color), &round_rect_path);          //填充路径
    m_pGraphics->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeNone);
}

CSize CDrawCommon::GetTextExtent(LPCTSTR str)
{
    if (m_pDC->GetSafeHdc() == NULL)
        return CSize();
    if (m_pfont != nullptr)
        m_pDC->SelectObject(m_pfont);
    return m_pDC->GetTextExtent(str);
}

//bool CDrawCommon::BitmapStretch(CImage * pImage, CImage * ResultImage, CSize size)
//{
//  if (pImage->IsDIBSection())
//  {
//      // 取得 pImage 的 DC
//      CDC* pImageDC1 = CDC::FromHandle(pImage->GetDC()); // Image 因為有自己的 DC, 所以必須使用 FromHandle 取得對應的 DC
//
//      CBitmap *bitmap1 = pImageDC1->GetCurrentBitmap();
//      BITMAP bmpInfo;
//      bitmap1->GetBitmap(&bmpInfo);
//
//      // 建立新的 CImage
//      //ResultImage->Create(size.cx, size.cy, bmpInfo.bmBitsPixel);
//      ResultImage->Create(size.cx, size.cy, 24);      //总是将目标图片转换成24位图
//      CDC* ResultImageDC = CDC::FromHandle(ResultImage->GetDC());
//
//      // 當 Destination 比較小的時候, 會根據 Destination DC 上的 Stretch Blt mode 決定是否要保留被刪除點的資訊
//      ResultImageDC->SetStretchBltMode(HALFTONE); // 使用最高品質的方式
//      ::SetBrushOrgEx(ResultImageDC->m_hDC, 0, 0, NULL); // 調整 Brush 的起點
//
//      // 把 pImage 畫到 ResultImage 上面
//      BOOL rtn = StretchBlt(*ResultImageDC, 0, 0, size.cx, size.cy, *pImageDC1, 0, 0, pImage->GetWidth(), pImage->GetHeight(), SRCCOPY);
//      // pImage->Draw(*ResultImageDC,0,0,StretchWidth,StretchHeight,0,0,pImage->GetWidth(),pImage->GetHeight());
//
//      pImage->ReleaseDC();
//      ResultImage->ReleaseDC();
//  }
//  return true;
//}

void CDrawCommon::ImageResize(const CImage& img_src, CImage& img_dest, CSize size)
{
    img_dest.Destroy();
    int bpp = img_src.GetBPP();
    if (bpp < 24)
        bpp = 24;       //总是将目标图片转换成24位图
    img_dest.Create(size.cx, size.cy, bpp);
    //使用GDI+更改图片大小时，左侧和上面会有一像素的灰边，因此在这里将其裁剪掉
    size.cx++;
    size.cy++;
    //使用GDI+高质量绘图
    img_src.Draw(img_dest.GetDC(), CRect(CPoint(-1, -1), size), Gdiplus::InterpolationMode::InterpolationModeHighQuality);
    img_dest.ReleaseDC();
}

void CDrawCommon::ImageResize(const CImage& img_src, const wstring& path_dest, int size, ImageType type)
{
    CImage imDest;

    //计算输出图片的大小
    CSize size_src{ img_src.GetWidth(), img_src.GetHeight() };
    CSize size_dest{ size_src };
    CCommon::SizeZoom(size_dest, size);

    ImageResize(img_src, imDest, size_dest);
    //输出为指定格式
    const GUID* pType{ &(GUID)GUID_NULL };
    switch (type)
    {
    case IT_JPG:
        pType = &Gdiplus::ImageFormatJPEG;
        break;
    case IT_PNG:
        pType = &Gdiplus::ImageFormatPNG;
        break;
    case IT_GIF:
        pType = &Gdiplus::ImageFormatGIF;
        break;
    default:
        pType = &Gdiplus::ImageFormatBMP;
        break;
    }

    imDest.Save(path_dest.c_str(), *pType);
}

void CDrawCommon::ImageResize(const wstring& path_src, const wstring& path_dest, int size, ImageType type)
{
    CImage imSrc;
    //读入原始图片
    imSrc.Load(path_src.c_str());
    ImageResize(imSrc, path_dest, size, type);
}

HICON CDrawCommon::LoadIconResource(UINT id, int width, int height)
{
    return (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(id), IMAGE_ICON, width, height, 0);
}

HBITMAP CDrawCommon::CopyBitmap(HBITMAP hSourceHbitmap)
{
    CDC sourceDC;
    CDC destDC;
    sourceDC.CreateCompatibleDC(NULL);
    destDC.CreateCompatibleDC(NULL);
    //The bitmap information. 
    BITMAP bm = { 0 };
    //Get the bitmap information. 
    ::GetObject(hSourceHbitmap, sizeof(bm), &bm);
    // Create a bitmap to hold the result 
    //HBITMAP hbmResult = ::CreateCompatibleBitmap(CClientDC(NULL), bm.bmWidth, bm.bmHeight);
    HBITMAP hbmResult = ::CreateBitmap(bm.bmWidth, bm.bmHeight, bm.bmPlanes, bm.bmBitsPixel, NULL);

    HBITMAP hbmOldSource = (HBITMAP)::SelectObject(sourceDC.m_hDC, hSourceHbitmap);
    HBITMAP hbmOldDest = (HBITMAP)::SelectObject(destDC.m_hDC, hbmResult);
    destDC.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &sourceDC, 0, 0, SRCCOPY);
    destDC.FillSolidRect(5, 5, 5, 5, RGB(78, 176, 255));

    // Restore DCs 
    ::SelectObject(sourceDC.m_hDC, hbmOldSource);
    ::SelectObject(destDC.m_hDC, hbmOldDest);
    ::DeleteObject(sourceDC.m_hDC);
    ::DeleteObject(destDC.m_hDC);

    return hbmResult;
}

void CDrawCommon::CopyBitmap(CBitmap& dest, CBitmap& src)
{
#if 1
    CDC sourceDC;
    CDC destDC;
    sourceDC.CreateCompatibleDC(NULL);
    destDC.CreateCompatibleDC(NULL);
    //The bitmap information. 
    BITMAP bm = { 0 };
    //Get the bitmap information. 
    ::GetObject(src.GetSafeHandle(), sizeof(bm), &bm);

    dest.DeleteObject();
    //dest.CreateCompatibleBitmap(&destDC, bm.bmWidth, bm.bmHeight);
    //这里如果使用上面一行注释掉的代码（CreateCompatibleBitmap）会导致BitBlt等到的图像为黑色，因为src和dest图像的位深度不一样
    dest.CreateBitmap(bm.bmWidth, bm.bmHeight, bm.bmPlanes, bm.bmBitsPixel, NULL);

    BITMAP bm_dest{};
    ::GetObject(dest.GetSafeHandle(), sizeof(bm_dest), &bm_dest);

    sourceDC.SelectObject(&src);
    destDC.SelectObject(&dest);
    int rtn = destDC.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &sourceDC, 0, 0, SRCCOPY);
    ::GetObject(dest.GetSafeHandle(), sizeof(bm_dest), &bm_dest);

    destDC.FillSolidRect(5, 5, 5, 5, RGB(78, 176, 255));
    ::GetObject(dest.GetSafeHandle(), sizeof(bm_dest), &bm_dest);
#else

    HBITMAP HBM = (HBITMAP)src.m_hObject;

    //HBITMAP HBM=(HBITMAP)hSourceBitmap->Detach();//如果希望清除掉原图资源
    //hDescBitmap = new CBitmap;
    dest.Attach(HBM);
#endif
}

void CDrawCommon::SaveBitmap(HBITMAP bitmap, LPCTSTR path)
{
    CImage img_tmp;
    img_tmp.Attach(bitmap);
    img_tmp.Save(path);
    img_tmp.Detach();
}

void CDrawCommon::ImageDrawAreaConvert(CSize image_size, CPoint& start_point, CSize& size, StretchMode stretch_mode, bool no_clip_area)
{
    if (size.cx == 0 || size.cy == 0)       //如果指定的size为0，则使用位图的实际大小绘制
    {
        size = CSize(image_size.cx, image_size.cy);
    }
    else
    {
        if (stretch_mode == StretchMode::FILL)
        {
            if (!no_clip_area)
                SetDrawArea(m_pDC, CRect(start_point, size));
            float w_h_ratio, w_h_ratio_draw;        //图像的宽高比、绘制大小的宽高比
            w_h_ratio = static_cast<float>(image_size.cx) / image_size.cy;
            w_h_ratio_draw = static_cast<float>(size.cx) / size.cy;
            if (w_h_ratio > w_h_ratio_draw)     //如果图像的宽高比大于绘制区域的宽高比，则需要裁剪两边的图像
            {
                int image_width;        //按比例缩放后的宽度
                image_width = image_size.cx * size.cy / image_size.cy;
                start_point.x -= ((image_width - size.cx) / 2);
                size.cx = image_width;
            }
            else
            {
                int image_height;       //按比例缩放后的高度
                image_height = image_size.cy * size.cx / image_size.cx;
                start_point.y -= ((image_height - size.cy) / 2);
                size.cy = image_height;
            }
        }
        else if (stretch_mode == StretchMode::FIT)
        {
            CSize draw_size = image_size;
            float w_h_ratio, w_h_ratio_draw;        //图像的宽高比、绘制大小的宽高比
            w_h_ratio = static_cast<float>(image_size.cx) / image_size.cy;
            w_h_ratio_draw = static_cast<float>(size.cx) / size.cy;
            if (w_h_ratio > w_h_ratio_draw)     //如果图像的宽高比大于绘制区域的宽高比
            {
                draw_size.cy = draw_size.cy * size.cx / draw_size.cx;
                draw_size.cx = size.cx;
                start_point.y += ((size.cy - draw_size.cy) / 2);
            }
            else
            {
                draw_size.cx = draw_size.cx * size.cy / draw_size.cy;
                draw_size.cy = size.cy;
                start_point.x += ((size.cx - draw_size.cx) / 2);
            }
            size = draw_size;
        }
    }
}
