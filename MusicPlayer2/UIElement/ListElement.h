#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
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
}

