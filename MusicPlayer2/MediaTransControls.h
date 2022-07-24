#pragma once

#include "Common.h"
#include "SongInfo.h"

#ifndef DISABLE_MEDIA_TRANS_CONTROLS
#include <sdkddkver.h>
#include <atlbase.h>
#include <windows.media.h>
#include <string>
#include <wrl.h>

using namespace ABI::Windows::Media;

class MediaTransControls {
public:
    MediaTransControls(void) {
        this->timeline = nullptr;
        this->controls = nullptr;
        this->updater = nullptr;
        this->music = nullptr;
        this->controls2 = nullptr;
    }
    ~MediaTransControls(void) {
        if (controls && m_EventRegistrationToken.value) {
            controls->remove_ButtonPressed(m_EventRegistrationToken);
        }
        if (controls2 && m_EventRegistrationToken2.value) {
            controls2->remove_PlaybackPositionChangeRequested(m_EventRegistrationToken2);
        }
        if (controls2 && m_EventRegistrationToken3.value) {
            controls2->remove_PlaybackRateChangeRequested(m_EventRegistrationToken3);
        }
    }
    /**
     * @brief Intitialize the interface
     * @param main
     * @return true if initalized
    */
    bool Init();
    void loadThumbnail(wstring fn);
    void loadThumbnail(const BYTE* content, size_t size);
    void loadThumbnailFromUrl(wstring url);
    bool IsActive();
    void ClearAll();
    void UpdateControls(Command cmd);
    void UpdateControlsMetadata(const SongInfo song);
    /// Update duration of stream, in milliseconds
    void UpdateDuration(int64_t duration);
    /// Update current time, in milliseconds
    void UpdatePosition(int64_t postion);
    /// Update current speed
    void UpdateSpeed(float speed);
protected:
    CComPtr<ISystemMediaTransportControlsTimelineProperties> timeline;
    Microsoft::WRL::ComPtr<ISystemMediaTransportControls> controls;
    Microsoft::WRL::ComPtr<ISystemMediaTransportControls2> controls2;
    CComPtr<ISystemMediaTransportControlsDisplayUpdater> updater;
    Microsoft::WRL::ComPtr<IMusicDisplayProperties> music;
    EventRegistrationToken m_EventRegistrationToken, m_EventRegistrationToken2, m_EventRegistrationToken3;
    void UpdateTitle(wstring title);
    void UpdateArtist(wstring artist);
    void UpdateAlbumArtist(wstring album_artist);
    void UpdateAlbumTitle(wstring album_title);
    void UpdateTrackNumber(UINT track);
    void UpdateAlbumTrackCount(UINT track_count);
    void UpdateGenre(wstring genre);
    void UpdateGenres(std::vector<wstring> genres);
    void OnButtonPressed(SystemMediaTransportControlsButton button);
    bool IsURL(wstring s);
    bool m_initailzed = false;
};

#else

class MediaTransControls
{
public:
    MediaTransControls();
    bool Init();
    void loadThumbnail(wstring fn);
    void loadThumbnail(const BYTE* content, size_t size);
    void loadThumbnailFromUrl(wstring url);
    bool IsActive();
    void ClearAll();
    void UpdateControls(Command cmd);
    void UpdateControlsMetadata(const SongInfo song);
    void UpdateDuration(int64_t duration);
    void UpdatePosition(int64_t postion);
    void UpdateSpeed(float speed)
};

#endif
