#include "stdafx.h"
#include "MediaTransControls.h"

#ifndef DISABLE_MEDIA_TRANS_CONTROLS
#include "player.h"
#include "WinVersionHelper.h"
#include "resource.h"

#include <atlbase.h>
#include <wrl.h>
#include <windows.media.h>
#include <shcore.h>
#pragma comment(lib, "RuntimeObject.lib")
#pragma comment(lib, "ShCore.lib")

/// The file content of SystemMediaTransportControlsInterop.h
/// SystemMediaTransportControlsInterop.h can not be found in Win 8.1 SDK
#ifndef ISystemMediaTransportControlsInterop
EXTERN_C const IID IID_ISystemMediaTransportControlsInterop;
MIDL_INTERFACE("ddb0472d-c911-4a1f-86d9-dc3d71a95f5a")
ISystemMediaTransportControlsInterop : public IInspectable{
 public:
  virtual HRESULT STDMETHODCALLTYPE GetForWindow(
      /* [in] */ __RPC__in HWND appWindow,
      /* [in] */ __RPC__in REFIID riid,
      /* [iid_is][retval][out] */
      __RPC__deref_out_opt void** mediaTransportControl) = 0;
};
#  endif

using namespace Windows::Foundation;
using namespace ABI::Windows::Media;
using namespace ABI::Windows::Storage;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

class MediaTransControls::MediaTransControlsImpl
{
public:
    bool m_initailzed = false;
    bool m_enabled = true;
    CComPtr<ISystemMediaTransportControlsTimelineProperties> timeline = nullptr;
    CComPtr<ISystemMediaTransportControlsDisplayUpdater> updater = nullptr;
    ComPtr<ISystemMediaTransportControls> controls = nullptr;
    ComPtr<ISystemMediaTransportControls2> controls2 = nullptr;
    ComPtr<IMusicDisplayProperties> music = nullptr;
    EventRegistrationToken m_EventRegistrationToken{}, m_EventRegistrationToken2{}, m_EventRegistrationToken3{};

    void UpdateTitle(const wchar_t* title) const {
        if (music) {
            music->put_Title(HStringReference(title).Get());
        }
    }
    void UpdateArtist(const wchar_t* artist) const {
        if (music) {
            auto ret = music->put_Artist(HStringReference(artist).Get());
            ASSERT(ret == S_OK);
        }
    }
    void UpdateAlbumArtist(const wchar_t* album_artist) const {
        if (music) {
            auto ret = music->put_AlbumArtist(HStringReference(album_artist).Get());
            ASSERT(ret == S_OK);
        }
    }
    void UpdateAlbumTitle(const wchar_t* album_title) const {
        if (music) {
            ComPtr<IMusicDisplayProperties2> p = nullptr;
            music.As(&p);
            if (p) {
                auto ret = p->put_AlbumTitle(HStringReference(album_title).Get());
                ASSERT(ret == S_OK);
            }
        }
    }
    void UpdateTrackNumber(UINT track) const {
        if (music) {
            ComPtr<IMusicDisplayProperties2> p = nullptr;
            music.As(&p);
            if (p) {
                auto ret = p->put_TrackNumber(track);
                ASSERT(ret == S_OK);
            }
        }
    }
    void UpdateAlbumTrackCount(UINT track_count) const {
        if (music) {
            ComPtr<IMusicDisplayProperties3> p = nullptr;
            music.As(&p);
            if (p) {
                auto ret = p->put_AlbumTrackCount(track_count);
                ASSERT(ret == S_OK);
            }
        }
    }
    void UpdateGenre(const wchar_t* genre) const {
        std::vector<wstring> genres(1, genre);
        UpdateGenres(genres);
    }
    void UpdateGenres(const std::vector<wstring>& genres) const {
        if (music) {
            ComPtr<IMusicDisplayProperties2> p = nullptr;
            music.As(&p);
            if (p) {
                ComPtr<ABI::Windows::Foundation::Collections::IVector<HSTRING>> vect = nullptr;
                auto ret = p->get_Genres(vect.GetAddressOf());
                ASSERT(ret == S_OK);
                if (vect) {
                    ret = vect->Clear();
                    ASSERT(ret == S_OK);
                    for (auto i = genres.begin(); i != genres.end(); i++) {
                        const auto& s = *i;
                        ret = vect->Append(HStringReference(s.c_str()).Get());
                        ASSERT(ret == S_OK);
                    }
                }
            }
        }
    }

