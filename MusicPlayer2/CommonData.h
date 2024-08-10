#pragma once
#include "ColorConvert.h"
#include "DrawCommon.h"
#include "Common.h"

namespace CONSTVAL
{
    const COLORREF BACKGROUND_COLOR = GRAY(255);		//更改此颜色的值可以修改主窗口背景色
    const double FULL_SCREEN_ZOOM_FACTOR = 1.5;
}

struct DeviceInfo	//播放设备的信息
{
    int index;		//设备的索引
    wstring name;	//设备的名称
    wstring driver;	//设备的驱动程序
    DWORD flags;	//设备的状态
};


struct FontStyle
{
    bool bold{ false };			//粗体
    bool italic{ false };		//斜体
    bool underline{ false };	//下划线
    bool strike_out{ false };	//删除线

    int ToInt()
    {
        int value = 0;
        if (bold)
            value |= 0x01;
        if (italic)
            value |= 0x02;
        if (underline)
            value |= 0x04;
        if (strike_out)
            value |= 0x08;
        return value;
    }

    void FromInt(int value)
    {
        bold = (value % 2 != 0);
        italic = ((value >> 1) % 2 != 0);
        underline = ((value >> 2) % 2 != 0);
        strike_out = ((value >> 3) % 2 != 0);
    }
};

struct FontInfo
{
    wstring name;	//字体名称
    int size;		//字体大小
    FontStyle style;	//字体样式
};


struct UIFont
{
private:
    CFont font;
    CFont font_l;

public:
    CFont& GetFont(bool large = false)
    {
        return (large ? font_l : font);
    }

    void SetFont(int font_size, LPCTSTR font_name, FontStyle style = FontStyle())
    {
        if (font_size < 5)
            font_size = 5;

        if (font.m_hObject)
            font.DeleteObject();
        CreateFontSimple(font, font_size, font_name, style);

        if (font_l.m_hObject)
            font_l.DeleteObject();
        CreateFontSimple(font_l, static_cast<int>(font_size * CONSTVAL::FULL_SCREEN_ZOOM_FACTOR), font_name, style);
    }

    void SetFont(FontInfo font_info)
    {
        SetFont(font_info.size, font_info.name.c_str(), font_info.style);
    }

    static void CreateFontSimple(CFont& font, int font_size, LPCTSTR font_name, FontStyle style = FontStyle())
    {
        font.CreateFont(
            FontSizeToLfHeight(font_size), // nHeight
            0, // nWidth
            0, // nEscapement
            0, // nOrientation
            (style.bold ? FW_BOLD : FW_NORMAL), // nWeight
            style.italic, // bItalic
            style.underline, // bUnderline
            style.strike_out, // cStrikeOut
            DEFAULT_CHARSET, // nCharSet
            OUT_DEFAULT_PRECIS, // nOutPrecision
            CLIP_DEFAULT_PRECIS, // nClipPrecision
            DEFAULT_QUALITY, // nQuality
            DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily
            font_name);
    }

    //将字号转成LOGFONT结构中的lfHeight
    static int FontSizeToLfHeight(int font_size)
    {
        HDC hDC = ::GetDC(HWND_DESKTOP);
        int lfHeight = -MulDiv(font_size, GetDeviceCaps(hDC, LOGPIXELSY), 72);
        ::ReleaseDC(HWND_DESKTOP, hDC);
        return lfHeight;
    }

};

struct FontSet
{
    const int FONT_SIZE_MIN{ 8 };
    const int FONT_SIZE_MAX{ 16 };
    std::map<int, UIFont> fonts;    //不同大小的默认字体

    UIFont dlg;                 // 窗口控件字体
    UIFont lyric;				//歌词字体
    UIFont lyric_translate;		//歌词翻译的字体
    UIFont cortana;				//搜索框字体
    UIFont cortana_translate;	//搜索框翻译字体


    void Init(LPCTSTR font_name);

    //获取一个指定大小的字体（目前支持8~16）
    UIFont& GetFontBySize(int font_size);

};


//播放列表中项目的显示格式
enum DisplayFormat
{
    DF_FILE_NAME,		//文件名
    DF_TITLE,			//标题
    DF_ARTIST_TITLE,	//艺术家 - 标题
    DF_TITLE_ARTIST		//标题 - 艺术家
};

