#pragma once

// 用户改变默认多媒体播放设备时发送
#define WM_DEFAULT_MULTIMEDIA_DEVICE_CHANGED (WM_USER+138)

class CMMNotificationClient;
struct IMMDeviceEnumerator;
struct IMMDevice;

class CDevicesManager
{
public:
    CDevicesManager(void);
    ~CDevicesManager(void);
    HRESULT InitializeDeviceEnumerator();
    void ReleaseDeviceEnumerator();

    void DefaultMultimediaDeviceChanged();

    friend class CMMNotificationClient;
private:
    IMMDeviceEnumerator* pEnum;
    CMMNotificationClient* client;
};

