#include "Precompiled.h"
#include "Scene.h"
#include "Camera.h"
#include "IsometricSprite.h"
#include "RenderSystem.h"
#include "Asset/Effect.h"
#include "Asset/EffectManager.h"
#include "Asset/Material.h"
#include "Asset/MaterialManager.h"
#include "Asset/Mesh.h"
#include "Asset/Texture.h"
#include "Asset/TextureManager.h"
#include "Utilities/Assert.h"
#include "Utilities/Logging.h"
#include "Windows/Time.h"

namespace TinyStarCraft
{

static const char EFFECT_SHARED_PARAM_NAME_GBUFFER0[] = "_gGbuffer0";
static const char EFFECT_SHARED_PARAM_NAME_GBUFFER1[] = "_gGbuffer1";
static const char EFFECT_SHARED_PARAM_NAME_HEIGHT_BUFFER[] = "_gHeightBuffer";
static const char EFFECT_SHARED_PARAM_NAME_REFRACTION_TEXTURE[] = "_gRefraction";
static const char EFFECT_SHARED_PARAM_NAME_RENDERTARGET_INVSIZE[] = "_gRenderTargetInvSize";
static const char EFFECT_SHARED_PARAM_NAME_VIEW_PROJ_MATRIX[] = "_gViewProjMatrix";
static const char EFFECT_SHARED_PARAM_NAME_VIEWPOINT[] = "_gViewPoint";
static const char EFFECT_SHARED_PARAM_NAME_TIME[] = "_gTime";

struct IsometricSpriteVertex
{
    D3DXVECTOR3 pos;
    D3DXVECTOR2 texcoord;
    float index;
};

//-------------------------------------------------------------------------------------------------
Scene::Scene(RenderSystem* renderSystem)
    : mRenderSystem(renderSystem),
      mCamera(nullptr),
      mTerrain(nullptr),
      mScreenQuadMesh(nullptr),
      mDeferredLightingEffect(nullptr)
{
}
//-------------------------------------------------------------------------------------------------
Scene::~Scene()
{
    ID3DXEffect* effectPtr = mSharedParamsEffect->getPointer();
    effectPtr->SetTexture(EFFECT_SHARED_PARAM_NAME_GBUFFER0, nullptr);
    effectPtr->SetTexture(EFFECT_SHARED_PARAM_NAME_GBUFFER1, nullptr);
    effectPtr->SetTexture(EFFECT_SHARED_PARAM_NAME_HEIGHT_BUFFER, nullptr);
    effectPtr->SetTexture(EFFECT_SHARED_PARAM_NAME_REFRACTION_TEXTURE, nullptr);

    _destroyRenderTargets();

    delete mScreenQuadMesh;
    delete mTerrain;
    delete mCamera;

    if (!mIsometricSprites.empty())
    {
        for (auto& it : mIsometricSprites)
            delete it;

        mIsometricSprites.clear();
    }
}
//-------------------------------------------------------------------------------------------------
bool Scene::initialize()
{
    // Create screen quad mesh.
    if (!_createScreenQuadMesh()) 
    {
        TINYSC_LOGLINE_ERR("Failed to create screen quad mesh.");
        return false;
    }

    // Create render targets
    if (!_createRenderTargets())
    {
        TINYSC_LOGLINE_ERR("Failed to create render targets.");
        return false;
    }

    if (!mIsometricSpriteRenderer.initialize(mRenderSystem->getD3DDevice()))
    {
        TINYSC_LOGLINE_ERR("Failed to initialize isometric sprite render helper.");
        return false;
    }

    // Retrieve the shared parameter effect
    mSharedParamsEffect = mRenderSystem->getEffectManager()->getEffect(EFFECT_RESOURCE_NAME_SHARED_PARAMS);
    TINYSC_ASSERT(mSharedParamsEffect != nullptr, "Shared parameters effect resource is not created.");

    // Retrieve the deferred lighting effect
    mDeferredLightingEffect = mRenderSystem->getEffectManager()->getEffect(EFFECT_RESOURCE_NAME_DEFERRED_LIGHTING);
    TINYSC_ASSERT(mDeferredLightingEffect != nullptr, "Deferred lighting effect resource is not created.");

    _initializeSharedEffectParameters();

    return true;
}
//-------------------------------------------------------------------------------------------------
Camera* Scene::createCamera(const Size2f& viewportSize, const D3DXVECTOR3& position)
{
    TINYSC_ASSERT(mCamera == nullptr, "There is already a camera being created.");
    mCamera = new Camera(viewportSize, position);
    return mCamera;
}
//-------------------------------------------------------------------------------------------------
Terrain* Scene::createTerrain(const Size2d& dimension, const std::vector<Tile>& tilesData)
{
    TINYSC_ASSERT(mTerrain == nullptr, "There is already a terrain being created.");

    mTerrain = new Terrain(mRenderSystem);

    if (!mTerrain->initialize(dimension, tilesData)) 
    {
        // Delete the terrain if failed.
        delete mTerrain;
        mTerrain = nullptr;
    }
    
    return mTerrain;
}
//-------------------------------------------------------------------------------------------------
Terrain* Scene::createTerrain(const Size2d& dimension, Tile initialTile)
{
    TINYSC_ASSERT(mTerrain == nullptr, "There is already a terrain being created.");

    mTerrain = new Terrain(mRenderSystem);

    if (!mTerrain->initialize(dimension, initialTile))
    {
        // Delete the terrain if failed.
        delete mTerrain;
        mTerrain = nullptr;
    }

    return mTerrain;
}
//-------------------------------------------------------------------------------------------------
void Scene::destroyTerrain()
{
    TINYSC_ASSERT(mTerrain == nullptr, "No destroyable terrain.");
    delete mTerrain;
    mTerrain = nullptr;
}
//-------------------------------------------------------------------------------------------------
IsometricSprite* Scene::createIsometricSprite(
    const D3DXVECTOR3& pos ,
    const Point2f& origin,
    const Size2f& dimension,
    const Rectf& textureRect,
    float heightScale,
    Material* material
    )
{
    IsometricSprite* isoSprite = new IsometricSprite(pos, origin, dimension, textureRect, heightScale, material);
    mIsometricSprites.push_back(isoSprite);
    return isoSprite;
}
//-------------------------------------------------------------------------------------------------
void Scene::destroyIsometricSprite(IsometricSprite* isoSprite)
{
    auto& it = std::find(mIsometricSprites.begin(), mIsometricSprites.end(), isoSprite);
    TINYSC_ASSERT(it != mIsometricSprites.end(), "Isometric sprite doesn't exist.");
    delete *it;
    mIsometricSprites.erase(it);
}
//-------------------------------------------------------------------------------------------------
bool Scene::render()
{
    HRESULT hr = 0;

    IDirect3DDevice9* D3DDevice = mRenderSystem->getD3DDevice();

    hr = D3DDevice->BeginScene();
    if (FAILED(hr))
        TINYSC_LOGLINE_D3D_ERR("IDirect3DDevice9::BeginScene", hr);

    if (SUCCEEDED(hr))
    {
        _updateFrameSharedEffectParameters();

        _setupGbuffersAndHeightBufferAsRendertargets();

        D3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
            D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f), 0.0f, 0);