//最近播放曲目列表显示范围
enum RecentPlayedRange
{
    RPR_ALL,			//显示全部
    RPR_TODAY,          //今天
    RPR_THREE_DAYS,     //最近三天
    RPR_WEAK,			//最近一个星期
    RPR_MONTH,			//最近一个月
    RPR_HALF_YEAR,		//最近半年
    RPR_YEAR,			//最近一年
};


//选项设置数据

struct DesktopLyricSettingData		//桌面歌词设置
{
    bool lyric_double_line{ false };
    FontInfo lyric_font;
    COLORREF text_color1{};
    COLORREF text_color2{};
    int text_gradient{};
    COLORREF highlight_color1{};
    COLORREF highlight_color2{};
    int highlight_gradient{};
    int opacity{ 100 };
    bool lock_desktop_lyric{ false };
    bool hide_lyric_window_without_lyric{ false };	//没有歌词时隐藏歌词窗口
    bool hide_lyric_window_when_paused{ false };	//暂停时隐藏歌词窗口
    bool lyric_background_penetrate{ false };
    bool show_unlock_when_locked{ true };           //桌面歌词锁定时显示解锁图标
    Alignment lyric_align{ Alignment::AUTO };   //歌词的对齐方式
};

struct LyricSettingData
{
    bool lyric_karaoke_disp{ true };			//可以是否以卡拉OK样式显示
    bool lyric_fuzzy_match{ true };				//歌词模糊匹配
    // bool save_lyric_in_offset{};				//是否将歌词保存在offset标签中，还是保存在每个时间标签中
    wstring lyric_path;							//歌词文件夹的路径
    bool use_inner_lyric_first{};				//优先使用内嵌歌词
    bool show_translate{ true };		        //歌词是否显示翻译
    bool donot_show_blank_lines{};              //单行和双行显示模式下不显示空白行
    bool show_song_info_if_lyric_not_exist{};   //是否在没有歌词时显示歌曲信息

    enum LyricSavePolicy		//歌词保存策略
    {
        LS_DO_NOT_SAVE,			//不保存（手动保存）
        LS_AUTO_SAVE,			//自动保存
        LS_INQUIRY				//询问
    };

    LyricSavePolicy lyric_save_policy{};		//歌词自动保存策略

    FontInfo lyric_font;						//歌词字体
    int lyric_line_space{ 2 };					//歌词的行间距
    Alignment lyric_align{ Alignment::AUTO };   //歌词的对齐方式

    bool cortana_info_enable{};				    //是否允许在Cortana的搜索框中显示信息
    bool cortana_show_lyric{ true };            //是否在Cortana搜索框中显示歌词
    bool cortana_lyric_double_line{ true };		//是否在Cortana搜索中以双行显示歌词
    int cortana_color{ 0 };						//Cortana搜索框的背景颜色（0：跟随系统，1：黑色，2：白色）
    bool cortana_show_album_cover{ true };		//是否在Cortana搜索框显示专辑封面
    bool cortana_icon_beat{ true };				//Cortana图标随音乐节奏跳动
    bool cortana_lyric_compatible_mode{ false };	//Cortana搜索框歌词显示使用兼容模式
    FontInfo cortana_font;						//搜索框字体
    bool cortana_lyric_keep_display{ false };	//搜索框歌词是否在暂停时保持显示
    bool cortana_show_spectrum{ false };		//是否在搜索框显示频谱
    bool cortana_opaque{ false };				//搜索框不透明
    Alignment cortana_lyric_align{ Alignment::AUTO };           //搜索框歌词对齐方式
    bool show_default_album_icon_in_search_box{ false };      //没有歌词时搜索框显示黑色胶片图标
    COLORREF cortana_transparent_color{};

    bool show_desktop_lyric{ false };			//显示桌面歌词
    DesktopLyricSettingData desktop_lyric_data;
};

