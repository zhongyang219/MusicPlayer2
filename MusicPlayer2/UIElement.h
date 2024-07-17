#pragma once
#include "CPlayerUIBase.h"
#include "MediaLibHelper.h"

//定义界面元素
namespace UiElement
{
    //所有界面元素的基类
    class Element
    {
    public:
        struct Value        //一个布局的数值
        {
            Value(bool _is_vertical, Element* _owner);
            void FromString(const std::string str);
            int GetValue(CRect parent_rect) const;   // 获取实际显示的数值
            bool IsValid() const;           // 返回true说明设置过数值
        private:
            int value{ 0 };                 // 如果is_percentate为true则值为百分比，否则为实际值
            bool valid{ false };            // 如果还没有设置过数值，则为false
            bool is_percentage{ false };    // 数值是否为百分比
            bool is_vertical{ false };      // 数值是否为垂直方向的
            Element* owner;
        };
        Value margin_left{ false, this };
        Value margin_right{ false, this };
        Value margin_top{ true, this };
        Value margin_bottom{ true, this };
        Value x{ false, this };
        Value y{ true, this };
        Value width{ false, this };
        Value height{ true, this };
        Value max_width{ false, this };
        Value max_height{ true, this };
        Value min_width{ false, this };
        Value min_height{ true, this };
        Value hide_width{ false, this };
        Value hide_height{ true, this };
        int proportion{ 0 };

        Element* pParent{};     //父元素
        std::vector<std::shared_ptr<Element>> childLst; //子元素列表
        std::string name;

        virtual void Draw();   //绘制此元素
        virtual bool IsEnable(CRect parent_rect) const;
        virtual int GetMaxWidth(CRect parent_rect) const;
        int GetWidth(CRect parent_rect) const;
        int GetHeight(CRect parent_rect) const;
        CRect GetRect() const;      //获取此元素在界面中的矩形区域
        void SetRect(CRect _rect);
        Element* RootElement();       //获取根节点
        void IterateAllElements(std::function<bool(UiElement::Element*)> func);  //遍历所有界面元素
        void SetUi(CPlayerUIBase* _ui);

    protected:
        CRect ParentRect() const;
        virtual void CalculateRect();           //计算此元素在界面中的矩形区域
        static void IterateElements(UiElement::Element* parent_element, std::function<bool(UiElement::Element*)> func);

        CRect rect;     //用于保存计算得到的元素的矩形区域
        CPlayerUIBase* ui;
    };

    //布局
    class Layout : public Element
    {
    public:
        enum Type
        {
            Vertical,
            Horizontal,
        };
        Type type;
        void CalculateChildrenRect();      //计算布局中所有子元素的位置
        virtual void Draw() override;
    };

    //包含多个元素的堆叠元素，同时只能显示一个元素
    class StackElement : public Element
    {
    public:
        void SetCurrentElement(int index);
        void SwitchDisplay(bool previous = false);
        virtual void Draw() override;
        bool click_to_switch{};     //鼠标点击时切换
        bool hover_to_switch{};     //鼠标指向时切换
        bool scroll_to_switch{};	//鼠标滚轮切换
        bool show_indicator{};
        int indicator_offset{};
        bool mouse_hover{};
        IPlayerUI::UIButton indicator{};        //指示器
        int GetCurIndex() const;

    protected:
        std::shared_ptr<Element> CurrentElement();
        std::shared_ptr<Element> GetElement(int index);

        int cur_index{};
    };

    //半透明的矩形
    class Rectangle : public Element
    {
    public:
        bool no_corner_radius{};
        bool theme_color{ true };
        CPlayerUIBase::ColorMode color_mode{ CPlayerUIBase::RCM_AUTO };
        virtual void Draw() override;
    };

    //按钮
    class Button : public Element
    {
    public:
        CPlayerUIBase::BtnKey key;      //按钮的类型
        bool big_icon;                  //如果为false，则图标尺寸为16x16，否则为20x20
        virtual void Draw() override;
        void FromString(const std::string& key_type);
    };

    //文本
    class Text : public Element
    {
    public:
        std::wstring text;
        Alignment align{};    //对齐方式
        enum Style       //文本的样式
        {
            Static,     //静止的文本
            Scroll,     //滚动的文本
            Scroll2     //另一种滚动的文本（只朝一个方向滚动）
        };
        Style style;

