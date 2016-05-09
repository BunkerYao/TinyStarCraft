#pragma once

#include "Texture.h"
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
     *	Clear the backbuffer.
     */
    bool clear(DWORD count, const D3DRECT* rects, DWORD flags, D3DCOLOR color, float z, DWORD stencil);

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

    /**
     *	Set a render target.
     *
     *  @return
     *  True if succeeded, otherwise false.
     */
    bool setRenderTarget(DWORD index, Texture* renderTarget);

    /**
     *	Reset the 0th render targets to the original render target.
     *
     *  @return
     *  True if succeeded, otherwise false.
     */
    bool resetRenderTarget();

    /**
     *	Create a texture.
     *
     *  @param size
     *  Texture size.
     *
     *  @param mipLevels
     *  The number of mipmap levels.
     *
     *  @param format
     *  Format of the texture.
     *
     *  @param isRenderTarget
     *  Whether the texture could be used as a render target or not.
     *
     *  @remarks
     *  The render system will destroy the created texture resource when render device is destroyed.
     */
    Texture* createTexture(const Size2<UINT>& size, UINT mipLevels, D3DFORMAT format, bool isRenderTarget);

    /**
     *	Create a texture from file.
     *
     *  @param srcFilename
     *  The file name of the image file to load.
     *
     *  @param size
     *  Prefered texture size.
     *  
     *  @param mipLevels
     *  The number of mipmap levels.
     *
     *  @param format
     *  Format of the texture.
     *  
     *  @param isRenderTarget
     *  Whether the texture could be used as a render target or not.
     *
     *  @remarks
     *  The render system will destroy the created texture resource when render device is destroyed.
     */
    Texture* createTextureFromFile(const std::string& srcFilename, const Size2<UINT>& size, UINT mipLevels, D3DFORMAT format,
        bool isRenderTarget);

    /**
     *	Destroy a texture created by this render system.
     */
    void destroyTexture(Texture* texture);

    /**
     *	Create a mesh.
     *
     *  @param numVertices
     *  The number of vertices in the mesh.
     *
     *  @param numFaces
     *  The number of faces in the mesh.
     *
     *  @param declaration
     *  Vertex declaration.
     *
     *  @remarks
     *  The render system will destroy the created mesh resource when render device is released.
     */
    ID3DXMesh* createMesh(DWORD numVertices, DWORD numFaces, const D3DVERTEXELEMENT9* declaration);

    /**
     *	Destroy a mesh created by this render system.
     */
    void destroyMesh(ID3DXMesh* mesh);

    /**
     *	Create an effect from file.
     *
     *  @param srcFilename
     *  File name of the effect source file.
     *
     *  @param flags
     *  Creation flags.
     *  
     *  @return
     *  Return NULL if failed.
     */
    ID3DXEffect* createEffectFromFile(const std::string& srcFilename, DWORD flags);

    /**
     *	Destroy an effect created by this render system.
     */
    void destroyEffect(ID3DXEffect* effect);

    /**
     *	Create a sprite.
     *
     *  @return
     *  Returns NULL if failed.
     */
    ID3DXSprite* createSprite();

    /**
     *	Destroy an sprite created by this render system.
     */
    void destroySprite(ID3DXSprite* sprite);

    /**
     *	Create a font.
     *
     *  @return
     *  Returns NULL if failed.
     */
    ID3DXFont* createFont(int height, UINT width, UINT weight, UINT mipLevels, BOOL italic, DWORD charSet, DWORD outputPrecision,
        DWORD quality, DWORD pitchAndFamily, const std::string& faceName);

    /**
     *	Destroy a font created by this render system.
     */
    void destroyFont(ID3DXFont* font);

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

    /**
     *	Release render targets.
     *  This should be called before device is about to reset.
     */
    void _releaseRenderTargets();

    /**
     *	Release render targets.
     *  This should be called after device is reseted.
     */
    bool _recreateRenderTargets();

    void _handleEffectsOnDeviceLost();

    void _handleEffectsOnDeviceReset();

    /** Create a texture. */
    IDirect3DTexture9* _createTextureImpl(const Size2<UINT>& size, UINT mipLevels, D3DFORMAT format, bool isRenderTarget);

    /** Create a texture from file. */
    IDirect3DTexture9* _createTextureFromFileImpl(const std::string& srcFilename, const Size2<UINT>& size, UINT mipLevels, 
        D3DFORMAT format, bool isRenderTarget);

    /** Create a mesh. */
    ID3DXMesh* _createMeshImpl(DWORD numVertices, DWORD numFaces, const D3DVERTEXELEMENT9* declaration);

    /** Create effect from file. */
    ID3DXEffect* _createEffectFromFileImpl(const std::string& srcFilename, DWORD flags);

    /** Create sprite. */
    ID3DXSprite* _createSpriteImpl();

    /** Create font. */
    ID3DXFont* _createFontImpl(int height, UINT width, UINT weight, UINT mipLevels, BOOL italic, DWORD charSet, DWORD outputPrecision,
        DWORD quality, DWORD pitchAndFamily, LPCSTR faceName);

private:
    IDirect3D9* mD3d;
    IDirect3DDevice9* mD3dDevice;
    D3DPRESENT_PARAMETERS mPresentParams;
    bool mDeviceNeedsReset;

    // This is the render target automatically created by the device.
    IDirect3DSurface9* mOriginalRenderTarget;

    // Gbuffers array.
    // 0 - A8R8G8B8 albedo in RGB channels, emissive in alpha channel.
    // 1 - X8R8G8B8 world space normal
    // 2 - R16F     world space height map
    IDirect3DTexture9* mGbuffers[3];

    struct TextureCreationDesc
    {
        Size2<UINT> size;
        UINT mipLevels;
        D3DFORMAT format;
        bool isRenderTarget;
        std::string srcFilename; // Should be empty if the texture is not created from file.

        TextureCreationDesc(const Size2<UINT>& size, UINT mipLevels, D3DFORMAT format, bool isRenderTarget, 
            const std::string& srcFilename)
            : size(size), mipLevels(mipLevels), format(format), isRenderTarget(isRenderTarget), srcFilename(srcFilename)
        {}
    };

    // Textures created by this render system.
    // Since the render targets should be recreated when device is reset, So save a texture description
    // for each texture to keep its creation settings.
    std::list<std::pair<Texture*, TextureCreationDesc>> mTextures;
    // Meshes 
    std::list<ID3DXMesh*> mMeshes;
    // Effects
    std::list<ID3DXEffect*> mEffects;
    // Sprites
    std::list<ID3DXSprite*> mSprites;
    // Fonts
    std::list<ID3DXFont*> mFonts;
};

};