struct ApperanceSettingData
{
    int window_transparency{ 100 };				//窗口透明度
    ColorTable theme_color;						//主题颜色
    bool theme_color_follow_system{ true };		//主题颜色跟随系统（仅Win8以上支持）
    bool show_album_cover;						//显示专辑封面
    CDrawCommon::StretchMode album_cover_fit{ CDrawCommon::StretchMode::FILL };		//专辑封面契合度（拉伸模式）
    bool enable_background{ true };
    bool album_cover_as_background{ false };	//将专辑封面作为背景
    bool show_spectrum{ true };					//显示频谱分析
    int sprctrum_height{ 100 };					//频谱分析高度比例（%）
    bool spectrum_low_freq_in_center{ false };  //频谱分析低频部分显示在中间
    bool use_old_style_specturm{ false };       //使用旧风格的频谱分析显示
    int background_transparency{ 80 };			//背景的透明度

    bool use_out_image{ true };					//使用外部图片作为专辑封面
    bool use_inner_image_first{ true };			//优先使用内嵌专辑封面
    wstring album_cover_path;                         // 专辑封面存储路径
    vector<wstring> default_album_name;			//默认的专辑封面文件名

    bool background_gauss_blur{ true };			//背景高斯模糊
    int gauss_blur_radius{ 60 };				//高斯模糊半径*10
    bool lyric_background{ true };				//歌词界面背景
    bool dark_mode{ false };					//深色模式

    bool draw_album_high_quality{ false };      //专辑封面图片使用Gdi+高质量绘图
    int ui_refresh_interval{ 100 };             //界面刷新的时间间隔

    int notify_icon_selected{};                 //使用的通知区图标
    bool notify_icon_auto_adapt{ false }; //通知区图标是否自动适应Win10深浅色模式

    bool button_round_corners{ false };     //按钮是否使用圆角风格

    wstring default_background;     //默认的背景图片

    int playlist_width_percent{ 50 };       //主界面播放列表宽度的百分比

    bool use_desktop_background{ false };   //使用桌面壁纸作为背景

    bool always_show_statusbar{ false };    //总是显示状态栏
    bool show_fps{ true };              //是否在状态栏显示帧率
    bool show_next_track{ false };      //是否在状态栏显示下一首播放曲目

    bool show_window_frame{ true };     //显示标准窗口边框
    bool show_minimize_btn_in_titlebar{ true };     //是否在标题栏显示“最小化”按钮
    bool show_maximize_btn_in_titlebar{ true };     //是否在标题栏显示“最大化”按钮
    bool show_minimode_btn_in_titlebar{ true };     //是否在标题栏显示“迷你模式”按钮
    bool show_fullscreen_btn_in_titlebar{ true };   //是否在标题栏显示“全屏模式”按钮
    bool show_skin_btn_in_titlebar{ false };        //是否在标题栏显示“切换界面”按钮
    bool show_settings_btn_in_titlebar{ false };    //是否在标题栏显示“设置”按钮
    bool show_dark_light_btn_in_titlebar{ false };  //是否在标题栏显示“深色模式/浅色模式”按钮
    int TitleDisplayItem() const;

    //如果为true时，当系统为Windows10/11时，如果使用的是自绘标题栏，则去掉标题栏顶部的白边
    //（目前还存在一些问题，当窗口得到或失去焦点时，窗口会闪烁）
    bool remove_titlebar_top_frame{ false };
};

struct GeneralSettingData
{
    bool id3v2_first{ false };					//优先获取ID3V2标签
    bool auto_download_lyric{ false };			//是否自动下载歌词
    bool auto_download_album_cover{ true };		//是否自动下载专辑封面
    bool auto_download_only_tag_full{ true };	//仅在歌曲信息完整时自动下载
    bool save_lyric_to_song_folder{ true };     // 将自动下载的歌词文件保存在歌曲文件夹
    bool save_album_to_song_folder{ true };     // 将自动下载的封面文件保存在歌曲文件夹
    bool download_lyric_text_and_translation_in_same_line{ true };  //下载的歌词原文和翻译在同一行
    bool check_update_when_start{ true };		//是否在程序启动时检查更新
    int update_source{};                        //更新源。0: GitHub; 1: Gitee
    bool minimize_to_notify_icon{ false };		//是否最小到通知区图标
    bool global_mouse_wheel_volume_adjustment{ true };  //全局鼠标滚轮音量调节