    static void OnButtonPressed(SystemMediaTransportControlsButton button)
    {
        switch (button) {
        case SystemMediaTransportControlsButton_Play:
            SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, ID_PLAY, 0);
            break;
        case SystemMediaTransportControlsButton_Pause:
            SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, ID_PAUSE, 0);
            break;
        case SystemMediaTransportControlsButton_Stop:
            SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, ID_STOP, 0);
            break;
        case SystemMediaTransportControlsButton_Previous:
            SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, ID_PREVIOUS, 0);
            break;
        case SystemMediaTransportControlsButton_Next:
            SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, ID_NEXT, 0);
            break;
        }
    }

    static bool IsURL(const wstring& s)
    {
        auto i = s.find(L"://");
        return i != std::wstring::npos && i > 1;
    }
};

MediaTransControls::MediaTransControls() : pImpl(std::make_unique<MediaTransControlsImpl>())
{
};

MediaTransControls::~MediaTransControls()
{
    if (pImpl->controls && pImpl->m_EventRegistrationToken.value) {
        pImpl->controls->remove_ButtonPressed(pImpl->m_EventRegistrationToken);
    }
    if (pImpl->controls2 && pImpl->m_EventRegistrationToken2.value) {
        pImpl->controls2->remove_PlaybackPositionChangeRequested(pImpl->m_EventRegistrationToken2);
    }
    if (pImpl->controls2 && pImpl->m_EventRegistrationToken3.value) {
        pImpl->controls2->remove_PlaybackRateChangeRequested(pImpl->m_EventRegistrationToken3);
    }
}

void MediaTransControls::SetEnabled(bool enable)
{
    pImpl->m_enabled = enable;
}

