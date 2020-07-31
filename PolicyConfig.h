#pragma once

// ----------------------------------------------------------------------------
// PolicyConfig.h
// Undocumented COM-interface IPolicyConfig.
// Use for set default audio render endpoint
// @author EreTIk
// ----------------------------------------------------------------------------

#include "windows.h"
#include "Mmdeviceapi.h"
#include "Propidl.h"
#include "Functiondiscoverykeys_devpkey.h"
#include <stdio.h>
#include <wchar.h>
#include <tchar.h>
#include <string>
#include <iostream>

interface DECLSPEC_UUID("f8679f50-850a-41cf-9c72-430f290290c8")
IPolicyConfig;
class DECLSPEC_UUID("870af99c-171d-4f9e-af0d-e63df40c2bc9")
CPolicyConfigClient;
// ----------------------------------------------------------------------------
// class CPolicyConfigClient
// {870af99c-171d-4f9e-af0d-e63df40c2bc9}
//
// interface IPolicyConfig
// {f8679f50-850a-41cf-9c72-430f290290c8}
//
// Query interface:
// CComPtr<IPolicyConfig> PolicyConfig;
// PolicyConfig.CoCreateInstance(__uuidof(CPolicyConfigClient));
//
// @compatible: Windows 7 and Later
// ----------------------------------------------------------------------------
interface IPolicyConfig : public IUnknown
{
public:

    virtual HRESULT GetMixFormat(
        PCWSTR,
        WAVEFORMATEX **
    );

    virtual HRESULT STDMETHODCALLTYPE GetDeviceFormat(
        PCWSTR,
        INT,
        WAVEFORMATEX **
    );

    virtual HRESULT STDMETHODCALLTYPE ResetDeviceFormat(
        PCWSTR
    );

    virtual HRESULT STDMETHODCALLTYPE SetDeviceFormat(
        PCWSTR,
        WAVEFORMATEX *,
        WAVEFORMATEX *
    );

    virtual HRESULT STDMETHODCALLTYPE GetProcessingPeriod(
        PCWSTR,
        INT,
        PINT64,
        PINT64
    );

    virtual HRESULT STDMETHODCALLTYPE SetProcessingPeriod(
        PCWSTR,
        PINT64
    );

    virtual HRESULT STDMETHODCALLTYPE GetShareMode(
        PCWSTR,
        struct DeviceShareMode *
    );

    virtual HRESULT STDMETHODCALLTYPE SetShareMode(
        PCWSTR,
        struct DeviceShareMode *
    );

    virtual HRESULT STDMETHODCALLTYPE GetPropertyValue(
        PCWSTR,
        const PROPERTYKEY &,
        PROPVARIANT *
    );

    virtual HRESULT STDMETHODCALLTYPE SetPropertyValue(
        PCWSTR,
        const PROPERTYKEY &,
        PROPVARIANT *
    );

    virtual HRESULT STDMETHODCALLTYPE SetDefaultEndpoint(
        __in PCWSTR wszDeviceId,
        __in ERole eRole
    );

    virtual HRESULT STDMETHODCALLTYPE SetEndpointVisibility(
        PCWSTR,
        INT
    );
};
