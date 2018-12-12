#pragma once
#include "DrawCommon.h"
#include "CPlayerUIBase.h"


class CPlayerUI2 : public CPlayerUIBase
{
private:
	struct DrawData
	{
		CRect cover_rect;
		CRect info_rect;
		CRect lyric_rect;
	};

public:
	CPlayerUI2(UIData& ui_data);
	~CPlayerUI2();

	virtual void Init(CDC* pDC) override;
	virtual void DrawInfo(bool narrow_mode, bool reset = false) override;		//ªÊ÷∆–≈œ¢

	virtual void RButtonUp(CPoint point, bool narrow_mode) override;
	virtual void MouseMove(CPoint point) override;
	virtual void LButtonUp(CPoint point) override;
	virtual void OnSizeRedraw(int cx, int cy, bool narrow_mode) override;

	virtual CRect GetThumbnailClipArea(bool narrow_mode) override;

protected:
	//void DrawControlBar(bool draw_background, CRect rect);

protected:
	DrawData m_draw_data;
	UIData& m_ui_data;
	std::shared_ptr<SLayoutData> m_pLayout{ nullptr };

	CFont m_title_font;
	CFont m_artist_font;
};