        enum Type       //文本的类型
        {
            UserDefine, //用户指定（text的值）
            Title,      //歌曲标题
            Artist,     //歌曲艺术家
            Album,      //歌曲唱片集
            ArtistTitle,    //艺术家 - 标题
            ArtistAlbum,    //艺术家 - 唱片集
            Format,     //歌曲格式
            PlayTime,   //播放时间
            PlayTimeAndVolume   //显示为播放时间，如果正在调整音量，则显示当前音量，一段时间后恢复
        };

        Type type;
        int font_size{ 9 };
        bool width_follow_text{};
        CPlayerUIBase::ColorMode color_mode{ CPlayerUIBase::RCM_AUTO };
        bool show_volume{};     //当type为PlayTimeAndVolume时有效，如果为true，则显示为音量

        virtual void Draw() override;
        virtual int GetMaxWidth(CRect parent_rect) const override;
        std::wstring GetText() const;

    private:
        mutable CDrawCommon::ScrollInfo scroll_info;
    };

    //专辑封面
    class AlbumCover : public Element
    {
    public:
        bool square{};
        bool show_info{};
        virtual void Draw() override;
        virtual void CalculateRect() override;
    };

    //频谱分析
    class Spectrum : public Element
    {
    public:
        bool draw_reflex{};     //是否绘制倒影
        bool fixed_width{};     //每个柱形是否使用相同的宽度
        Alignment align{ Alignment::LEFT };     //对齐方式
        CUIDrawer::SpectrumCol type{ CUIDrawer::SC_64 };     //频谱分析的类型
        virtual void Draw() override;
        virtual bool IsEnable(CRect parent_rect) const override;
    };

    //曲目信息（包含播放状态、文件名、歌曲标识、速度）
    class TrackInfo : public Element
    {
    public:
        virtual void Draw() override;
    };

    //工具栏
    class Toolbar : public Element
    {
    public:
        bool show_translate_btn{};      //是否在工具栏上显示“显示歌词翻译”按钮
        virtual void Draw() override;
    };

    //进度条
    class ProgressBar : public Element
    {
    public:
        bool show_play_time{};
        bool play_time_both_side{};
        virtual void Draw() override;
    };

    //歌词
    class Lyrics : public Element
    {
    public:
        bool no_background = false;         // 总是不使用歌词背景
        bool use_default_font = false;      // 固定使用默认字体
        int font_size{ 9 };                 // 使用默认字体时的字号
        virtual void Draw() override;
    protected:
        bool IsParentRectangle() const;     //判断父元素中是否有矩形元素
    };

    //音量
    class Volume : public Element
    {
    public:
        bool show_text{ true };     //是否在音量图标旁边显示文本
        bool adj_btn_on_top{ false };   //音量调节按钮是否显示在音量图标的上方
        virtual void Draw() override;
    };

    //节拍指示
    class BeatIndicator : public Element
    {
    public:
        virtual void Draw() override;
    };

    //列表元素
    class ListElement : public Element
    {
    public:
        friend class CPlayerUIBase;

        virtual void Draw() override;
        void LButtonUp(CPoint point);
        void LButtonDown(CPoint point);
        void MouseMove(CPoint point);
        bool RButtunUp(CPoint point);
        void RButtonDown(CPoint point);
        bool MouseWheel(int delta, CPoint point);
        bool DoubleClick(CPoint point);

        void EnsureItemVisible(int index);  //确保指定项在播放列表中可见
        void RestrictOffset();             //将播放列表偏移量限制在正确的范围
        void CalculateItemRects();         //计算播放列表中每一项的矩形区域，保存在playlist_info.item_rects中
        int ItemHeight() const;
        void SetItemSelected(int index);

