#pragma once
#include "stdafx.h"

class IPlayerUI
{
public:
	struct UIButton		//界面中绘制的按钮
	{
		CRect rect;				//按钮的矩形区域
		bool hover{ false };	//鼠标是否指向按钮
		bool pressed{ false };	//按钮是否按下
		bool enable{ true };	//按钮是否启用
	};

public:
	IPlayerUI(){}
	virtual ~IPlayerUI(){}

	virtual void Init(CDC* pDC) = 0;
	virtual void DrawInfo(bool reset = false) = 0;

	virtual CRect GetThumbnailClipArea() = 0;

	virtual void UpdateMouseToolTip(int btn, LPCTSTR str) = 0;
	virtual void UpdateToolTipPosition() = 0;
	virtual void UpdatePlayPauseButtonTip() = 0;
	virtual void UpdateFullScreenTip() = 0;

	virtual bool SetCursor() = 0;

    virtual CToolTipCtrl& GetToolTipCtrl() = 0;
};