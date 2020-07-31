// Adapted from https://github.com/DanStevens/AudioEndPointController

#include "audioendpointcontroller.h"
#include <QDebug>

AudioEndpointController::AudioEndpointController()
{
    m_state->pDeviceFormatStr = nullptr;
    m_state->pDeviceFormatStr = _T(DEVICE_OUTPUT_FORMAT);
    m_state->deviceStateFilter = DEVICE_STATE_ACTIVE;
    m_state->option = 0;
    createDeviceEnumerator(m_state);
}

void AudioEndpointController::refreshList()
{
    allAudioDevices.clear();
    m_state->option = 0;
    createDeviceEnumerator(m_state);
}

void AudioEndpointController::onChangeRequest(QString device)
{
    m_requestedDevice = device;
    m_state->option = 0;

    //Option 0 iterates over all audio devices. If there is a device name matching to
    //requested device, createDeviceEnumerator sets m_state->option to corresponding index.
    createDeviceEnumerator(m_state);

    //If match was found, run again and this time the matching device will be set default.
    if(m_state->option != 0) {
        createDeviceEnumerator(m_state);
    }
}

// Create a multimedia device enumerator.
void AudioEndpointController::createDeviceEnumerator(TGlobalState* state)
{
    state->pEnum = NULL;
    state->hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
        (void**)&state->pEnum);
    if(SUCCEEDED(state->hr)) {
        prepareDeviceEnumerator(state);
    }
}

// Prepare the device enumerator
void AudioEndpointController::prepareDeviceEnumerator(TGlobalState* state)
{
    state->hr = state->pEnum->EnumAudioEndpoints(eRender, state->deviceStateFilter, &state->pDevices);
    if(SUCCEEDED(state->hr)) {
        enumerateOutputDevices(state);
    }
    state->pEnum->Release();
}

// Enumerate the output devices
void AudioEndpointController::enumerateOutputDevices(TGlobalState* state)
{
    UINT count;
    state->pDevices->GetCount(&count);

    // If option is less than 1, list devices
    if(state->option < 1) {
        // Get default device
        IMMDevice* pDefaultDevice;
        state->hr = state->pEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDefaultDevice);
        if(SUCCEEDED(state->hr)) {
            state->hr = pDefaultDevice->GetId(&state->strDefaultDeviceID);

            // Iterate all devices
            for (int i = 1; i <= (int)count; i++) {
                state->hr = state->pDevices->Item(i - 1, &state->pCurrentDevice);
                if(SUCCEEDED(state->hr)) {
                    state->hr = printDeviceInfo(state->pCurrentDevice, i);
                    state->pCurrentDevice->Release();
                }
            }
        }
    }
    // If option corresponds with the index of an audio device, set it to default
    else if (state->option <= (int)count) {
        state->hr = state->pDevices->Item(state->option - 1, &state->pCurrentDevice);
        if(SUCCEEDED(state->hr)) {
            LPWSTR strID = NULL;
            state->hr = state->pCurrentDevice->GetId(&strID);
            if (SUCCEEDED(state->hr)) {
                state->hr = SetDefaultAudioPlaybackDevice(strID);
            }
            state->pCurrentDevice->Release();
        }
    }
    // Otherwise inform user than option doesn't correspond with a device
    else {
        qDebug() << "No audio device with index" << state->option;
    }

    state->pDevices->Release();
}

std::wstring AudioEndpointController::getDeviceProperty(IPropertyStore* pStore, const PROPERTYKEY key)
{
    PROPVARIANT prop;
    PropVariantInit(&prop);
    HRESULT hr = pStore->GetValue(key, &prop);
    if(SUCCEEDED(hr)) {
        std::wstring result (prop.pwszVal);
        PropVariantClear(&prop);
        return result;
    } else {
        return std::wstring (L"");
    }
}

QVector<QString> AudioEndpointController::getAllAudioDevices()
{
    return allAudioDevices;
}

HRESULT AudioEndpointController::printDeviceInfo(IMMDevice* pDevice, int index)
{
    // Device ID
    LPWSTR strID = NULL;
    HRESULT hr = pDevice->GetId(&strID);
    if(!SUCCEEDED(hr)) {
        return hr;
    }
    // Device state
    DWORD dwState;
    hr = pDevice->GetState(&dwState);
    if(!SUCCEEDED(hr)) {
        return hr;
    }
    IPropertyStore *pStore;
    hr = pDevice->OpenPropertyStore(STGM_READ, &pStore);
    if(SUCCEEDED(hr)) {
        std::wstring friendlyName = getDeviceProperty(pStore, PKEY_Device_FriendlyName);
        std::wstring description = getDeviceProperty(pStore, PKEY_Device_DeviceDesc);
        std::wstring interfaceFriendlyName = getDeviceProperty(pStore, PKEY_DeviceInterface_FriendlyName);
        allAudioDevices.push_back(QString::fromStdWString(description));
        if(m_requestedDevice == QString::fromStdWString(description)) {
            m_state->option = index;
        }
        pStore->Release();
    }
    return hr;
}

HRESULT AudioEndpointController::SetDefaultAudioPlaybackDevice(LPCWSTR devID)
{
    IPolicyConfig *pPolicyConfig;
    ERole reserved = eConsole;

    HRESULT hr = CoCreateInstance(__uuidof(CPolicyConfigClient),
        NULL, CLSCTX_ALL, __uuidof(IPolicyConfig), (LPVOID *)&pPolicyConfig);
    if (SUCCEEDED(hr)) {
        hr = pPolicyConfig->SetDefaultEndpoint(devID, reserved);
        pPolicyConfig->Release();
    }
    return hr;
}
