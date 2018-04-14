//CStatic类的派生类，用于在Static控件上绘制文本
#pragma once
#include "afxwin.h"
class CStaticEx :
	public CStatic
{
public:
	CStaticEx();
	~CStaticEx();

	enum class TextType
	{
		NORMAL,
		SPLIT,
		SCROLL
	};

	void DrawWindowText(LPCTSTR lpszString);	//在控件上绘制有颜色的文本（需要配合SetTextColor使用）
	void DrawWindowText(LPCTSTR lpszString, int split);	//在控件上绘制分割颜色的文本，split为颜色分割的位置，取值为0~1000（用于迷你模式下歌词动态显示）
	void DrawScrollText(LPCTSTR lpszString, int pixel, bool reset = false);	//在控件上绘制滚动的文本（当长度不够时），pixel指定此函数调用一次移动的像素值，如果reset为true，则滚动到初始位置
	void SetTextColor(COLORREF textColor);		//设置控件文本颜色
	void SetText2Color(COLORREF textColor);
	CString GetString() const;			//获取控件文本
	void SetTextCenter(bool center) { m_center = center; }	//设置文本居中

protected:
	COLORREF m_TextColor;	//控件文字颜色
	COLORREF m_TextBackColor;	//控件文本的第2种颜色
	CString m_text;			//控件上的文本
	bool m_center{ false };		//文本是否居中

	TextType m_text_type;
	int m_split;
	int m_scroll_pixel;
	bool m_scroll_reset;

	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
	afx_msg void OnPaint();
};

