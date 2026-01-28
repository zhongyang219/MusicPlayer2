#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    //文本
    class Text : public Element
    {
    public:
        enum Style       //文本的样式
        {
            Static,     //静止的文本
            Scroll,     //滚动的文本
            Scroll2     //另一种滚动的文本（只朝一个方向滚动）
        };

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

        enum ColorStyle
        {
            Default,
            Emphasis1,
            Emphasis2
        };

    protected:
        std::wstring text;
        Alignment align{};    //对齐方式
        Style style;

        Type type;
        int font_size{ 9 };
        bool width_follow_text{};
        CPlayerUIBase::ColorMode color_mode{ CPlayerUIBase::RCM_AUTO };
        bool show_volume{};     //当type为PlayTimeAndVolume时有效，如果为true，则显示为音量
        ColorStyle color_style{};

    public:
        virtual void Draw() override;
        virtual int GetMaxWidth(CRect parent_rect) const override;
        virtual bool MouseMove(CPoint point) override;

        void SetShowVolumn(bool val) { show_volume = val; }
        std::wstring GetText() const;
        void SetText(const std::wstring& str_text);

        virtual void FromXmlNode(tinyxml2::XMLElement* xml_node);

    private:
        mutable CDrawCommon::ScrollInfo scroll_info;
        bool out_of_bounds = false;
        bool last_hover = false;
    };
}

