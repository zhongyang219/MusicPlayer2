#include "stdafx.h"
#include "PlayerUIPanel.h"
#include "UserUi.h"
#include "PlayQueuePanel.h"

CPlayerUIPanel::CPlayerUIPanel(CPlayerUIBase* ui, ePanelType panel_type)
	: m_ui(ui)
{
	UINT res_id = GetPanelResId(panel_type);
	string xml = CCommon::GetTextResourceRawData(res_id);
	if (!xml.empty())
		LoadUIData(xml);
}

UINT CPlayerUIPanel::GetPanelResId(ePanelType panel_type)
{
	switch (panel_type)
	{
	case ePanelType::PlayQueue: return IDR_PLAY_QUEUE_PANEL;
	}
	return 0;
}

void CPlayerUIPanel::LoadUIData(const std::string& xml_contents)
{
	//读取xml
	tinyxml2::XMLDocument doc;
	auto rtn = doc.Parse(xml_contents.c_str(), xml_contents.size());
	if (rtn == tinyxml2::XML_SUCCESS)
	{
		doc.RootElement();
		m_root_element = CUserUi::BuildUiElementFromXmlNode(doc.RootElement(), m_ui);
	}
	//初始化搜索框
	if (m_root_element != nullptr)
	{
		m_root_element->IterateAllElements([&](UiElement::Element* element) ->bool {
			UiElement::SearchBox* search_box{ dynamic_cast<UiElement::SearchBox*>(element) };
			if (search_box != nullptr)
			{
				search_box->InitSearchBoxControl(theApp.m_pMainWnd);
				theApp.m_pMainWnd->SetFocus();
			}
			return false;
		});
	}
}

void CPlayerUIPanel::Draw()
{
	if (m_visible)
	{
		//绘制一层半透明的黑色背景
		CRect draw_rect = m_ui->GetDrawRect();
		if (m_ui->IsDrawTitleBar())
		{
			SLayoutData layoutData;
			draw_rect.top += layoutData.titlabar_height;
		}

		BYTE alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency);
		m_ui->GetDrawer().FillAlphaRect(draw_rect, m_ui->GetUIColors().color_back, alpha);

		CRect rect_panel = GetPanelRect();
		rect_panel &= draw_rect;
		//绘制面板背景
		alpha = 255 - (255 - alpha) / 2;
		m_ui->GetDrawer().FillAlphaRect(rect_panel, m_ui->GetUIColors().color_back, alpha);

		//绘制面板
		m_root_element->SetRect(rect_panel);
		m_root_element->Draw();
	}
}

void CPlayerUIPanel::SetVisible(bool visible)
{
	m_visible = visible;
}

bool CPlayerUIPanel::IsVisible() const
{
	return m_visible;
}

std::shared_ptr<UiElement::Element> CPlayerUIPanel::GetRootElement() const
{
	return m_root_element;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 根据面板类型创建面板对象
std::unique_ptr<CPlayerUIPanel> CreatePanel(ePanelType panel_type, CPlayerUIBase* ui)
{
	switch (panel_type)
	{
	case ePanelType::PlayQueue:
		return std::make_unique<CPlayQueuePanel>(ui);
	}
	return nullptr;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CPanelManager::DrawPanel()
{
	for (const auto& panel : m_panels)
		panel.second->Draw();
}

CPanelManager::CPanelManager(CPlayerUIBase* ui)
	: m_ui(ui)
{
}

CPlayerUIPanel* CPanelManager::GetPanel(ePanelType panel_type)
{
	auto iter = m_panels.find(panel_type);
	if (iter != m_panels.end())
	{
		return iter->second.get();
	}
	else
	{
		auto result = m_panels.emplace(panel_type, CreatePanel(panel_type, m_ui));
		return result.first->second.get();
	}
}

CPlayerUIPanel* CPanelManager::GetVisiblePanel()
{
	for (const auto& panel : m_panels)
	{
		if (panel.second->IsVisible())
			return panel.second.get();
	}

	return nullptr;
}

void CPanelManager::ShowHidePanel(ePanelType panel_type)
{
	//先隐藏其他面板
	for (const auto& panel : m_panels)
	{
		if (panel.first != panel_type)
			panel.second->SetVisible(false);
	}

	CPlayerUIPanel* panel = GetPanel(panel_type);
	bool visible = panel->IsVisible();
	panel->SetVisible(!visible);
}

void CPanelManager::HideAllPanel()
{
	for (const auto& panel : m_panels)
	{
		panel.second->SetVisible(false);
	}
}
