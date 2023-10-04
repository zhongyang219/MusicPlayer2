#include "stdafx.h"
#include "MediaTransControls.h"

#ifndef DISABLE_MEDIA_TRANS_CONTROLS

#include "Resource.h"
#include "shcore.h"
#include "WinVersionHelper.h"
#include "player.h"
#include "AudioTag.h"

#pragma comment(lib, "RuntimeObject.lib")
#pragma comment(lib, "ShCore.lib")

#include <wrl.h>
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
using namespace ABI::Windows::Storage;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

void MediaTransControls::SetEnabled(bool enable)
{
    m_enabled = enable;
}

bool MediaTransControls::Init() {
    if (!m_enabled || m_initailzed) return true;
    /// Windows 8.1 or later is required
    if (!CWinVersionHelper::IsWindows81OrLater()) return false;
    CComPtr<ISystemMediaTransportControlsInterop> op;
    HRESULT ret;
    if ((ret = GetActivationFactory(HStringReference(L"Windows.Media.SystemMediaTransportControls").Get(), &op)) != S_OK) {
        controls = nullptr;
        return false;
    }
    if ((ret = op->GetForWindow(AfxGetMainWnd()->GetSafeHwnd(), IID_PPV_ARGS(&controls))) != S_OK) {
        controls = nullptr;
        return false;
    }
    controls->put_PlaybackStatus(MediaPlaybackStatus::MediaPlaybackStatus_Closed);
    ret = controls->get_DisplayUpdater(&this->updater);
    ASSERT(ret == S_OK);
    if (ret != S_OK) {
        controls = nullptr;
        return false;
    }
    ret = updater->put_Type(MediaPlaybackType::MediaPlaybackType_Music);
    ASSERT(ret == S_OK);
    if (ret != S_OK) {
        controls = nullptr;
        updater = nullptr;
        return false;
    }
    ret = updater->get_MusicProperties(&music);
    ASSERT(ret == S_OK);
    if (ret != S_OK) {
        controls = nullptr;
        updater = nullptr;
        return false;
    }
    auto callbackButtonPressed = Callback<ABI::Windows::Foundation::ITypedEventHandler<SystemMediaTransportControls*, SystemMediaTransportControlsButtonPressedEventArgs*>>(
        [this](ISystemMediaTransportControls*, ISystemMediaTransportControlsButtonPressedEventArgs* pArgs) {
            HRESULT ret;
            SystemMediaTransportControlsButton button;
            if ((ret = pArgs->get_Button(&button)) != S_OK) {
                return ret;
            }
            OnButtonPressed(button);
            return S_OK;
        });
    ret = controls->add_ButtonPressed(callbackButtonPressed.Get(), &m_EventRegistrationToken);
    ASSERT(ret == S_OK);
    if (ret != S_OK) {
        controls = nullptr;
        updater = nullptr;
        music = nullptr;
        return false;
    }
    controls->put_IsPlayEnabled(true);
    controls->put_IsPauseEnabled(true);
    controls->put_IsStopEnabled(true);
    controls->put_IsPreviousEnabled(true);
    controls->put_IsNextEnabled(true);
    ret = controls->put_IsEnabled(true);
    ASSERT(ret == S_OK);
    m_initailzed = true;
    ret = ActivateInstance(HStringReference(RuntimeClass_Windows_Media_SystemMediaTransportControlsTimelineProperties).Get(), &timeline);
    if (timeline && ret == S_OK) {
        timeline->put_StartTime(ABI::Windows::Foundation::TimeSpan { 0 });
        timeline->put_Position(ABI::Windows::Foundation::TimeSpan{ 0 });
        timeline->put_MinSeekTime(ABI::Windows::Foundation::TimeSpan{ 0 });
        timeline->put_EndTime(ABI::Windows::Foundation::TimeSpan{ 0 });
        timeline->put_MaxSeekTime(ABI::Windows::Foundation::TimeSpan{ 0 });
    }
    controls.As(&controls2);
    if (controls2) {
        controls2->UpdateTimelineProperties(timeline);
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
        controls2->add_PlaybackPositionChangeRequested(callbackPlaybackPositionChangeRequested.Get(), &m_EventRegistrationToken2);
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
        controls2->add_PlaybackRateChangeRequested(callbackPlaybackRateChangeRequested.Get(), &m_EventRegistrationToken3);
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
    if (!m_enabled || !fn.length() || !updater) return;
    if (IsURL(fn)) return loadThumbnailFromUrl(fn);
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
    updater->put_Thumbnail(stream);
    updater->Update();
}

void MediaTransControls::loadThumbnail(const BYTE* content, size_t size) {
    if (!m_enabled || !content || !size || !updater) return;
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
    updater->put_Thumbnail(stream);
    updater->Update();
}

void MediaTransControls::loadThumbnailFromUrl(wstring url) {
    if (!m_enabled || !url.length() || !updater) return;
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
    ret = updater->put_Thumbnail(stream);
    ASSERT(ret == S_OK);
    updater->Update();
}

bool MediaTransControls::IsActive() {
    if (!m_enabled)
        return false;
    if (controls) {
        boolean enabled;
        HRESULT hr;
        if ((hr = controls->get_IsEnabled(&enabled)) == S_OK) {
            return enabled;
        }
    }
    return false;
}

void MediaTransControls::UpdateTitle(wstring title) {
    if (music) {
        music->put_Title(HStringReference(title.c_str()).Get());
    }
}

void MediaTransControls::UpdateArtist(wstring artist) {
    if (music) {
        auto ret = music->put_Artist(HStringReference(artist.c_str()).Get());
        ASSERT(ret == S_OK);
    }
}

void MediaTransControls::ClearAll()
{
    if (m_enabled && updater)
        updater->ClearAll();
}

void MediaTransControls::UpdateControls(Command cmd)
{
    if (m_enabled && controls)
    {
        switch (cmd) {
        case Command::PLAY:
            controls->put_PlaybackStatus(MediaPlaybackStatus_Playing);
            break;
        case Command::PAUSE:
            controls->put_PlaybackStatus(MediaPlaybackStatus_Paused);
            break;
        case Command::STOP:
            controls->put_PlaybackStatus(MediaPlaybackStatus_Stopped);
            break;
        default:
            ASSERT(FALSE);
            break;
        }
    }
}

void MediaTransControls::UpdateControlsMetadata(const SongInfo song)
{
    if (m_enabled && updater && music) {
        updater->put_Type(MediaPlaybackType_Music);
        UpdateTitle((!song.IsTitleEmpty() || song.file_path.empty()) ? song.GetTitle() : song.GetFileName());
        UpdateArtist(song.GetArtist());
        UpdateAlbumTitle(song.GetAlbum());
        UpdateTrackNumber(song.track);
        if (!song.IsGenreEmpty())
            UpdateGenre(song.GetGenre());
        if (song.total_tracks != 0)
            UpdateAlbumTrackCount(song.total_tracks);
        if (!song.album_artist.empty())
            UpdateAlbumArtist(song.album_artist);
        updater->Update();
    }
}

void MediaTransControls::OnButtonPressed(SystemMediaTransportControlsButton button) {
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

bool MediaTransControls::IsURL(wstring s) {
    auto i = s.find(L"://");
    return i != std::wstring::npos && i > 1;
}

void MediaTransControls::UpdateDuration(int64_t duration) {
    if (!m_enabled)
        return;
    HRESULT hr;
    if (timeline) {
        hr = timeline->put_EndTime(ABI::Windows::Foundation::TimeSpan{ duration * 10000 });
        ASSERT(S_OK == hr);
        hr = timeline->put_MaxSeekTime(ABI::Windows::Foundation::TimeSpan{ duration * 10000 });
        ASSERT(S_OK == hr);
    }
    if (controls2) {
        hr = controls2->UpdateTimelineProperties(timeline);
        ASSERT(S_OK == hr);
    }
}

void MediaTransControls::UpdatePosition(int64_t postion) {
    if (!m_enabled)
        return;
    HRESULT hr;
    if (timeline) {
        hr = timeline->put_Position(ABI::Windows::Foundation::TimeSpan{ postion * 10000 });
        ASSERT(S_OK == hr);
    }
    if (controls2) {
        hr = controls2->UpdateTimelineProperties(timeline);
        ASSERT(S_OK == hr);
    }
}

void MediaTransControls::UpdateAlbumArtist(wstring album_artist) {
    if (music) {
        auto ret = music->put_AlbumArtist(HStringReference(album_artist.c_str()).Get());
        ASSERT(ret == S_OK);
    }
}

void MediaTransControls::UpdateAlbumTitle(wstring album_title) {
    if (music) {
        ComPtr<IMusicDisplayProperties2> p = nullptr;
        music.As(&p);
        if (p) {
            auto ret = p->put_AlbumTitle(HStringReference(album_title.c_str()).Get());
            ASSERT(ret == S_OK);
        }
    }
}

void MediaTransControls::UpdateTrackNumber(UINT track) {
    if (music) {
        ComPtr<IMusicDisplayProperties2> p = nullptr;
        music.As(&p);
        if (p) {
            auto ret = p->put_TrackNumber(track);
            ASSERT(ret == S_OK);
        }
    }
}

void MediaTransControls::UpdateAlbumTrackCount(UINT track_count) {
    if (music) {
        ComPtr<IMusicDisplayProperties3> p = nullptr;
        music.As(&p);
        if (p) {
            auto ret = p->put_AlbumTrackCount(track_count);
            ASSERT(ret == S_OK);
        }
    }
}

void MediaTransControls::UpdateGenre(wstring genre) {
    std::vector<wstring> genres(1, genre);
    UpdateGenres(genres);
}

void MediaTransControls::UpdateGenres(std::vector<wstring> genres) {
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
                    auto &s = *i;
                    ret = vect->Append(HStringReference(s.c_str()).Get());
                    ASSERT(ret == S_OK);
                }
            }
        }
    }
}

void MediaTransControls::UpdateSpeed(float speed) {
    if (m_enabled && controls2) {
        auto ret = controls2->put_PlaybackRate(speed);
        ASSERT(ret == S_OK);
    }
}

#else

MediaTransControls::MediaTransControls()
{
}

void MediaTransControls::SetEnabled(bool enable)
{
}

bool MediaTransControls::Init()
{
    return false;
}

void MediaTransControls::loadThumbnail(wstring fn)
{
}

void MediaTransControls::loadThumbnail(const BYTE* content, size_t size)
{
}
void MediaTransControls::loadThumbnailFromUrl(wstring url)
{
}
bool MediaTransControls::IsActive()
{
    return false;
}
void MediaTransControls::ClearAll()
{
}

void MediaTransControls::UpdateControls(Command cmd)
{
}

void MediaTransControls::UpdateControlsMetadata(const SongInfo song)
{
}

void MediaTransControls::UpdateDuration(int64_t duration) {}

void MediaTransControls::UpdatePosition(int64_t postion) {}

void MediaTransControls::UpdateSpeed(float speed) {}
#endif
