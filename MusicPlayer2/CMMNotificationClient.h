#pragma once
#include <mmdeviceapi.h>
class CDevicesManager;
class CMMNotificationClient : public IMMNotificationClient
{
public:
	CMMNotificationClient(IMMDeviceEnumerator* _pEnum, CDevicesManager* manager);
	virtual ~CMMNotificationClient();
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();
	HRESULT STDMETHODCALLTYPE QueryInterface(
		REFIID riid, VOID** ppvInterface);
	HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(
		EDataFlow flow, ERole role,
		LPCWSTR pwstrDeviceId);
	HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR pwstrDeviceId);
	HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR pwstrDeviceId);
	HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(
		LPCWSTR pwstrDeviceId,
		DWORD dwNewState);
	HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(
		LPCWSTR pwstrDeviceId,
		const PROPERTYKEY key);
private:
	LONG _cRef;
	IMMDeviceEnumerator* _pEnum;
	CDevicesManager* manager;
};

