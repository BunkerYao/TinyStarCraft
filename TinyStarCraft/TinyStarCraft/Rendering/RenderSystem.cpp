#include "Precompiled.h"
#include "RenderSystem.h"
#include "Utilities/Assert.h"
#include "Utilities/Logging.h"

namespace TinyStarCraft
{

RenderSystem::RenderSystem()
    : mD3d(NULL),
      mD3dDevice(NULL),
      mDeviceNeedsReset(false),
      mGbuffers{NULL, NULL, NULL}
{
}

RenderSystem::~RenderSystem()
{
    // Release the original render target.
    ULONG releaseNumber = mOriginalRenderTarget->Release();
    if (releaseNumber > 0)
        TINYSC_LOGLINE_INFO("Release original render target unsuccessfully %d", releaseNumber);

    // Destroy all textures.
    if (!mTextures.empty()) {
        for (auto& it : mTextures)
            delete it.first;

        mTextures.clear();
    }

    // Destroy all meshes.
    if (!mMeshes.empty()) {
        for (auto& it : mMeshes)
            it->Release();

        mMeshes.clear();
    }

    // Destroy all effects.
    if (!mEffects.empty()) {
        for (auto& it : mEffects)
            it->Release();

        mEffects.clear();
    }

    // Destroy all sprites.
    if (!mSprites.empty()) {
        for (auto& it : mSprites)
            it->Release();

        mSprites.clear();
    }

    // Destroy all fonts.
    if (!mFonts.empty()) {
        for (auto& it : mFonts)
            it->Release();

        mFonts.clear();
    }

    if (mD3dDevice) {
        releaseNumber = mD3dDevice->Release();
        TINYSC_LOGLINE_INFO("IDirect3DDevice::Release %d", releaseNumber);
    }

    if (mD3d) {
        releaseNumber = mD3d->Release();
        TINYSC_LOGLINE_INFO("IDirect3D9::Release %d", releaseNumber);
    }
}

bool RenderSystem::init(HWND hWnd, const RenderSystemConfig& config)
{
    // Initialize the d3d device.
    return _initD3dDevice(hWnd, config);
}

bool RenderSystem::clear(DWORD count, const D3DRECT* rects, DWORD flags, D3DCOLOR color, float z, DWORD stencil)
{
    HRESULT hr = mD3dDevice->Clear(count, rects, flags, color, z, stencil);
    if (FAILED(hr))
        TINYSC_LOGLINE_ERR("IDirect3DDevice9::Clear failed 0x%08x %s", hr, ::DXGetErrorString(hr));

    return SUCCEEDED(hr);
}

bool RenderSystem::beginScene()
{
    // Call the device's beginScene function.
    HRESULT hr = mD3dDevice->BeginScene();

    if (FAILED(hr)) {
        TINYSC_LOGLINE_ERR("IDirect3DDevice::BeginScene failed 0x%08x %s", hr, ::DXGetErrorString(hr));
        return false;
    }

    return true;
}

bool RenderSystem::endScene()
{
    // Call the device's endScene function.
    HRESULT hr = mD3dDevice->EndScene();

    if (FAILED(hr)) {
        TINYSC_LOGLINE_ERR("IDirect3DDevice::EndScene failed 0x%08x %s", hr, ::DXGetErrorString(hr));
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
        TINYSC_LOGLINE_ERR("IDirect3DDevice::TestCooperativeLevel 0x%08x %s", hr, ::DXGetErrorString(hr));

        if (hr == D3DERR_DEVICENOTRESET) {
            // The device can be reset now.
            // Release all device-dependent resources first.
            if (!_onDeviceLost()) {
                TINYSC_LOGLINE_ERR("Present failed because error occured when handling device lost.");
                return PRESENTERR_ERROR;
            }

            // Reset the device with the current present parameters.
            hr = mD3dDevice->Reset(&mPresentParams);
            TINYSC_LOGLINE_INFO("IDirect3DDevice::Reset 0x%08x %s", hr, ::DXGetErrorString(hr));

            if (FAILED(hr)) {
                // Reset still failed.
                TINYSC_LOGLINE_ERR("Present failed because device reset failed.");
                return PRESENTERR_ERROR;
            }

            // Device has been reset, recreate device-depent resources.
            if (!_onDeviceReset()) {
                TINYSC_LOGLINE_ERR("Present failed because error occured when handling device reset.");
                return PRESENTERR_ERROR;
            }

            mDeviceNeedsReset = false;
        }
        else if (hr == D3DERR_DEVICELOST) {
            // Can't be reset yet.
            return PRESENTERR_DEVICE_NOT_RESET;
        }
        else {
            // Other error happened.
            TINYSC_LOGLINE_ERR("Present failed.");
            return PRESENTERR_ERROR;
        }
    }

    hr = mD3dDevice->Present(NULL, NULL, NULL, NULL);

    if (FAILED(hr)) {
        TINYSC_LOGLINE_INFO("IDirect3DDevice::Present failed 0x%08x %s", hr, ::DXGetErrorString(hr));

        if (D3DERR_DEVICELOST == hr) {
            // The device is lost
            TINYSC_LOGLINE_INFO("Detect device lost.");

            mDeviceNeedsReset = true;
        }
        else {
            // Other error happened.
            TINYSC_LOGLINE_ERR("Present failed.");
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

bool RenderSystem::setRenderTarget(DWORD index, Texture* renderTarget)
{
    HRESULT hr = 0;
    if (renderTarget) {
        IDirect3DSurface9* surface = NULL;
        renderTarget->mD3dTexture->GetSurfaceLevel(0, &surface);
        hr = mD3dDevice->SetRenderTarget(index, surface);
        surface->Release();
    }
    else {
        hr = mD3dDevice->SetRenderTarget(index, NULL);
    }

    if (FAILED(hr)) 
        TINYSC_LOGLINE_ERR("ID3DDevice9::SetRenderTarget failed 0x%08x %s", hr, ::DXGetErrorString(hr));
    
    return SUCCEEDED(hr);
}

bool RenderSystem::resetRenderTarget()
{
    HRESULT hr = mD3dDevice->SetRenderTarget(0, mOriginalRenderTarget);
    if (FAILED(hr)) 
        TINYSC_LOGLINE_ERR("ID3DDevice9::SetRenderTarget failed 0x%08x %s", hr, ::DXGetErrorString(hr));

    return SUCCEEDED(hr);
}

bool RenderSystem::_initD3dDevice(HWND hWnd, const RenderSystemConfig& config)
{
    TINYSC_LOGLINE_INFO("RenderSystemConfig: BackbufferSize(%d*%d), VSync(%d)", config.backbufferSize.x, config.backbufferSize.y,
        config.isVSyncEnabled);

    // Create D3D object
    mD3d = ::Direct3DCreate9(D3D_SDK_VERSION);
    TINYSC_LOGLINE_INFO("::Direct3DCreate9 %p", (void*)mD3d);
    if (!mD3d) {
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

// Use windowed mode in debug mode.
#ifdef _DEBUG
    mPresentParams.Windowed = TRUE;
#else
    mPresentParams.Windowed = FALSE;
#endif

    // Create the device.
    HRESULT hr = 0;
    hr = mD3d->CreateDevice(0, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &mPresentParams, &mD3dDevice);
    TINYSC_LOGLINE_INFO("IDirect3D9::CreateDevice 0x%08x %s", hr, ::DXGetErrorString(hr));
    if (FAILED(hr)) {
        TINYSC_LOGLINE_ERR("Failed to create device.");
        return false;
    }

    // Get the original render target.
    hr = mD3dDevice->GetRenderTarget(0, &mOriginalRenderTarget);
    if (FAILED(hr)) {
        TINYSC_LOGLINE_ERR("Failed to get the original render target.");
        return false;
    }

    return true;
}

bool RenderSystem::_onDeviceLost()
{
    // Release the original render target.
    ULONG releaseNumber = mOriginalRenderTarget->Release();
    if (releaseNumber > 0) {
        TINYSC_LOGLINE_ERR("Release the original render target unsuccessfully %ld.", releaseNumber);
        return false;
    }

    // Release all textures created with render target flag set to true.
    _releaseRenderTargets();

    _handleEffectsOnDeviceLost();

    return true;
}

bool RenderSystem::_onDeviceReset()
{
    // Obtain the original render target.
    HRESULT hr = mD3dDevice->GetRenderTarget(0, &mOriginalRenderTarget);
    if (FAILED(hr)) {
        TINYSC_LOGLINE_ERR("Failed to get the original render target.");
        return false;
    }

    // Recreate all textures created with render target flag set to true.
    if (!_recreateRenderTargets())
        return false;

    _handleEffectsOnDeviceReset();

    return true;
}

void RenderSystem::_releaseRenderTargets()
{
    ULONG releaseNumber = 0;
    for (auto& it : mTextures) {
        if (it.second.isRenderTarget) {
            releaseNumber = it.first->mD3dTexture->Release();
            if (releaseNumber > 0) 
                TINYSC_LOGLINE_ERR("Render target %p released unsuccessfully %d.", (void*)it.first->mD3dTexture, releaseNumber);
        }
    }
}

bool RenderSystem::_recreateRenderTargets()
{
    for (auto& it : mTextures) {
        if (it.second.isRenderTarget) {
            IDirect3DTexture9* renderTarget = NULL;
            Size2<UINT> size = it.second.size;
            UINT mipLevels = it.second.mipLevels;
            D3DFORMAT format = it.second.format;

            if (it.second.srcFilename == "") {
                // The render target is not created from file.
                TINYSC_LOGLINE_INFO("Recreate render target: Size(%d*%d) MipLevels(%d) Format(%d).", size.x, size.y, mipLevels, format);
                renderTarget = _createTextureImpl(size, mipLevels, format, true);
            }
            else {
                // The render target is created from file.
                TINYSC_LOGLINE_INFO("Recreate render target from file: File(%s) Size(%d*%d) MipLevels(%d) Format(%d).", 
                    it.second.srcFilename.c_str(), size.x, size.y, mipLevels, format);
                renderTarget = _createTextureFromFileImpl(it.second.srcFilename, size, mipLevels, format, true);
            }

            if (renderTarget == nullptr) {
                // If one render target can't be recreated, stop and return false to indicate an error happens.
                TINYSC_LOGLINE_ERR("Render target can't be recreated.");
                return false;
            }

            it.first->mD3dTexture = renderTarget;
        }
    }

    return true;
}

void RenderSystem::_handleEffectsOnDeviceLost()
{
    for (auto& it : mEffects)
        it->OnLostDevice();
}

void RenderSystem::_handleEffectsOnDeviceReset()
{
    for (auto& it : mEffects)
        it->OnResetDevice();
}

IDirect3DTexture9* RenderSystem::_createTextureImpl(const Size2<UINT>& size, UINT mipLevels, D3DFORMAT format, bool isRenderTarget)
{
    IDirect3DTexture9* texture = NULL;

    HRESULT hr = ::D3DXCreateTexture(
        mD3dDevice,
        size.x,
        size.y,
        mipLevels,
        isRenderTarget ? D3DUSAGE_RENDERTARGET : 0, // Dynamic textures is not used.
        format,
        isRenderTarget ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, // Use default pool only for render targets.
        &texture
        );

    if (FAILED(hr)) {
        TINYSC_LOGLINE_ERR("D3DXCreateTexture failed 0x%08x %s. Size(%d*%d) mipLevels(%d) Format(%d) RenderTarget(%d).", hr,
            ::DXGetErrorString(hr), size.x, size.y, mipLevels, format, isRenderTarget);
        return NULL;
    }

    return texture;
}

IDirect3DTexture9* RenderSystem::_createTextureFromFileImpl(const std::string& srcFilename, const Size2<UINT>& size, UINT mipLevels,
    D3DFORMAT format, bool isRenderTarget)
{
    IDirect3DTexture9* texture = NULL;

    HRESULT hr = ::D3DXCreateTextureFromFileEx(
        mD3dDevice,
        srcFilename.c_str(),
        size.x,
        size.y,
        mipLevels,
        isRenderTarget ? D3DUSAGE_RENDERTARGET : 0, // Dynamic textures is not used.
        format,
        isRenderTarget ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, // Use default pool only for render targets.
        D3DX_DEFAULT,
        D3DX_DEFAULT,
        0,
        NULL,
        NULL,
        &texture
        );

    if (FAILED(hr)) {
        TINYSC_LOGLINE_ERR("D3DXCreateTextureFromFileEx failed 0x%08x %s. File(%s) Size(%d*%d) mipLevels(%d) Format(%d) RenderTarget(%d).",
            hr, ::DXGetErrorString(hr), srcFilename.c_str(), size.x, size.y, mipLevels, format, isRenderTarget);
        return NULL;
    }

    return texture;
}

ID3DXMesh* RenderSystem::_createMeshImpl(DWORD numVertices, DWORD numFaces, const D3DVERTEXELEMENT9* declaration)
{
    ID3DXMesh* mesh = NULL;

    HRESULT hr = ::D3DXCreateMesh(
        numFaces,
        numVertices,
        D3DXMESH_MANAGED | D3DXMESH_32BIT,
        declaration,
        mD3dDevice,
        &mesh
        );

    if (FAILED(hr)) {
        TINYSC_LOGLINE_ERR("D3DXCreateMesh failed 0x%08x %s. NumVertices(%d) NumFaces(%d).", hr, ::DXGetErrorString(hr), numVertices,
            numFaces)
        return NULL;
    }

    return mesh;
}

ID3DXEffect* RenderSystem::_createEffectFromFileImpl(const std::string& srcFilename, DWORD flags)
{
    ID3DXEffect* effect = NULL;
    ID3DXBuffer* errorBuffer = NULL;
    HRESULT hr = ::D3DXCreateEffectFromFile(
        mD3dDevice,
        srcFilename.c_str(),
        NULL,
        NULL,
        flags,
        NULL,
        &effect,
        &errorBuffer
        );

    if (errorBuffer) {
        TINYSC_LOGLINE_INFO("Effect compilation message: %s", (char*)errorBuffer->GetBufferPointer());
        errorBuffer->Release();
    }

    if (FAILED(hr)) {
        TINYSC_LOGLINE_ERR("D3DXCreateEffectFromFile result 0x%08x %s. File(%s) Flags(%d).", hr, ::DXGetErrorString(hr),
            srcFilename.c_str(), flags);
    }

    return effect;
}

ID3DXSprite* RenderSystem::_createSpriteImpl()
{
    ID3DXSprite* sprite = NULL;
    HRESULT hr = ::D3DXCreateSprite(mD3dDevice, &sprite);
    if (FAILED(hr))
        TINYSC_LOGLINE_ERR("::D3DXCreateSprite failed 0x%08x %s.", hr, ::DXGetErrorString(hr));

    return sprite;
}

ID3DXFont* RenderSystem::_createFontImpl(int height, UINT width, UINT weight, UINT mipLevels, BOOL italic, DWORD charSet,
    DWORD outputPrecision, DWORD quality, DWORD pitchAndFamily, LPCSTR faceName)
{
    ID3DXFont* font = NULL;
    HRESULT hr = ::D3DXCreateFont(mD3dDevice, height, width, weight, mipLevels, italic, charSet, outputPrecision, quality,
        pitchAndFamily, faceName, &font);
    if (FAILED(hr))
        TINYSC_LOGLINE_ERR("::D3DXCreateFont failed 0x%08x %s.", hr, ::DXGetErrorString(hr));

    return font;
}

Texture* RenderSystem::createTexture(const Size2<UINT>& size, UINT mipLevels, D3DFORMAT format, bool isRenderTarget)
{
    IDirect3DTexture9* texture = _createTextureImpl(size, mipLevels, format, isRenderTarget);
    if (texture == nullptr)
        return nullptr;

    Texture* newTexture = new Texture(texture);
    TextureCreationDesc desc(size, mipLevels, format, isRenderTarget, "");
    mTextures.push_back(std::make_pair(newTexture, desc));

    return newTexture;
}

Texture* RenderSystem::createTextureFromFile(const std::string& srcFilename, const Size2<UINT>& size, UINT mipLevels,
    D3DFORMAT format, bool isRenderTarget)
{
    IDirect3DTexture9* texture = _createTextureFromFileImpl(srcFilename, size, mipLevels, format, isRenderTarget);
    if (texture == nullptr)
        return nullptr;

    Texture* newTexture = new Texture(texture);
    TextureCreationDesc desc(size, mipLevels, format, isRenderTarget, srcFilename);
    mTextures.push_back(std::make_pair(newTexture, desc));

    return newTexture;
}

void RenderSystem::destroyTexture(Texture* texture)
{
    auto& it = std::find_if(mTextures.begin(), mTextures.end(), 
        [&](const std::pair<Texture*, TextureCreationDesc>& p) { return p.first == texture; }
    );
    TINYSC_ASSERT(it != mTextures.end(), "The texture is not created by this render system.");

    delete texture;
    mTextures.erase(it);
}

ID3DXMesh* RenderSystem::createMesh(DWORD numVertices, DWORD numFaces, const D3DVERTEXELEMENT9* declaration)
{
    ID3DXMesh* mesh = _createMeshImpl(numVertices, numFaces, declaration);
    if (mesh == nullptr)
        return nullptr;

    mMeshes.push_back(mesh);

    return mesh;
}

void RenderSystem::destroyMesh(ID3DXMesh* mesh)
{
    auto& it = std::find(mMeshes.begin(), mMeshes.end(), mesh);
    TINYSC_ASSERT(it != mMeshes.end(), "The mesh is not created by this render system.");

    mesh->Release();
    mMeshes.erase(it);
}

ID3DXEffect* RenderSystem::createEffectFromFile(const std::string& srcFilename, DWORD flags)
{
    ID3DXEffect* effect = _createEffectFromFileImpl(srcFilename, flags);
    if (effect == NULL)
        return NULL;

    mEffects.push_back(effect);

    return effect;
}

void RenderSystem::destroyEffect(ID3DXEffect* effect)
{
    auto& it = std::find(mEffects.begin(), mEffects.end(), effect);
    TINYSC_ASSERT(it != mEffects.end(), "The effect is not created by this render system.");

    effect->Release();
    mEffects.erase(it);
}

ID3DXSprite* RenderSystem::createSprite()
{
    ID3DXSprite* sprite = _createSpriteImpl();
    if (sprite == NULL)
        return NULL;

    mSprites.push_back(sprite);

    return sprite;
}

void RenderSystem::destroySprite(ID3DXSprite* sprite)
{
    auto& it = std::find(mSprites.begin(), mSprites.end(), sprite);
    TINYSC_ASSERT(it != mSprites.end(), "The sprite is not created by this render system.");

    sprite->Release();
    mSprites.erase(it);
}

ID3DXFont* RenderSystem::createFont(int height, UINT width, UINT weight, UINT mipLevels, BOOL italic, DWORD charSet,
    DWORD outputPrecision, DWORD quality, DWORD pitchAndFamily, const std::string& faceName)
{
    ID3DXFont* font = _createFontImpl(height, width, weight, mipLevels, italic, charSet, outputPrecision, quality, pitchAndFamily, 
        faceName.c_str());
    if (font == NULL)
        return NULL;

    mFonts.push_back(font);

    return font;
}

void RenderSystem::destroyFont(ID3DXFont* font)
{
    auto& it = std::find(mFonts.begin(), mFonts.end(), font);
    TINYSC_ASSERT(it != mFonts.end(), "The font is not created by this render system.");

    font->Release();
    mFonts.erase(it);
}

}