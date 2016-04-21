#pragma once

#include "Utilities/Size2.h"

namespace TinyStarCraft
{

/**
 *	Render system configuration
 */
struct RenderSystemConfig
{
    Size2d backbufferSize;
    bool isVSyncEnabled;
};


/**
 *	Render system takes the charge all of the game's rendering task. It manages the 
 *  render device, provides multiple rendering method and optimize them by batching
 *  draw calls. It also manages the mesh and shader resources associated with the 
 *  corresponding rendering task.
 *  The render system's functions is mainly called by the Scene object.
 */
class RenderSystem
{
public:
    /**
     *	Constrctor
     *
     *  @remarks
     *  Call RenderSystem::init to initialize the system.
     */
    RenderSystem();

    /**
     *	Destroy all resources and the render device.
     */
    ~RenderSystem();

    /**
     *	Initialize the render system.
     *
     *  @param hWnd
     *  Handle of the window for the render system to render on.
     *
     *  @param config 
     *  The initial configuration for this render system.
     *  
     *  @return
     *  Returns true if secceeded, otherwise return false.
     */
    bool init(HWND hWnd, const RenderSystemConfig& config);

    /**
     *	Begin to do rendering tasks.
     *
     *  @remarks
     *  Must be called before any render operations happens. End the rendering by
     *  calling RenderSystem::endScene.
     *
     *  @return
     *  True if no error encountered.
     */
    bool beginScene();

    /**
     *	End the last RenderSystem::beginScene call.
     *
     *  @return
     *  True if no error encountered.
     */
    bool endScene();

    static const int PRESENTERR_OK = 0;                 // No error encountered.
    static const int PRESENTERR_DEVICE_NOT_RESET = 1;   // The device is lost but can't be reset yet.
    static const int PRESENTERR_ERROR = 2;              // The device is lost and other error happened.

    /**
     *	Present the backbuffer to the screen.
     *  
     *  @remraks
     *  If the device needs a reset, this function will try to reset the device.
     *  The reset could be caused by device lost or render system reconfiguration.
     *
     *  @return
     *  Return value will be one of PRESENTERR_OK, PRESENTERR_DEVICE_NOT_RESET and PRESENTERR_ERROR.
     *  If PRESENTERR_DEVICE_NOT_RESET is returned then don't do any render operations and try calling
     *  present again at next frame. If PRESENTERR_ERROR is returned then there could be device fatal
     *  errors or resource creation failure so the render system can't be recoverd.
     */
    int present();

    /**
     *	Reset the render system with the new configration.
     *
     *  @remarks
     *  Won't make changes immediately since the reset operation will only happen when RenderSystem::preset
     *  is called next time.
     */
    void reset(const RenderSystemConfig& config);

    /**
     *	Get the current render configuration.
     */
    RenderSystemConfig getConfig() const;

private:
    /**
     *	Initialize d3d device.
     *
     *  @remarks
     *  Called by RenderSystem::init.  
     *
     *  @return
     *  Ture if secceeded, otherwise returns false.
     */
    bool _initD3dDevice(HWND hWnd, const RenderSystemConfig& config);

    /**
     *	Release gbuffers, dynamic textures and reset effect files.
     *  
     *  @return
     *  True if succeeded, otherwise return false.
     */
    bool _onDeviceLost();

    /**
     *	Recreate gbuffers, dynamic textures and reset effect files.
     *
     *  @return
     *  True if succeeded, otherwise return false.
     */
    bool _onDeviceReset();

private:
    IDirect3D9* mD3d;
    IDirect3DDevice9* mD3dDevice;
    D3DPRESENT_PARAMETERS mPresentParams;
    bool mDeviceNeedsReset;
};

};

