#include "stdafx.h"
#include "MediaTransControls.h"
#ifndef DISABLE_MEDIA_TRANS_CONTROLS

#include "MediaTransControlsImpl.h"

#include "player.h"
#include "WinVersionHelper.h"
#include "resource.h"

MediaTransControls::MediaTransControls() : pImpl(std::make_unique<MediaTransControlsImpl>())
{
}

MediaTransControls::~MediaTransControls()
{
}

void MediaTransControls::SetEnabled(bool enable)
{
    pImpl->SetEnabled(enable);
}

bool MediaTransControls::InitSMTC()
{
    /// Windows 8.1 or later is required
    if (!CWinVersionHelper::IsWindows81OrLater())
        return false;
    auto ButtonCallback = [](SystemMediaTransportControlsButton type)
        {
            switch (type)
            {
            case SystemMediaTransportControlsButton::SystemMediaTransportControlsButton_Play:
                SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, ID_PLAY, 0);
                break;
            case SystemMediaTransportControlsButton::SystemMediaTransportControlsButton_Pause:
                SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, ID_PAUSE, 0);
                break;
            case SystemMediaTransportControlsButton::SystemMediaTransportControlsButton_Stop:
                SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, ID_STOP, 0);
                break;
            case SystemMediaTransportControlsButton::SystemMediaTransportControlsButton_Previous:
                SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, ID_PREVIOUS, 0);
                break;
            case SystemMediaTransportControlsButton::SystemMediaTransportControlsButton_Next:
                SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, ID_NEXT, 0);
                break;
            }
        };
    auto SeekCallback = [](INT64 postion)
        {
            if (CPlayer::GetInstance().GetPlayStatusMutex().try_lock_for(std::chrono::milliseconds(1000)))
            {
                CPlayer::GetInstance().SeekTo(static_cast<int>(postion / 10000));
                CPlayer::GetInstance().GetPlayStatusMutex().unlock();
            }
        };
    auto SetSpeedCallback = [](DOUBLE rate)
        {
            CPlayer::GetInstance().SetSpeed(static_cast<float>(rate));
        };

    return pImpl->InitSMTC(AfxGetMainWnd()->GetSafeHwnd(), ButtonCallback, SeekCallback, SetSpeedCallback);
}

void MediaTransControls::loadThumbnail(wstring fn)
{
    auto i = fn.find(L"://");
    if (i != std::wstring::npos && i > 1)
        pImpl->loadThumbnailFromUrl(fn);
    else
        pImpl->loadThumbnailFromFile(fn);
}

void MediaTransControls::loadThumbnail(const BYTE* content, size_t size)
{
    pImpl->loadThumbnailFromBuff(content, size);
}

bool MediaTransControls::IsActive()
{
    return pImpl->IsActive();
}

void MediaTransControls::ClearAll()
{
    pImpl->ClearAll();
}

void MediaTransControls::UpdateControls(PlaybackStatus status)
{
    MediaPlaybackStatus status_media{};
    switch (status)
    {
    case PlaybackStatus::Closed: status_media = MediaPlaybackStatus::MediaPlaybackStatus_Closed; break;
    case PlaybackStatus::Changing: status_media = MediaPlaybackStatus::MediaPlaybackStatus_Changing; break;
    case PlaybackStatus::Stopped: status_media = MediaPlaybackStatus::MediaPlaybackStatus_Stopped; break;
    case PlaybackStatus::Playing: status_media = MediaPlaybackStatus::MediaPlaybackStatus_Playing; break;
    case PlaybackStatus::Paused: status_media = MediaPlaybackStatus::MediaPlaybackStatus_Paused; break;
    default: ASSERT(FALSE); break;
    }
    pImpl->UpdateControls(status_media);
}

void MediaTransControls::UpdateControlsMetadata(const SongInfo& song)
{
    const wstring& title = (!song.IsTitleEmpty() || song.file_path.empty()) ? song.GetTitle() : song.GetFileName();
    vector<wstring> genres(1, song.GetGenre());
    pImpl->UpdateControlsMetadata(title, song.GetAlbum(), song.album_artist, song.GetAlbum(), genres, song.track, song.total_tracks);
}

void MediaTransControls::UpdateDuration(int64_t duration)
{
    pImpl->UpdateDuration(duration);
}

void MediaTransControls::UpdatePosition(int64_t postion)
{
    pImpl->UpdatePosition(postion);
}

void MediaTransControls::UpdateSpeed(float speed)
{
    pImpl->UpdateSpeed(speed);
}

#else

MediaTransControls::MediaTransControls() {}
MediaTransControls::~MediaTransControls() {}
void MediaTransControls::SetEnabled(bool enable) {}
bool MediaTransControls::InitSMTC() { return false; }
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
