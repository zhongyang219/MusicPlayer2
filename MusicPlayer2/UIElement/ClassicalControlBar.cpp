#include "stdafx.h"
#include "ClassicalControlBar.h"
#include "TinyXml2Helper.h"
#include "UserUi.h"

UiElement::ClassicalControlBar::ClassicalControlBar()
    : Element()
{
    height.FromString("56");
}

void UiElement::ClassicalControlBar::Draw()
{
    if (rect.Width() < ui->DPI(350))
        height.FromString("56");
    else
        height.FromString("36");
    CalculateRect();

    Element::Draw();
}

void UiElement::ClassicalControlBar::InitComplete()
{
    //从资源加载xml布局
    string xml_contents = CCommon::GetTextResourceRawData(IDR_CLASSICAL_CONTROL_BAR);
    if (!xml_contents.empty())
    {
        //读取xml
        tinyxml2::XMLDocument doc;
        auto rtn = doc.Parse(xml_contents.c_str(), xml_contents.size());
        if (rtn == tinyxml2::XML_SUCCESS)
        {
            doc.RootElement();
            auto root_element = CUserUi::BuildUiElementFromXmlNode(doc.RootElement(), ui);
            for (auto& ele : root_element->childLst)
            {
                AddChild(ele);
            }
        }
    }

    //如果show_switch_display_btn为false，则找到key为switchDisplay的Button元素，并将它移除
    IterateAllElements([&](UiElement::Element* element) ->bool {
        Button* button = dynamic_cast<Button*>(element);
        if (button != nullptr && button->key == CPlayerUIBase::BTN_SWITCH_DISPLAY)
        {
            if (!show_switch_display_btn)
            {
                if (button->pParent != nullptr)
                {
                    auto iter = std::find_if(button->pParent->childLst.begin(), button->pParent->childLst.end(), [&](const std::shared_ptr<Element>& cur_ele) {
                        return cur_ele.get() == button;
                        });
                    if (iter != button->pParent->childLst.end())
                        button->pParent->childLst.erase(iter);
                }
            }
        }
        return false;
    });
}