    wstring language_;                          // 这个是设置状态（空字符串为跟随系统）
    bool portable_mode{ false };                //如果为true，则程序所有数据都保存到exe所在目录下，否则保存到Appdata\Romaing目录下
};

struct PlaySettingData
{
    bool stop_when_error{ true };				//出现错误时停止播放
    bool auto_play_when_start{ false };			//程序启动时自动播放
    bool continue_when_switch_playlist{ false };//若当前播放歌曲存在于切换到的播放列表则保持播放状态不变
    bool show_taskbar_progress{ false };		//在任务栏按钮上显示播放进度
    bool show_playstate_icon{ true };			//在任务栏按钮上显示播放状态的角标
    wstring output_device;						//播放设备的名称
    int device_selected{};
    bool fade_effect{ true };                   //播放淡入淡出效果
    int fade_time{ 500 };                      //淡入淡出时间（毫秒）
    bool use_media_trans_control{};             //使用系统MediaTransportControls

    bool use_mci{ false };              //是否使用MCI内核
    /// 是否使用ffmpeg内核
    bool use_ffmpeg{ false };
    /// ffmpeg内核缓存时长（单位：s）
    int ffmpeg_core_cache_length { 15 };
    /// ffmpeg内核最大重试次数
    int ffmpeg_core_max_retry_count { 3 };
    /// ffmpeg内核非本地文件重试间隔时间（单位s）
    int ffmpeg_core_url_retry_interval { 5 };
    /// ffmpeg内核是否启用WASAPI
    int ffmpeg_core_enable_WASAPI { false };
    /// ffmpeg内核是否启用WASAPI独占模式
    int ffmpeg_core_enable_WASAPI_exclusive_mode { false };
    /// ffmpeg内核seek等操作最大等待时间
    int ffmpeg_core_max_wait_time { 3000 };

    //MIDI设置
    wstring sf2_path;							//MIDI音色库路径
    bool midi_use_inner_lyric{ false };			//播放MIDI音乐时显示内嵌歌词
};

struct GlobalHotKeySettingData
{
    bool hot_key_enable = true;
    bool global_multimedia_key_enable{ true };	//是否在全局范围内启用多媒体键
};

enum MediaLibDisplayItem
{
    MLDI_ARTIST = 1,
    MLDI_ALBUM = (1 << 1),
    MLDI_GENRE = (1 << 2),
    MLDI_YEAR = (1 << 3),
    MLDI_TYPE = (1 << 4),
    MLDI_BITRATE = (1 << 5),
    MLDI_RATING = (1 << 6),
    MLDI_ALL = (1 << 7),
    MLDI_RECENT = (1 << 8),
    MLDI_FOLDER_EXPLORE = (1 << 9)
};

struct MediaLibSettingData
{
    vector<wstring> media_folders;      //媒体库文件夹浏览中显示的文件夹
    vector<wstring> artist_split_ext;   // 艺术家分割例外，设置名字本身含有分隔符（/;&、）的艺术家（22/7）
    bool hide_only_one_classification;  //媒体库中将只有一项的分类归到其他类中
    bool disable_delete_from_disk;      //禁用从磁盘删除
    bool show_tree_tool_tips;           //树控件显示鼠标提示
    bool update_media_lib_when_start_up;    //启动时自动更新媒体库
    bool ignore_too_short_when_update;      // 自动更新/刷新媒体库时忽略时长低于阈值的文件（不影响手动加入）
    int file_too_short_sec;                 // 音频低时长阈值（清理媒体库功能也使用这个设置）
    bool remove_file_not_exist_when_update{};   //更新媒体库时移除不存在的音频文件
    bool disable_drag_sort;				//禁止通过拖放排序
    DisplayFormat display_format{};		//播放列表中项目的显示样式
    bool insert_begin_of_playlist{ false };         // 向播放列表添加歌曲时插入开头而不是追加到末尾
    bool show_playlist_tooltip{};         //显示播放列表工具提示
    bool float_playlist_follow_main_wnd{};  //浮动播放列表跟随主窗口
    bool playlist_btn_for_float_playlist{ false };      // 指定主界面中进度条右侧的“显示/隐藏播放列表”按钮的功能是否为显示浮动播放列表
    int playlist_item_height{ 24 };
    RecentPlayedRange recent_played_range{};	//最近播放曲目列表的显示范围
    int display_item{};                 //媒体库显示的项目
    bool write_id3_v2_3{ false };       //写入的ID3V2版本是否为2.3，否则为2.4
    bool enable_lastfm { false };       ///是否启用Last.fm相关功能
    int lastfm_least_perdur { 50 };     ///将记录写入缓存要求播放的百分比
    int lastfm_least_dur { 60 };        ///将记录写入缓存要求播放的秒数
    bool lastfm_auto_scrobble { true }; ///是否自动上传缓存里的记录
    int lastfm_auto_scrobble_min { 1 }; ///自动上传缓存里的记录的下限
    bool lastfm_enable_https { false }; ///是否使用HTTPS与last.fm服务器通信
    bool lastfm_enable_nowplaying{ true }; ///是否上传当前播放歌曲
};

