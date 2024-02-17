#pragma once
#include "SongInfo.h"

enum class PlaybackStatus
{
    Closed = 0,
    Changing = 1,
    Stopped = 2,
    Playing = 3,
    Paused = 4,
};

class MediaTransControls {
public:
    MediaTransControls();
    ~MediaTransControls();
    void SetEnabled(bool enable);
    bool Init();
    void loadThumbnail(wstring fn);
    void loadThumbnail(const BYTE* content, size_t size);
    void loadThumbnailFromUrl(wstring url);
    bool IsActive();
    void ClearAll();
    void UpdateControls(PlaybackStatus status);
    void UpdateControlsMetadata(const SongInfo& song);
    /// Update duration of stream, in milliseconds
    void UpdateDuration(int64_t duration);
    /// Update current time, in milliseconds
    void UpdatePosition(int64_t postion);
    /// Update current speed
    void UpdateSpeed(float speed);

#ifndef DISABLE_MEDIA_TRANS_CONTROLS
private:
    class MediaTransControlsImpl;
    std::unique_ptr<MediaTransControlsImpl> pImpl;
#endif
};
