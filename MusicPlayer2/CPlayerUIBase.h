#pragma once
#include "MusicPlayer2.h"
#include "IPlayerUI.h"
#include "CPlayerUIHelper.h"
#include "CUIDrawer.h"
#include "IconMgr.h"
#include "IMouseEvent.h"

#define WM_MAIN_MENU_POPEDUP (WM_USER+117)      //显示弹出式主菜单的消息，wPara为表示菜单显示位置的CPoint的指针

struct SLayoutData
{
    const int margin = theApp.DPI(4);                           //边缘的余量
    const int width_threshold = theApp.DPI(600);                //界面从普通界面模式切换到窄界面模式时界面宽度的阈值
    const int info_height = theApp.DPI(216);                    //窄界面模式时显示信息区域的高度
    const int height_threshold = theApp.DPI(260);               //界面布局从big或narrow变为small时的高度
    const int path_edit_height = theApp.DPI(24);                //当前路径Edit控件的高度
    const int search_edit_height = theApp.DPI(26);              //歌曲搜索框Edit控件区域的高度
    const int toolbar_height = theApp.DPI(24);                  //播放列表工具栏的高度
    const int titlabar_height = theApp.DPI(28);                 //标题栏的高度
    const int menubar_height = theApp.DPI(24);                  //菜单栏的高度
};

namespace PlayerUiConstVal
{
    const int BTN_MAX_NUM = 1000;
}

class CPlayerUIBase : public IPlayerUI, public IMouseEvent
{
public:
    CPlayerUIBase(UIData& ui_data, CWnd* pMainWnd);
    ~CPlayerUIBase();

    virtual CToolTipCtrl& GetToolTipCtrl() override { return m_tool_tip; }

    friend class UiFontGuard;

public:
    void Init(CDC* pDC) override;
    virtual void DrawInfo(bool reset = false) override final;

    // IMouseEvent
    virtual bool LButtonDown(CPoint point) override;
    virtual bool RButtonUp(CPoint point) override;
    virtual bool MouseMove(CPoint point) override;
    virtual bool LButtonUp(CPoint point) override;
    virtual bool RButtonDown(CPoint point) override;
    virtual bool MouseWheel(int delta, CPoint point) override;
    virtual bool DoubleClick(CPoint point) override;
    virtual bool MouseLeave() override;

    virtual CRect GetThumbnailClipArea() override;
    void UpdateRepeatModeToolTip();
    void UpdateSongInfoToolTip();
    void UpdatePlayPauseButtonTip() override;
    virtual void UpdateFullScreenTip() override;
    void UpdateTitlebarBtnToolTip();       //更新标题栏上的最大化/还原按钮的鼠标提示
    virtual void UpdateVolumeToolTip();
    void UpdatePlaylistBtnToolTip();
    void UpdateDarkLightModeBtnToolTip();
    void HideTooltip();

    virtual bool SetCursor() override;

    void ClearBtnRect();

    int Margin() const;
    int EdgeMargin(bool x = true) const;
    int WidthThreshold() const;
    int DrawAreaHeight() const;     //窄界面模式下显示播放列表时绘图区的高度
    int TopRightButtonsWidth() const;

    virtual bool PointInControlArea(CPoint point) const;        //判断一个点的位置是否在控件区域
    bool PointInTitlebarArea(CPoint point) const;
    bool PointInAppIconArea(CPoint point) const;
    bool PointInMenubarArea(CPoint point) const;

    //获取界面的名称
    virtual wstring GetUIName() { return wstring(); }

    enum class UiSize
    {
        BIG,
        NARROW,
        SMALL
    };

    UiSize GetUiSize() const;

    virtual void UiSizeChanged(UiSize last_ui_size) {}

    static CString GetCmdShortcutKeyForTooltips(UINT id);      //获取用于显示在鼠标提示中的键盘快捷键

    CRect GetDrawRect() const;
    CRect GetClientDrawRect() const;    //获取绘图客户区的矩形区域（不包含自绘标题栏、菜单栏、状态栏）
    CRect GetAppIconRect() const;       //获取应用程序图标矩形区域

    CUIDrawer& GetDrawer() { return m_draw; }
    const UIColors& GetUIColors() const { return m_colors; }
    CWnd* GetOwner() const { return m_pMainWnd; }

