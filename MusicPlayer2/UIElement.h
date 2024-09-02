#pragma once
#include "CPlayerUIBase.h"
#include "MediaLibHelper.h"
#include <set>

class CUiSearchBox;

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
        virtual int GetWidth(CRect parent_rect) const;
        virtual int GetHeight(CRect parent_rect) const;
        CRect GetRect() const;      //获取此元素在界面中的矩形区域
        void SetRect(CRect _rect);
        virtual void ClearRect();
        Element* RootElement();       //获取根节点
        //遍历所有界面元素
        //visible_only为true时，遇到stackElement时，只遍历stackElement下面可见的子节点
        void IterateAllElements(std::function<bool(UiElement::Element*)> func, bool visible_only = false);
        void SetUi(CPlayerUIBase* _ui);
        void AddChild(std::shared_ptr<Element> child);

        //鼠标消息虚函数。
        //即使鼠标的位置不在当前元素的矩形区域内，函数仍然会响应，因此在重写这些虚函数时需要先使用rect.PtInRect(point)判断鼠标位置是否在矩形区域内。
        virtual void LButtonUp(CPoint point) {}
        virtual void LButtonDown(CPoint point) {}
        virtual void MouseMove(CPoint point) {}
        virtual bool RButtunUp(CPoint point) { return false; }
        virtual void RButtonDown(CPoint point) {}
        virtual bool MouseWheel(int delta, CPoint point) { return false; }
        virtual bool DoubleClick(CPoint point) { return false; }
        virtual void MouseLeave() {}

    protected:
        CRect ParentRect() const;
        virtual void CalculateRect();           //计算此元素在界面中的矩形区域
        static void IterateElements(UiElement::Element* parent_element, std::function<bool(UiElement::Element*)> func, bool visible_only = false);

        CRect rect;     //用于保存计算得到的元素的矩形区域
        CPlayerUIBase* ui{};
    };

    namespace TooltipIndex
    {
        const int PLAYLIST = 900;
        const int TAB_ELEMENT = 901;
        const int PLAYLIST_DROP_DOWN_BTN = 902;
        const int PLAYLIST_MENU_BTN = 903;
        const int SEARCHBOX_CLEAR_BTN = 904;
    }

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
        bool big_icon{};                //如果为false，则图标尺寸为16x16，否则为20x20
        bool show_text{};               //是否在图标右侧显示文本
        int font_size{ 9 };             //字体大小，仅在show_text为true时有效
        virtual void Draw() override;
        void FromString(const std::string& key_type);
        virtual int GetMaxWidth(CRect parent_rect) const override;
        virtual void ClearRect() override;
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
        int font_size{ 9 };
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
        bool show_song_info = false;        //没有歌词时总是显示歌曲信息
        virtual void Draw() override;
        virtual void ClearRect() override;
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
        virtual void LButtonUp(CPoint point) override;
        virtual void LButtonDown(CPoint point) override;
        virtual void MouseMove(CPoint point) override;
        virtual bool RButtunUp(CPoint point) override;
        void ShowContextMenu(CMenu* menu, CWnd* cmd_reciver);
        virtual void RButtonDown(CPoint point) override;
        virtual bool MouseWheel(int delta, CPoint point) override;
        virtual void MouseLeave() override;
        virtual bool DoubleClick(CPoint point) override;
        virtual void ClearRect() override;

        void EnsureItemVisible(int index);  //确保指定项在播放列表中可见
        void EnsureHighlightItemVisible();  //确保高亮行可见
        void RestrictOffset();             //将播放列表偏移量限制在正确的范围
        void CalculateItemRects();         //计算播放列表中每一项的矩形区域，保存在playlist_info.item_rects中
        int ItemHeight() const;
        void SetItemSelected(int index);    //设置单个项目选中
        int GetItemSelected() const;        //获取单个项目选中
        void SetItemsSelected(const vector<int>& indexes);  //设置多个项目选中
        void GetItemsSelected(vector<int>& indexes) const;  //获取多个项目选中
        bool IsItemSelected(int index) const;   //判断指定行是否选中
        bool IsMultipleSelected() const;        //是否选中了超过1个项目

        void SelectAll();                   //全选（仅IsMultipleSelectionEnable返回true时支持）
        void SelectNone();                  //取消所有选择
        void SelectReversed();              //反向选择（仅IsMultipleSelectionEnable返回true时支持）

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
        virtual CWnd* GetCmdRecivedWnd() { return nullptr; }        //获取右键菜单命令的接收窗口，如果返回空指针，则在CUIWindowCmdHelper中响应
        virtual void OnDoubleClicked() {};
        virtual void OnClicked() {};
        virtual int GetHoverButtonCount() { return 0; }     //获取鼠标指向一行时要显示的按钮数量
        virtual int GetHoverButtonColumn() { return 0; }    //获取鼠标指向时要显示的按钮所在列
        virtual IconMgr::IconType GetHoverButtonIcon(int index, int row) { return IconMgr::IT_NO_ICON; } //获取鼠标指向一行时按钮的图标
        virtual std::wstring GetHoverButtonTooltip(int index, int row) { return std::wstring(); }     //获取鼠标指向一行时按钮的鼠标提示
        virtual void OnHoverButtonClicked(int btn_index, int row) {}    //响应鼠标指向时按钮点击
        IPlayerUI::UIButton& GetHoverButtonState(int btn_index);        //获取储存鼠标指向时按钮信息的结构体
        virtual int GetUnHoverIconCount(int row) { return 0; }          //获取鼠标未指向的行要显示的图标数量（列为GetHoverButtonColumn返回的列）
        virtual IconMgr::IconType GetUnHoverIcon(int index, int row) { return IconMgr::IT_NO_ICON; }   //获取鼠标未指向的行要显示的图标

        virtual bool IsMultipleSelectionEnable() { return false; }      //是否允许多选
        virtual void OnRowCountChanged();       //当列表行数发生变化时响应此函数

        virtual void QuickSearch(const std::wstring& key_word);         //根据关键执行快速搜索（筛选出匹配的项）
        virtual bool IsItemMatchKeyWord(int row, const std::wstring& key_word);     //判断指定行是否匹配关键字（用于快速搜索功能，默认匹配每一列中的文本，只要有一列的文本匹配就返回true，派生类可重写此函数）

        int GetDisplayRowCount();       //获取要显示的行数。（处于搜索状态时返回搜索结果数量，正常状态下同GetRowCount）
        bool IsRowDisplayed(int row);   //判断一行是否显示。（仅处于搜索状态时不匹配的行会返回false）

        void SetRelatedSearchBox(SearchBox* search_box) { related_search_box = search_box; }

        int item_height{ 28 };
        int font_size{ 9 };

    private:
        void DisplayRowToAbsoluteRow(int& row); //将显示的行号转换为绝对行号
        void AbsoluteRowToDisplayRow(int& row); //将绝对行号转换为显示的行号
        int GetDisplayedIndexByPoint(CPoint point);

    protected:
        int GetListIndexByPoint(CPoint point);

    protected:
        bool mouse_pressed{ };          //鼠标左键是否按下
        bool hover{};                   //指标指向播放列表区域
        CPoint mouse_pos;               //鼠标指向的区域
        CPoint mouse_pressed_pos;       //鼠标按下时的位置
        int mouse_pressed_offset{};     //鼠标按下时播放列表的位移
        int playlist_offset{};          //当前播放列表滚动的位移
        std::set<int> items_selected; //选中的序号
        CDrawCommon::ScrollInfo selected_item_scroll_info;  //绘制选中项滚动文本的结构体
        std::vector<CRect> item_rects;  //播放列表中每个项目的矩形区域
        CRect scrollbar_rect{};         //滚动条的位置
        CRect scrollbar_handle_rect;    //滚动条把手的位置
        bool scrollbar_hover{};         //鼠标指向滚动条
        bool scrollbar_handle_pressed{};    //滚动条把手被按下
        int scroll_handle_length_comp{};    //计算滚动条把手长度时的补偿量
        std::map<int, IPlayerUI::UIButton> hover_buttons;   //鼠标指向时的按钮
        int last_row_count{};
    private:
        std::vector<int> search_result; //保存搜索结果的序号
        bool searched{};                //是否处于搜索状态
        SearchBox* related_search_box{};    //关联的键框
    };


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

        //鼠标指向一行时显示的按钮
        enum BtnKey
        {
            BTN_PLAY,
            BTN_ADD,
            BTN_FAVOURITE,
            BTN_MAX
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
        virtual CWnd* GetCmdRecivedWnd() override;
        virtual void OnDoubleClicked() override;
        virtual void OnClicked() override;
        virtual int GetHoverButtonCount() override;
        virtual int GetHoverButtonColumn() override;
        virtual IconMgr::IconType GetHoverButtonIcon(int index, int row) override;
        virtual std::wstring GetHoverButtonTooltip(int index, int row) override;
        virtual void OnHoverButtonClicked(int btn_index, int row) override;
        virtual int GetUnHoverIconCount(int row) override;
        virtual IconMgr::IconType GetUnHoverIcon(int index, int row) override;

        virtual bool IsMultipleSelectionEnable() override { return true; }
        virtual void OnRowCountChanged() override;

        virtual bool IsItemMatchKeyWord(int row, const std::wstring& key_word);

    private:
        int last_highlight_row{ -1 };
    };

    //最近播放
    class RecentPlayedList : public ListElement
    {
    public:
        enum Column
        {
            COL_NAME,
            COL_COUNT,
            COL_MAX
        };

        // 通过 ListElement 继承
        std::wstring GetItemText(int row, int col) override;
        int GetRowCount() override;
        int GetColumnCount() override;
        int GetColumnWidth(int col, int total_width) override;
        virtual int GetColumnScrollTextWhenSelected() override;
        virtual IconMgr::IconType GetIcon(int row) override;
        virtual bool HasIcon() override;
        virtual void OnDoubleClicked() override;
        virtual CMenu* GetContextMenu(bool item_selected) override;
        virtual int GetHoverButtonCount() override;
        virtual int GetHoverButtonColumn() override;
        virtual IconMgr::IconType GetHoverButtonIcon(int index, int row) override;
        virtual std::wstring GetHoverButtonTooltip(int index, int row) override;
        virtual void OnHoverButtonClicked(int btn_index, int row) override;
    };

    //媒体库项目列表
    class MediaLibItemList : public ListElement
    {
    public:
        CMediaClassifier::ClassificationType type{};

        enum Column
        {
            COL_NAME,
            COL_COUNT,
            COL_MAX
        };

        //鼠标指向一行时显示的按钮
        enum BtnKey
        {
            BTN_PLAY,
            BTN_ADD,
            BTN_MAX
        };

        // 通过 ListElement 继承
        std::wstring GetItemText(int row, int col) override;
        int GetRowCount() override;
        int GetColumnCount() override;
        int GetColumnWidth(int col, int total_width) override;
        virtual std::wstring GetEmptyString() override;
        virtual int GetHighlightRow() override;
        virtual int GetColumnScrollTextWhenSelected() override;
        virtual CMenu* GetContextMenu(bool item_selected) override;
        virtual void OnDoubleClicked() override;
        virtual int GetHoverButtonCount() override;
        virtual int GetHoverButtonColumn() override;
        virtual IconMgr::IconType GetHoverButtonIcon(int index, int row) override;
        virtual std::wstring GetHoverButtonTooltip(int index, int row) override;
        virtual void OnHoverButtonClicked(int btn_index, int row) override;
    private:
        int last_highlight_row{ -1 };
 };

    //当前播放列表指示
    class PlaylistIndicator : public Element
    {
    public:
        virtual void Draw() override;
        virtual void LButtonUp(CPoint point) override;
        virtual void LButtonDown(CPoint point) override;
        virtual void MouseMove(CPoint point) override;
        virtual void MouseLeave() override;
        virtual void ClearRect() override;

        int font_size{ 9 };

        IPlayerUI::UIButton btn_drop_down;
        IPlayerUI::UIButton btn_menu;
        CRect rect_name;
    };

    class ClassicalControlBar : public Element
    {
    public:
        ClassicalControlBar();
        virtual void Draw() override;

    public:
        bool show_switch_display_btn{};
    };

    //导航栏
    class NavigationBar : public Element
    {
    public:
        virtual void Draw() override;
        virtual void LButtonUp(CPoint point) override;
        virtual void MouseMove(CPoint point) override;
        virtual bool RButtunUp(CPoint point) override;
        virtual void MouseLeave() override;

        enum IconType
        {
            ICON_AND_TEXT,
            ICON_ONLY,
            TEXT_ONLY
        };

        enum Orientation
        {
            Horizontal,
            Vertical,
        };

        IconType icon_type{};
        Orientation orientation{ Horizontal };
        int item_space{};
        int item_height{ 28 };
        int font_size{ 9 };
        std::vector<std::string> tab_list;
        std::vector<CRect> item_rects;
        std::vector<std::wstring> labels;
        int SelectedIndex();
        int hover_index{ -1 };
    private:
        void FindStackElement();        //查找StackElement
        bool find_stack_element{};      //如果已经查找过StackElement，则为true
        StackElement* stack_element{};
        int selected_index{};
        int last_hover_index{ -1 };
    };

    //媒体库的文件夹
    class MediaLibFolder : public ListElement
    {
    public:
        enum Column
        {
            COL_NAME,
            COL_COUNT,
            COL_MAX
        };

        //鼠标指向一行时显示的按钮
        enum BtnKey
        {
            BTN_PLAY,
            BTN_ADD,
            BTN_MAX
        };

        // 通过 ListElement 继承
        std::wstring GetItemText(int row, int col) override;
        int GetRowCount() override;
        int GetColumnCount() override;
        int GetColumnWidth(int col, int total_width) override;
        virtual int GetHighlightRow() override;
        virtual int GetColumnScrollTextWhenSelected() override;
        virtual CMenu* GetContextMenu(bool item_selected) override;
        virtual void OnDoubleClicked() override;
        virtual int GetHoverButtonCount() override;
        virtual int GetHoverButtonColumn() override;
        virtual IconMgr::IconType GetHoverButtonIcon(int index, int row) override;
        virtual std::wstring GetHoverButtonTooltip(int index, int row) override;
        virtual void OnHoverButtonClicked(int btn_index, int row) override;
    };

    //媒体库的播放列表
    class MediaLibPlaylist : public ListElement
    {
    public:
        enum Column
        {
            COL_NAME,
            COL_COUNT,
            COL_MAX
        };
        
        // 通过 ListElement 继承
        std::wstring GetItemText(int row, int col) override;
        int GetRowCount() override;
        int GetColumnCount() override;
        int GetColumnWidth(int col, int total_width) override;
        virtual int GetHighlightRow() override;
        virtual int GetColumnScrollTextWhenSelected() override;
        virtual CMenu* GetContextMenu(bool item_selected) override;
        virtual void OnDoubleClicked() override;
        virtual int GetHoverButtonCount() override;
        virtual int GetHoverButtonColumn() override;
        virtual IconMgr::IconType GetHoverButtonIcon(int index, int row) override;
        virtual std::wstring GetHoverButtonTooltip(int index, int row) override;
        virtual void OnHoverButtonClicked(int btn_index, int row) override;
    };

    //我喜欢的音乐列表
    class MyFavouriteList : public ListElement
    {
    public:
        enum Column
        {
            COL_INDEX,
            COL_TRACK,
            COL_TIME,
            COL_MAX
        };

        //鼠标指向一行时显示的按钮
        enum BtnKey
        {
            BTN_PLAY,
            BTN_ADD,
            BTN_MAX
        };

        // 通过 ListElement 继承
        std::wstring GetItemText(int row, int col) override;
        int GetRowCount() override;
        int GetColumnCount() override;
        int GetColumnWidth(int col, int total_width) override;
        virtual int GetHighlightRow() override;
        virtual int GetColumnScrollTextWhenSelected() override;
        virtual CMenu* GetContextMenu(bool item_selected) override;
        virtual void OnDoubleClicked() override;
        virtual std::wstring GetEmptyString() override;
        virtual int GetHoverButtonCount() override;
        virtual int GetHoverButtonColumn() override;
        virtual IconMgr::IconType GetHoverButtonIcon(int index, int row) override;
        virtual std::wstring GetHoverButtonTooltip(int index, int row) override;
        virtual void OnHoverButtonClicked(int btn_index, int row) override;
        virtual bool IsMultipleSelectionEnable() override;
    };

    //所有曲目列表
    class AllTracksList : public ListElement
    {
    public:
        enum Column
        {
            COL_INDEX,
            COL_TRACK,
            COL_TIME,
            COL_MAX
        };

        //鼠标指向一行时显示的按钮
        enum BtnKey
        {
            BTN_PLAY,
            BTN_ADD,
            BTN_FAVOURITE,
            BTN_MAX
        };

        // 通过 ListElement 继承
        std::wstring GetItemText(int row, int col) override;
        int GetRowCount() override;
        int GetColumnCount() override;
        int GetColumnWidth(int col, int total_width) override;
        virtual int GetHighlightRow() override;
        virtual int GetColumnScrollTextWhenSelected() override;
        virtual CMenu* GetContextMenu(bool item_selected) override;
        virtual void OnDoubleClicked() override;
        virtual std::wstring GetEmptyString() override;
        virtual int GetHoverButtonCount() override;
        virtual int GetHoverButtonColumn() override;
        virtual IconMgr::IconType GetHoverButtonIcon(int index, int row) override;
        virtual std::wstring GetHoverButtonTooltip(int index, int row) override;
        virtual void OnHoverButtonClicked(int btn_index, int row) override;
        virtual int GetUnHoverIconCount(int row) override;
        virtual IconMgr::IconType GetUnHoverIcon(int index, int row) override;
        virtual bool IsMultipleSelectionEnable() override;

    private:
        int last_highlight_row{ -1 };
    };

    //迷你频谱
    class MiniSpectrum : public Element
    {
    public:
        virtual void Draw() override;
    };

    //占位符
    class PlaceHolder : public Element
    {
    public:
        virtual int GetWidth(CRect parent_rect) const override;
        virtual int GetHeight(CRect parent_rect) const override;

        bool show_when_use_system_titlebar{};   //仅当开启“使用系统标准标题栏”时才显示

    private:
        bool IsHide() const;
    };

    //树控件
    //派生类只需要继承GetRootNodes函数返回树的数据即可
    class TreeElement : public ListElement
    {
    public:
        //树的一个节点
        struct Node
        {
            std::map<int, std::wstring> texts;   //一行的文本，key是列号，value是文本
            std::vector<std::shared_ptr<Node>> child_list;     //子节点列表
            Node* parent{};     //父节点
            bool collapsed{};   //是否折叠

            void AddChild(std::shared_ptr<Node> child);
            int GetLevel() const;       //获取节点的级别，如果节点没有父节点，则级别为0

            //按顺序遍历子节点
            //func：遍历节点时的回调函数，如果要结束遍历，则返回true，否则返回false
            //ignore_invisible：忽略被折叠的节点
            void IterateNodeInOrder(std::function<bool(Node*)> func, bool ignore_invisible);
        };

        virtual std::vector<std::shared_ptr<Node>>& GetRootNodes() = 0;   //获取顶级节点

        int GetItemLevel(int row);          //获取该行的级别（级别每加1，第一列会缩进一定距离）
        bool IsCollapsable(int row);        //该行是否可以折叠（如果为true，则显示折叠图标）
        bool IsCollapsed(int row);          //该行是否折叠

        // 通过 Element 继承
        virtual void LButtonUp(CPoint point) override;
        virtual void MouseMove(CPoint point) override;
        virtual void MouseLeave() override;
        virtual bool DoubleClick(CPoint point) override;

        // 通过 ListElement 继承
        std::wstring GetItemText(int row, int col) override;
        int GetRowCount() override;
        //树控件不使用基类ListElement的搜索逻辑
        virtual void QuickSearch(const std::wstring& key_word) override;
        virtual void OnRowCountChanged() override;

        std::map<int, CRect> collapsd_rects;     //折叠标志的矩形区域（key是行）
        int collaps_indicator_hover_row{ -1 };    //鼠标指向的折叠标志的行号

    protected:
        int GetNodeIndex(const Node* node);     //查找一个节点的序号（如果节点被折叠或不存在则返回-1）
        Node* GetNodeByIndex(int index);    //根据一个节点的序号查找节点（忽略被折叠的节点）
        bool IsNodeMathcKeyWord(const Node* node, const std::wstring& key_word);  //判断一个节点是否匹配关键字
        bool IsNodeDisplayed(const Node* node);
        void IterateDisplayedNodeInOrder(std::function<bool(Node*)> func);      //遍历所有可见的节点
        std::set<const Node*> tree_search_result; //保存搜索结果
        bool tree_searched{};               //是否处于搜索状态
    };

    class TestTree : public TreeElement
    {
    public:
        TestTree();
        static std::shared_ptr<Node> CreateNode(std::wstring name, std::shared_ptr<Node> parent);

        virtual IconMgr::IconType GetIcon(int row) { return IconMgr::IT_Folder; }
        virtual bool HasIcon() { return true; }
        virtual int GetColumnCount() override;
        virtual int GetColumnWidth(int col, int total_width) override;
        virtual std::vector<std::shared_ptr<Node>>& GetRootNodes() override;

    private:
        std::vector<std::shared_ptr<Node>> root_nodes;
    };

    //媒体库中的文件夹浏览
    class FolderExploreTree : public TreeElement
    {
    public:
        enum Column
        {
            COL_NAME,
            COL_COUNT,
            COL_MAX
        };

        //鼠标指向一行时显示的按钮
        enum BtnKey
        {
            BTN_PLAY,
            BTN_ADD,
            BTN_MAX
        };

        static std::shared_ptr<Node> CreateNode(std::wstring name, int song_num, std::shared_ptr<Node> parent);
        std::wstring GetNodePath(Node* node);       //获取一个节点的路径
        std::wstring GetSelectedPath();             //获取选中节点的路径

        virtual IconMgr::IconType GetIcon(int row) { return IconMgr::IT_Folder; }
        virtual bool HasIcon() { return true; }
        virtual int GetColumnCount() override;
        virtual int GetColumnWidth(int col, int total_width) override;
        virtual int GetColumnScrollTextWhenSelected() override;
        virtual CMenu* GetContextMenu(bool item_selected) override;
        virtual void OnDoubleClicked() override;
        virtual std::wstring GetEmptyString() override;
        virtual int GetHoverButtonCount() override;
        virtual int GetHoverButtonColumn() override;
        virtual IconMgr::IconType GetHoverButtonIcon(int index, int row) override;
        virtual std::wstring GetHoverButtonTooltip(int index, int row) override;
        virtual void OnHoverButtonClicked(int btn_index, int row) override;
        virtual bool IsMultipleSelectionEnable() override;

        virtual std::vector<std::shared_ptr<Node>>& GetRootNodes() override;
    };

    //搜索框
    class SearchBox : public Element
    {
    public:
        SearchBox();
        ~SearchBox();
        void InitSearchBoxControl(CWnd* pWnd);  //初始化搜索框控件。pWnd：父窗口
        void OnKeyWordsChanged();
        void Clear();
        ListElement* GetListElement() { return list_element; }

        virtual void Draw() override;
        virtual void MouseMove(CPoint point) override;
        virtual void MouseLeave() override;
        virtual void LButtonUp(CPoint point) override;
        virtual void LButtonDown(CPoint point) override;

        bool hover{};       //如果鼠标指向搜索框，则为true
        std::wstring key_word;  //搜索框中的文本
        CUiSearchBox* search_box_ctrl{};    //搜索框控件
        CRect icon_rect;    //图标的区域
        CPlayerUIBase::UIButton clear_btn;      //清除按钮

    private:
        void FindListElement();         //查找ListElement
        bool find_list_element{};       //如果已经查找过ListElement，则为true
        ListElement* list_element{};    //关联的ListElement
    };
}

/////////////////////////////////////////////////////////////////////////////////////////
class CElementFactory
{
public:
    std::shared_ptr<UiElement::Element> CreateElement(const std::string& name, CPlayerUIBase* ui);
};
