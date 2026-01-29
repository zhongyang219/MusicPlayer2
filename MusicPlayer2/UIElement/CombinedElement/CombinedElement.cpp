#include "stdafx.h"
#include "CombinedElement.h"
#include "TinyXml2Helper.h"
#include "UserUi.h"

UiElement::CombinedElement::CombinedElement(UINT res_id)
    : UiElement::Element(), m_res_id(res_id)
{
}

void UiElement::CombinedElement::InitComplete()
{
    //从资源加载xml布局
    string xml_contents = CCommon::GetTextResourceRawData(m_res_id);
    if (!xml_contents.empty())
    {
        //读取xml
        tinyxml2::XMLDocument doc;
        auto rtn = doc.Parse(xml_contents.c_str(), xml_contents.size());
        if (rtn == tinyxml2::XML_SUCCESS)
        {
            doc.RootElement();
            auto root_element = CUserUi::BuildUiElementFromXmlNode(doc.RootElement(), ui);
            for (auto& ele : root_element->ChildList())
            {
                AddChild(ele);
            }
        }
    }
}