        virtual std::wstring GetItemText(int row, int col) = 0;
        virtual int GetRowCount() = 0;
        virtual int GetColumnCount() = 0;
        virtual int GetColumnWidth(int col, int total_width) = 0;
        virtual IconMgr::IconType GetIcon(int row) { return IconMgr::IT_NO_ICON; }
        virtual bool HasIcon() { return false; }
        virtual std::wstring GetEmptyString() { return std::wstring(); }    //列表为空时显示的文本
        virtual int GetHighlightRow() { return -1; }
        virtual int GetColumnScrollTextWhenSelected() { return -1; }    //获取选中时需要滚动显示的列
        virtual bool ShowTooltip() { return false; }
        virtual std::wstring GetToolTipText(int row) { return std::wstring(); }
        virtual int GetToolTipIndex() const { return 0; }
        virtual CMenu* GetContextMenu(bool item_selected) { return nullptr; }
        virtual void OnDoubleClicked() {};
        virtual void OnClicked() {};

        int item_height{ 28 };

    protected:
        int GetPlaylistIndexByPoint(CPoint point);
        void Clicked(CPoint point);     //当播放列表被点击时调用此函数

    protected:
        bool mouse_pressed{ };          //鼠标左键是否按下
        bool hover{};                   //指标指向播放列表区域
        CPoint mouse_pressed_pos;       //鼠标按下时的位置
        int mouse_pressed_offset{};     //鼠标按下时播放列表的位移
        int playlist_offset{};          //当前播放列表滚动的位移
        int item_selected{ -1 };        //选中项的序号
        CDrawCommon::ScrollInfo selected_item_scroll_info;  //绘制选中项滚动文本的结构体
        std::vector<CRect> item_rects;  //播放列表中每个项目的矩形区域
        CRect scrollbar_rect{};         //滚动条的位置
        CRect scrollbar_handle_rect;    //滚动条把手的位置
        bool scrollbar_hover{};         //鼠标指向滚动条
        bool scrollbar_handle_pressed{};    //滚动条把手被按下
        int scroll_handle_length_comp{};    //计算滚动条把手长度时的补偿量
    };

    const int PLAYLIST_TOOLTIP_INDEX = 999;

    //播放列表
    class Playlist : public ListElement
    {
    public:
        enum Column
        {
            COL_INDEX,
            COL_TRACK,
            COL_TIME,
            COL_MAX
        };

        // 通过 ListElement 继承
        std::wstring GetItemText(int row, int col) override;
        int GetRowCount() override;
        int GetColumnCount() override;
        virtual int GetColumnWidth(int col, int total_width) override;
        virtual std::wstring GetEmptyString() override;
        virtual int GetHighlightRow() override;
        virtual int GetColumnScrollTextWhenSelected() override;
        virtual bool ShowTooltip() override;
        virtual std::wstring GetToolTipText(int row) override;
        virtual int GetToolTipIndex() const override;
        virtual CMenu* GetContextMenu(bool item_selected) override;
        virtual void OnDoubleClicked() override;
        virtual void OnClicked() override;
    private:
        int last_highlight_row{ -1 };
    };

    //最近播放
    class RecentPlayedList : public ListElement
    {
    public:
        // 通过 ListElement 继承
        std::wstring GetItemText(int row, int col) override;
        int GetRowCount() override;
        int GetColumnCount() override;
        int GetColumnWidth(int col, int total_width) override;
        virtual int GetColumnScrollTextWhenSelected() override;
        virtual IconMgr::IconType GetIcon(int row) override;
        virtual bool HasIcon() override;
        virtual void OnDoubleClicked() override;
    };

    //媒体库项目列表
    class MediaLibItemList : public ListElement
    {
    public:
        CMediaClassifier::ClassificationType type;

        // 通过 ListElement 继承
        std::wstring GetItemText(int row, int col) override;
        int GetRowCount() override;
        int GetColumnCount() override;
        int GetColumnWidth(int col, int total_width) override;
        virtual int GetHighlightRow() override;
        virtual int GetColumnScrollTextWhenSelected() override;
        virtual void OnDoubleClicked() override;
    private:
        int last_highlight_row{ -1 };
    };

    //当前播放列表指示
    class PlaylistIndicator : public Element
    {
    public:
        virtual void Draw() override;
    };

    class ClassicalControlBar : public Element
    {
    public:
        ClassicalControlBar();
        virtual void Draw() override;

    public:
        bool show_switch_display_btn{};
    };
}

/////////////////////////////////////////////////////////////////////////////////////////
class CElementFactory
{
public:
    std::shared_ptr<UiElement::Element> CreateElement(const std::string& name, CPlayerUIBase* ui);
};
