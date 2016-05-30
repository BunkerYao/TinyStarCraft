#pragma once

#include "IsometricSpriteRenderer.h"
#include "Terrain.h"

namespace TinyStarCraft
{

class Camera;
class Effect;
class EffectManager;
class IsometricSprite;
class MaterialManager;
class Mesh;
class RenderSystem;
class Terrain;
class Texture;
class TextureManager;

class Scene
{
public:
    static constexpr char EFFECT_RESOURCE_NAME_SHARED_PARAMS[24] = "__scene_shared_params__";
    static constexpr char EFFECT_RESOURCE_NAME_DEFERRED_LIGHTING[28] = "__scene_deferred_lighting__";

public:
    /** Constructor */
    explicit Scene(RenderSystem* renderSystem);

    /** Destructor */
    ~Scene();

    /** Initialize the scene */
    bool initialize();

    /** 
        Create camera. 
    @remarks
        Only one camera can be created at the same time.
     */
    Camera* createCamera(const Size2f& viewportSize, const D3DXVECTOR3& position);

    /** Retrieve the camera */
    Camera* getCamera() const { return mCamera; }

    /** 
        Create terrain 
    @remarks
        Only one terrain can be created at the same time.
    @return
        Returns the created terrain. Returns nullptr if failed.
     */
    Terrain* createTerrain(const Size2d& dimension, const std::vector<Tile>& tilesData);

    /**
        Create terrain
    @remarks
        Only one terrain can be created at the same time.
    @return
        Returns the created terrain. Returns nullptr if failed.
     */
    Terrain* createTerrain(const Size2d& dimension, Tile initialTile = Tile(ETileType::Flat, 0, false, 0.0f));

    /** Destroy the terrain. */
    void destroyTerrain();

    /** Retrieve the terrain */
    Terrain* getTerrain() const { return mTerrain; }

    /** Create an isometric sprite */
    IsometricSprite* createIsometricSprite(
        const D3DXVECTOR3& pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f), 
        const Point2f& origin = Point2f::ZERO(),
        const Size2f& dimension = Size2f::ZERO(), 
        const Rectf& textureRect = Rectf(Point2f::ZERO(), Point2f::ONE()),
        float heightScale = 0.0f,
        Material* material = nullptr
        );

    void destroyIsometricSprite(IsometricSprite* isoSprite);

    /** Render the scene */
    bool render();

    /** Handles device lost event. */
    void onDeviceLost();

    /** Handles device reset event. */
    bool onDeviceReset();

private:
    /** Create the screen quad mesh */
    bool _createScreenQuadMesh();

    /** Create Gbuffers and refraction texture */
    bool _createRenderTargets();

    /** Destroy Gbuffers and refraction texture */
    void _destroyRenderTargets();

    /** Setup Gbuffers and height buffer as render targes */
    void _setupGbuffersAndHeightBufferAsRendertargets();

    /** Intialize shared parameters */
    void _initializeSharedEffectParameters();

    /** Setup values of shared effect parameters which varies from frame to frame. */
    void _updateFrameSharedEffectParameters();

    /** Perform deferred lighting */
    void _doDeferredLightingPass();

    /** 
        Grab current content on the defaut render target to the refraction texture by 
        IDirect3DDevice9::StretchCopy 
    */
    void _updateRefractionTexture();

    void _drawIsometricSprites();

private:
    RenderSystem* mRenderSystem;

    Texture* mGbuffers[2];
    Texture* mHeightBuffer;
    Texture* mRefractionTexture;

    Effect* mDeferredLightingEffect;
    Effect* mSharedParamsEffect;

    Mesh* mScreenQuadMesh;

    Camera* mCamera;
    Terrain* mTerrain;

    typedef std::vector<IsometricSprite*> IsometricSpritePtrVector;
    IsometricSpritePtrVector mIsometricSprites;

    IsometricSpriteRenderer mIsometricSpriteRenderer;
};

};

