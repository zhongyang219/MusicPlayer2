#pragma once
#include "CPlayerUIBase.h"
#include "UIElement.h"

class CUserUi :
    public CPlayerUIBase
{
public:
    CUserUi(UIData& ui_data, CWnd* pMainWnd, const std::wstring& xml_path, int index);
    ~CUserUi();

    // 通过 CPlayerUIBase 继承
    virtual void _DrawInfo(CRect draw_rect, bool reset = false) override;
    virtual CString GetUIName() override;

private:
    int m_index;
    std::wstring m_xml_path;
    std::shared_ptr<UiElement::Element> m_root_default;
    std::shared_ptr<UiElement::Element> m_root_ui_big;
    std::shared_ptr<UiElement::Element> m_root_ui_narrow;
    std::shared_ptr<UiElement::Element> m_root_ui_small;
    std::wstring m_ui_name;

protected:
    virtual int GetUiIndex() override;
    void LoadUi();      //从xml文件载入界面
};