        mTerrain->drawTerrain(mCamera);

        _drawIsometricSprites();

        mRenderSystem->resetDefaultRenderTarget();

        _doDeferredLightingPass();

        _updateRefractionTexture();

        mTerrain->drawWater(mCamera);

        hr = D3DDevice->EndScene();
        if (FAILED(hr))
            TINYSC_LOGLINE_D3D_ERR("IDirect3DDevice::EndScene", hr);
    }

    return SUCCEEDED(hr);
}
//-------------------------------------------------------------------------------------------------
void Scene::onDeviceLost()
{
    _destroyRenderTargets();
}
//-------------------------------------------------------------------------------------------------
bool Scene::onDeviceReset()
{
    if (!_createRenderTargets())
    {
        TINYSC_LOGLINE_ERR("Failed to create render target.");
        return false;
    }

    _initializeSharedEffectParameters();

    return false;
}
//-------------------------------------------------------------------------------------------------
bool Scene::_createScreenQuadMesh()
{
    TINYSC_ASSERT(mScreenQuadMesh == nullptr, "There is already a quad mesh being created.");

    mScreenQuadMesh = new Mesh(mRenderSystem->getD3DDevice());

    D3DVERTEXELEMENT9 elements[] =
    {
        { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        D3DDECL_END()
    };

    if (!mScreenQuadMesh->create(2, 4, D3DXMESH_MANAGED, elements))
        return false;

    D3DXVECTOR3* vertices = nullptr;
    mScreenQuadMesh->getPointer()->LockVertexBuffer(0, (void**)&vertices);

    vertices[0] = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);
    vertices[1] = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
    vertices[2] = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
    vertices[3] = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);

    mScreenQuadMesh->getPointer()->UnlockVertexBuffer();

    unsigned short* indices = nullptr;
    mScreenQuadMesh->getPointer()->LockIndexBuffer(0, (void**)&indices);

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 0;
    indices[4] = 2;
    indices[5] = 3;

    mScreenQuadMesh->getPointer()->UnlockIndexBuffer();

    return true;
}
//-------------------------------------------------------------------------------------------------
bool Scene::_createRenderTargets()
{
    IDirect3DSurface9* defaultRenderTarget = mRenderSystem->getDefaultRenderTarget();

    D3DSURFACE_DESC defaultRenderTargetDesc;
    HRESULT hr = defaultRenderTarget->GetDesc(&defaultRenderTargetDesc);
    TINYSC_ASSERT(SUCCEEDED(hr), "Failed to get default render target desc.");

    const Size2d renderTargetSize(defaultRenderTargetDesc.Width, defaultRenderTargetDesc.Height);

    // Create Gbuffers
    //

    static const D3DFORMAT GbufferFormats[2] = { D3DFMT_A8R8G8B8, D3DFMT_X8R8G8B8 };
    std::string GbufferName;

    for (int i = 0; i < 2; ++i) 
    {
        GbufferName = "__Gbuffer" + std::to_string(i);
        mGbuffers[i] = mRenderSystem->getTextureManager()->
            createTexture(GbufferName, renderTargetSize, 1, GbufferFormats[i], true);

        if (mGbuffers[i] == nullptr) 
        {
            TINYSC_LOGLINE_ERR("Failed to create Gbuffer%d.", i);
            return false;
        }
    }

    // Create height buffer
    //

    mHeightBuffer = mRenderSystem->getTextureManager()->
        createTexture("__HeightBuffer", renderTargetSize, 1, D3DFMT_R16F, true);

    if (mHeightBuffer == nullptr)
    {
        TINYSC_LOGLINE_ERR("Failed to create height buffer");
        return false;
    }

    // Create refraction texture
    //

    mRefractionTexture = mRenderSystem->getTextureManager()->
        createTexture("__Refraction", renderTargetSize, 1, D3DFMT_X8R8G8B8, true);

    if (mRefractionTexture == nullptr) 
    {
        TINYSC_LOGLINE_ERR("Failed to create refraction render target.");
        return false;
    }

    return true;
}
//-------------------------------------------------------------------------------------------------
void Scene::_destroyRenderTargets()
{
    // Destroy Gbuffers.
    for (int i = 0; i < 2; ++i) 
    {
        if (mGbuffers[i]) 
        {
            mRenderSystem->getTextureManager()->destroyResource(mGbuffers[i]);
            mGbuffers[i] = nullptr;
        }
    }

    // Destroy height buffer
    if (mHeightBuffer)
    {
        mRenderSystem->getTextureManager()->destroyResource(mHeightBuffer);
        mHeightBuffer = nullptr;
    }

    // Destroy rafraction render target.
    if (mRefractionTexture) 
    {
        mRenderSystem->getTextureManager()->destroyResource(mRefractionTexture);
        mRefractionTexture = nullptr;
    }
}
//-------------------------------------------------------------------------------------------------
void Scene::_setupGbuffersAndHeightBufferAsRendertargets()
{
    IDirect3DSurface9* GbufferSurface0 = nullptr;
    IDirect3DSurface9* GbufferSurface1 = nullptr;
    IDirect3DSurface9* heightBufferSurface = nullptr;

    mGbuffers[0]->getPointer()->GetSurfaceLevel(0, &GbufferSurface0);
    mGbuffers[1]->getPointer()->GetSurfaceLevel(0, &GbufferSurface1);
    mHeightBuffer->getPointer()->GetSurfaceLevel(0, &heightBufferSurface);

    IDirect3DDevice9* D3DDevice = mRenderSystem->getD3DDevice();

    D3DDevice->SetRenderTarget(0, GbufferSurface0);
    D3DDevice->SetRenderTarget(1, GbufferSurface1);
    D3DDevice->SetRenderTarget(2, heightBufferSurface);

    GbufferSurface0->Release();
    GbufferSurface1->Release();;
    heightBufferSurface->Release();
}
//-------------------------------------------------------------------------------------------------
void Scene::_initializeSharedEffectParameters()
{
    ID3DXEffect* effectPtr = mSharedParamsEffect->getPointer();

    effectPtr->SetTexture(EFFECT_SHARED_PARAM_NAME_GBUFFER0, mGbuffers[0]->getPointer());
    effectPtr->SetTexture(EFFECT_SHARED_PARAM_NAME_GBUFFER1, mGbuffers[1]->getPointer());
    effectPtr->SetTexture(EFFECT_SHARED_PARAM_NAME_HEIGHT_BUFFER, mGbuffers[2]->getPointer());
    effectPtr->SetTexture(EFFECT_SHARED_PARAM_NAME_REFRACTION_TEXTURE, mRefractionTexture->getPointer());

    D3DSURFACE_DESC desc;
    mRenderSystem->getDefaultRenderTarget()->GetDesc(&desc);

    D3DXVECTOR4 rendertargetInvSize(-1.0f / desc.Width, 1.0f / desc.Height, 0.0f, 0.0f);
    effectPtr->SetVector(EFFECT_SHARED_PARAM_NAME_RENDERTARGET_INVSIZE, &rendertargetInvSize);
}
//-------------------------------------------------------------------------------------------------
void Scene::_updateFrameSharedEffectParameters()
{
    ID3DXEffect* effectPtr = mSharedParamsEffect->getPointer();

    // Update view projection matrix
    D3DXMATRIX viewProjMatrix = mCamera->getViewMatrix() * mCamera->getProjMatrix();
    effectPtr->SetMatrix(EFFECT_SHARED_PARAM_NAME_VIEW_PROJ_MATRIX, &viewProjMatrix);

    // Update viewpoint position
    effectPtr->SetVector(EFFECT_SHARED_PARAM_NAME_VIEWPOINT, &D3DXVECTOR4(mCamera->getPosition(), 1.0f));
    // Update frame time
    effectPtr->SetFloat(EFFECT_SHARED_PARAM_NAME_TIME, Time::getFrameTime());
}
//-------------------------------------------------------------------------------------------------
void Scene::_doDeferredLightingPass()
{
    UINT passesCount = 0;
    mDeferredLightingEffect->getPointer()->Begin(&passesCount, 0);
    mDeferredLightingEffect->getPointer()->BeginPass(0);
    mScreenQuadMesh->getPointer()->DrawSubset(0);
    mDeferredLightingEffect->getPointer()->EndPass();
    mDeferredLightingEffect->getPointer()->End();
}
//-------------------------------------------------------------------------------------------------
void Scene::_updateRefractionTexture()
{
    IDirect3DSurface9* refractionRenderTargetSurface = nullptr;
    IDirect3DSurface9* defaultRenderTargetSurface = nullptr;

    mRefractionTexture->getPointer()->GetSurfaceLevel(0, &refractionRenderTargetSurface);
    defaultRenderTargetSurface = mRenderSystem->getDefaultRenderTarget();

    D3DSURFACE_DESC defaultRenderTargetSurfDesc;
    defaultRenderTargetSurface->GetDesc(&defaultRenderTargetSurfDesc);

    RECT copyRect;
    ::SetRect(
        &copyRect,
        0,
        0,
        defaultRenderTargetSurfDesc.Width,
        defaultRenderTargetSurfDesc.Height
        );

    mRenderSystem->getD3DDevice()->
        StretchRect(
            defaultRenderTargetSurface,
            &copyRect,
            refractionRenderTargetSurface,
            &copyRect,
            D3DTEXF_NONE
            );

    refractionRenderTargetSurface->Release();
}
//-------------------------------------------------------------------------------------------------
void Scene::_drawIsometricSprites()
{
    if (mIsometricSprites.empty())
        return;

    // Test code, assume sprite have same material
    //

    Material* material = mIsometricSprites.front()->getMaterial();
    //material->apply();

    ID3DXEffect* effectPtr = material->getEffect()->getPointer();

    UINT passesCount = 0;
    effectPtr->Begin(&passesCount, 0);
    effectPtr->BeginPass(0);

    mIsometricSpriteRenderer.draw(mIsometricSprites, mRenderSystem->getD3DDevice(), effectPtr);

    effectPtr->EndPass();
    effectPtr->End();
}
//-------------------------------------------------------------------------------------------------

}