    // 将字符串形如“%(KEY_STR)”格式的字符替换成当前<language>.ini中对应id的字符串
    static void ReplaceUiStringRes(wstring& str);

public:
    enum BtnKey     //标识按钮的类型
    {
        BTN_INVALID,            //无效的按钮
        BTN_REPETEMODE,         //“循环模式”按钮
        BTN_VOLUME,             //音量按钮
        BTN_TRANSLATE,          //歌词翻译按钮
        BTN_SKIN,               //切换界面按钮
        BTN_SKIN_TITLEBAR,      //标题栏上的切换界面按钮
        BTN_EQ,                 //音效设定按钮
        BTN_SETTING,            //设置按钮
        BTN_SETTING_TITLEBAR,   //标题栏上的设置按钮
        BTN_MINI,               //迷你模式按钮
        BTN_MINI_TITLEBAR,      //标题栏上的迷你模式按钮
        BTN_INFO,               //曲目信息按钮
        BTN_FIND,               //查找歌曲按钮
        BTN_LRYIC,              //桌面歌词按钮
        BTN_AB_REPEAT,          //AB重复按钮
        BTN_STOP,               //停止
        BTN_PREVIOUS,           //上一曲
        BTN_PLAY_PAUSE,         //播放/暂停
        BTN_NEXT,               //下一曲
        BTN_SHOW_PLAYLIST,      //显示/隐藏播放列表
        BTN_MEDIA_LIB,          //媒体库
        BTN_FULL_SCREEN_TITLEBAR, //标题栏上的全屏显示按钮
        BTN_FULL_SCREEN,        //全屏显示按钮
        BTN_MENU_TITLEBAR,      //标题栏上的主菜单按钮
        BTN_MENU,               //主菜单按钮
        BTN_FAVOURITE,          //“我喜欢”按钮
        BTN_CLOSE,              //关闭按钮（迷你模式）
        BTN_MINIMIZE,           //最小化按钮
        BTN_MAXIMIZE,           //最大化按钮
        BTN_APP_CLOSE,          //关闭按钮
        BTN_ADD_TO_PLAYLIST,    //添加到播放列表按钮
        BTN_SWITCH_DISPLAY,     //切换界面中的stackElement
        BTN_DARK_LIGHT,         //切换深色/浅色模式
        BTN_DARK_LIGHT_TITLE_BAR, //标题栏中的切换深色/浅色模式
        BTN_LOCATE_TO_CURRENT,  //播放列表定位到当前播放
        BTN_OPEN_FOLDER,        //打开文件夹
        BTN_NEW_PLAYLIST,       //新建播放列表
        BTN_PLAY_MY_FAVOURITE,  //播放“我喜欢的音乐”
        BTN_PLAY_ALL_TRACKS,    //播放“所有曲目”
        BTN_MEDIALIB_FOLDER_SORT, //媒体库“文件夹”排序方式
        BTN_MEDIALIB_PLAYLIST_SORT, //媒体库“播放列表”排序方式
        BTN_KARAOKE,            //歌词卡拉OK模式显示
        BTN_SHOW_PLAY_QUEUE,    //显示正确播放队列
        BTN_CLOSE_PANEL,        //关闭面板
        BTN_SHOW_PANEL,         //显示面板
        BTN_SHOW_HIDE_ELEMENT,  //显示/隐藏一个元素
        BTN_CLOSE_PANEL_TITLE_BAR,  //标题栏中的关闭面板按钮
        BTN_SHOW_SETTINGS_PANEL,  //显示设置面板

        //菜单栏
        MENU_FILE,
        MENU_PLAY_CONTROL,
        MENU_PLAYLIST,
        MENU_LYRICS,
        MENU_VIEW,
        MENU_TOOLS,
        MENU_HELP,

        BTN_MAX,
    };

    enum ColorMode
    {
        RCM_AUTO,
        RCM_DARK,
        RCM_LIGHT
    };

    // 获取参数按钮当前应当使用的图标类型
    // 将BtnKey枚举和当前状态组合映射为IconMgr::IconType枚举
    IconMgr::IconType GetBtnIconType(BtnKey key);