bool MediaTransControls::Init() {
    if (!pImpl->m_enabled || pImpl->m_initailzed) return true;
    /// Windows 8.1 or later is required
    if (!CWinVersionHelper::IsWindows81OrLater()) return false;
    CComPtr<ISystemMediaTransportControlsInterop> op;
    HRESULT ret;
    if ((ret = GetActivationFactory(HStringReference(L"Windows.Media.SystemMediaTransportControls").Get(), &op)) != S_OK) {
        pImpl->controls = nullptr;
        return false;
    }
    if ((ret = op->GetForWindow(AfxGetMainWnd()->GetSafeHwnd(), IID_PPV_ARGS(&pImpl->controls))) != S_OK) {
        pImpl->controls = nullptr;
        return false;
    }
    pImpl->controls->put_PlaybackStatus(MediaPlaybackStatus::MediaPlaybackStatus_Closed);
    ret = pImpl->controls->get_DisplayUpdater(&this->pImpl->updater);
    ASSERT(ret == S_OK);
    if (ret != S_OK) {
        pImpl->controls = nullptr;
        return false;
    }
    ret = pImpl->updater->put_Type(MediaPlaybackType::MediaPlaybackType_Music);
    ASSERT(ret == S_OK);
    if (ret != S_OK) {
        pImpl->controls = nullptr;
        pImpl->updater = nullptr;
        return false;
    }
    ret = pImpl->updater->get_MusicProperties(&pImpl->music);
    ASSERT(ret == S_OK);
    if (ret != S_OK) {
        pImpl->controls = nullptr;
        pImpl->updater = nullptr;
        return false;
    }
    auto callbackButtonPressed = Callback<ABI::Windows::Foundation::ITypedEventHandler<SystemMediaTransportControls*, SystemMediaTransportControlsButtonPressedEventArgs*>>(
        [this](ISystemMediaTransportControls*, ISystemMediaTransportControlsButtonPressedEventArgs* pArgs) {
            HRESULT ret;
            SystemMediaTransportControlsButton button;
            if ((ret = pArgs->get_Button(&button)) != S_OK) {
                return ret;
            }
            pImpl->OnButtonPressed(button);
            return S_OK;
        });
    ret = pImpl->controls->add_ButtonPressed(callbackButtonPressed.Get(), &pImpl->m_EventRegistrationToken);
    ASSERT(ret == S_OK);
    if (ret != S_OK) {
        pImpl->controls = nullptr;
        pImpl->updater = nullptr;
        pImpl->music = nullptr;
        return false;
    }
    pImpl->controls->put_IsPlayEnabled(true);
    pImpl->controls->put_IsPauseEnabled(true);
    pImpl->controls->put_IsStopEnabled(true);
    pImpl->controls->put_IsPreviousEnabled(true);
    pImpl->controls->put_IsNextEnabled(true);
    ret = pImpl->controls->put_IsEnabled(true);
    ASSERT(ret == S_OK);
    pImpl->m_initailzed = true;
    ret = ActivateInstance(HStringReference(RuntimeClass_Windows_Media_SystemMediaTransportControlsTimelineProperties).Get(), &pImpl->timeline);
    if (pImpl->timeline && ret == S_OK) {
        pImpl->timeline->put_StartTime(ABI::Windows::Foundation::TimeSpan { 0 });
        pImpl->timeline->put_Position(ABI::Windows::Foundation::TimeSpan{ 0 });
        pImpl->timeline->put_MinSeekTime(ABI::Windows::Foundation::TimeSpan{ 0 });
        pImpl->timeline->put_EndTime(ABI::Windows::Foundation::TimeSpan{ 0 });
        pImpl->timeline->put_MaxSeekTime(ABI::Windows::Foundation::TimeSpan{ 0 });
    }
    pImpl->controls.As(&pImpl->controls2);
    if (pImpl->controls2) {
        pImpl->controls2->UpdateTimelineProperties(pImpl->timeline);
        auto callbackPlaybackPositionChangeRequested = Callback<ABI::Windows::Foundation::ITypedEventHandler<SystemMediaTransportControls*, PlaybackPositionChangeRequestedEventArgs*>>(
            [this](ISystemMediaTransportControls*, IPlaybackPositionChangeRequestedEventArgs* pArgs) {
                HRESULT ret;
                ABI::Windows::Foundation::TimeSpan time;
                if ((ret = pArgs->get_RequestedPlaybackPosition(&time)) == S_OK) {
                    if (CPlayer::GetInstance().GetPlayStatusMutex().try_lock_for(std::chrono::milliseconds(1000))) {
                        CPlayer::GetInstance().SeekTo(static_cast<int>(time.Duration / 10000));
                        CPlayer::GetInstance().GetPlayStatusMutex().unlock();
                    }
                }
                return S_OK;
            });
        pImpl->controls2->add_PlaybackPositionChangeRequested(callbackPlaybackPositionChangeRequested.Get(), &pImpl->m_EventRegistrationToken2);
        auto callbackPlaybackRateChangeRequested = Callback<ABI::Windows::Foundation::ITypedEventHandler<SystemMediaTransportControls*, PlaybackRateChangeRequestedEventArgs*>>(
            [this](ISystemMediaTransportControls*, IPlaybackRateChangeRequestedEventArgs* pArgs) {
                HRESULT ret;
                double rate = 1.0;
                ret = pArgs->get_RequestedPlaybackRate(&rate);
                if (ret == S_OK) {
                    CPlayer::GetInstance().SetSpeed((float)rate);
                }
                return S_OK;
        });
        pImpl->controls2->add_PlaybackRateChangeRequested(callbackPlaybackRateChangeRequested.Get(), &pImpl->m_EventRegistrationToken3);
    }
    return true;
}

template <typename T>
bool AwaitForIAsyncOperation(CComPtr<ABI::Windows::Foundation::IAsyncOperation<T>> io) {
    CComPtr<ABI::Windows::Foundation::IAsyncInfo> info;
    ABI::Windows::Foundation::AsyncStatus status;
    HRESULT ret;
    info = io;
    while (true) {
        if ((ret = info->get_Status(&status)) != S_OK) {
            return false;
        }
        if (status != ABI::Windows::Foundation::AsyncStatus::Started) {
            if (status == ABI::Windows::Foundation::AsyncStatus::Completed) return true;
            return false;
        }
        Sleep(10);
    }
}

