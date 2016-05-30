#pragma once

#include "Utilities/Size2.h"

namespace TinyStarCraft
{

/**
  	Render system configuration
 */
struct RenderSystemConfig
{
    Size2d backbufferSize;
    bool isVSyncEnabled;
    bool isWindowed;

    RenderSystemConfig() = default;

    RenderSystemConfig(const Size2d backbufferSize, bool isVSyncEnabled, bool isWindowed)
        : backbufferSize(backbufferSize), isVSyncEnabled(isVSyncEnabled), isWindowed(isWindowed)
    {}
};


/**
 	Inherit from this class to receive device lost/reset event.
 */
class RenderSystemListener
{
public:
    virtual ~RenderSystemListener() {}

    virtual void onDeviceLost() {}
    virtual void onDeviceReset() {}
};


class EffectManager;
class MaterialManager;
class TextureManager;


/**
    Manages D3D9 device.
@remarks
  	Render system manages D3D9 render device creation and destroy, it also detect
    device lost and restore the device from a lost state. When device is lost/reset,
    it will notify the registered listener.
 */
class RenderSystem
{
public:
    /**
      	Constrctor
    @remarks
        Call RenderSystem::init to initialize the render device.
     */
    explicit RenderSystem(RenderSystemListener* listener);

    /**
      	Destructor
    @remarks
        Destroy the render device. All D3D9 resources should be released before 
        destruct the render system.
     */
    ~RenderSystem();

    /** Get the render device interface. */
    IDirect3DDevice9* getD3DDevice() const { return mD3DDevice; }

    /**
      	Initialize the rendering device.
    @param hWnd
        Handle of the render window. 
    @param config 
        Initial configuration.
    @return
        Returns true if secceeded, otherwise returns false.
     */
    bool initialize(HWND hWnd, const RenderSystemConfig& config);

    // No error is encounted.
    static const int PRESENTERR_OK = 0;
    // The device is in lost state.
    static const int PRESENTERR_DEVICE_NOT_RESET = 1;  
    // The device is lost and can't be restored.
    static const int PRESENTERR_ERROR = 2;              

    /**
      	Present the backbuffer to the screen.
    @remraks
        If device is lost, the method first try to restore the device. After the device is reset,
        the backbuffer will be presented. Present might be failed because of a lost device.
    @return
        Return value will be one of PRESENTERR_OK, PRESENTERR_DEVICE_NOT_RESET and PRESENTERR_ERROR.
        If PRESENTERR_DEVICE_NOT_RESET is returned then don't do any render operations and try calling
        present again at next frame. If PRESENTERR_ERROR is returned then there could be non-recoverable 
        device error so the application should quit.
     */
    int present();

    /** Retrieve the default render target */
    IDirect3DSurface9* getDefaultRenderTarget() const { return mDefaultRenderTarget; }

    /** Set the default render target to the 0th render target. */
    bool resetDefaultRenderTarget();

    /**
      	Reset the render system with a new configration.
     */
    bool reset(const RenderSystemConfig& config);

    /**	Get the current configuration. */
    RenderSystemConfig getConfig() const;

    /** Get the effect manager */
    EffectManager* getEffectManager() const { return mEffectManager; }

    /** Get the material manager */
    MaterialManager* getMaterialManager() const { return mMaterialManager; }

    /** Get the texture manager */
    TextureManager* getTextureManager() const { return mTextureManager; }

private:

    /** Reset the rendering device with the current present parameters. */
    bool _resetImpl();

    /** 
        Get called after device is lost 
    @remarks
        Releases the default rendertarget, unload all rendertarget textures in texturemanager,
        notify all effects in effectmanager and fire the lost event.
     */
    void _onDeviceLost();

    /**
        Get called after device is reset
    @remarks
        Obtain the default rendertarget, reload all rendertarget textures in texturemanager,
        notify all effects in effectmanager and fire the reset event.
    */
    bool _onDeviceReset();

private:
    IDirect3D9* mD3d;
    IDirect3DDevice9* mD3DDevice;

    // The rendertarget automatically created by device. 
    IDirect3DSurface9* mDefaultRenderTarget;

    // Current present parameters
    D3DPRESENT_PARAMETERS mPresentParams;

    // A flag to indicate if a device is in lost state.
    bool mIsDeviceLost;

    EffectManager* mEffectManager;
    MaterialManager* mMaterialManager;
    TextureManager* mTextureManager;

    // Registerded listener to receive device lost/reset event.
    RenderSystemListener* mListener;
};

};

