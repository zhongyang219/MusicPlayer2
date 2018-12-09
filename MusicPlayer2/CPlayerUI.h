#pragma once
#include "DrawCommon.h"
#include "CPlayerUIBase.h"


class CPlayerUI : public CPlayerUIBase
{
private:
	struct DrawData
	{
		CRect draw_rect;					//绘图区域

		CRect cover_rect;					//显示专辑封面的矩形区域（以绘图区域左上角为原点）
		CRect repetemode_rect;				//显示“循环模式”的矩形区域
		bool repetemode_hover{ false };		//鼠标指向了“循环模式”的矩形区域
		UIButton volume_btn;				//音量按钮
		CRect volume_up_rect, volume_down_rect;	//音量调整条增加和减少音量的矩形区域
		bool show_volume_adj{ false };		//显示音量调整按钮
		UIButton translate_btn;				//歌词翻译按钮
		UIButton skin_btn;
	};

public:
	CPlayerUI(UIData& ui_data);
	~CPlayerUI();

	virtual void Init(CDC* pDC) override;
	virtual void DrawInfo(bool narrow_mode, bool reset = false) override;		//绘制信息

	virtual void RButtonUp(CPoint point, bool narrow_mode) override;
	virtual void MouseMove(CPoint point) override;
	virtual void LButtonUp(CPoint point) override;
	virtual void OnSizeRedraw(int cx, int cy, bool narrow_mode) override;


	virtual CRect GetThumbnailClipArea(bool narrow_mode) override;

protected:
	void DrawLyricsSingleLine(CRect lyric_rect);			//绘制歌词（窄界面模式下单行显示），参数为显示歌词的矩形区域
	void DrawLyricsMulityLine(CRect lyric_rect, CDC* pDC);			//绘制歌词（普通模式下多行显示），参数为显示歌词的矩形区域

protected:
	DrawData m_draw_data;
	UIData& m_ui_data;
	std::shared_ptr<SLayoutData> m_pLayout{ nullptr };
};

