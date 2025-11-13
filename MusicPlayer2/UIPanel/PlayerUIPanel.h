#pragma once
#include "UIElement/UIElement.h"
#include "CPlayerUIBase.h"

enum class ePanelType
{
	PlayQueue,
};

class CPlayerUIPanel
{
public:
	CPlayerUIPanel(CPlayerUIBase* ui, ePanelType panel_type);
	CPlayerUIPanel(CPlayerUIBase* ui, const std::wstring file_name);

	static UINT GetPanelResId(ePanelType panel_type);

	//从xml读取UI配置
	void LoadUIData(const std::string& xml_contents);

	//绘制面板
	virtual void Draw();

	void SetVisible(bool visible);
	bool IsVisible() const;

	CRect GetPanelRect() { return m_panel_rect; }

	std::shared_ptr<UiElement::Element> GetRootElement() const;

protected:
	//计算面板的矩形区域，如果为空，则默认为整个界面区域
	virtual CRect CalculatePanelRect() { return CRect(); }

protected:
	std::shared_ptr<UiElement::Element> m_root_element;
	CPlayerUIBase* m_ui;
	bool m_visible{ false };
	CRect m_panel_rect;
};


class CPanelManager
{
public:
	CPanelManager(CPlayerUIBase* ui);
	CPlayerUIPanel* GetPanel(ePanelType panel_type);
	CPlayerUIPanel* GetPanel(const std::wstring& file_name);
	CPlayerUIPanel* GetVisiblePanel();
	void ShowHidePanel(ePanelType panel_type);
	void ShowHidePanel(const std::wstring& file_name);
	void HideAllPanel();
	void DrawPanel();

private:
	CPlayerUIBase* m_ui;
	std::map<ePanelType, std::unique_ptr<CPlayerUIPanel>> m_panels;				//从资源读取的面板
	std::map<std::wstring, std::unique_ptr<CPlayerUIPanel>> m_panels_in_files;	//从文件读取的面板
};
