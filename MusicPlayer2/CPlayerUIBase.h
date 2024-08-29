#pragma once
#include "MusicPlayer2.h"
#include "IPlayerUI.h"
#include "CPlayerUIHelper.h"
#include "CUIDrawer.h"
#include "IconMgr.h"

#define WM_MAIN_MENU_POPEDUP (WM_USER+117)      //显示弹出式主菜单的消息，wPara为表示菜单显示位置的CPoint的指针

namespace UiElement
{
    class Element;
    class Rectangle;
    class Button;
    class Text;
    class AlbumCover;
    class Spectrum;
    class TrackInfo;
    class Toolbar;
    class ProgressBar;
    class Lyrics;
    class Volume;
    class BeatIndicator;
    class StackElement;
    class ListElement;
    class Playlist;
    class PlaylistIndicator;
    class ClassicalControlBar;
    class MediaLibItemList;
    class MediaLibPlaylist;
    class MediaLibFolder;
    class RecentPlayedList;
    class NavigationBar;
    class MyFavouriteList;
    class AllTracksList;
    class MiniSpectrum;
    class FolderExploreTree;
    class SearchBox;
}

struct SLayoutData
{
    const int margin = theApp.DPI(4);                           //边缘的余量
    const int width_threshold = theApp.DPI(600);                //界面从普通界面模式切换到窄界面模式时界面宽度的阈值
    const int info_height = theApp.DPI(216);                    //窄界面模式时显示信息区域的高度
    const int path_edit_height = theApp.DPI(24);                //当前路径Edit控件的高度
    const int search_edit_height = theApp.DPI(26);              //歌曲搜索框Edit控件区域的高度
    const CSize spectral_size{ theApp.DPI(120), theApp.DPI(90) };   //频谱分析区域的大小
    const int toolbar_height = theApp.DPI(24);                  //播放列表工具栏的高度
    const int titlabar_height = theApp.DPI(28);                 //标题栏的高度
    const int menubar_height = theApp.DPI(24);                  //菜单栏的高度
};

namespace PlayerUiConstVal
{
    const int BTN_MAX_NUM = 1000;
}

class CPlayerUIBase : public IPlayerUI
{
public:
    CPlayerUIBase(UIData& ui_data, CWnd* pMainWnd);
    ~CPlayerUIBase();

    virtual CToolTipCtrl& GetToolTipCtrl() override { return m_tool_tip; }

    friend class UiElement::Element;
    friend class UiElement::Rectangle;
    friend class UiElement::Button;
    friend class UiElement::Text;
    friend class UiElement::AlbumCover;
    friend class UiElement::Spectrum;
    friend class UiElement::TrackInfo;
    friend class UiElement::Toolbar;
    friend class UiElement::ProgressBar;
    friend class UiElement::Lyrics;
    friend class UiElement::Volume;
    friend class UiElement::BeatIndicator;
    friend class UiElement::StackElement;
    friend class UiElement::ListElement;
    friend class UiElement::Playlist;
    friend class UiElement::PlaylistIndicator;
    friend class UiElement::ClassicalControlBar;
    friend class UiElement::MediaLibItemList;
    friend class UiElement::MediaLibPlaylist;
    friend class UiElement::MediaLibFolder;
    friend class UiElement::RecentPlayedList;
    friend class UiElement::NavigationBar;
    friend class UiElement::MyFavouriteList;
    friend class UiElement::AllTracksList;
    friend class UiElement::MiniSpectrum;
    friend class UiElement::FolderExploreTree;
    friend class UiElement::SearchBox;

    friend class UiFontGuard;

public:
    void Init(CDC* pDC) override;
    virtual void DrawInfo(bool reset = false) override final;
    virtual void ClearInfo() override;

    virtual bool LButtonDown(CPoint point) override;
    virtual void RButtonUp(CPoint point) override;
    virtual void MouseMove(CPoint point) override;
    virtual bool LButtonUp(CPoint point) override;
    virtual void RButtonDown(CPoint point) override;
    virtual bool MouseWheel(int delta, CPoint point) override;
    virtual bool DoubleClick(CPoint point) override;

    virtual CRect GetThumbnailClipArea() override;
    void UpdateRepeatModeToolTip();
    void UpdateSongInfoToolTip();
    void UpdatePlayPauseButtonTip() override;
    virtual void UpdateFullScreenTip() override;
    void UpdateTitlebarBtnToolTip();       //更新标题栏上的最大化/还原按钮的鼠标提示
    virtual void UpdateVolumeToolTip();
    void UpdatePlaylistBtnToolTip();
    void UpdateDarkLightModeBtnToolTip();
    void UpdateToolTipPositionLater();

    virtual bool SetCursor() override;
    virtual void MouseLeave() override;

    void ClearBtnRect();

