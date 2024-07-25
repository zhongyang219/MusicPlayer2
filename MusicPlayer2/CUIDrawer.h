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

    // 设置歌词&翻译字体
    void SetLyricFont(CFont* lyric_font, CFont* lyric_tr_font);

    void DrawLryicCommon(CRect rect, Alignment align = Alignment::AUTO, bool show_song_info = false);

    int GetLyricTextHeight() const;
    virtual void Create(CDC* pDC, CFont* pFont/* = nullptr */) override;
    bool IsDrawMultiLine(int height) const;			//根据一个高度判断是否绘制多行歌词
    void SetForCortanaLyric(bool for_cortana_lyric = true);

    // 调用绘制多行滚动歌词
    void DrawLyricTextMultiLine(CRect rect, Alignment align = Alignment::AUTO, bool show_song_info = false);
    // 调用以自适应绘制单双行歌词，需要提供静态变量flag以记忆双行切换状态
    void DrawLyricTextSingleLine(CRect rect, int& flag, bool double_line = true, Alignment align = Alignment::AUTO, bool show_song_info = false);

    //绘制频谱分析
    //rect：频谱的矩形区域
    //col：频谱的柱形的数量
    //draw_reflex：是否绘制倒影
    //fixed_width：每个柱形是否使用相同的宽度
    void DrawSpectrum(CRect rect, SpectrumCol col = SC_64, bool draw_reflex = false, bool low_freq_in_center = false, bool fixed_width = false, Alignment alignment = Alignment::LEFT);
    
    //绘制频谱分析
    //col_width：每一个柱形的宽度
    //gap_width：柱形间间隙的宽度
    //cols：频谱的柱形的数量，必须为2的n次方，且小于或等于SPECTRUM_COL
    //color：频谱分析的颜色
    //draw_reflex：是否绘制倒影
    //draw_peak: 是否绘制顶端
    //sprctrum_height: 设置频谱整体的高度，大于0有效，否则使用设置中的值theApp.m_app_setting_data.sprctrum_height
    void DrawSpectrum(CRect rect, int col_width, int gap_width, int cols, COLORREF color, bool draw_reflex = false, bool low_freq_in_center = false, Alignment alignment = Alignment::LEFT, bool draw_peak = true, int sprctrum_height = 0);

    int DPI(int pixel);

private:
    // 实际绘制双行歌词
    void DrawLyricDoubleLine(CRect rect, LPCTSTR lyric, LPCTSTR next_lyric, Alignment align, int progress, bool switch_flag, int fade_percent = 100);

private:
    UIColors& m_colors;
    CFont* m_lyric_font = nullptr;
    CFont* m_lyric_tr_font = nullptr;
    bool m_for_cortana_lyric{ false };		//是否用于显示搜索框歌词
};