    //获取按钮的文本
    std::wstring GetButtonText(BtnKey key_type) const;

protected:
    virtual void _DrawInfo(CRect draw_rect, bool reset = false) = 0;
    virtual void PreDrawInfo();
    void DrawBackground();

public:
    void DrawSongInfo(CRect rect, int font_size = 9, bool reset = false);
    void DrawPlayEffectTag(CRect parent_rect, CRect& previous_item_rect);   //绘制播放效果的标签。parent_rect播放效果标签所在父元素的区域；前一个元素的区域，绘制后会更新此矩形区域
    void DrawRectangle(const CRect& rect, bool no_corner_radius = false, bool theme_color = true, ColorMode color_mode = RCM_AUTO);       //绘制矩形。如果no_corner_radius为true，则总是绘制直角矩形，忽略“使用圆角风格按钮”的设置；theme_color：是否使用主题彦颜色
    void DrawRectangle(CRect rect, COLORREF color);
    void DrawBeatIndicator(CRect rect);
    CRect DrawProgressBar(CRect rect, bool play_time_both_side = false);               //绘制进度条（包含时间）。play_time_both_side如果为true，则播放时间显示的进度条的两侧，否则显示在进度条的右侧（返回进度条部分的矩形区域）
    CRect DrawProgess(CRect rect);                   //绘制进度条（返回进度条部分的矩形区域）
    void DrawTopRightIcons();           //绘制右上角的图标
    void DrawCurrentTime();             //在右上角绘制当前系统时间
    void DrawAlbumCover(CRect rect);                //绘制专辑封面
    void DrawAlbumCoverWithInfo(CRect rect);        //绘制专辑封面，并在上面绘制歌曲的标题和艺术家
    void DrawLyrics(CRect rect, CFont* lyric_font, CFont* lyric_tr_font, bool with_background, bool show_song_info = false);        //绘制歌词 rect：歌曲区域；with_background是否绘制背景；show_song_info:是否总是在没有歌词时显示歌曲信息
    /**
     * @brief   绘制stackElement的指示器
     * @param   UIButton indicator 指示器信息
     * @param   int num 指示器中元素的数量
     * @param   int index 指示器当前索引
     */
    void DrawStackIndicator(UIButton indicator, int num, int index);
    void DrawUiMenuBar(CRect rect);
    void DrawMiniSpectrum(CRect rect);      //绘制图标大小的迷你频谱

    // 实际绘制一个图标
    void DrawUiIcon(const CRect& rect, IconMgr::IconType icon_type, IconMgr::IconStyle icon_style = IconMgr::IconStyle::IS_Auto, IconMgr::IconSize icon_size = IconMgr::IconSize::IS_DPI_16);
    // 绘制一个UI按钮 (使用GetBtnIconType取得的图标)
    void DrawUIButton(const CRect& rect, BtnKey key_type, bool big_icon = false, bool show_text = false, int font_size = 9, bool checked = false);
    void DrawUIButton(const CRect& rect, BtnKey key_type, UIButton& btn, bool big_icon = false, bool show_text = false, int font_size = 9, bool checked = false);
    void DrawUIButton(const CRect& rect, UIButton& btn, IconMgr::IconType icon_type, bool big_icon = false, const std::wstring& text = std::wstring(), int font_size = 9, bool checked = false);
    // 绘制一个UI按钮，以text文本作为图标
    void DrawTextButton(CRect rect, BtnKey btn_type, LPCTSTR text, bool checked = false);
    void DrawTextButton(CRect rect, UIButton& btn, LPCTSTR text, bool checked = false);

    //获取绘图的默认不透明度
    BYTE GetDefaultAlpha() const;

    virtual void AddMouseToolTip(int btn, LPCTSTR str);      //为一个按钮添加鼠标提示
    virtual void UpdateMouseToolTip(int btn, LPCTSTR str) override;
    virtual void UpdateMouseToolTipPosition(int btn, CRect rect);

    virtual void AddToolTips();         //为每一个按钮添加鼠标提示（由于按钮的矩形区域只有在第一次绘图之后才能确定，所以此函数必须在第一次绘图之后调用）

    //响应一个按钮点击
    virtual bool ButtonClicked(BtnKey btn_type, const UIButton& btn);
    //响应一个按钮右键点击
    virtual bool ButtonRClicked(BtnKey btn_type, const UIButton& btn);