    int Margin() const;
    int EdgeMargin(bool x = true) const;
    int WidthThreshold() const;
    int DrawAreaHeight() const;     //窄界面模式下显示播放列表时绘图区的高度

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

    virtual void UiSizeChanged() {}

    static CString GetCmdShortcutKeyForTooltips(UINT id);      //获取用于显示在鼠标提示中的键盘快捷键

    CRect GetVolumeRect() const;    //获取音量图标的矩形区域

protected:
    // 将字符串形如“%(KEY_STR)”格式的字符替换成当前<language>.ini中对应id的字符串
    static void ReplaceUiStringRes(wstring& str);

public:
    enum BtnKey     //标识按钮的类型
    {
        BTN_REPETEMODE,         //“循环模式”按钮
        BTN_VOLUME,             //音量按钮
        BTN_VOLUME_UP,
        BTN_VOLUME_DOWN,
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
        BTN_PROGRESS,           //进度条
        BTN_COVER,              //专辑封面
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
        BTN_MEDIALIB_FOLDER_SORT, //媒体库“文件夹”排序方式
        BTN_MEDIALIB_PLAYLIST_SORT, //媒体库“播放列表”排序方式
        BTN_KARAOKE,            //歌词卡拉OK模式显示

        //菜单栏
        MENU_FILE,
        MENU_PLAY_CONTROL,
        MENU_PLAYLIST,
        MENU_LYRICS,
        MENU_VIEW,
        MENU_TOOLS,
        MENU_HELP,

        BTN_INVALID,            //无效的按钮
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
    std::wstring GetButtonText(BtnKey key_type);

protected:
    struct DrawData
    {
        //CRect cover_rect;
        CRect lyric_rect;
        CRect thumbnail_rect;
    };

protected:
    virtual void _DrawInfo(CRect draw_rect, bool reset = false) = 0;
    virtual void PreDrawInfo();
    void SetDrawRect();
    void DrawBackground();
    void DrawSongInfo(CRect rect, int font_size = 9, bool reset = false);
    void DrawRectangle(const CRect& rect, bool no_corner_radius = false, bool theme_color = true, ColorMode color_mode = RCM_AUTO);       //绘制矩形。如果no_corner_radius为true，则总是绘制直角矩形，忽略“使用圆角风格按钮”的设置；theme_color：是否使用主题彦颜色
    void DrawToolBar(CRect rect, bool draw_translate_button);
    void DrawToolBarWithoutBackground(CRect rect, bool draw_translate_button);
    void DrawBeatIndicator(CRect rect);
    void DrawVolumnAdjBtn();
    void DrawControlBar(CRect rect, bool draw_switch_display_btn = false);
    void DrawProgressBar(CRect rect, bool play_time_both_side = false);               //绘制进度条（包含时间）。play_time_both_side如果为true，则播放时间显示的进度条的两侧，否则显示在进度条的右侧
    void DrawProgess(CRect rect);                   //绘制进度条
    void DrawTranslateButton(CRect rect);
    void DrawDesktopLyricButton(CRect rect);
    void DrawKaraokeButton(CRect rect);
    void DrawTopRightIcons();           //绘制右上角的图标
    void DrawCurrentTime();             //在右上角绘制当前系统时间
    void DrawAlbumCover(CRect rect);                //绘制专辑封面
    void DrawAlbumCoverWithInfo(CRect rect);        //绘制专辑封面，并在上面绘制歌曲的标题和艺术家
    void DrawVolumeButton(CRect rect, bool adj_btn_top = false, bool show_text = true);     //adj_btn_top：点击后弹出的音量调整按钮是否在上方；show_text：是否显示文本
    void DrawABRepeatButton(CRect rect);
    void DrawLyrics(CRect rect, CFont* lyric_font, CFont* lyric_tr_font, bool with_background, bool show_song_info = false);        //绘制歌词 rect：歌曲区域；with_background是否绘制背景；show_song_info:是否总是在没有歌词时显示歌曲信息
    void DrawList(CRect rect, UiElement::ListElement* list_element, int item_height);                  //绘制播放列表
    void DrawCurrentPlaylistIndicator(CRect rect, UiElement::PlaylistIndicator* playlist_indicator);      //绘制当前播放列表指示
    /**
     * @brief   绘制stackElement的指示器
     * @param   UIButton indicator 指示器信息
     * @param   int num 指示器中元素的数量
     * @param   int index 指示器当前索引
     */
    void DrawStackIndicator(UIButton indicator, int num, int index);
    void DrawUiMenuBar(CRect rect);
    void DrawNavigationBar(CRect rect, UiElement::NavigationBar* tab_element);
    void DrawMiniSpectrum(CRect rect);      //绘制图标大小的迷你频谱
    void DrawSearchBox(CRect rect, UiElement::SearchBox* search_box);

