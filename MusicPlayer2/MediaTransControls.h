#pragma once

#include "Common.h"

#ifndef DISABLE_MEDIA_TRANS_CONTROLS
#include <sdkddkver.h>
#include <atlbase.h>
#include <windows.media.h>
#include <string>

using namespace ABI::Windows::Media;

class MediaTransControls {
public:
    MediaTransControls(void) {
        this->controls = nullptr;
        this->updater = nullptr;
        this->music = nullptr;
    }
    ~MediaTransControls(void) {
        if (controls && m_EventRegistrationToken.value) {
            controls->remove_ButtonPressed(m_EventRegistrationToken);
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
    void UpdateControlsMetadata(const wstring& title, const wstring& artist);
protected:
    CComPtr<ISystemMediaTransportControls> controls;
    CComPtr<ISystemMediaTransportControlsDisplayUpdater> updater;
    CComPtr<IMusicDisplayProperties> music;
    EventRegistrationToken m_EventRegistrationToken;
    void UpdateTitle(wstring title);
    void UpdateArtist(wstring artist);
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
    void UpdateControlsMetadata(const wstring& title, const wstring& artist);
};

#endif
