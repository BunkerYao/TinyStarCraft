#include "Precompiled.h"
#include "GameWindow.h"
#include "Time.h"
#include "Asset/Effect.h"
#include "Asset/EffectManager.h"
#include "Asset/Material.h"
#include "Asset/MaterialManager.h"
#include "Asset/Mesh.h"
#include "Asset/Texture.h"
#include "Asset/TextureManager.h"
#include "Rendering/Camera.h"
#include "Rendering/IsometricSprite.h"
#include "Rendering/RenderSystem.h"
#include "Rendering/Scene.h"
#include "Rendering/Terrain.h"
#include "Rendering/TerrainModifier.h"
#include "Utilities/DebugOutput.h"

using namespace TinyStarCraft;

class Application : public RenderSystemListener
{
public:
    Application()
        : mGameWindow(WndProc),
          mRenderSystem(nullptr),
          mTextureManager(nullptr),
          mMaterialManager(nullptr),
          mTerrainModifier(nullptr),
          mScene(nullptr)
    {
    }

    ~Application()
    {
        delete mScene;
        delete mTerrainModifier;
        delete mRenderSystem;
    }

    bool init()
    {
        // Redirect stdout
        FILE* stream = nullptr;
        freopen_s(&stream, "Log.log", "w", stdout);

        // Create a window
        if (mGameWindow.create(Rectd::makeRect(30, 30, 800, 600), "TinyStarCraft") == false)
            return false;

        // Save the application instance as the userdata. So it can be retrieved in the WndProc.
        ::SetWindowLong(mGameWindow.getHWND(), GWLP_USERDATA, (LONG)this);

        // Create render system.
        mRenderSystem = new RenderSystem(this);
        RenderSystemConfig renderConfig(Size2d(800, 600), true, true);
        if (!mRenderSystem->initialize(mGameWindow.getHWND(), renderConfig))
            return false;

        // Create effect manager.
        mEffectManager = mRenderSystem->getEffectManager();

        // Create effect manager.
        mMaterialManager = mRenderSystem->getMaterialManager();

        // Create texture manager.
        mTextureManager = mRenderSystem->getTextureManager();

        if (!mEffectManager->createEffectFromFile(Scene::EFFECT_RESOURCE_NAME_SHARED_PARAMS, "./Resources/Effects/SharedParameters.cso"))
            return false;

        if (!mEffectManager->createEffectFromFile(Scene::EFFECT_RESOURCE_NAME_DEFERRED_LIGHTING, "./Resources/Effects/DeferredLighting.cso"))
            return false;

        if (!mEffectManager->createEffectFromFile(Terrain::EFFECT_RESOURCE_NAME_TERRAIN, "./Resources/Effects/Terrain.cso"))
            return false;

        if (!mEffectManager->createEffectFromFile(Terrain::EFFECT_RESOURCE_NAME_WATER, "./Resources/Effects/Water.cso"))
            return false;

        if (!mEffectManager->createEffectFromFile("default_isometric_sprite", "./Resources/Effects/DefaultIsometricSprite.cso"))
            return false;

        // Load textures.
        mTextureManager->createTextureFromFile("Dirt", "./Resources/Textures/terr_dirt-grass.jpg");
        mTextureManager->createTextureFromFile("Rock", "./Resources/Textures/terr_rock-dirt.jpg");
        mTextureManager->createTextureFromFile("grass0", "./Resources/Textures/crackeddirt-2048.jpg");
        mTextureManager->createTextureFromFile("grass1", "./Resources/Textures/grass_1024.jpg");
        mTextureManager->createTextureFromFile("Control", "./Resources/Textures/TerrainControl.dds");
        mTextureManager->createTextureFromFile("water_normal_0", "./Resources/Textures/WaterNormal0.jpg");
        mTextureManager->createTextureFromFile("water_normal_1", "./Resources/Textures/WaterNormal1.jpg");
        mTextureManager->createTextureFromFile("clouds", "./Resources/Textures/clouds.jpg");
        mTextureManager->createTextureFromFile("texel_mapping", "./Resources/Textures/TexelMapping.bmp", Size2d(800, 600), 1);

        // Create scene.
        mScene = new Scene(mRenderSystem);
        // Initialize the scene
        if (!mScene->initialize())
            return false;

        // Create camera
        mScene->createCamera(Size2f(800.0f, 600.0f), D3DXVECTOR3(500.0f, 400.0f, 500.0f));

        // Create terrain
        Terrain* terrain = mScene->createTerrain(Size2d(64, 64));

        terrain->setBlendTexture(0, mTextureManager->getTexture("Dirt"));
        terrain->setBlendTexture(1, mTextureManager->getTexture("Rock"));
        terrain->setBlendTexture(2, mTextureManager->getTexture("grass0"));
        terrain->setBlendTexture(3, mTextureManager->getTexture("grass1"));
        terrain->setControlTexture(mTextureManager->getTexture("Control"));

        terrain->setWaveTexture(0, mTextureManager->getTexture("water_normal_0"));
        terrain->setWaveTexture(1, mTextureManager->getTexture("water_normal_1"));

        // Create a terrain modifier
        mTerrainModifier = new TerrainModifier(mScene->getTerrain());

        Material* testSpriteMaterial = mMaterialManager->createMaterial("test_sprite", "default_isometric_sprite");
        IsometricSprite* isoSprite = mScene->createIsometricSprite();
        isoSprite->setPosition(D3DXVECTOR3(200.0f, 0.0f, 200.0f));
        isoSprite->setDimension(Size2f(100, 100));
        isoSprite->setOrigin(Point2f(100.0f, 100.0f));
        isoSprite->setHeightScale(100.0f);
        isoSprite->setMaterial(testSpriteMaterial);

        IsometricSprite* isoSprite1 = mScene->createIsometricSprite();
        isoSprite1->setPosition(D3DXVECTOR3(300.0f, 18.0f, 200.0f));
        isoSprite1->setDimension(Size2f(100, 100));
        isoSprite1->setOrigin(Point2f(100.0f, 100.0f));
        isoSprite1->setHeightScale(100.0f);
        isoSprite1->setMaterial(testSpriteMaterial);

        // Intialize the timer
        Time::init();
        
        return true;
    }

