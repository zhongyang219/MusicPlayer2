#include "stdafx.h"
#include "MediaTransControlsImpl.h"

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
#endif

using Windows::Foundation::GetActivationFactory;
using Windows::Foundation::ActivateInstance;
using Microsoft::WRL::Wrappers::HStringReference;

using namespace ABI::Windows::Media;
using namespace ABI::Windows::Storage;
using namespace Microsoft::WRL;


MediaTransControlsImpl::MediaTransControlsImpl(HWND appWindow, std::function<void(SystemMediaTransportControlsButton)> ButtonCallback, std::function<void(INT64)> SeekCallback, std::function<void(double)> SetSpeedCallback)
{
    CComPtr<ISystemMediaTransportControlsInterop> op;
    HRESULT ret;
    if ((ret = GetActivationFactory(HStringReference(RuntimeClass_Windows_Media_SystemMediaTransportControls).Get(), &op)) != S_OK) {
        controls = nullptr;
        return;
    }
    if ((ret = op->GetForWindow(appWindow, IID_PPV_ARGS(&controls))) != S_OK) {
        controls = nullptr;
        return;
    }
    controls->put_PlaybackStatus(MediaPlaybackStatus::MediaPlaybackStatus_Closed);
    ret = controls->get_DisplayUpdater(&this->updater);
    ASSERT(ret == S_OK);
    if (ret != S_OK) {
        controls = nullptr;
        return;
    }
    ret = updater->put_Type(MediaPlaybackType::MediaPlaybackType_Music);
    ASSERT(ret == S_OK);
    if (ret != S_OK) {
        controls = nullptr;
        updater = nullptr;
        return;
    }
    ret = updater->get_MusicProperties(&music);
    ASSERT(ret == S_OK);
    if (ret != S_OK) {
        controls = nullptr;
        updater = nullptr;
        return;
    }
    auto callbackButtonPressed = Callback<ABI::Windows::Foundation::ITypedEventHandler<SystemMediaTransportControls*, SystemMediaTransportControlsButtonPressedEventArgs*>>(
        [ButtonCallback](ISystemMediaTransportControls*, ISystemMediaTransportControlsButtonPressedEventArgs* pArgs) {
            HRESULT ret;
            SystemMediaTransportControlsButton button;
            if ((ret = pArgs->get_Button(&button)) != S_OK) {
                return ret;
            }
            ButtonCallback(button);
            return S_OK;
        });
    ret = controls->add_ButtonPressed(callbackButtonPressed.Get(), &m_EventRegistrationToken);
    ASSERT(ret == S_OK);
    if (ret != S_OK) {
        controls = nullptr;
        updater = nullptr;
        music = nullptr;
        return;
    }
    controls->put_IsPlayEnabled(true);
    controls->put_IsPauseEnabled(true);
    controls->put_IsStopEnabled(true);
    controls->put_IsPreviousEnabled(true);
    controls->put_IsNextEnabled(true);
    ret = controls->put_IsEnabled(true);
    ASSERT(ret == S_OK);
    ret = ActivateInstance(HStringReference(RuntimeClass_Windows_Media_SystemMediaTransportControlsTimelineProperties).Get(), &timeline);
    if (timeline && ret == S_OK) {
        timeline->put_StartTime(ABI::Windows::Foundation::TimeSpan{ 0 });
        timeline->put_Position(ABI::Windows::Foundation::TimeSpan{ 0 });
        timeline->put_MinSeekTime(ABI::Windows::Foundation::TimeSpan{ 0 });
        timeline->put_EndTime(ABI::Windows::Foundation::TimeSpan{ 0 });
        timeline->put_MaxSeekTime(ABI::Windows::Foundation::TimeSpan{ 0 });
    }
    controls.As(&controls2);
    if (controls2) {
        controls2->UpdateTimelineProperties(timeline);
        auto callbackPlaybackPositionChangeRequested = Callback<ABI::Windows::Foundation::ITypedEventHandler<SystemMediaTransportControls*, PlaybackPositionChangeRequestedEventArgs*>>(
            [SeekCallback](ISystemMediaTransportControls*, IPlaybackPositionChangeRequestedEventArgs* pArgs) {
                HRESULT ret;
                ABI::Windows::Foundation::TimeSpan time;
                if ((ret = pArgs->get_RequestedPlaybackPosition(&time)) == S_OK) {
                    SeekCallback(time.Duration);
                }
                return S_OK;
            });
        controls2->add_PlaybackPositionChangeRequested(callbackPlaybackPositionChangeRequested.Get(), &m_EventRegistrationToken2);
        auto callbackPlaybackRateChangeRequested = Callback<ABI::Windows::Foundation::ITypedEventHandler<SystemMediaTransportControls*, PlaybackRateChangeRequestedEventArgs*>>(
            [SetSpeedCallback](ISystemMediaTransportControls*, IPlaybackRateChangeRequestedEventArgs* pArgs) {
                HRESULT ret;
                DOUBLE rate = 1.0;
                ret = pArgs->get_RequestedPlaybackRate(&rate);
                if (ret == S_OK) {
                    SetSpeedCallback(rate);
                }
                return S_OK;
            });
        controls2->add_PlaybackRateChangeRequested(callbackPlaybackRateChangeRequested.Get(), &m_EventRegistrationToken3);
    }
}

