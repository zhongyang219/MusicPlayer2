#pragma once
#include "CPlayerUIBase.h"

//定义界面元素
namespace UiElement
{
    //所有界面元素的基类
    class Element
    {
    public:
        struct Value        //一个布局的数值
        {
            bool is_percentage{ false };    //数值是否为百分比
            bool is_vertical{ false };      //数值是否为垂直方向的
            int value{ 0 };                //如果is_percentate为true，则值为百分比，否则为实际值
            Value(bool _is_vertical);
            void FromString(const std::string str);
            int GetValue(CRect parent_rect, CPlayerUIBase* ui) const;
            bool IsValid() const;
        };
        Value margin_left{ false };
        Value margin_right{ false };
        Value margin_top{ true };
        Value margin_bottom{ true };
        Value x{ false };
        Value y{ true };
        Value width{ false };
        Value height{ true };

        Element* pParent{};     //父元素
        std::vector<std::shared_ptr<Element>> childLst; //子元素列表
        std::string name;

        virtual void Draw(CPlayerUIBase* ui);   //绘制此元素
        CRect GetRect() const;      //获取此元素在界面中的矩形区域
        void SetRect(CRect _rect);
        bool HasSiblings() const;                   //是否存在兄弟节点

    protected:
        CRect ParentRect(CPlayerUIBase* ui) const;
        void CalculateRect(CPlayerUIBase* ui);           //计算此元素在界面中的矩形区域

        CRect rect;
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
        void CalculateChildrenRect(CPlayerUIBase* ui);
    };

    //颜色填充的矩形
    class Rectangle : public Element
    {
    public:
        virtual void Draw(CPlayerUIBase* ui) override;
    };

    //按钮
    class Button : public Element
    {
    public:
        CPlayerUIBase::BtnKey key;      //按钮的类型
        bool big_icon;                  //如果为false，则图标尺寸为16x16，否则为20x20
        virtual void Draw(CPlayerUIBase* ui) override;
        void FromString(const std::string& key_type);
    };

    //文本
    class Text : public Element
    {
    public:
        std::wstring text;
        Alignment align;    //对齐方式
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
            Format      //歌曲格式
        };
        Type type;
        int font_size{9};

        virtual void Draw(CPlayerUIBase* ui) override;
    private:
        mutable CDrawCommon::ScrollInfo scroll_info;
    };

    //专辑封面
    class AlbumCover : public Element
    {
    public:
        bool square{};
        virtual void Draw(CPlayerUIBase* ui) override;
    };

    //频谱分析
    class Spectrum : public Element
    {
    public:
        bool draw_reflex{};     //是否绘制倒影
        virtual void Draw(CPlayerUIBase* ui) override;
    };

    //曲目信息（包含播放状态、文件名、歌曲标识、速度）
    class TrackInfo : public Element
    {
    public:
        virtual void Draw(CPlayerUIBase* ui) override;
    };

    //工具栏
    class Toolbar : public Element
    {
    public:
        bool show_translate_btn{};      //是否在工具栏上显示“显示歌词翻译”按钮
        virtual void Draw(CPlayerUIBase* ui) override;
    };

    //进度条
    class ProgressBar : public Element
    {
    public:
        bool show_play_time{ true };
        virtual void Draw(CPlayerUIBase* ui) override;
    };

    //歌词
    class Lyrics : public Element
    {
    public:
        virtual void Draw(CPlayerUIBase* ui) override;
    };

    //音量
    class Volume : public Element
    {
    public:
        bool show_text{ true };     //是否在音量图标旁边显示文本
        virtual void Draw(CPlayerUIBase* ui) override;
    };

    //节拍指示
    class BeatIndicator : public Element
    {
    public:
        virtual void Draw(CPlayerUIBase* ui) override;
    };
}

/////////////////////////////////////////////////////////////////////////////////////////
class CElementFactory
{
public:
    std::shared_ptr<UiElement::Element> CreateElement(const std::string& name);
};
