#pragma once
class IconMgr
{
public:
    IconMgr();
    virtual ~IconMgr();

    enum IconType
    {
        IT_App,                     // 应用图标
        IT_Stop,                    // 停止
        IT_Play,                    // 播放
        IT_Pause,                   // 暂停
        IT_Play_Pause,              // 播放/暂停
        IT_Previous,                // 上一曲
        IT_Next,                    // 下一曲
        IT_Favorite_On,             // 添加到我喜欢的音乐（空心♥）
        IT_Favorite_Off,            // 从我喜欢的音乐移除（实心♥）
        IT_Media_Lib,               // 媒体库
        IT_Playlist,                // 播放列表

        IT_Menu,                    // 菜单
        IT_Full_Screen_On,          // 进入全屏
        IT_Full_Screen_Off,         // 退出全屏
        IT_Minimize,                // 最小化
        IT_Maxmize_On,              // 进入最大化
        IT_Maxmize_Off,             // 退出最大化
        IT_Close,                   // 关闭

        IT_Play_Order,              // 顺序播放
        IT_Loop_Playlist,           // 列表循环
        IT_loop_track,              // 单曲循环
        IT_Play_Shuffle,            // 无序播放
        IT_Play_Random,             // 随机播放
        IT_Play_Track,              // 单曲播放
        IT_Setting,                 // 设置
        IT_Equalizer,               // 均衡器
        IT_Skin,                    // 切换界面（皮肤）
        IT_Mini_On,                 // 进入迷你模式
        IT_Mini_Off,                // 退出迷你模式
        IT_Info,                    // 曲目信息
        IT_Find,                    // 查找歌曲
        IT_Dark_Mode_On,            // 进入深色模式（月亮）
        IT_Dark_Mode_Off,           // 退出深色模式（太阳）
        IT_Volume0,                 // 音量: 静音
        IT_Volume1,                 // 音量: )
        IT_Volume2,                 // 音量: ))
        IT_Volume3,                 // 音量: )))
        IT_Switch_Display,          // 切换显示（xml界面堆栈元素）

        IT_Folder,                  // 文件夹
        IT_Lyric,                   // 歌词
        IT_Playlist_Dock,           // 停靠播放列表/视图
        IT_Help,                    // 帮助

        IT_Locate,                  // 定位

        IT_Triangle_Left,           // 向左三角形
        IT_Triangle_Up,             // 向上三角形
        IT_Triangle_Right,          // 向右三角形
        IT_Triangle_Down,           // 向下三角形

        IT_NO_ICON,
    };

    enum IconStyle
    {
        IS_Auto,
        IS_OutlinedDark,            // 深色的图标（浅色模式用）
        IS_OutlinedLight,           // 浅色的图标（深色模式用）
        IS_Filled,
        IS_Color,
    };

    enum IconSize
    {
        IS_Default,                 // SM_CXICON * SM_CYICON
        IS_DPI_16,
        IS_DPI_16_Full_Screen,
        IS_DPI_20,
        IS_DPI_20_Full_Screen,
        IS_DPI_32,
        IS_DPI_32_Full_Screen,
        IS_ORG_512,
    };

    HICON GetHICON(IconType type, IconStyle style = IS_Auto, IconSize size = IS_DPI_16);

    static CSize GetIconSize(IconSize size);

private:

    int MakeKey(IconType type, IconStyle style, IconSize size)
    {
        ASSERT(style < 10 && size < 10);
        return type * 100 + style * 10 + size;
    }
    std::tuple<UINT, UINT, UINT, UINT> GetIconID(IconType type);

    std::map<int, HICON> m_icon_map;
};

