#include <atlbase.h>
#include <wrl.h>
#include <windows.media.h>

using ABI::Windows::Media::SystemMediaTransportControlsButton;
using ABI::Windows::Media::MediaPlaybackStatus;

class MediaTransControlsImpl
{
public:
    MediaTransControlsImpl(HWND appWindow,
        std::function<void(SystemMediaTransportControlsButton)> ButtonCallback,
        std::function<void(INT64)> SeekCallback,
        std::function<void(DOUBLE)> SetSpeedCallback
    );
    ~MediaTransControlsImpl();

    void loadThumbnailFromFile(const wstring& file_path);
    void loadThumbnailFromBuff(const BYTE* content, size_t size);
    void loadThumbnailFromUrl(const wstring& url);
    bool IsActive();
    void ClearAll();
    void UpdateDuration(int64_t duration);
    void UpdatePosition(int64_t postion);
    void UpdateSpeed(float speed);
    void UpdateControls(MediaPlaybackStatus status);
    void UpdateControlsMetadata(const wstring& title, const wstring& artist, const wstring& album_artist, const wstring& album_title, const vector<wstring>& genres, UINT track, UINT track_count);

private:
    CComPtr<ABI::Windows::Media::ISystemMediaTransportControlsTimelineProperties> timeline = nullptr;
    CComPtr<ABI::Windows::Media::ISystemMediaTransportControlsDisplayUpdater> updater = nullptr;
    Microsoft::WRL::ComPtr<ABI::Windows::Media::ISystemMediaTransportControls> controls = nullptr;
    Microsoft::WRL::ComPtr<ABI::Windows::Media::ISystemMediaTransportControls2> controls2 = nullptr;
    Microsoft::WRL::ComPtr<ABI::Windows::Media::IMusicDisplayProperties> music = nullptr;
    EventRegistrationToken m_EventRegistrationToken{}, m_EventRegistrationToken2{}, m_EventRegistrationToken3{};
};
