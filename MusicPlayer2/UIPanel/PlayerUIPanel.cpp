#include "stdafx.h"
#include "PlayerUIPanel.h"
#include "UserUi.h"
#include "PlayQueuePanel.h"
#include "ListPreviewPanel.h"

CPlayerUIPanel::CPlayerUIPanel(CPlayerUIBase* ui, UINT res_id)
	: m_ui(ui)
{
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

CPlayerUIPanel::CPlayerUIPanel(CPlayerUIBase* ui, std::shared_ptr<UiElement::Panel> panel_element)
	: m_ui(ui)
{
	m_root_element = panel_element;
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
					m_root_element = std::dynamic_pointer_cast<UiElement::Panel>(CUserUi::BuildUiElementFromXmlNode(xml_child, m_ui));
			}
		});
	}
}

void CPlayerUIPanel::Draw()
{
	if (m_root_element != nullptr)
	{
		CRect draw_rect = m_ui->GetClientDrawRect();
		//绘制一层半透明的黑色背景
		if (!m_root_element->IsFullFill())
		{
			BYTE alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency);
			m_ui->GetDrawer().FillAlphaRect(draw_rect, m_ui->GetUIColors().color_back, alpha);
		}

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

std::shared_ptr<UiElement::Panel> CPlayerUIPanel::GetRootElement() const
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
	case ePanelType::ListPreview:
		return std::make_unique<CListPreviewPanel>(ui);
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

void CPanelManager::AddPanelFromUi(const std::wstring& id, std::unique_ptr<CPlayerUIPanel>&& panel)
{
	m_panels[PanelKey(ePanelType::PanelFromUi, id)] = std::move(panel);
}

bool CPanelManager::IsPanelFullFill() const
{
	CPlayerUIPanel* panel = GetVisiblePanel();
	if (panel != nullptr)
	{
		UiElement::Panel* panel_element = panel->GetRootElement().get();
		if (panel_element != nullptr)
			return panel_element->IsFullFill();
	}
	return false;
}

CPanelManager::CPanelManager(CPlayerUIBase* ui)
	: m_ui(ui)
{
}

CPlayerUIPanel* CPanelManager::GetVisiblePanel() const
{
	//仅栈顶的面板可见
	if (m_displayed_panels.empty())
		return nullptr;
	else
		return m_displayed_panels.top();
}

CPlayerUIPanel* CPanelManager::ShowPanel(PanelKey key)
{
	CPlayerUIPanel* panel = nullptr;
	auto iter = m_panels.find(key);
	if (iter != m_panels.end())
	{
		panel = iter->second.get();
	}
	//面板不存在时创建
	else
	{
		if (key.type == ePanelType::PanelFromFile)
		{
			if (!key.id.empty())
			{
				auto result = m_panels.emplace(key, std::make_unique<CPlayerUIPanel>(m_ui, key.id));
				panel = result.first->second.get();
			}
		}
		else if (key.type == ePanelType::PanelFromUi)
		{
			//UI中的面板不在此处创建，通过AddPanel函数添加
		}
		else
		{
			auto result = m_panels.emplace(key, CreatePanel(key.type, m_ui));
			panel = result.first->second.get();
		}

	}
	if (panel != nullptr)
	{
		//将显示的面板入栈
		m_displayed_panels.push(panel);
	}
	return panel;
}

void CPanelManager::HidePanel()
{
	//将当前显示的面板出栈
	if (!m_displayed_panels.empty())
		m_displayed_panels.pop();
}
