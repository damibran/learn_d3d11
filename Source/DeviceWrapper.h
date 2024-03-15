#pragma once

#include <assert.h>
#include <d3d11.h>
#include <winrt/base.h>

namespace dmbrn {
// TODO: add device selection routine
static inline class DeviceWrapper {
public:
    DeviceWrapper()
    {
        UINT createDeviceFlags = 0;
#ifndef NDEBUG
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // !NDEBUG
        D3D_FEATURE_LEVEL featureLevel;
        const D3D_FEATURE_LEVEL featureLevelArray[2] = {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_0,
        };
        HRESULT res = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &pd3dDevice, &featureLevel, &pd3dDeviceContext);
        if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
            res = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &pd3dDevice, &featureLevel, &pd3dDeviceContext);
        if (res != S_OK)
            assert(false);

        winrt::init_apartment();

    }
    ~DeviceWrapper()
    {
        if (pd3dDeviceContext) {
            pd3dDeviceContext->Release();
            pd3dDeviceContext = nullptr;
        }
        if (pd3dDevice) {
            pd3dDevice->Release();
            pd3dDevice = nullptr;
        }
    }

    ID3D11Device* getDevice()
    {
        return pd3dDevice;
    }

    ID3D11DeviceContext* getContext()
    {
        return pd3dDeviceContext;
    }

private:
    ID3D11Device* pd3dDevice = nullptr;
    ID3D11DeviceContext* pd3dDeviceContext = nullptr;
};
}