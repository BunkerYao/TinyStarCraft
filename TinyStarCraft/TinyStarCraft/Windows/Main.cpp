#include "Precompiled.h"

#include "GameWindow.h"
#include "Asset/MaterialManager.h"
#include "Asset/TextureManager.h"
#include "Rendering/Camera.h"
#include "Rendering/RenderSystem.h"
#include "Terrain/Terrain.h"
#include "Terrain/TerrainBuilder.h"
#include "Terrain/TerrainMaterial.h"
#include "Utilities/DebugOutput.h"

using namespace TinyStarCraft;

class Application
{
public:
    Application()
        : mGameWindow(WndProc),
          mTextureManager(nullptr),
          mCamera(nullptr),
          mTerrain(nullptr),
          mTerrainBuilder(Size2d(64, 64))
    {
    }

    ~Application()
    {
        delete mMaterialManager;
        delete mTextureManager;
        delete mTerrain;
        delete mCamera;
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
        RenderSystemConfig renderConfig = { {800, 600}, true };
        if (!mRenderSystem.init(mGameWindow.getHWND(), renderConfig))
            return false;

        // Create effect manager.
        mMaterialManager = new MaterialManager(&mRenderSystem);

        // Create texture manager.
        mTextureManager = new TextureManager(&mRenderSystem);

        // Try to load a texture.
        mTextureManager->createTextureFromFile("Test", "./Resources/Textures/marine_diffuse_blood__.dds");
        mTextureManager->createTextureFromFile("Dirt", "./Resources/Textures/terr_dirt-grass.jpg");
        mTextureManager->createTextureFromFile("Rock", "./Resources/Textures/terr_rock-dirt.jpg");
        mTextureManager->createTextureFromFile("grass0", "./Resources/Textures/crackeddirt-2048.jpg");
        mTextureManager->createTextureFromFile("grass1", "./Resources/Textures/grass_1024.jpg");
        mTextureManager->createTextureFromFile("Control", "./Resources/Textures/TerrainControl.dds");

        // Create an terrain material.
        TerrainMaterial* terrainMaterial = static_cast<TerrainMaterial*>(mMaterialManager->createMaterial("terrain", "terrain"));
        if (terrainMaterial == nullptr)
            return false;

        terrainMaterial->beginParameterChange();
        terrainMaterial->setBlendTexture(0, mTextureManager->getTexture("Dirt"));
        terrainMaterial->setBlendTexture(1, mTextureManager->getTexture("Rock"));
        terrainMaterial->setBlendTexture(2, mTextureManager->getTexture("grass0"));
        terrainMaterial->setBlendTexture(3, mTextureManager->getTexture("grass1"));
        terrainMaterial->setControlTexture(mTextureManager->getTexture("Control"));
        terrainMaterial->endParameterChange();

        // Create a camera
        mCamera = new Camera(Size2f(800.0f, 600.0f), D3DXVECTOR3(2000.0f, 1200.0f, 2000.0f));

        // Create a terrain
        mTerrain = new Terrain(&mRenderSystem);
        mTerrain->create(mTerrainBuilder.getDimension(), mTerrainBuilder.getCellsData(), mTerrainBuilder.getAltitudeLevelData(), 
            terrainMaterial);
        
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

            // TODO: Frame logic and render
            mRenderSystem.clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);
            mRenderSystem.beginScene();
            mTerrain->draw(mCamera);
            mRenderSystem.endScene();

            mRenderSystem.present();
        }

        return 0;
    }

    /**
     *	This function handles application messages.
     *  
     *  @return
     *  Return true to indicate the application has consumed the message, otherwise return false.
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
            Ray ray = mCamera->screenPointToRay(cursorPos);

            TerrainRaycastHit hit;
            if (mTerrain->raycast(ray, &hit)) {
                if (uMsg == WM_LBUTTONUP)
                    mTerrainBuilder.highten(hit.cellLocation);
                else
                    mTerrainBuilder.lower(hit.cellLocation);

                delete mTerrain;
                mTerrain = new Terrain(&mRenderSystem);
                TerrainMaterial* material = static_cast<TerrainMaterial*>(mMaterialManager->getMaterial("terrain"));
                mTerrain->create(mTerrainBuilder.getDimension(), mTerrainBuilder.getCellsData(), mTerrainBuilder.getAltitudeLevelData(),
                    material);
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

        if (application) {
            if (application->handleMessage(hWnd, uMsg, wParam, lParam))
                return 0;
            else
                return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
        
        return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

private:
    GameWindow mGameWindow;
    RenderSystem mRenderSystem;
    MaterialManager* mMaterialManager;
    TextureManager* mTextureManager;
    Terrain* mTerrain;
    Camera* mCamera;
    TerrainBuilder mTerrainBuilder;
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