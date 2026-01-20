#pragma once
#include "PlayerUIPanel.h"

//UI面板的类型
enum class ePanelType
{
	PanelFromUi,	//来自ui的面板
	PanelFromFile,	//来自文件的面板
	PanelFromRes,	//来自资源的面板（内置的面板）
};

class CPanelManager
{
public:
	CPanelManager(CPlayerUIBase* ui);

	struct PanelKey
	{
		ePanelType type;	//面板的类型
		std::wstring id;	//当面板类型为PanelFromUi时为id，面板类型为PanelFromFile时为文件名
		UINT resId{};		//当面板类型为PanelFromRes时的资源id

		PanelKey(ePanelType _type, const std::wstring& _id)
			: type(_type), id(_id)
		{
		}

		PanelKey(UINT _resId)
			: type(ePanelType::PanelFromRes), resId(_resId)
		{
		}

		bool operator<(const PanelKey& key) const
		{
			if (type != key.type)
				return type < key.type;
			if (resId != key.resId)
				return resId < key.resId;
			else
				return id < key.id;
		}

		bool operator==(const PanelKey& key) const
		{
			return type == key.type && id == key.id && resId == key.resId;
		}
	};

	CPlayerUIPanel* GetTopPanel() const;		//获取最顶层面板
	CPlayerUIPanel* ShowPanel(PanelKey key);
	void HidePanel();
	void DrawPanel();
	void AddPanel(PanelKey key, std::unique_ptr<CPlayerUIPanel>&& panel);		//添加一个ui中的面板
	bool PanelExist(PanelKey key);
	bool IsPanelFullFill() const;	//面板是否占满了整个窗口

private:
	CPlayerUIBase* m_ui;
	std::list<CPlayerUIPanel*> m_displayed_panels;		//显示的面板
	std::map<PanelKey, std::unique_ptr<CPlayerUIPanel>> m_panels;		//保存所有已加载的面板
};

