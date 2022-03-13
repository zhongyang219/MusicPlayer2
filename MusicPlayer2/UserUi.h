#pragma once
#include "CPlayerUIBase.h"
#include "UIElement.h"

class CUserUi :
    public CPlayerUIBase
{
public:
    CUserUi(UIData& ui_data, CWnd* pMainWnd, const std::wstring& xml_path);
    ~CUserUi();

    void SetIndex(int index);
    bool IsIndexValid() const;

    // 通过 CPlayerUIBase 继承
    virtual void _DrawInfo(CRect draw_rect, bool reset = false) override;
    virtual CString GetUIName() override;

private:
    int m_index{ INT_MAX };
    std::wstring m_xml_path;
    std::shared_ptr<UiElement::Element> m_root_default;
    std::shared_ptr<UiElement::Element> m_root_ui_big;
    std::shared_ptr<UiElement::Element> m_root_ui_narrow;
    std::shared_ptr<UiElement::Element> m_root_ui_small;
    std::wstring m_ui_name;

public:
    virtual int GetUiIndex() override;

protected:
    void LoadUi();      //从xml文件载入界面
};
