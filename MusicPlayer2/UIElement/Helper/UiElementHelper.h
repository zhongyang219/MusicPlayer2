#pragma once
#include "IconMgr.h"
class UiElementHelper
{
public:
	static IconMgr::IconType NameToIconType(const std::string& icon_name, IconMgr::IconType default_icon = IconMgr::IT_NO_ICON);

};

