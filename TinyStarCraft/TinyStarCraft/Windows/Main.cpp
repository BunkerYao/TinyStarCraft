#include "Precompiled.h"

#include "GameWindow.h"
#include "Asset/MaterialManager.h"
#include "Asset/TextureManager.h"
#include "Rendering/Camera.h"
#include "Rendering/RenderSystem.h"
#include "Terrain/Terrain.h"
#include "Terrain/TerrainMaterial.h"

using namespace TinyStarCraft;

class Application
{
public:
    Application()
        : mGameWindow(WndProc),
          mTextureManager(nullptr),
          mCamera(nullptr)
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
        mTextureManager->createTextureFromFile("Test", "./Resources/marine_diffuse_blood__.dds");

        // Create an terrain material.
        mMaterialManager->createMaterial("terrain", "terrain");

        // Create a camera
        mCamera = new Camera(Size2f(800.0f, 600.0f), D3DXVECTOR3(600.0f, 400.0f, 600.0f));

        // Create a terrain
        Size2d terrainDimension(16, 16);
        std::vector<int> cellsData(terrainDimension.x * terrainDimension.y, 0);
        for (int i = 0; i < 15; ++i)
            cellsData[i] = i;
        std::vector<int> altitudeLevelData(terrainDimension.x * terrainDimension.y, 0);
        altitudeLevelData[0] = 0;
        TerrainMaterial* terrainMaterial = static_cast<TerrainMaterial*>(mMaterialManager->getMaterial("terrain"));
        mTerrain = new Terrain(&mRenderSystem);
        mTerrain->create(terrainDimension, cellsData, altitudeLevelData, terrainMaterial);
        
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
            ::PostQuitMessage(0);
            return true;

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