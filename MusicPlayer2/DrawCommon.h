//这个类用于定义用于绘图的函数
#pragma once
class CDrawCommon
{
public:

	//拉伸模式
	enum class StretchMode
	{
		STRETCH,		//拉伸，会改变比例
		FILL,			//填充，不改变比例，会裁剪长边
		FIT			//适应，不会改变比例，不裁剪
	};

	//用于在DrawScrollText函数调用时使用的一些需要在函数调用完毕后继续存在的变量
	struct ScrollInfo
	{
		int shift_cnt{};		//移动的次数
		bool shift_dir{};		//移动的方向，右移为false，左移为true
		int freez{};			//当该变量大于0时，文本不滚动，直到小于等于0为止
		bool dir_changed{ false };	//如果方向发生了变化，则为true
	};

	CDrawCommon();
	~CDrawCommon();

	void Create(CDC* pDC, CWnd* pMainWnd);
	//void SetBackColor(COLORREF back_color);		//设置绘制文本时填充的背景颜色
	//COLORREF GetBackColor() const { return m_backColor; }
	void SetFont(CFont* pfont);		//设置绘制文本的字体
	void SetDC(CDC* pDC);		//设置绘图的DC
	CDC* GetDC() { return m_pDC; }

	//下面是绘制文本的函数，功能和CStaticEx类中的同名函数相同，不同点在于这些函数不依赖于特定的控件
	void DrawWindowText(CRect rect, LPCTSTR lpszString, COLORREF color, bool center, bool no_clip_area = false);	//在指定的矩形区域内绘制有颜色的文本，如果no_clip_area为true，则不在输出文字时限制绘图区域
	void DrawWindowText(CRect rect, LPCTSTR lpszString, COLORREF color1, COLORREF color2, int split, bool center, bool no_clip_area = false);	//在指定的矩形区域内绘制分割颜色的文本，split为颜色分割的位置，取值为0~1000（用于歌词动态显示），如果no_clip_area为true，则不在输出文字时限制绘图区域
	void DrawScrollText(CRect rect, LPCTSTR lpszString, COLORREF color, int pixel, bool center, ScrollInfo& scroll_info, bool reset = false);	//在控件上绘制滚动的文本（当长度不够时），pixel指定此函数调用一次移动的像素值，如果reset为true，则滚动到初始位置
	void DrawScrollText2(CRect rect, LPCTSTR lpszString, COLORREF color, int pixel, bool center, ScrollInfo& scroll_info, bool reset = false);	//函数功能和DrawScrollText一样，只是这个函数只会从左到右滚动，不会更换方向

	static void SetDrawArea(CDC* pDC, CRect rect);

	void DrawBitmap(CBitmap& bitmap, CPoint start_point, CSize size, StretchMode stretch_mode);
	void DrawBitmap(UINT bitmap_id, CPoint start_point, CSize size, StretchMode stretch_mode);
	void DrawBitmap(HBITMAP hbitmap, CPoint start_point, CSize size, StretchMode stretch_mode);

	void FillRect(CRect rect, COLORREF color);
	void FillAlphaRect(CRect rect, COLORREF color, BYTE alpha);		//填充一个半透明的矩形（参照http://blog.csdn.net/lee353086/article/details/38311421）

	void DrawRectTopFrame(CRect rect, COLORREF color, int pilex = 1);
private:
	CDC* m_pDC{};		//用于绘图的CDC类的指针
	CWnd* m_pMainWnd{};	//绘图窗口的句柄
	//COLORREF m_backColor{ RGB(255,255,255) };
	CFont* m_pfont{};
};

