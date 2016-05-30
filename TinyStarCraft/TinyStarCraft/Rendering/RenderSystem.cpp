#include "Precompiled.h"
#include "RenderSystem.h"
#include "Asset/EffectManager.h"
#include "Asset/MaterialManager.h"
#include "Asset/TextureManager.h"
#include "Utilities/Assert.h"
#include "Utilities/Logging.h"

namespace TinyStarCraft
{

RenderSystem::RenderSystem(RenderSystemListener* listener)
    : mD3d(nullptr),
      mD3DDevice(nullptr),
      mIsDeviceLost(false),
      mDefaultRenderTarget(nullptr),
      mMaterialManager(nullptr),
      mEffectManager(nullptr),
      mTextureManager(nullptr),
      mListener(listener)
{
}

RenderSystem::~RenderSystem()
{
    ULONG releaseNumber = 0;

    delete mMaterialManager;
    delete mEffectManager;
    delete mTextureManager;

    if (mDefaultRenderTarget) 
    {
        // Release the default render target
        releaseNumber = mDefaultRenderTarget->Release();
        TINYSC_ASSERT(releaseNumber == 0, "Default render target released unsuccessfully.");
    }

    if (mD3DDevice) 
    {
        // Release the rendering device
        releaseNumber = mD3DDevice->Release();
        TINYSC_LOGLINE_INFO("IDirect3DDevice::Release %d", releaseNumber);
    }

    if (mD3d) 
    {
        // Release D3D object
        releaseNumber = mD3d->Release();
        TINYSC_LOGLINE_INFO("IDirect3D9::Release %d", releaseNumber);
    }
}

bool RenderSystem::initialize(HWND hWnd, const RenderSystemConfig& config)
{
    TINYSC_LOGLINE_INFO("RenderSystemConfig: BackbufferSize(%d*%d), VSync(%d)", 
        config.backbufferSize.x, config.backbufferSize.y, config.isVSyncEnabled);

    // Create D3D object
    mD3d = ::Direct3DCreate9(D3D_SDK_VERSION);
    TINYSC_LOGLINE_INFO("::Direct3DCreate9 %p", (void*)mD3d);
    if (!mD3d) 
    {
        TINYSC_LOGLINE_ERR("Failed to create IDirect3D9 interface.");
        return false;
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
    mPresentParams.Windowed = config.isWindowed;

    // Create the device.
    HRESULT hr = 0;
    hr = mD3d->CreateDevice(0, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &mPresentParams, &mD3DDevice);
    TINYSC_LOGLINE_INFO("IDirect3D9::CreateDevice 0x%08x %s", hr, ::DXGetErrorString(hr));
    if (FAILED(hr)) 
    {
        TINYSC_LOGLINE_ERR("Failed to create device.");
        return false;
    }

    // Obtain the default render target.
    hr = mD3DDevice->GetRenderTarget(0, &mDefaultRenderTarget);
    TINYSC_ASSERT(SUCCEEDED(hr), "Failed to obtain default render target.");

    // Create effect manager
    mEffectManager = new EffectManager(mD3DDevice);
    if (!mEffectManager->initialize())
    {
        TINYSC_LOGLINE_ERR("Failed to initialize effect manager.");
        return false;
    }

    // Create material manager
    mMaterialManager = new MaterialManager(mEffectManager);

    // Create texture manager
    mTextureManager = new TextureManager(mD3DDevice);
    if (!mTextureManager->initialize())
    {
        TINYSC_LOGLINE_ERR("Failed to initialize texture manager.");
        return false;
    }

    return true;
}

int RenderSystem::present()
{
    HRESULT hr = 0;

    if (mIsDeviceLost) 
    {
        // If the device is already lost, then check if it can be reset now and
        // try to reset.
        hr = mD3DDevice->TestCooperativeLevel();
        TINYSC_LOGLINE_INFO("IDirect3DDevice::TestCooperativeLevel 0x%08x %s", hr, ::DXGetErrorString(hr));

        if (hr == D3DERR_DEVICENOTRESET) 
        {
            // The device can be reset now.
            if (!_resetImpl())
                // Reset failed
                return PRESENTERR_ERROR;

            mIsDeviceLost = false;
        }
        else if (hr == D3DERR_DEVICELOST) 
        {
            // Can't be reset yet.
            return PRESENTERR_DEVICE_NOT_RESET;
        }
        else 
        {
            // Other error happened.
            return PRESENTERR_ERROR;
        }
    }

    hr = mD3DDevice->Present(NULL, NULL, NULL, NULL);

    if (FAILED(hr)) 
    {
        // Present failed
        TINYSC_LOGLINE_INFO("IDirect3DDevice::Present failed 0x%08x %s", hr, ::DXGetErrorString(hr));

        if (D3DERR_DEVICELOST == hr) 
            // The device is lost, set the flag and try to restore next time
            mIsDeviceLost = true;
        else 
            // Other error happened.
            return PRESENTERR_ERROR;
    }

    return PRESENTERR_OK;
}

bool RenderSystem::resetDefaultRenderTarget()
{
    HRESULT hr = mD3DDevice->SetRenderTarget(0, mDefaultRenderTarget);

    if (FAILED(hr))
        TINYSC_LOGLINE_ERR("Faild to set render target 0x%08x %s.", hr, ::DXGetErrorString(hr));

    return SUCCEEDED(hr);
}

bool RenderSystem::reset(const RenderSystemConfig& config)
{
    // Modify the current present parameters.
    mPresentParams.BackBufferHeight = config.backbufferSize.y;
    mPresentParams.BackBufferWidth = config.backbufferSize.x;
    mPresentParams.PresentationInterval = config.isVSyncEnabled ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;
    mPresentParams.Windowed = config.isWindowed;

    // Reset the device
    return _resetImpl();
}

RenderSystemConfig RenderSystem::getConfig() const
{
    RenderSystemConfig config;
    config.backbufferSize.x = mPresentParams.BackBufferWidth;
    config.backbufferSize.y = mPresentParams.BackBufferHeight;
    config.isVSyncEnabled = (mPresentParams.PresentationInterval == D3DPRESENT_INTERVAL_DEFAULT);
    config.isWindowed = (mPresentParams.Windowed == TRUE); 
    return config;
}

bool RenderSystem::_resetImpl()
{
    _onDeviceLost();

    // Reset the device with the current present parameters.
    HRESULT hr = mD3DDevice->Reset(&mPresentParams);
    TINYSC_LOGLINE_INFO("IDirect3DDevice::Reset 0x%08x %s", hr, ::DXGetErrorString(hr));

    if (FAILED(hr))
    {
        // Reset failed.
        TINYSC_LOGLINE_ERR("Device reset failed.");
        return false;
    }

    // Device has been reset
    //

    if (!_onDeviceReset())
    {
        TINYSC_LOGLINE_ERR("_onDeviceReset returns false.");
        return false;
    }
    
    return true;
}

void RenderSystem::_onDeviceLost()
{
    if (mDefaultRenderTarget)
    {
        // Release the default render target.
        ULONG releaseNumber = mDefaultRenderTarget->Release();
        TINYSC_ASSERT(releaseNumber == 0, "Default render target released unsuccessfully.");
    }

    // Unload all render targets in texture manager
    mTextureManager->unloadRenderTargets();

    mEffectManager->onDeviceLost();

    // Call the listener to notify device lost.
    mListener->onDeviceLost();
}

bool RenderSystem::_onDeviceReset()
{
    // Obtain the default render target
    HRESULT hr = mD3DDevice->GetRenderTarget(0, &mDefaultRenderTarget);
    TINYSC_ASSERT(SUCCEEDED(hr), "Failed to obtain default render target.");

    // Reload all render targets
    if (!mTextureManager->reloadRenderTargets())
    {
        TINYSC_LOGLINE_ERR("Failed to reload texture manager render targets.")
        return false;
    }

    mEffectManager->onDeviceReset();

    // Restore textures on materials
    mMaterialManager->restoreMaterialTextures();

    // Call the listener to notify device reset.
    mListener->onDeviceReset();

    return true;
}

}