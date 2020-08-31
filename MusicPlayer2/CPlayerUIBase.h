#pragma once
#include "MusicPlayer2.h"
#include "IPlayerUI.h"
#include "CPlayerUIHelper.h"
#include "CUIDrawer.h"

#define WM_MAIN_MENU_POPEDUP (WM_USER+117)		//显示弹出式主菜单的消息，wPara为表示菜单显示位置的CPoint的指针

struct SLayoutData
{
    const int margin = theApp.DPI(4);							//边缘的余量
    const int width_threshold = theApp.DPI(600);				//界面从普通界面模式切换到窄界面模式时界面宽度的阈值
    const int info_height = theApp.DPI(216);					//窄界面模式时显示信息区域的高度
    const int path_edit_height = theApp.DPI(32);				//前路径Edit控件区域的高度
    const int search_edit_height = theApp.DPI(26);				//歌曲搜索框Edit控件区域的高度
    //const int select_folder_width = theApp.DPI(90);	//“选择文件夹”按钮的宽度
    const CSize spectral_size{ theApp.DPI(120), theApp.DPI(90) };	//频谱分析区域的大小
    const int toolbar_height = theApp.DPI(24);                  //播放列表工具栏的高度
};


class CPlayerUIBase : public IPlayerUI
{
public:
    CPlayerUIBase(UIData& ui_data, CWnd* pMainWnd);
    ~CPlayerUIBase();

    virtual CToolTipCtrl& GetToolTipCtrl() override { return m_tool_tip; }

public:
    void Init(CDC* pDC) override;
    virtual void DrawInfo(bool reset = false) override final;
    virtual void ClearInfo() override;

    virtual void LButtonDown(CPoint point) override;
    virtual void RButtonUp(CPoint point) override;
    virtual void MouseMove(CPoint point) override;
    virtual void LButtonUp(CPoint point) override;

    virtual CRect GetThumbnailClipArea() override;
    void UpdateRepeatModeToolTip();
    void UpdateSongInfoToolTip();
    void UpdatePlayPauseButtonTip() override;
    virtual void UpdateFullScreenTip() override;

    virtual bool SetCursor() override;
    virtual void MouseLeave() override;

    void ClearBtnRect();

    int Margin() const;
    int EdgeMargin(bool x = true) const;
    int WidthThreshold() const;
    int DrawAreaHeight() const;		//窄界面模式下显示播放列表时绘图区的高度

public:
    enum BtnKey		//标识按钮的类型
    {
        BTN_REPETEMODE,			//“循环模式”按钮
        BTN_VOLUME,				//音量按钮
        BTN_VOLUME_UP,
        BTN_VOLUME_DOWN,
        BTN_TRANSLATE,			//歌词翻译按钮
        BTN_SKIN,				//切换界面按钮
        BTN_EQ,					//音效设定按钮
        BTN_SETTING,			//设置按钮
        BTN_MINI,				//迷你模式按钮
        BTN_INFO,				//曲目信息按钮
        BTN_FIND,				//查找歌曲按钮
        BTN_LRYIC,              //桌面歌词按钮
		BTN_AB_REPEAT,			//AB重复按钮
        BTN_STOP,				//停止
        BTN_PREVIOUS,			//上一曲
        BTN_PLAY_PAUSE,			//播放/暂停
        BTN_NEXT,				//下一曲
        BTN_SHOW_PLAYLIST,		//显示/隐藏播放列表
        BTN_SELECT_FOLDER,		//选择文件夹
        BTN_PROGRESS,			//进度条
        BTN_COVER,				//专辑封面
        BTN_FULL_SCREEN,		//全屏显示按钮
        BTN_MENU,				//主菜单按钮
        BTN_FAVOURITE,		    //“我喜欢”按钮
        BTN_CLOSE,				//关闭按钮（迷你模式）
        BTN_RETURN,				//返回按钮（迷你模式）

    };

protected:
	struct DrawData
    {
        //CRect cover_rect;
        CRect lyric_rect;
        CRect thumbnail_rect;
    };

protected:
    virtual void _DrawInfo(bool reset = false) = 0;
    virtual void PreDrawInfo();
    void SetDrawRect();
    void DrawBackground();
    void DrawSongInfo(CRect rect, bool reset = false);
    void DrawToolBar(CRect rect, bool draw_translate_button);
    void DrawVolumnAdjBtn();
    void DrawControlBar(CRect rect);
    void DrawProgressBar(CRect rect);
    void DrawTranslateButton(CRect rect);
    int DrawTopRightIcons();			//绘制右上角的图标。返回总宽度
    void DrawCurrentTime();				//在右上角绘制当前系统时间
    void DrawStatusBar(CRect rect, bool reset = false);
    void DrawAlbumCover(CRect rect);

    void DrawUIButton(CRect rect, UIButton& btn, const IconRes& icon);
    void DrawControlButton(CRect rect, UIButton& btn, const IconRes& icon);
    void DrawTextButton(CRect rect, UIButton& btn, LPCTSTR text, bool back_color = false);

    virtual void AddMouseToolTip(BtnKey btn, LPCTSTR str) = 0;		//为一个按钮添加鼠标提示
    virtual void UpdateMouseToolTip(BtnKey btn, LPCTSTR str) = 0;
	virtual void UpdateMouseToolTip(int btn, LPCTSTR str) override { UpdateMouseToolTip(static_cast<BtnKey>(btn), str); }

    virtual void UpdateToolTipPosition() = 0;

    virtual void AddToolTips();			//为每一个按钮添加鼠标提示（由于按钮的矩形区域只有在第一次绘图之后才能确定，所以此函数必须在第一次绘图之后调用）

    static CRect DrawAreaToClient(CRect rect, CRect draw_area);
    static CRect ClientAreaToDraw(CRect rect, CRect draw_area);

    bool IsDrawNarrowMode();			//是否使用窄界面模式绘图
    bool IsDrawBackgroundAlpha() const;	//是否需要绘制透明背景

    wstring GetDisplayFormatString();       //获取显示格式的字符串

    int DPI(int pixel);
    int DPI(double pixel);
    double DPIDouble(double pixel);
    double GetScrollTextPixel(bool slower = false);       //计算滚动文本一次滚动的像素值，如果slower为true，则滚动得稍微慢一点

    virtual bool IsDrawLargeIcon();        //是否绘制大图标

    virtual int GetClassId() { return 0; }

private:
    void SetRepeatModeToolTipText();
    void SetSongInfoToolTipText();
    void SetCoverToolTipText();
    void DrawControlBarBtn(CRect rect, UIButton& btn, const IconRes& icon);
    void DrawPlayTag(CRect rect, LPCTSTR str_text);

protected:
    CWnd* m_pMainWnd = nullptr;
    CDC* m_pDC;
    UIColors m_colors;
    CUIDrawer m_draw{ m_colors };		//用于绘制文本的对象
    SLayoutData m_layout;
    //CFont m_font_time;
    DrawData m_draw_data;


    CToolTipCtrl m_tool_tip;

    CString m_repeat_mode_tip;
    CString m_info_tip;
    CString m_cover_tip;

    UIData& m_ui_data;

    //UI 数据
    CRect m_draw_rect;						//绘图区域
    bool m_show_volume_adj{ false };		//显示音量调整按钮

    std::map<BtnKey, UIButton> m_buttons;

    const int m_progress_on_top_threshold = theApp.DPI(350);		//当控制条的宽度小于此值，将进度条显示在播放控制按钮的上方

    bool m_first_draw{ true };

private:
    CBitmap m_mem_bitmap_static;
};

