#pragma once

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

