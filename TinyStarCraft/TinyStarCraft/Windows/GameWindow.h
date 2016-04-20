#pragma once

#include "Utilities/Rect2.h"

namespace TinyStarCraft
{

/**
 *	The game window.
 *
 *  @remarks
 *  The window is standalone and non-sizeable.
 */
class GameWindow
{
public:
    /**
     *	Construct a window object.
     *
     *  @remarks
     *  The constructor just register window class and won't create window.
     *  To create the window, call GameWindow::create
     */
    explicit GameWindow(WNDPROC wndProcFunc);

    /**
     *	Destroy the window
     */
    ~GameWindow();

    /**
     *	Create and show the window.
     *
     *  @param clientRect
     *  The client rectangle of the window.
     *
     *  @param title
     *  The title of the window.
     *
     *  @return
     *  True if succeeded, false if failed.
     */
    bool create(const Rectd& clientRect, const std::wstring& title);

    /** Get the handle */
    HWND getHWND() const;

private:
    HWND mhWnd;
};

};