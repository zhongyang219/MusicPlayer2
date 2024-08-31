#pragma once
#include "CPlayerUIBase.h"
#include "UIElement.h"
#include "TinyXml2Helper.h"

class CUserUi :
    public CPlayerUIBase
{
public:
    CUserUi(CWnd* pMainWnd, const std::wstring& xml_path);
    CUserUi(CWnd* pMainWnd, UINT id);    //此构造函数不传递xml文件的路径，id为xml界面资源ID
    ~CUserUi();

    void LoadFromContents(const std::string& xml_contents);
    void SetIndex(int index);
    bool IsIndexValid() const;

    //遍历所有界面元素
    //visible_only为true时，遇到stackElement时，只遍历stackElement下面可见的子节点
    void IterateAllElements(std::function<bool(UiElement::Element*)> func, bool visible_only = false);
    void IterateAllElementsInAllUi(std::function<bool(UiElement::Element*)> func);    //遍历每一个界面中的所有元素（包含big、narrow、small三个界面）
    void VolumeAdjusted();      //当音量调整时需要调用此函数
    void ResetVolumeToPlayTime();   //定时器SHOW_VOLUME_TIMER_ID响应时需要调用此函数
    void PlaylistLocateToCurrent();     //播放列表控件使正在播放的条目可见
    void ListLocateToCurrent();         //ui中的所有列表使正在播放的条目可见
    void InitSearchBox(CWnd* pWnd);

    //清除所有搜索框的搜索状态。其中模板参数T是搜索框关联的列表元素的类型
    template<class T>
    void ClearSearchResult();

    void SaveStatackElementIndex(CArchive& archive);
    void LoadStatackElementIndex(CArchive& archive);

    //查找一个UiElement中指定类型的元素
    template<class T>
    T* FindElement();

    //遍历所有指定类型的元素
    //visible_only为true时，遇到stackElement时，只遍历stackElement下面可见的子节点
    template<class T>
    void IterateAllElements(std::function<bool(T*)> func, bool visible_only = false);

    enum { SHOW_VOLUME_TIMER_ID = 1635 };

    // 通过 CPlayerUIBase 继承
    virtual void _DrawInfo(CRect draw_rect, bool reset = false) override;
    virtual wstring GetUIName() override;
    virtual bool LButtonUp(CPoint point) override;
    virtual bool LButtonDown(CPoint point) override;
    virtual void MouseMove(CPoint point) override;
    virtual void MouseLeave() override;
    virtual void RButtonUp(CPoint point) override;
    virtual void RButtonDown(CPoint point) override;
    virtual bool MouseWheel(int delta, CPoint point) override;
    virtual bool DoubleClick(CPoint point) override;
    virtual void UiSizeChanged() override;
    virtual bool SetCursor() override;

protected:
    int m_index{ INT_MAX };
    std::wstring m_xml_path;
    std::shared_ptr<UiElement::Element> m_root_default;
    std::shared_ptr<UiElement::Element> m_root_ui_big;
    std::shared_ptr<UiElement::Element> m_root_ui_narrow;
    std::shared_ptr<UiElement::Element> m_root_ui_small;
    std::wstring m_ui_name;
    std::map<UiElement::Element*, std::vector<std::shared_ptr<UiElement::Element>>>  m_stack_elements;      //保存所有的stackElement。key是其所在的ui节点，value是该ui节点下所有stackElement
    bool m_last_mouse_in_draw_area{};

public:
    virtual int GetUiIndex() override;

    //确保每个界面的序号唯一
    static void UniqueUiIndex(std::vector<std::shared_ptr<CUserUi>>& ui_list);

protected:
    std::shared_ptr<UiElement::Element> GetCurrentTypeUi() const;

    static std::shared_ptr<CUserUi> FindUiByIndex(const std::vector<std::shared_ptr<CUserUi>>& ui_list, int ui_index, std::shared_ptr<CUserUi> except);
    static int GetMaxUiIndex(const std::vector<std::shared_ptr<CUserUi>>& ui_list);
    std::shared_ptr<UiElement::Element> BuildUiElementFromXmlNode(tinyxml2::XMLElement* xml_node);      //从一个xml节点创建UiElement::Element元素及其所有子元素的对象

    virtual const std::vector<std::shared_ptr<UiElement::Element>>& GetStackElements() const;

protected:
    virtual void SwitchStackElement() override;
};

template<class T>
inline void CUserUi::ClearSearchResult()
{
    IterateAllElementsInAllUi([&](UiElement::Element* element) ->bool {
        UiElement::SearchBox* search_box{ dynamic_cast<UiElement::SearchBox*>(element) };
        if (search_box != nullptr)
        {
            T* list_emelent = dynamic_cast<T*>(search_box->GetListElement());
            if (list_emelent != nullptr)
            {
                search_box->Clear();
            }
        }
        return false;
    });
}

template<class T>
inline T* CUserUi::FindElement()
{
    T* element_found{};
    IterateAllElements([&](UiElement::Element* element) ->bool {
        T* ele = dynamic_cast<T*>(element);
        if (ele != nullptr)
        {
            element_found = ele;
            return true;
        }
        return false;
    });

    return element_found;
}

template<class T>
inline void CUserUi::IterateAllElements(std::function<bool(T*)> func, bool visible_only)
{
    IterateAllElements([&](UiElement::Element* element) ->bool {
        T* ele = dynamic_cast<T*>(element);
        if (ele != nullptr)
        {
            return func(ele);
        }
        return false;
    }, visible_only);
}
