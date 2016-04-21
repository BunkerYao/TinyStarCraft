#include "Precompiled.h"
#include "RenderSystem.h"

namespace TinyStarCraft
{

RenderSystem::RenderSystem()
    : mD3d(NULL),
      mD3dDevice(NULL),
      mDeviceNeedsReset(false)
{
}

RenderSystem::~RenderSystem()
{
    if (mD3dDevice) {
        ULONG number = mD3dDevice->Release();
        printf("IDirect3DDevice::Release returned with number %d\n", number);
    }

    if (mD3d) {
        ULONG number = mD3d->Release();
        printf("IDirect3D9::Release returned with number %d\n", number);
    }
}

bool RenderSystem::init(HWND hWnd, const RenderSystemConfig& config)
{
    // Initialize the d3d device.
    if (!_initD3dDevice(hWnd, config))
        return false;



    return true;
}

bool RenderSystem::beginScene()
{
    // Call the device's beginScene function.
    HRESULT hr = mD3dDevice->BeginScene();

    if (FAILED(hr)) {
        printf("IDirect3DDevice::BeginScene failed with result 0x%08x %s\n", hr, ::DXGetErrorString(hr));
        return false;
    }

    return true;
}

bool RenderSystem::endScene()
{
    // Call the device's endScene function.
    HRESULT hr = mD3dDevice->EndScene();

    if (FAILED(hr)) {
        printf("IDirect3DDevice::EndScene failed with result 0x%08x %s\n", hr, ::DXGetErrorString(hr));
        return false;
    }

    return true;
}

int RenderSystem::present()
{
    HRESULT hr = 0;

    // If the device is already lost, then check if it can be reset now and
    // try to reset.
    if (mDeviceNeedsReset) {
        hr = mD3dDevice->TestCooperativeLevel();

        if (hr == D3DERR_DEVICENOTRESET) {
            // The device can be reset now.
            // Release all device-dependent resources first.
            if (!_onDeviceLost())
                return PRESENTERR_ERROR;

            // Reset the device with the current present parameters.
            hr = mD3dDevice->Reset(&mPresentParams);

            if (FAILED(hr)) {
                // Reset still failed.
                printf("IDirect3DDevice::Reset failed with result 0x%08x %s\n", hr, ::DXGetErrorString(hr));
                return PRESENTERR_ERROR;
            }

            printf("Device reset.\n");

            // Device has been reset, recreate device-depent resources.
            if (!_onDeviceReset())
                return PRESENTERR_ERROR;

            mDeviceNeedsReset = false;
        }
        else if (hr == D3DERR_DEVICELOST) {
            // Can't be reset yet.
            return PRESENTERR_DEVICE_NOT_RESET;
        }
        else {
            // Other error happened.
            printf("IDirect3DDevice::TestCooperativeLevel returned with result 0x%08x %s\n", hr, ::DXGetErrorString(hr));
            return PRESENTERR_ERROR;
        }
    }

    hr = mD3dDevice->Present(NULL, NULL, NULL, NULL);

    if (FAILED(hr)) {
        if (D3DERR_DEVICELOST == hr) {
            // The device is lost
            printf("Device lost.\n");

            mDeviceNeedsReset = true;

            if (!_onDeviceLost())
                return PRESENTERR_ERROR;    
        }
        else {
            // Other error happens.
            printf("IDirect3DDevice::Present returned with result 0x%08x %s\n", hr, ::DXGetErrorString(hr));
            return PRESENTERR_ERROR;
        }
    }

    return PRESENTERR_OK;
}

void RenderSystem::reset(const RenderSystemConfig& config)
{
    // Modify the current present parameters.
    mPresentParams.BackBufferHeight = config.backbufferSize.y;
    mPresentParams.BackBufferWidth = config.backbufferSize.x;
    mPresentParams.PresentationInterval = config.isVSyncEnabled ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;

    // Set the flag so the device will be reset when present is called next time.
    mDeviceNeedsReset = true;
}

RenderSystemConfig RenderSystem::getConfig() const
{
    RenderSystemConfig config;
    config.backbufferSize.x = mPresentParams.BackBufferWidth;
    config.backbufferSize.y = mPresentParams.BackBufferHeight;
    config.isVSyncEnabled = (mPresentParams.PresentationInterval == D3DPRESENT_INTERVAL_DEFAULT);
    return config;
}

bool RenderSystem::_initD3dDevice(HWND hWnd, const RenderSystemConfig& config)
{
    // Create D3D object
    mD3d = ::Direct3DCreate9(D3D_SDK_VERSION);

    if (!mD3d) {
        printf("Direct3DCreate9 failed.\n");
        return false;
    }
    else {
        printf("Direct3DCreate9 succeeded.\n");
    }

    // Populate the present parameters struct.
    mPresentParams.AutoDepthStencilFormat = D3DFORMAT::D3DFMT_D24S8;
    mPresentParams.BackBufferCount = 1;
    mPresentParams.BackBufferFormat = D3DFORMAT::D3DFMT_X8R8G8B8;
    mPresentParams.BackBufferHeight = config.backbufferSize.y;
    mPresentParams.BackBufferWidth = config.backbufferSize.x;
    mPresentParams.EnableAutoDepthStencil = TRUE;
    mPresentParams.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
    mPresentParams.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    mPresentParams.hDeviceWindow = hWnd;
    mPresentParams.MultiSampleQuality = 0;
    mPresentParams.MultiSampleType = D3DMULTISAMPLE_NONE;
    mPresentParams.PresentationInterval = config.isVSyncEnabled ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;
    mPresentParams.SwapEffect = D3DSWAPEFFECT::D3DSWAPEFFECT_DISCARD;

// Use windowed mode in debug mode.
#ifdef _DEBUG
    mPresentParams.Windowed = TRUE;
#else
    mPresentParams.Windowed = FALSE;
#endif

    // Create the device.
    HRESULT hr = 0;
    hr = mD3d->CreateDevice(0, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &mPresentParams, &mD3dDevice);

    printf("IDirect3D9::CreateDevice 0x%08x %s\n", hr, ::DXGetErrorString(hr));

    if (FAILED(hr)) 
        return false;

    return true;
}

bool RenderSystem::_onDeviceLost()
{
    // TODO:

    return true;
}

bool RenderSystem::_onDeviceReset()
{
    // TODO:

    return true;
}

}