    bool IsDrawBackgroundAlpha() const; //是否需要绘制透明背景
    bool IsDrawStatusBar() const;       //是否需要绘制状态栏
    bool IsDrawTitleBar() const;        //是否需要绘制标题栏
    bool IsDrawMenuBar() const;         //是否需要绘制菜单栏
    bool IsDrawLargeIcon() const;        //是否绘制大图标

    static wstring GetDisplayFormatString();       //获取显示格式的字符串

protected:
    CString GetVolumeTooltipString();       //获取音量鼠标提示字符串

public:
    int DPI(int pixel) const;
    int DPI(double pixel) const;
    double DPIDouble(double pixel);
    int CalculateRoundRectRadius(const CRect& rect);        //计算绘制圆角矩形的半径
    double GetScrollTextPixel(bool slower = false);       //计算滚动文本一次滚动的像素值，如果slower为true，则滚动得稍微慢一点


    //切换堆叠元素（查找当前界面中第一个堆叠元素，并执行一次切换）
    virtual void SwitchStackElement() {}

    /**
     * @brief   切换堆叠元素
     * @param   id 堆叠元素的id（如果为空时查找第一个堆叠元素）
     * @param   index 要切换到堆叠元素的索引（如果为负数时默认切换到下一个索引）
     */
    virtual void SwitchStackElement(std::string id, int index) {}

protected:
    bool IsMiniMode() const;
    virtual bool IsDrawTitlebarLeftBtn() const { return false; }  //是否显示标题栏左侧图标

public:
    virtual int GetUiIndex() { return 1; }  //UI的序号，用于区分每个界面，不会为0

    void ShowUiTipInfo(const std::wstring& info);       //在界面的中央显示一个提示信息，几秒钟后自动消失

    //获取一个按钮或其他界面元素的鼠标提示（tooltip_index可以是按钮的key，或者Element::TooltipIndex中的枚举值）
    std::wstring GetItemTooltip(int tooltip_index);

    //跳过一帧画面
    void SkipNextFrame();

private:
    void SetRepeatModeToolTipText();
    void SetSongInfoToolTipText();
    void DrawPlayTag(CRect rect, LPCTSTR str_text);

    void DrawStatusBar(CRect rect, bool reset = false);
    void DrawTitleBar(CRect rect);

    int GetToolTipIdOffset();

protected:
    CWnd* m_pMainWnd = nullptr;
    CDC* m_pDC = nullptr;
    UIColors m_colors;
    CUIDrawer m_draw{ m_colors };       //用于绘制文本的对象
    SLayoutData m_layout;
    //CFont m_font_time;
    CRect m_thumbnail_rect;


    CToolTipCtrl m_tool_tip;

    wstring m_repeat_mode_tip;
    wstring m_info_tip;

    UIData& m_ui_data;

    //UI 数据
    CRect m_draw_rect;                      //绘图区域

    //这个map只保存UI中的标题栏、菜单栏和音量按钮的信息，其他按钮信息应该保存在各自的UiElement::Button中，如果map出现了其他UI中的其他按钮应为异常情况
    std::map<BtnKey, UIButton> m_buttons;

    bool m_first_draw{ true };

private:
    enum { UI_TIP_INFO_TIMER_ID = 1728 };

    static bool m_show_ui_tip_info;
    wstring m_ui_tip_info;
    int m_top_right_buttons_width{};
    CRect m_app_icon_rect{};        //标题栏应用图标区域

    bool m_skip_next_frame{};
};

//用于在UI中设置字体。
//在需要设置字体时，创建此类的一个局部对象，它会在构造时设置字体，并在析构时恢复原来的字体
class UiFontGuard
{
public:
    UiFontGuard(CPlayerUIBase* _ui, int font_size)
        : ui(_ui)
    {
        if (ui != nullptr)
        {
            bool big_font{ ui->m_ui_data.full_screen && ui->IsDrawLargeIcon() };
            //设置字体
            old_font = ui->m_draw.SetFont(&theApp.m_font_set.GetFontBySize(font_size).GetFont(big_font));
        }
    }
    ~UiFontGuard()
    {
        if (ui != nullptr)
        {
            //恢复原来的字体
            if (old_font != nullptr)
                ui->m_draw.SetFont(old_font);
        }
    }

private:
    CFont* old_font{};
    CPlayerUIBase* ui{};
};
