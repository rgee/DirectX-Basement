#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#include "dxManager.h"

/*
 * Globals
 */

/* Window handle */
HWND hWnd;
int windowHeight = 900;
int windowWidth = 1440;
bool texturesEnabled = true;

/* DirectX Manager */
DXManager dx;

LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {

        case WM_KEYDOWN:
            switch(wParam)
            {
                case VK_ESCAPE: PostQuitMessage(0);
                break;
            }
        break;

        /* User clicks the close button */
        case WM_DESTROY: PostQuitMessage(0);
        break;
    }

    /* Pass the events on out of the application. */
    return DefWindowProc(hWnd, message, wParam, lParam);
}

/* Initialize the window */
bool initWindow(HWND &hWnd, HINSTANCE hInstance, int windowWidth, int height)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = (WNDPROC)wndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = 0;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = TEXT("DirectXness");
    wcex.hIconSm = 0;

    RegisterClassEx(&wcex);

    RECT rect = {0, 0, windowWidth, height};
    AdjustWindowRect(&rect, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);

    /* Actually create the window from the above class */
    hWnd = CreateWindow("DirectXness",
                        "DirectXness",
                        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        rect.right - rect.left,
                        rect.bottom - rect.top,
                        NULL,
                        NULL,
                        hInstance,
                        NULL);

    if (!hWnd) {
        return false;
    }

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);
    return true;
}

/* Entry Point */
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    if(!initWindow(hWnd, hInstance, windowWidth, windowHeight)) {
        return 0;
    }

    if(!dx.Initialize(&hWnd)) {
        return 0;
    }

    /* Message loop */
    MSG msg = {0};
    while(WM_QUIT != msg.message)
    {
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) == TRUE)
        {
            /* Translate virtual-key messages into character messages. */
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        dx.Render();
    }
    return (int)msg.wParam;
}