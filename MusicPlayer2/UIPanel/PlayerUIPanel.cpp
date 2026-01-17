#include "stdafx.h"
#include "PlayerUIPanel.h"
#include "UserUi.h"

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

		//计算面板矩形区域
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

		//判断面板是否占满整个窗口
		m_is_full_fill = (m_panel_rect.left == draw_rect.left) && (m_panel_rect.top == draw_rect.top) && (m_panel_rect.right == draw_rect.right) && (m_panel_rect.bottom == draw_rect.bottom);

		//绘制一层半透明的黑色背景
		if (!m_is_full_fill)
		{
			BYTE alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency);
			m_ui->GetDrawer().FillAlphaRect(draw_rect, m_ui->GetUIColors().color_back, alpha);
		}

		//绘制面板背景
		if (!IsFullFill())
		{
			BYTE alpha = 255;
			if (m_ui->IsDrawBackgroundAlpha())
				alpha = 255 - (255 - alpha) / 2;

			CRect draw_rect = m_ui->GetClientDrawRect();
			//当面板的四个边都没有帖靠UI绘图矩形区域时，才绘制圆角矩形
			bool draw_round_background = m_panel_rect.left != draw_rect.left && m_panel_rect.top != draw_rect.top && m_panel_rect.right != draw_rect.right && m_panel_rect.bottom != draw_rect.bottom;
			if (theApp.m_app_setting_data.button_round_corners && draw_round_background)
				m_ui->GetDrawer().DrawRoundRect(m_panel_rect, m_ui->GetUIColors().color_back, m_ui->CalculateRoundRectRadius(m_panel_rect), alpha);
			else
				m_ui->GetDrawer().FillAlphaRect(m_panel_rect, m_ui->GetUIColors().color_back, alpha, true);
		}

		//绘制面板
		m_root_element->Draw();
	}
}

std::shared_ptr<UiElement::Panel> CPlayerUIPanel::GetRootElement() const
{
	return m_root_element;
}

bool CPlayerUIPanel::IsFullFill() const
{
	return m_is_full_fill;
}
