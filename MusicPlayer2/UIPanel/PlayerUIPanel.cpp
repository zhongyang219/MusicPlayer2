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

CPlayerUIPanel::CPlayerUIPanel(CPlayerUIBase* ui, const std::wstring file_name)
	: m_ui(ui)
{
	std::wstring file_path = theApp.m_local_dir + L"skins\\panels\\" + file_name;
	std::string xml;
	if (CCommon::GetFileContent(file_path.c_str(), xml))
		LoadUIData(xml);
}

CPlayerUIPanel::CPlayerUIPanel(CPlayerUIBase* ui, std::shared_ptr<UiElement::Element> panel_element)
	: m_ui(ui)
{
	m_root_element = panel_element;
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
		tinyxml2::XMLElement* root = doc.RootElement();
		CTinyXml2Helper::IterateChildNode(root, [&](tinyxml2::XMLElement* xml_child) {
			std::string item_name = CTinyXml2Helper::ElementName(xml_child);
			//查找panel元素
			if (item_name == "panel")
			{
				if (m_root_element == nullptr)
					m_root_element = CUserUi::BuildUiElementFromXmlNode(xml_child, m_ui);
			}
		});
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
	if (m_visible && m_root_element != nullptr)
	{
		//绘制一层半透明的黑色背景
		SLayoutData layoutData;
		CRect draw_rect = m_ui->GetClientDrawRect();

		BYTE alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency);
		m_ui->GetDrawer().FillAlphaRect(draw_rect, m_ui->GetUIColors().color_back, alpha);

		m_panel_rect = CalculatePanelRect();
		//如果未通过代码获取面板的矩形区域，则根据ui计算面板区域
		if (m_panel_rect.IsRectEmpty())
		{
			m_root_element->CalculateRect(draw_rect);
			m_panel_rect = m_root_element->GetRect();
		}
		else
		{
			m_panel_rect &= draw_rect;
			m_root_element->SetRect(m_panel_rect);
		}

		//绘制面板
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
static std::unique_ptr<CPlayerUIPanel> CreatePanel(ePanelType panel_type, CPlayerUIBase* ui)
{
	switch (panel_type)
	{
	case ePanelType::PlayQueue:
		return std::make_unique<CPlayQueuePanel>(ui);
	default:
		return std::make_unique<CPlayerUIPanel>(ui, panel_type);
	}
	return nullptr;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CPanelManager::DrawPanel()
{
	CPlayerUIPanel* cur_panel = GetVisiblePanel();
	if (cur_panel != nullptr)
		cur_panel->Draw();
}

void CPanelManager::AddPanel(const std::wstring& id, std::unique_ptr<CPlayerUIPanel>&& panel)
{
	m_panels_in_ui[id] = std::move(panel);
}

const std::map<std::wstring, std::unique_ptr<CPlayerUIPanel>>& CPanelManager::GetPanelsInUi() const
{
	return m_panels_in_ui;
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

CPlayerUIPanel* CPanelManager::GetPanel(const std::wstring& file_name)
{
	auto iter = m_panels_in_files.find(file_name);
	if (iter != m_panels_in_files.end())
	{
		return iter->second.get();
	}
	else
	{
		auto result = m_panels_in_files.emplace(file_name, std::make_unique<CPlayerUIPanel>(m_ui, file_name));
		return result.first->second.get();
	}
}

CPlayerUIPanel* CPanelManager::GetPanelById(const std::wstring& id)
{
	auto iter = m_panels_in_ui.find(id);
	if (iter != m_panels_in_ui.end())
	{
		return iter->second.get();
	}
	return nullptr;
}

CPlayerUIPanel* CPanelManager::GetVisiblePanel()
{
	for (const auto& panel : m_panels)
	{
		if (panel.second->IsVisible())
			return panel.second.get();
	}
	for (const auto& panel : m_panels_in_files)
	{
		if (panel.second->IsVisible())
			return panel.second.get();
	}
	for (const auto& panel : m_panels_in_ui)
	{
		if (panel.second->IsVisible())
			return panel.second.get();
	}

	return nullptr;
}

void CPanelManager::ShowHidePanel(CPlayerUIPanel* panel)
{
	if (panel != nullptr)
	{
		bool visible = panel->IsVisible();
		//先隐藏其他面板
		HideAllPanel();
		panel->SetVisible(!visible);
	}
}

void CPanelManager::HideAllPanel()
{
	for (const auto& panel : m_panels)
	{
		panel.second->SetVisible(false);
	}
	for (const auto& panel : m_panels_in_files)
	{
		panel.second->SetVisible(false);
	}
	for (const auto& panel : m_panels_in_ui)
	{
		panel.second->SetVisible(false);
	}
}
