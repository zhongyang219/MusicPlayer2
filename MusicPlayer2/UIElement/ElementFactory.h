#pragma once
#include "UIElement/UIElement.h"
#include "UIElement/Layout.h"
#include "UIElement/Text.h"
#include "UIElement/PlaylistElement.h"
#include "UIElement/ListElement.h"
#include "UIElement/StackElement.h"
#include "UIElement/Button.h"
#include "UIElement/Rectangle.h"
#include "UIElement/AlbumCover.h"
#include "UIElement/Spectrum.h"
#include "UIElement/ProgressBar.h"
#include "UIElement/Volume.h"
#include "UIElement/ClassicalControlBar.h"
#include "UIElement/LyricsElement.h"
#include "UIElement/MediaLibItemList.h"
#include "UIElement/NavigationBar.h"
#include "UIElement/PlaylistIndicator.h"
#include "UIElement/TrackInfo.h"
#include "UIElement/PlaceHolder.h"
#include "UIElement/SearchBox.h"
#include "UIElement/BeatIndicator.h"
#include "UIElement/RecentPlayedList.h"
#include "UIElement/MediaLibPlaylist.h"
#include "UIElement/MediaLibFolder.h"
#include "UIElement/MyFavouriteList.h"
#include "UIElement/FolderExploreTree.h"
#include "UIElement/AllTracksList.h"
#include "UIElement/MiniSpectrum.h"
#include "UIElement/ElementSwitcher.h"
#include "UIElement/PanelElement.h"
#include "UIElement/IconElement.h"
#include "UIElement/RatingElement.h"
#include "UIElement/TracksList.h"
#include "UIElement/CheckBox.h"
#include "UIElement/ToggleButton.h"

namespace UiElement
{
    /////////////////////////////////////////////////////////////////////////////////////////
    class CElementFactory
    {
    public:
        std::shared_ptr<UiElement::Element> CreateElement(const std::string& name, CPlayerUIBase* ui);
    };
}