MediaTransControlsImpl::~MediaTransControlsImpl()
{
    if (controls && m_EventRegistrationToken.value) {
        controls->remove_ButtonPressed(m_EventRegistrationToken);
    }
    if (controls2 && m_EventRegistrationToken2.value) {
        controls2->remove_PlaybackPositionChangeRequested(m_EventRegistrationToken2);
    }
    if (controls2 && m_EventRegistrationToken3.value) {
        controls2->remove_PlaybackRateChangeRequested(m_EventRegistrationToken3);
    }
    if (updater)
        updater->ClearAll();
    if (controls)
        controls->put_IsEnabled(false);
}

namespace {
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
}

void MediaTransControlsImpl::loadThumbnailFromFile(const wstring& file_path)
{
    if (!file_path.length() || !updater) return;
    HRESULT ret;
    CComPtr<IStorageFileStatics> sfs;
    if ((ret = GetActivationFactory(HStringReference(RuntimeClass_Windows_Storage_StorageFile).Get(), &sfs)) != S_OK) {
        return;
    }
    CComPtr<ABI::Windows::Foundation::IAsyncOperation<StorageFile*>> af;
    if ((ret = sfs->GetFileFromPathAsync(HStringReference(file_path.c_str()).Get(), &af)) != S_OK) {
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

void MediaTransControlsImpl::loadThumbnailFromBuff(const BYTE* content, size_t size)
{
    if (!content || !size || !updater) return;
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

void MediaTransControlsImpl::loadThumbnailFromUrl(const wstring& url)
{
    if (!url.length() || !updater) return;
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

bool MediaTransControlsImpl::IsActive()
{
    if (controls) {
        boolean enabled;
        HRESULT hr;
        if ((hr = controls->get_IsEnabled(&enabled)) == S_OK) {
            return enabled;
        }
    }
    return false;
}

void MediaTransControlsImpl::ClearAll()
{
    if (updater)
        updater->ClearAll();
}

void MediaTransControlsImpl::UpdateDuration(int64_t duration)
{
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

void MediaTransControlsImpl::UpdatePosition(int64_t postion)
{
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

void MediaTransControlsImpl::UpdateSpeed(float speed)
{
    if (controls2) {
        auto ret = controls2->put_PlaybackRate(speed);
        ASSERT(ret == S_OK);
    }
}

void MediaTransControlsImpl::UpdateControls(MediaPlaybackStatus status)
{
    if (controls)
        controls->put_PlaybackStatus(status);
}

void MediaTransControlsImpl::UpdateControlsMetadata(const wstring& title, const wstring& artist, const wstring& album_artist, const wstring& album_title, const vector<wstring>& genres, UINT track, UINT track_count)
{
    if (updater && music)
    {
        HRESULT ret = 0;
        ret = updater->put_Type(MediaPlaybackType::MediaPlaybackType_Music);
        ASSERT(ret == S_OK);
        // 标题
        ret = music->put_Title(HStringReference(title.c_str()).Get());
        ASSERT(ret == S_OK);
        // 艺术家
        ret = music->put_Artist(HStringReference(artist.c_str()).Get());
        ASSERT(ret == S_OK);
        // 专辑艺术家
        ret = music->put_AlbumArtist(HStringReference(album_artist.c_str()).Get());
        ASSERT(ret == S_OK);

        ComPtr<IMusicDisplayProperties2> p2 = nullptr;
        ret = music.As(&p2);
        if (p2)
        {
            // 专辑名
            ret = p2->put_AlbumTitle(HStringReference(album_title.c_str()).Get());
            ASSERT(ret == S_OK);
            // 流派
            ComPtr<ABI::Windows::Foundation::Collections::IVector<HSTRING>> vect = nullptr;
            auto ret = p2->get_Genres(vect.GetAddressOf());
            ASSERT(ret == S_OK);
            if (vect) {
                ret = vect->Clear();
                ASSERT(ret == S_OK);
                for (const auto& s : genres) {
                    ret = vect->Append(HStringReference(s.c_str()).Get());
                    ASSERT(ret == S_OK);
                }
            }
            // 音轨号
            ret = p2->put_TrackNumber(track);
            ASSERT(ret == S_OK);
        }

        ComPtr<IMusicDisplayProperties3> p3 = nullptr;
        ret = music.As(&p3);
        if (p3)
        {
            // 此专辑的总音轨数
            ret = p3->put_AlbumTrackCount(track_count);
            ASSERT(ret == S_OK);
        }
        updater->Update();
    }
}
