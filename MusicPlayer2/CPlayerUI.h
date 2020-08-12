#pragma once
#include "DrawCommon.h"
#include "CPlayerUIBase.h"


class CPlayerUI : public CPlayerUIBase
{
public:
	CPlayerUI(UIData& ui_data, CWnd* pMainWnd);
	~CPlayerUI();

	//virtual void RButtonUp(CPoint point) override;
	virtual void MouseMove(CPoint point) override;
	//virtual void OnSizeRedraw(int cx, int cy) override;

	//virtual CRect GetThumbnailClipArea() override;

	virtual void AddMouseToolTip(BtnKey btn, LPCTSTR str) override;		//为一个按钮添加鼠标提示
	virtual void UpdateMouseToolTip(BtnKey btn, LPCTSTR str) override;
	virtual void UpdateToolTipPosition() override;

private:
	virtual void _DrawInfo(bool reset = false) override;		//绘制信息
    virtual int GetClassId() override;

	void DrawLyricsArea(CRect lyric_rect);			//普通模式下绘制歌词区域
	CSize SpectralSize();
};

