#include "stdafx.h"
#include "MediaTransControls.h"
#ifndef DISABLE_MEDIA_TRANS_CONTROLS

#include "MediaTransControlsImpl.h"

#include "player.h"
#include "WinVersionHelper.h"
#include "resource.h"

MediaTransControls::MediaTransControls()
{
}

MediaTransControls::~MediaTransControls()
{
}

bool MediaTransControls::InitSMTC(bool enable)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    pImpl.reset();
    if (!enable)
        return true;

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
    pImpl = std::make_unique<MediaTransControlsImpl>(AfxGetMainWnd()->GetSafeHwnd(), ButtonCallback, SeekCallback, SetSpeedCallback);
    return pImpl->IsActive();
}

void MediaTransControls::loadThumbnail(const wstring& fn)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!pImpl) return;
    auto i = fn.find(L"://");
    if (i != std::wstring::npos && i > 1)
        pImpl->loadThumbnailFromUrl(fn);
    else
        pImpl->loadThumbnailFromFile(fn);
}

void MediaTransControls::loadThumbnail(const BYTE* content, size_t size)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!pImpl) return;
    pImpl->loadThumbnailFromBuff(content, size);
}

bool MediaTransControls::IsActive()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!pImpl) return false;
    return pImpl->IsActive();
}

void MediaTransControls::ClearAll()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!pImpl) return;
    pImpl->ClearAll();
}

void MediaTransControls::UpdateControls(PlaybackStatus status)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!pImpl) return;
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
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!pImpl) return;
    const wstring& title = (!song.IsTitleEmpty() || song.file_path.empty()) ? song.GetTitle() : song.GetFileName();
    vector<wstring> genres(1, song.GetGenre());
    pImpl->UpdateControlsMetadata(title, song.GetAlbum(), song.album_artist, song.GetAlbum(), genres, song.track, song.total_tracks);
    pImpl->UpdateDuration(song.length().toInt());   // milliseconds
}

void MediaTransControls::UpdatePosition(int64_t postion, bool force)
{
    // 降低更新频率，每秒更新一次，在我的系统（win10使用ModernFlyouts）实际最高有效值大概是500ms一次
    static int64_t last{};
    if (last == postion / 1000 && !force)
        return;
    last = postion / 1000;
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!pImpl) return;
    pImpl->UpdatePosition(postion);
}

void MediaTransControls::UpdateSpeed(float speed)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!pImpl) return;
    pImpl->UpdateSpeed(speed);
}

#else

MediaTransControls::MediaTransControls() {}
MediaTransControls::~MediaTransControls() {}
void MediaTransControls::SetEnabled(bool enable) {}
bool MediaTransControls::InitSMTC(bool enable) { return false; }
void MediaTransControls::loadThumbnail(wstring fn) {}
void MediaTransControls::loadThumbnail(const BYTE* content, size_t size) {}
bool MediaTransControls::IsActive() { return false; }
void MediaTransControls::ClearAll() {}
void MediaTransControls::UpdateControls(PlaybackStatus status) {}
void MediaTransControls::UpdateControlsMetadata(const SongInfo& song) {}
void MediaTransControls::UpdatePosition(int64_t postion) {}
void MediaTransControls::UpdateSpeed(float speed) {}

#endif