void MediaTransControls::loadThumbnail(wstring fn) {
    if (!pImpl->m_enabled || !fn.length() || !pImpl->updater) return;
    if (pImpl->IsURL(fn)) return loadThumbnailFromUrl(fn);
    HRESULT ret;
    CComPtr<IStorageFileStatics> sfs;
    if ((ret = GetActivationFactory(HStringReference(RuntimeClass_Windows_Storage_StorageFile).Get(), &sfs)) != S_OK) {
        return;
    }
    CComPtr<ABI::Windows::Foundation::IAsyncOperation<StorageFile*>> af;
    if ((ret = sfs->GetFileFromPathAsync(HStringReference(fn.c_str()).Get(), &af)) != S_OK) {
        return;
    }
    /// Present file
    CComPtr<IStorageFile> f;
    if (!AwaitForIAsyncOperation(af)) {
        return;
    }
    if ((ret = af->GetResults(&f)) != S_OK) {
        return;
    }
    CComPtr<Streams::IRandomAccessStreamReferenceStatics> rasrs;
    if ((ret = GetActivationFactory(HStringReference(RuntimeClass_Windows_Storage_Streams_RandomAccessStreamReference).Get(), &rasrs)) != S_OK) {
        return;
    }
    CComPtr<Streams::IRandomAccessStreamReference> stream;
    if ((ret = rasrs->CreateFromFile(f, &stream)) != S_OK) {
        return;
    }
    pImpl->updater->put_Thumbnail(stream);
    pImpl->updater->Update();
}

void MediaTransControls::loadThumbnail(const BYTE* content, size_t size) {
    if (!pImpl->m_enabled || !content || !size || !pImpl->updater) return;
    ComPtr<Streams::IRandomAccessStream> s;
    HRESULT ret;
    if ((ret = ActivateInstance(HStringReference(RuntimeClass_Windows_Storage_Streams_InMemoryRandomAccessStream).Get(), s.GetAddressOf())) != S_OK) {
        return;
    }
    ComPtr<IStream> writer;
    CreateStreamOverRandomAccessStream(s.Get(), IID_PPV_ARGS(writer.GetAddressOf()));
    writer->Write(content, (ULONG)size, nullptr);
    CComPtr<Streams::IRandomAccessStreamReferenceStatics> rasrs;
    if ((ret = GetActivationFactory(HStringReference(RuntimeClass_Windows_Storage_Streams_RandomAccessStreamReference).Get(), &rasrs)) != S_OK) {
        return;
    }
    CComPtr<Streams::IRandomAccessStreamReference> stream;
    if ((ret = rasrs->CreateFromStream(s.Get(), &stream)) != S_OK) {
        return;
    }
    pImpl->updater->put_Thumbnail(stream);
    pImpl->updater->Update();
}

void MediaTransControls::loadThumbnailFromUrl(wstring url) {
    if (!pImpl->m_enabled || !url.length() || !pImpl->updater) return;
    HRESULT ret;
    CComPtr<ABI::Windows::Foundation::IUriRuntimeClassFactory> u;
    if ((ret = Windows::Foundation::GetActivationFactory(HStringReference(RuntimeClass_Windows_Foundation_Uri).Get(), &u)) != S_OK) {
        return;
    }
    CComPtr<ABI::Windows::Foundation::IUriRuntimeClass> uri;
    if ((ret = u->CreateUri(HStringReference(url.c_str()).Get(), &uri)) != S_OK) {
        return;
    }
    CComPtr<Streams::IRandomAccessStreamReferenceStatics> rasrs;
    if ((ret = GetActivationFactory(HStringReference(RuntimeClass_Windows_Storage_Streams_RandomAccessStreamReference).Get(), &rasrs)) != S_OK) {
        return;
    }
    CComPtr<Streams::IRandomAccessStreamReference> stream;
    if ((ret = rasrs->CreateFromUri(uri, &stream)) != S_OK) {
        return;
    }
    ret = pImpl->updater->put_Thumbnail(stream);
    ASSERT(ret == S_OK);
    pImpl->updater->Update();
}

bool MediaTransControls::IsActive() {
    if (!pImpl->m_enabled)
        return false;
    if (pImpl->controls) {
        boolean enabled;
        HRESULT hr;
        if ((hr = pImpl->controls->get_IsEnabled(&enabled)) == S_OK) {
            return enabled;
        }
    }
    return false;
}

void MediaTransControls::ClearAll()
{
    if (pImpl->m_enabled && pImpl->updater)
        pImpl->updater->ClearAll();
}

