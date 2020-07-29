#pragma once
#include "DrawCommon.h"
#include "CPlayerUIHelper.h"

class CUIDrawer :
    public CDrawCommon
{
public:
    enum SpectrumCol        //绘制频谱分析的柱形数量
    {
        SC_64,
        SC_32,
        SC_16,
        SC_8
    };

public:
    CUIDrawer(UIColors& colors);
    ~CUIDrawer();

    void DrawLryicCommon(CRect rect, Alignment align = Alignment::CENTER);

    int GetLyricTextHeight() const;
    virtual void Create(CDC* pDC, CWnd* pMainWnd /* = nullptr */) override;
    bool IsDrawMultiLine(int height) const;			//根据一个高度判断是否绘制多行歌词
    void SetForCortanaLyric(bool for_cortana_lyric = true);

    void DrawLyricTextMultiLine(CRect rect, Alignment align = Alignment::CENTER);
    void DrawLyricTextSingleLine(CRect rect, bool double_line = true, Alignment align = Alignment::CENTER);

    //绘制频谱分析
    //rect：频谱的矩形区域
    //col：频谱的柱形的数量
    //draw_reflex：是否绘制倒影
    void DrawSpectrum(CRect rect, SpectrumCol col = SC_64, bool draw_reflex = false, bool low_freq_in_center = false);
    
    //绘制频谱分析
    //col_width：每一个柱形的宽度
    //gap_width：柱形间间隙的宽度
    //cols：频谱的柱形的数量，必须为2的n次方，且小于或等于SPECTRUM_COL
    //color：频谱分析的颜色
    //draw_reflex：是否绘制倒影
    void DrawSpectrum(CRect rect, int col_width, int gap_width, int cols, COLORREF color, bool draw_reflex = false, bool low_freq_in_center = false);

private:
    CFont* SetLyricFont();
    CFont* SetLyricFontTranslated();
    void DrawLyricDoubleLine(CRect rect, LPCTSTR lyric, LPCTSTR next_lyric, int progress, int fade_percent = 100);

private:
    UIColors& m_colors;
    bool m_for_cortana_lyric{ false };		//是否用于显示搜索框歌词
};

