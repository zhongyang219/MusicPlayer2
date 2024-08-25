#pragma once
class IconMgr
{
public:
    IconMgr();
    virtual ~IconMgr();

    enum IconType
    {   // 命名以其(点击时的)功能为准
        IT_App,                     // 应用图标
        IT_App_Monochrome,          // 应用图标(单色)
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
        IT_Loop_Track,              // 单曲循环
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
        IT_Music,                   // 音符/打开文件
        IT_Link,                    // 曲别针/链接
        IT_Exit,                    // 退出
        IT_Rewind,                  // 快退
        IT_Fast_Forward,            // 快进
        IT_Speed_Up,                // 加速播放
        IT_Slow_Down,               // 减速播放
        IT_Add,                     // 添加
        IT_Save,                    // 保存
        IT_Save_As,                 // 另存为
        IT_Sort_Mode,               // 排序模式
        IT_Playlist_Display_Mode,   // 播放列表显示方式
        IT_Locate,                  // 定位
        IT_Lyric,                   // 歌词
        IT_Copy,                    // 复制
        IT_Edit,                    // 编辑
        IT_Unlink,                  // 取消关联
        IT_Folder_Explore,          // 文件夹浏览/搜索
        IT_Internal_Lyric,          // 内嵌歌词
        IT_Download,                // 下载
        IT_Download_Batch,          // 批量下载
        IT_Playlist_Dock,           // 停靠播放列表/视图
        IT_Playlist_Float,          // 浮动播放列表
        IT_Pin,                     // 图钉/置顶
        IT_Convert,                 // 格式转换
        IT_Online,                  // 在线查看
        IT_Shortcut,                // 快捷方式
        IT_Album_Cover,             // 专辑封面
        IT_Statistics,              // 统计
        IT_File_Relate,             // 文件关联
        IT_Help,                    // 帮助
        IT_Fix,                     // 扳手/修复
        IT_Star,                    // 星星/分级
        IT_Artist,                  // 艺术家
        IT_Album,                   // 专辑
        IT_Genre,                   // 流派
        IT_Year,                    // 年份
        IT_Bitrate,                 // 比特率
        IT_History,                 // 最近播放
        IT_Key_Board,               // 快捷键
        IT_Reverb,                  // 混响
        IT_Double_Line,             // 双行
        IT_Lock,                    // 锁/桌面歌词锁定
        IT_Play_As_Next,            // 下一首播放
        IT_Rename,                  // 重命名
        IT_Play_In_Playlist,        // 添加到新播放列表并播放
        IT_Play_In_Folder,          // 在文件夹模式中播放
        IT_Tag,                     // 标签
        IT_More,                    // 更多
        IT_NowPlaying,              // 正在播放
        IT_Karaoke,                 // 歌词卡拉OK效果
        IT_Refresh,                 // 刷新
        IT_NewFolder,               // 添加新文件夹
        IT_Background,              // 背景穿透

        IT_Le_Tag_Insert,           // Le 插入时间标签
        IT_Le_Tag_Replace,          // Le 替换时间标签
        IT_Le_Tag_Delete,           // Le 删除时间标签
        IT_Le_Save,                 // Le 保存
        IT_Le_Find,                 // Le 文本查找
        IT_Le_Replace,              // Le 文本替换

        IT_Triangle_Left,           // 向左三角形
        IT_Triangle_Up,             // 向上三角形
        IT_Triangle_Right,          // 向右三角形
        IT_Triangle_Down,           // 向下三角形

        IT_Ok,                      // 圆/确认
        IT_Cancel,                  // 叉/取消

        IT_TreeCollapsed,           // 折叠的树节点
        IT_TreeExpanded,            // 展开的树节点

        IT_Default_Cover_Playing,   // 默认封面（播放中）
        IT_Default_Cover_Stopped,   // 默认封面（已停止）

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
        IS_DPI_16,
        IS_DPI_16_Full_Screen,
        IS_DPI_20,
        IS_DPI_20_Full_Screen,
        IS_DPI_32,
        IS_DPI_32_Full_Screen,
        IS_ORG_512,
        IS_ALL,
    };

    // 获取需要的图标，不要在代码中大量直接调用
    // 请为同类调用提供一层包装以免之后修改困难
    HICON GetHICON(IconType type, IconStyle style, IconSize size);

    static CSize GetIconSize(IconSize size)
    {
        int width = GetIconWidth(size);
        return CSize(width, width);
    }
    static int GetIconWidth(IconSize size);

private:

    int MakeKey(IconType type, IconStyle style, IconSize size)
    {
        ASSERT(style < 10 && size < 10);
        return type * 100 + style * 10 + size;
    }
    std::tuple<UINT, UINT, UINT, UINT> GetIconID(IconType type);

    std::map<int, HICON> m_icon_map;
    std::shared_mutex m_shared_mutex;   // 线程同步对象
};

