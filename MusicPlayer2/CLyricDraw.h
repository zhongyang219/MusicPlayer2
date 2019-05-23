#pragma once
#include "DrawCommon.h"
#include "CPlayerUIHelper.h"

class CLyricDraw :
    public CDrawCommon
{
public:
    CLyricDraw(UIColors& colors);
    ~CLyricDraw();

    void DrawLryicCommon(CRect rect);
    int GetLyricTextHeight();
    virtual void Create(CDC* pDC, CWnd* pMainWnd /* = nullptr */) override;
    bool IsDrawMultiLine(int height) const;			//根据一个高度判断是否绘制多行歌词

private:
    void DrawLyricTextMultiLine(CRect rect);
    void DrawLyricTextSingleLine(CRect rect);
    void DrawLyricDoubleLine(CRect rect, LPCTSTR lyric, LPCTSTR next_lyric, int progress);

private:
    UIColors& m_colors;
    int m_lyric_text_height;
};