    int run()
    {
        while (true) {
            // Process all windows messages before rendering.
            MSG msg;
            while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                // Return immediately if WM_QUIT is encountered.
                if (msg.message == WM_QUIT) 
                    return (int)msg.wParam;

                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }

            Time::startFrame();

            POINT cursorPos;
            ::GetCursorPos(&cursorPos);
            ::ScreenToClient(mGameWindow.getHWND(), &cursorPos);

            D3DXVECTOR3 cameraVelocity(0.0f, 0.0f, 0.0f);
            if (cursorPos.x < 20) 
            {
                cameraVelocity.x = 5.0f;
                cameraVelocity.z = -5.0f;
            }
            else if (cursorPos.x > 780)
            {
                cameraVelocity.x = -5.0f;
                cameraVelocity.z = 5.0f;
            }

            if (cursorPos.y < 20)
            {
                cameraVelocity.x += -5.0f;
                cameraVelocity.z += -5.0f;
            }
            else if (cursorPos.y > 580)
            {
                cameraVelocity.x += 5.0f;
                cameraVelocity.z += 5.0f;
            }

            mScene->getCamera()->setPosition(mScene->getCamera()->getPosition() + cameraVelocity);

            mScene->render();

            mRenderSystem->present();
        }

        return 0;
    }

    /**
      	This function handles application messages.
    @return
        Return true to indicate the application has consumed the message, otherwise return false.
     */ 
    bool handleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_DESTROY:
        {
            ::PostQuitMessage(0);
            return true;
        }
        case WM_RBUTTONUP:
        case WM_LBUTTONUP:
        {
            Point2f cursorPos((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
            Ray ray = mScene->getCamera()->screenPointToRay(cursorPos);

            TerrainRaycastHit hit;
            if (mScene->getTerrain()->raycast(ray, &hit)) {
                if (uMsg == WM_LBUTTONUP) {
                    if (wParam & MK_CONTROL)
                        mTerrainModifier->hightenTile(hit.tileLocation);
                    else
                        mTerrainModifier->hightenGround(hit.tileLocation);
                }
                else {
                    if (wParam & MK_CONTROL)
                        mTerrainModifier->lowerTile(hit.tileLocation);
                    else
                        mTerrainModifier->lowerGround(hit.tileLocation);
                }

                mTerrainModifier->updateTerrain();
            }
        }
        default:
            break;
        }

        // No message is consumed.
        return false;
    }

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        // Retrieve the application instance from userdata.
        Application* application = (Application*)::GetWindowLong(hWnd, GWLP_USERDATA);

        if (application && application->handleMessage(hWnd, uMsg, wParam, lParam)) 
            return 0;
        
        return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

private:

    virtual void onDeviceLost() override
    {
        mScene->onDeviceLost();
    }

    virtual void onDeviceReset() override
    {
        mScene->onDeviceReset();
    }

private:
    GameWindow mGameWindow;
    RenderSystem *mRenderSystem;
    EffectManager* mEffectManager;
    MaterialManager* mMaterialManager;
    TextureManager* mTextureManager;
    Scene* mScene;
    TerrainModifier* mTerrainModifier;
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    Application app;
    if (app.init())
        return app.run();
    
    return 0;
}