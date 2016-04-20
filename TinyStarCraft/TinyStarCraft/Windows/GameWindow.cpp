#include "Precompiled.h"
#include "GameWindow.h"
#include "Utilities/Exceptions.h"

// The window class name
const wchar_t WINDOW_CLASS_NAME[] = L"GameWindow";

// The window style (Non-sizeable)
const unsigned WINDOW_STYLE = WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX)& ~(WS_SIZEBOX);

namespace TinyStarCraft
{

GameWindow::GameWindow(WNDPROC wndProcFunc)
    : mhWnd(NULL)
{
    HINSTANCE hInst = ::GetModuleHandle(NULL);

    // Register window class.
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = wndProcFunc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = ::GetModuleHandle(NULL);
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(::GetStockObject(BLACK_BRUSH));
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = WINDOW_CLASS_NAME;
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    ::RegisterClassEx(&wcex);
}

GameWindow::~GameWindow()
{
    ::DestroyWindow(mhWnd);
}

bool GameWindow::create(const Rectd& clientRect, const std::wstring& title)
{
    // Compute actual window rectangle from client rectangle.
    RECT rect;
    ::SetRect(&rect, clientRect.getLeft(), clientRect.getTop(), clientRect.getRight(), clientRect.getBottom());
    ::AdjustWindowRect(&rect, WINDOW_STYLE, FALSE);

    mhWnd = ::CreateWindow(WINDOW_CLASS_NAME, title.c_str(), WINDOW_STYLE, rect.left, rect.top, 
        rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, ::GetModuleHandle(NULL), NULL);

    if (mhWnd == NULL)
        return false;

    ::ShowWindow(mhWnd, SW_SHOW);
    ::UpdateWindow(mhWnd);

    return true;
}

HWND GameWindow::getHWND() const
{
    return mhWnd;
}

}