struct NonCategorizedSettingData
{
    int volum_step{ 3 };			//点击主界面中的音量调节时一次调整的步长
    int mouse_volum_step{ 2 };		//通过鼠标滚轮调节音量时的步长
    int volume_map{ 100 };			//音量映射（例如：如果将此值从100改为60，则当音量设置为最大（100%）时的音量大小为原来的60%）
    bool show_cover_tip{ true };	//是否显示专辑封面上的鼠标提示
    //wstring default_back_image_path{};	//没有专辑封面时的默认背景的路径
    bool no_sf2_warning{ true };	//是否在没有MIDI音色库时弹出提示信息
    bool show_hide_menu_bar_tip{ true };	//是隐藏菜单栏是否弹出提示信息
    bool always_on_top{ false };	//是否总是置顶
    wstring default_osu_img;

    bool float_playlist{ false };		//浮动播放列表（不应该用此变量来判断浮动播放列表是否存在）
    CSize playlist_size{ 320, 530 };		//浮动播放列表的大小

    int max_album_cover_size{ 800 };
    bool show_debug_info{ false };

    int light_mode_default_transparency{ 80 };
    int dark_mode_default_transparency{ 40 };

    vector<wstring> default_file_type;
    vector<wstring> user_defined_type_ffmpeg;   //FFMPEG内核时用户添加的文件格式扩展名

    enum eLogType
    {
        LT_NONE = 0,
        LT_NORMAL = 1,
        LT_ERROR = 2
    };

    int debug_log{ 0 };     //是否写入日志信息
};


//界面相关的一些选项
struct UIData
{
    bool narrow_mode;					//窄界面模式
    bool show_playlist{ true };
    bool show_menu_bar{ true };
    bool full_screen{ false };

    int draw_area_width;                //绘图区的宽度
    int draw_area_height;               //绘图区的高度
    CImage default_background;			//默认的背景
    CCriticalSection default_background_sync;

    bool ShowWindowMenuBar() const ;/* { return show_menu_bar && show_window_frame && !full_screen; }*/
    bool ShowUiMenuBar() const;/* { return show_menu_bar && !show_window_frame && !full_screen; }*/
};


struct ImageSet
{
    Gdiplus::Image* default_cover_img{};
    Gdiplus::Image* default_cover_not_played_img{};
    string default_cover_img_data;
    string default_cover_not_played_img_data;

    ~ImageSet()
    {
        SAFE_DELETE(default_cover_img);
        SAFE_DELETE(default_cover_not_played_img);
    }
};

//通过构造函数传递一个bool变量的引用，在构造时将其置为true，析构时置为false
class CFlagLocker
{
public:
    CFlagLocker(bool& flag)
        : m_flag(flag)
    {
        m_flag = true;
    }

    ~CFlagLocker()
    {
        m_flag = false;
    }

private:
    bool& m_flag;
};


struct MediaUpdateThreadPara
{
    int num_added{};                       //更新媒体库时新增（包括更新）的音频文件数量
    int process_percent{};                  // 更新媒体库进度%
    bool thread_exit{};             //如果为true，则线程应该退出
    bool force;                     // 为true时无视修改时间强制刷新
};
