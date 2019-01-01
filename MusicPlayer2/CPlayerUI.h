#pragma once
#include "DrawCommon.h"
#include "CPlayerUIBase.h"


class CPlayerUI : public CPlayerUIBase
{
private:
	struct DrawData
	{

		CRect cover_rect;					//显示专辑封面的矩形区域（以绘图区域左上角为原点）
	};

public:
	CPlayerUI(UIData& ui_data);
	~CPlayerUI();

	virtual void DrawInfo(bool reset = false) override;		//绘制信息

	virtual void RButtonUp(CPoint point) override;
	virtual void MouseMove(CPoint point) override;
	virtual void LButtonUp(CPoint point) override;
	virtual void OnSizeRedraw(int cx, int cy) override;

	virtual CRect GetThumbnailClipArea() override;

	virtual void AddMouseToolTip(BtnKey btn, LPCTSTR str) override;		//为一个按钮添加鼠标提示
	virtual void UpdateMouseToolTip(BtnKey btn, LPCTSTR str) override;
	virtual void UpdateToolTipPosition() override;

protected:
	void DrawLyricsSingleLine(CRect lyric_rect);			//绘制歌词（窄界面模式下单行显示），参数为显示歌词的矩形区域
	void DrawLyricsMulityLine(CRect lyric_rect, CDC* pDC);			//绘制歌词（普通模式下多行显示），参数为显示歌词的矩形区域

protected:
	DrawData m_draw_data;
};

