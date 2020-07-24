#pragma once
#include "ColorConvert.h"

struct UIColors		//界面颜色
{
    COLORREF color_text;				//文本颜色
    COLORREF color_text_lable;			//标签文本的颜色
    COLORREF color_text_2;				//歌词未播放文本的颜色
    COLORREF color_text_heighlight;		//鼠标指向时文本的颜色
    COLORREF color_back;				//背景颜色
    COLORREF color_lyric_back;			//歌词界面背景颜色
    COLORREF color_control_bar_back;	//控制条背景颜色
    COLORREF color_spectrum;			//频谱分析柱形的颜色
    COLORREF color_spectrum_cover;		//有专辑封面时的频谱分析柱形的颜色
    COLORREF color_spectrum_back;		//频谱分析的背景颜色
    COLORREF color_button_back;			//歌词翻译按钮的背景色
    COLORREF color_button_pressed;		//按钮按下的颜色
    COLORREF color_button_hover;		//按钮指向时的颜色
};

class CPlayerUIHelper
{
public:
    CPlayerUIHelper();
    ~CPlayerUIHelper();

    static UIColors GetUIColors(const ColorTable& colorTable, bool dark, bool draw_alpha = true);

    static bool IsMidiLyric();		//是否绘制MIDI音乐的歌词
    static bool IsDrawStatusBar();  //是否绘制状态栏
};

