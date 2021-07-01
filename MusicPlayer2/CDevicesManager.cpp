/******************************************************************************
* This file is part of DefaultAudioChanger.
* Copyright (c) 2011 Sergiu Giurgiu
*
* DefaultAudioChanger is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* DefaultAudioChanger is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with DefaultAudioChanger.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "CDevicesManager.h"
#include "CMMNotificationClient.h"

#define SAFE_RELEASE(punk)  \
              if ((punk) != nullptr)  \
                { (punk)->Release(); (punk) = nullptr; }

CDevicesManager::CDevicesManager(void) :pEnum(nullptr), client(nullptr)
{

}


CDevicesManager::~CDevicesManager(void)
{
    ReleaseDeviceEnumerator();
    delete client;
}

HRESULT CDevicesManager::InitializeDeviceEnumerator()
{
    HRESULT hr = S_OK;
    if (!pEnum)
    {
        hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,
            CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnum);
        client = new CMMNotificationClient(pEnum, this);
        pEnum->RegisterEndpointNotificationCallback(client);
    }
    return hr;
}


void CDevicesManager::ReleaseDeviceEnumerator()
{
    if (pEnum) {
        pEnum->UnregisterEndpointNotificationCallback(client);
    }
    SAFE_RELEASE(pEnum)
}

void CDevicesManager::DefaultMultimediaDeviceChanged()
{
    TRACE("PostMessage: WM_DEFAULT_MULTIMEDIA_DEVICE_CHANGED\n");
    PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_DEFAULT_MULTIMEDIA_DEVICE_CHANGED, 0, 0);
}

