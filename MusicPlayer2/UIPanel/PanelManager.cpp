#include "stdafx.h"
#include "PanelManager.h"
#include "PlayQueuePanel.h"
#include "ListPreviewPanel.h"
#include "TestPanel.h"
#include "SettingsPanel.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 根据面板类型创建面板对象
static std::unique_ptr<CPlayerUIPanel> CreatePanelFromRes(UINT resId, CPlayerUIBase* ui)
{
	switch (resId)
	{
	case IDR_PLAY_QUEUE_PANEL:
		return std::make_unique<CPlayQueuePanel>(ui);
	case IDR_LIST_PREVIEW_PANEL:
		return std::make_unique<CListPreviewPanel>(ui);
	case IDR_TEST_PANEL:
		return std::make_unique<CTestPanel>(ui);
	case IDR_SETTINGS_PANEL:
		return std::make_unique<CSettingsPanel>(ui);
	default:
		return std::make_unique<CPlayerUIPanel>(ui, resId);
	}
	return nullptr;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CPanelManager::CPanelManager(CPlayerUIBase* ui)
	: m_ui(ui)
{
}

void CPanelManager::DrawPanel()
{
	if (!m_displayed_panels.empty())
	{
		CPlayerUIPanel* top_panel = m_displayed_panels.front();
		//如果栈顶的面板是不全屏面板，查找全屏面板并绘制
		if (!top_panel->IsFullFill())
		{
			auto iter = m_displayed_panels.begin();
			++iter;
			for (; iter != m_displayed_panels.end(); ++iter)
			{
				if ((*iter)->IsFullFill())
				{
					(*iter)->Draw();
				}
			}
		}

		//绘制栈顶面板
		top_panel->Draw();
	}
}

void CPanelManager::AddPanel(PanelKey key, std::unique_ptr<CPlayerUIPanel>&& panel)
{
	m_panels[key] = std::move(panel);
}

bool CPanelManager::PanelExist(PanelKey key)
{
	auto iter = m_panels.find(key);
	return iter != m_panels.end();
}

bool CPanelManager::IsPanelFullFill() const
{
	for (auto& panel : m_displayed_panels)
	{
		if (panel->IsFullFill())
			return true;
	}
	return false;
}

CPlayerUIPanel* CPanelManager::GetTopPanel() const
{
	//仅栈顶的面板可见
	if (m_displayed_panels.empty())
		return nullptr;
	else
		return m_displayed_panels.front();
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
		else if (key.type == ePanelType::PanelFromRes)
		{
			auto result = m_panels.emplace(key, CreatePanelFromRes(key.resId, m_ui));
			panel = result.first->second.get();
		}

	}
	if (panel != nullptr)
	{
		//将显示的面板入栈
		if (GetTopPanel() != panel)
			m_displayed_panels.push_front(panel);
	}
	return panel;
}

void CPanelManager::HidePanel()
{
	//将当前显示的面板出栈
	if (!m_displayed_panels.empty())
		m_displayed_panels.pop_front();
}

