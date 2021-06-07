#include "stdafx.h"
#include "CMMNotificationClient.h"
#include "CDevicesManager.h"


CMMNotificationClient::CMMNotificationClient(IMMDeviceEnumerator* _pEnum, CDevicesManager* manager) :_cRef(1),
_pEnum(_pEnum), manager(manager)
{

}

CMMNotificationClient::~CMMNotificationClient()
{
    //SAFE_RELEASE(_pEnum);
}

ULONG STDMETHODCALLTYPE CMMNotificationClient::AddRef()
{
    return InterlockedIncrement(&_cRef);
}

ULONG STDMETHODCALLTYPE CMMNotificationClient::Release()
{
    ULONG ulRef = InterlockedDecrement(&_cRef);
    if (0 == ulRef)
    {
        delete this;
    }
    return ulRef;
}

HRESULT STDMETHODCALLTYPE CMMNotificationClient::QueryInterface(
    REFIID riid, VOID** ppvInterface)
{
    if (IID_IUnknown == riid)
    {
        AddRef();
        *ppvInterface = (IUnknown*)this;
    }
    else if (__uuidof(IMMNotificationClient) == riid)
    {
        AddRef();
        *ppvInterface = (IMMNotificationClient*)this;
    }
    else
    {
        *ppvInterface = NULL;
        return E_NOINTERFACE;
    }
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CMMNotificationClient::OnDefaultDeviceChanged(
    EDataFlow flow, ERole role,
    LPCWSTR pwstrDeviceId)
{
    char* pszFlow = "?????";
    char* pszRole = "?????";

    switch (flow)
    {
    case eRender:
        pszFlow = "eRender";
        break;
    case eCapture:
        pszFlow = "eCapture";
        break;
    }

    switch (role)
    {
    case eConsole:
        pszRole = "eConsole";
        break;
    case eMultimedia:
        pszRole = "eMultimedia";
        break;
    case eCommunications:
        pszRole = "eCommunications";
        break;
    }

    TRACE("  -->New default device: flow = %s, role = %s\n",
        pszFlow, pszRole);

    if (flow == eRender && role == eMultimedia)
        manager->DefaultMultimediaDeviceChanged();

    return S_OK;
}
HRESULT STDMETHODCALLTYPE CMMNotificationClient::OnDeviceAdded(LPCWSTR pwstrDeviceId)
{
    TRACE("  -->Added device\n");
    //manager->notifyDeviceAddedListeners(pwstrDeviceId);

    return S_OK;
};

HRESULT STDMETHODCALLTYPE CMMNotificationClient::OnDeviceRemoved(LPCWSTR pwstrDeviceId)
{
    TRACE("  -->Removed device");
    //manager->notifyDeviceRemovedListeners(pwstrDeviceId);

    return S_OK;
}
HRESULT STDMETHODCALLTYPE CMMNotificationClient::OnDeviceStateChanged(
    LPCWSTR pwstrDeviceId,
    DWORD dwNewState)
{
    char* pszState = "?????";

    switch (dwNewState)
    {
    case DEVICE_STATE_ACTIVE:
        pszState = "ACTIVE";
        //manager->notifyDeviceAddedListeners(pwstrDeviceId);
        break;
    case DEVICE_STATE_DISABLED:
        pszState = "DISABLED";
        //manager->notifyDeviceRemovedListeners(pwstrDeviceId);
        break;
    case DEVICE_STATE_NOTPRESENT:
        pszState = "NOTPRESENT";
        //manager->notifyDeviceRemovedListeners(pwstrDeviceId);
        break;
    case DEVICE_STATE_UNPLUGGED:
        pszState = "UNPLUGGED";
        //manager->notifyDeviceRemovedListeners(pwstrDeviceId);
        break;
    }

    TRACE("  -->New device state is DEVICE_STATE_%s (0x%8.8x)\n",
        pszState, dwNewState);

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CMMNotificationClient::OnPropertyValueChanged(
    LPCWSTR pwstrDeviceId,
    const PROPERTYKEY key)
{
    //TRACE("  -->Changed device property "
    //    "{%8.8x-%4.4x-%4.4x-%2.2x%2.2x-%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x}#%d\n",
    //    key.fmtid.Data1, key.fmtid.Data2, key.fmtid.Data3,
    //    key.fmtid.Data4[0], key.fmtid.Data4[1],
    //    key.fmtid.Data4[2], key.fmtid.Data4[3],
    //    key.fmtid.Data4[4], key.fmtid.Data4[5],
    //    key.fmtid.Data4[6], key.fmtid.Data4[7],
    //    key.pid);
    return S_OK;
}