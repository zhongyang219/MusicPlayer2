#include "stdafx.h"
#include "ElementFactory.h"

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<UiElement::Element> UiElement::CElementFactory::CreateElement(const std::string& name, CPlayerUIBase* ui)
{
    std::shared_ptr<UiElement::Element> element;
    if (name == "verticalLayout")
    {
        auto layout = std::make_shared<UiElement::Layout>();
        layout->type = UiElement::Layout::Vertical;
        element = layout;
    }
    else if (name == "horizontalLayout")
    {
        auto layout = std::make_shared<UiElement::Layout>();
        layout->type = UiElement::Layout::Horizontal;
        element = layout;
    }
    else if (name == "stackElement")
        element = std::make_shared<UiElement::StackElement>();
    else if (name == "rectangle")
        element = std::make_shared<UiElement::Rectangle>();
    else if (name == "button")
        element = std::make_shared<UiElement::Button>();
    else if (name == "text")
        element = std::make_shared<UiElement::Text>();
    else if (name == "albumCover")
        element = std::make_shared<UiElement::AlbumCover>();
    else if (name == "spectrum")
        element = std::make_shared<UiElement::Spectrum>();
    else if (name == "trackInfo")
        element = std::make_shared<UiElement::TrackInfo>();
    else if (name == "progressBar")
        element = std::make_shared<UiElement::ProgressBar>();
    else if (name == "lyrics")
        element = std::make_shared<UiElement::Lyrics>();
    else if (name == "volume")
        element = std::make_shared<UiElement::Volume>();
    else if (name == "beatIndicator")
        element = std::make_shared<UiElement::BeatIndicator>();
    else if (name == "playlist")
        element = std::make_shared<UiElement::Playlist>();
    else if (name == "playlistIndicator")
        element = std::make_shared<UiElement::PlaylistIndicator>();
    else if (name == "classicalControlBar")
        element = std::make_shared<UiElement::ClassicalControlBar>();
    else if (name == "recentPlayedList")
        element = std::make_shared<UiElement::RecentPlayedList>();
    else if (name == "mediaLibItemList")
        element = std::make_shared<UiElement::MediaLibItemList>();
    else if (name == "navigationBar")
        element = std::make_shared<UiElement::NavigationBar>();
    else if (name == "mediaLibFolder")
        element = std::make_shared<UiElement::MediaLibFolder>();
    else if (name == "mediaLibPlaylist")
        element = std::make_shared<UiElement::MediaLibPlaylist>();
    else if (name == "myFavouriteList")
        element = std::make_shared<UiElement::MyFavouriteList>();
    else if (name == "allTracksList")
        element = std::make_shared<UiElement::AllTracksList>();
    else if (name == "miniSpectrum")
        element = std::make_shared<UiElement::MiniSpectrum>();
    else if (name == "placeHolder")
        element = std::make_shared<UiElement::PlaceHolder>();
    else if (name == "medialibFolderExplore")
        element = std::make_shared<UiElement::FolderExploreTree>();
    else if (name == "searchBox")
        element = std::make_shared<UiElement::SearchBox>();
    else if (name == "elementSwitcher")
        element = std::make_shared<UiElement::ElementSwitcher>();
    else if (name == "icon")
        element = std::make_shared<UiElement::Icon>();
    else if (name == "ui" || name == "root" || name == "element")
        element = std::make_shared<UiElement::Element>();
    else if (name == "panel")
        element = std::make_shared<UiElement::Panel>();
    else if (name == "rating")
        element = std::make_shared<UiElement::RatingElement>();
    else if (name == "trackList")
        element = std::make_shared<UiElement::TrackList>();
    else if (name == "checkBox")
        element = std::make_shared<UiElement::CheckBox>();
    else if (name == "toggleButton")
        element = std::make_shared<UiElement::ToggleButton>();
    else if (name == "toggleSettingGroup")
        element = std::make_shared<UiElement::ToggleSettingGroup>();

    if (element != nullptr)
        element->SetUi(ui);
    return element;
}
