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

    int GetLyricTextHeight() const;
    virtual void Create(CDC* pDC, CWnd* pMainWnd /* = nullptr */) override;
    bool IsDrawMultiLine(int height) const;			//根据一个高度判断是否绘制多行歌词
    void SetForCortanaLyric(bool for_cortana_lyric = true);

    void DrawLyricTextMultiLine(CRect rect);
    void DrawLyricTextSingleLine(CRect rect, bool double_line = true);

private:
    void SetLyricFont();
    void SetLyricFontTranslated();
    void DrawLyricDoubleLine(CRect rect, LPCTSTR lyric, LPCTSTR next_lyric, int progress);

private:
    UIColors& m_colors;
    bool m_for_cortana_lyric{ false };		//是否用于显示搜索框歌词
};

