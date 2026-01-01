#pragma once
#include "PlayerUIPanel.h"
#include "UIElement/Text.h"
#include "UIElement/TracksList.h"
#include "UIElement/IconElement.h"
class CListPreviewPanel : public CPlayerUIPanel
{
public:
	CListPreviewPanel(CPlayerUIBase* ui);

	void SetListData(const ListItem& list_item);

private:
	UiElement::Text* m_title_text{};
	UiElement::TrackList* m_track_list{};
	UiElement::Icon* m_icon{};
	UiElement::Text* m_track_num_text{};
};

