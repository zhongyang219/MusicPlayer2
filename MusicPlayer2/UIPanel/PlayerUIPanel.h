#pragma once
#include "UIElement/PanelElement.h"
#include "CPlayerUIBase.h"

class CPlayerUIPanel
{
public:
	CPlayerUIPanel(CPlayerUIBase* ui, UINT res_id);
	CPlayerUIPanel(CPlayerUIBase* ui, const std::wstring file_name);
	CPlayerUIPanel(CPlayerUIBase* ui, std::shared_ptr<UiElement::Panel> panel_element);

	//面板显示时被调用
	virtual void OnPanelShown() {};

	//从xml读取UI配置
	void LoadUIData(const std::string& xml_contents);

	//绘制面板
	virtual void Draw();

	CRect GetPanelRect() { return m_panel_rect; }

	std::shared_ptr<UiElement::Panel> GetRootElement() const;

	bool IsFullFill() const;

protected:
	//计算面板的矩形区域，如果为空，则默认为整个界面区域
	virtual CRect CalculatePanelRect() { return CRect(); }

protected:
	std::shared_ptr<UiElement::Panel> m_root_element;
	CPlayerUIBase* m_ui;
	CRect m_panel_rect;
	bool m_is_full_fill{};
};

