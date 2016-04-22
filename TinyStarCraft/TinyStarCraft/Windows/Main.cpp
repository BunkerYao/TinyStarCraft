#include "Precompiled.h"

#include "GameWindow.h"
#include "Asset/TextureManager.h"
#include "Rendering/RenderSystem.h"

using namespace TinyStarCraft;

class Application
{
public:
    Application()
        : mGameWindow(WndProc),
          mTextureManager(nullptr)
    {
    }

    ~Application()
    {
        delete mTextureManager;
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

        // Create texture manager.
        mTextureManager = new TextureManager(&mRenderSystem);

        // Try to load a texture.
        mTextureManager->createFromFile("Test", "./Resources/marine_diffuse_blood__.dds");

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
    TextureManager* mTextureManager;
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