    // 实际绘制一个图标
    void DrawUiIcon(const CRect& rect, IconMgr::IconType icon_type, IconMgr::IconStyle icon_style = IconMgr::IconStyle::IS_Auto, IconMgr::IconSize icon_size = IconMgr::IconSize::IS_DPI_16);
    // 绘制一个UI按钮 (使用GetBtnIconType取得的图标)
    void DrawUIButton(const CRect& rect, BtnKey key_type, bool big_icon = false, bool show_text = false, int font_size = 9, bool checked = false);
    void DrawUIButton(const CRect& rect, UIButton& btn, IconMgr::IconType icon_type, bool big_icon = false, const std::wstring& text = std::wstring(), int font_size = 9, bool checked = false);
    // 绘制一个工具条按钮（将rect四面缩小 DPI(2) 后调用DrawUIButton）
    void DrawControlBarBtn(CRect rect, BtnKey btn_type);
    // 绘制一个UI按钮，以text文本作为图标
    void DrawTextButton(CRect rect, BtnKey btn_type, LPCTSTR text, bool checked = false);

    virtual void AddMouseToolTip(BtnKey btn, LPCTSTR str);      //为一个按钮添加鼠标提示
    virtual void UpdateMouseToolTip(BtnKey btn, LPCTSTR str);
    virtual void UpdateMouseToolTip(int btn, LPCTSTR str) override { UpdateMouseToolTip(static_cast<BtnKey>(btn), str); }
    virtual void UpdateMouseToolTipPosition(int btn, CRect rect);

    virtual void UpdateToolTipPosition() override;

    virtual void AddToolTips();         //为每一个按钮添加鼠标提示（由于按钮的矩形区域只有在第一次绘图之后才能确定，所以此函数必须在第一次绘图之后调用）

    //当绘图区域的原点不是窗口的原点的时候需要使用这两个函数在绘图区域坐标和窗口坐标中转换
    //现在这两个函数已弃用
    static CRect DrawAreaToClient(CRect rect, CRect draw_area);
    static CRect ClientAreaToDraw(CRect rect, CRect draw_area);

    bool IsDrawNarrowMode() const;            //是否使用窄界面模式绘图
    bool IsDrawBackgroundAlpha() const; //是否需要绘制透明背景
    virtual bool IsDrawStatusBar() const;       //是否需要绘制状态栏
    virtual bool IsDrawTitleBar() const;        //是否需要绘制标题栏
    virtual bool IsDrawMenuBar() const;         //是否需要绘制菜单栏

    static wstring GetDisplayFormatString();       //获取显示格式的字符串
    CString GetVolumeTooltipString();       //获取音量鼠标提示字符串

    int DPI(int pixel) const;
    int DPI(double pixel) const;
    double DPIDouble(double pixel);
    double GetScrollTextPixel(bool slower = false);       //计算滚动文本一次滚动的像素值，如果slower为true，则滚动得稍微慢一点
    int CalculateRoundRectRadius(const CRect& rect);        //计算绘制圆角矩形的半径

    virtual bool IsDrawLargeIcon() const;        //是否绘制大图标

    virtual void SwitchStackElement() {}

    bool IsMiniMode() const;

public:
    virtual int GetUiIndex() { return 1; }  //UI的序号，用于区分每个界面，不会为0

    void ShowUiTipInfo(const std::wstring& info);       //在界面的中央显示一个提示信息，几秒钟后自动消失

private:
    void SetRepeatModeToolTipText();
    void SetSongInfoToolTipText();
    void SetCoverToolTipText();
    void DrawPlayTag(CRect rect, LPCTSTR str_text);

    void DrawStatusBar(CRect rect, bool reset = false);
    void DrawTitleBar(CRect rect);

    int GetToolTipIdOffset();

protected:
    CWnd* m_pMainWnd = nullptr;
    CDC* m_pDC;
    UIColors m_colors;
    CUIDrawer m_draw{ m_colors };       //用于绘制文本的对象
    SLayoutData m_layout;
    //CFont m_font_time;
    DrawData m_draw_data;


    CToolTipCtrl m_tool_tip;

    wstring m_repeat_mode_tip;
    wstring m_info_tip;
    wstring m_cover_tip;

    UIData& m_ui_data;

    //UI 数据
    CRect m_draw_rect;                      //绘图区域
    bool m_show_volume_adj{ false };        //显示音量调整按钮
    bool m_show_volume_text{};        //是否显示音量文本

    std::map<BtnKey, UIButton> m_buttons;

    const int m_progress_on_top_threshold = theApp.DPI(350);        //当控制条的宽度小于此值，将进度条显示在播放控制按钮的上方

    bool m_first_draw{ true };

private:
    CBitmap m_mem_bitmap_static;

    bool m_need_update_tooltip_pos{ false };   //是否需要更新鼠标提示

    enum { UI_TIP_INFO_TIMER_ID = 1728 };

    static bool m_show_ui_tip_info;
    wstring m_ui_tip_info;
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
