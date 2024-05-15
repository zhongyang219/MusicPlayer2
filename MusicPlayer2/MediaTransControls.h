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

class MediaTransControlsImpl;

class MediaTransControls {
public:
    MediaTransControls();
    ~MediaTransControls();
    bool InitSMTC(bool enable);
    void loadThumbnail(const wstring& fn);
    void loadThumbnail(const BYTE* content, size_t size);
    bool IsActive();
    void ClearAll();
    void UpdateControls(PlaybackStatus status);
    void UpdateControlsMetadata(const SongInfo& song);
    /// Update current time, in milliseconds
    void UpdatePosition(int64_t postion, bool force = false);
    /// Update current speed
    void UpdateSpeed(float speed);

#ifndef DISABLE_MEDIA_TRANS_CONTROLS
private:
    std::mutex m_mutex;     // 使用pImpl指针期间锁定此互斥量
    std::unique_ptr<MediaTransControlsImpl> pImpl;
#endif
};