void MediaTransControls::UpdateControls(PlaybackStatus status)
{
    if (pImpl->m_enabled && pImpl->controls)
    {
        switch (status)
        {
        case PlaybackStatus::Closed:
            pImpl->controls->put_PlaybackStatus(MediaPlaybackStatus_Closed);
            break;
        case PlaybackStatus::Changing:
            pImpl->controls->put_PlaybackStatus(MediaPlaybackStatus_Changing);
            break;
        case PlaybackStatus::Stopped:
            pImpl->controls->put_PlaybackStatus(MediaPlaybackStatus_Stopped);
            break;
        case PlaybackStatus::Playing:
            pImpl->controls->put_PlaybackStatus(MediaPlaybackStatus_Playing);
            break;
        case PlaybackStatus::Paused:
            pImpl->controls->put_PlaybackStatus(MediaPlaybackStatus_Paused);
            break;
        default:
            ASSERT(FALSE);
            break;
        }
    }
}

void MediaTransControls::UpdateControlsMetadata(const SongInfo& song)
{
    if (pImpl->m_enabled && pImpl->updater && pImpl->music) {
        pImpl->updater->put_Type(MediaPlaybackType_Music);
        pImpl->UpdateTitle((!song.IsTitleEmpty() || song.file_path.empty()) ? song.GetTitle().c_str() : song.GetFileName().c_str());
        pImpl->UpdateArtist(song.GetArtist().c_str());
        pImpl->UpdateAlbumTitle(song.GetAlbum().c_str());
        pImpl->UpdateTrackNumber(song.track);
        if (!song.IsGenreEmpty())
            pImpl->UpdateGenre(song.GetGenre().c_str());
        if (song.total_tracks != 0)
            pImpl->UpdateAlbumTrackCount(song.total_tracks);
        if (!song.album_artist.empty())
            pImpl->UpdateAlbumArtist(song.album_artist.c_str());
        pImpl->updater->Update();
    }
}

void MediaTransControls::UpdateDuration(int64_t duration) {
    if (!pImpl->m_enabled)
        return;
    HRESULT hr;
    if (pImpl->timeline) {
        hr = pImpl->timeline->put_EndTime(ABI::Windows::Foundation::TimeSpan{ duration * 10000 });
        ASSERT(S_OK == hr);
        hr = pImpl->timeline->put_MaxSeekTime(ABI::Windows::Foundation::TimeSpan{ duration * 10000 });
        ASSERT(S_OK == hr);
    }
    if (pImpl->controls2) {
        hr = pImpl->controls2->UpdateTimelineProperties(pImpl->timeline);
        ASSERT(S_OK == hr);
    }
}

void MediaTransControls::UpdatePosition(int64_t postion) {
    if (!pImpl->m_enabled)
        return;
    HRESULT hr;
    if (pImpl->timeline) {
        hr = pImpl->timeline->put_Position(ABI::Windows::Foundation::TimeSpan{ postion * 10000 });
        ASSERT(S_OK == hr);
    }
    if (pImpl->controls2) {
        hr = pImpl->controls2->UpdateTimelineProperties(pImpl->timeline);
        ASSERT(S_OK == hr);
    }
}

void MediaTransControls::UpdateSpeed(float speed) {
    if (pImpl->m_enabled && pImpl->controls2) {
        auto ret = pImpl->controls2->put_PlaybackRate(speed);
        ASSERT(ret == S_OK);
    }
}

#else

MediaTransControls::MediaTransControls() {}
MediaTransControls::~MediaTransControls() {}
void MediaTransControls::SetEnabled(bool enable) {}
bool MediaTransControls::Init() { return false; }
void MediaTransControls::loadThumbnail(wstring fn) {}
void MediaTransControls::loadThumbnail(const BYTE* content, size_t size) {}
void MediaTransControls::loadThumbnailFromUrl(wstring url) {}
bool MediaTransControls::IsActive() { return false; }
void MediaTransControls::ClearAll() {}
void MediaTransControls::UpdateControls(PlaybackStatus status) {}
void MediaTransControls::UpdateControlsMetadata(const SongInfo& song) {}
void MediaTransControls::UpdateDuration(int64_t duration) {}
void MediaTransControls::UpdatePosition(int64_t postion) {}
void MediaTransControls::UpdateSpeed(float speed) {}

#endif
