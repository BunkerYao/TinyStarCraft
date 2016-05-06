#include "Precompiled.h"
#include "RenderSystem.h"
#include "Utilities/Assert.h"

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
    // Destroy all textures.
    if (!mTextures.empty()) {
        for (auto& it : mTextures)
            delete it.first;

        mTextures.clear();
    }

    // Destroy all meshes.
    if (!mMeshes.empty()) {
        for (auto& it : mMeshes)
            delete it;

        mMeshes.clear();
    }

    _destroyGbuffers();

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

    // Create Gbuffers
    if (!_createGbuffers())
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
    _releaseRenderTargets();

    // TODO:

    return true;
}

bool RenderSystem::_onDeviceReset()
{
    if (!_recreateRenderTargets())
        return false;

    // TODO:

    return true;
}

void RenderSystem::_releaseRenderTargets()
{
    for (auto& it : mTextures) {
        if (it.second.isRenderTarget) {
            it.first->get()->Release();
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

            if (it.second.srcFilename == "")
                // The render target is not created from file.
                renderTarget = _createTextureImpl(size, mipLevels, format, true);
            else
                // The render target is created from file.
                renderTarget = _createTextureFromFileImpl(it.second.srcFilename, size, mipLevels, format, true);

            // One render target can't be recreated, stop and return false to indicate an error happens.
            // TODO: Log some message
            if (renderTarget == nullptr)
                return false;

            it.first->mResource = renderTarget;
        }
    }

    return true;
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
        printf("D3DXCreateTexture failed with result 0x%08x %s.\n", hr, ::DXGetErrorString(hr));
        printf("\tWidth:%d Height:%d Format:%d\n", size.x, size.y, format);
        return nullptr;
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
        printf("D3DXCreateTextureFromFileEx failed with result 0x%08x %s.\n", hr, ::DXGetErrorString(hr));
        printf("\tFilename: %s Width:%d Height:%d Format:%d\n", srcFilename.c_str(), size.x, size.y, format);
        return nullptr;
    }

    return texture;
}

ID3DXMesh* RenderSystem::_createMeshImpl(DWORD numVertices, DWORD numFaces, const D3DVERTEXELEMENT9* declaration)
{
    ID3DXMesh* mesh = NULL;

    HRESULT hr = ::D3DXCreateMesh(
        numFaces,
        numVertices,
        D3DXMESH_MANAGED,
        declaration,
        mD3dDevice,
        &mesh
        );

    if (FAILED(hr)) {
        printf("D3DXCreateMesh failed with result 0x%08x %s.\n", hr, ::DXGetErrorString(hr));
        printf("\tFaces:%d Vertices:%d\n", numFaces, numVertices);
        return nullptr;
    }

    return mesh;
}

bool RenderSystem::_createGbuffers()
{
    D3DFORMAT gbufferFormats[] = { D3DFMT_A8R8G8B8, D3DFMT_X8R8G8B8, D3DFMT_R16F };
    HRESULT hr = 0;

    for (int i = 0; i < 3; ++i) {
        hr = ::D3DXCreateTexture(
            mD3dDevice,
            mPresentParams.BackBufferWidth,
            mPresentParams.BackBufferHeight,
            1,
            D3DUSAGE_RENDERTARGET,
            gbufferFormats[i],
            D3DPOOL_DEFAULT,
            &mGbuffers[i]
            );

        printf("Create Gbuffer%d : D3DXCreateTexture 0x%08x %s.\n", i, hr, ::DXGetErrorString(hr));

        if (FAILED(hr))
            return false;
    }

    return true;
}

void RenderSystem::_destroyGbuffers()
{
    for (int i = 0; i < 3; ++i) {
        if (mGbuffers[i]) {
            ULONG number = mGbuffers[i]->Release();
            TINYSC_ASSERT(number == 0, "Gbuffer release unsuccessful.");
            printf("Release Gbuffer%d : IDirect3DTexture9::Release %d\n", i, number);
            mGbuffers[i] = NULL;
        }
    }
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

    delete it->first;
    mTextures.erase(it);
}

Mesh* RenderSystem::createMesh(DWORD numVertices, DWORD numFaces, const D3DVERTEXELEMENT9* declaration)
{
    ID3DXMesh* mesh = _createMeshImpl(numVertices, numFaces, declaration);
    if (mesh == nullptr)
        return nullptr;

    Mesh* newMesh = new Mesh(mesh);
    mMeshes.push_back(newMesh);

    return newMesh;
}

void RenderSystem::destroyMesh(Mesh* mesh)
{
    auto& it = std::find(mMeshes.begin(), mMeshes.end(), mesh);
    TINYSC_ASSERT(it != mMeshes.end(), "The mesh is not created by this render system.");

    delete *it;
    mMeshes.erase(it);
}

}