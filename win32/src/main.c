/*
 * main.c - Application entry point and initialization
 * Windows NT 4.0+ compatible PPh 21 calculator
 */

#include "../include/pphcalc.h"
#include <stdio.h>

/* Global application state */
AppState g_appState = {0};

/* Instance handle */
static HINSTANCE g_hInstance = NULL;

/* Window class name */
static const char* MAIN_WINDOW_CLASS = "PPHCalculatorMainWnd";

/*
 * WinMain - Application entry point
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
    HACCEL hAccel;

    /* Avoid unreferenced parameter warning */
    (void)hPrevInstance;
    (void)lpCmdLine;

    /* Store instance handle */
    g_hInstance = hInstance;

    /* Initialize PPHC library */
    pph_init();

    /* Initialize common controls */
    {
        INITCOMMONCONTROLSEX icex;
        icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
        icex.dwICC = ICC_LISTVIEW_CLASSES | ICC_UPDOWN_CLASS | ICC_BAR_CLASSES;
        InitCommonControlsEx(&icex);
    }

    /* Register window class */
    if (!InitApplication(hInstance)) {
        MessageBox(NULL, "Failed to register window class", "Error", MB_ICONERROR);
        return 1;
    }

    /* Create main window */
    if (!InitInstance(hInstance, nCmdShow)) {
        MessageBox(NULL, "Failed to create main window", "Error", MB_ICONERROR);
        return 1;
    }

    /* Load accelerators (keyboard shortcuts) */
    hAccel = NULL;  /* TODO: Load from resources if needed */

    /* Message loop */
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!hAccel || !TranslateAccelerator(g_appState.hwndMain, hAccel, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    /* Cleanup */
    CleanupColorResources();

    return (int)msg.wParam;
}

/*
 * InitApplication - Register window class
 */
BOOL InitApplication(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    /* Fill in window class structure */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = MainWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);  /* TODO: Load custom icon */
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(COLOR_WINDOW_BG);
    wcex.lpszMenuName = NULL;  /* TODO: Add menu if needed */
    wcex.lpszClassName = MAIN_WINDOW_CLASS;
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    return RegisterClassEx(&wcex);
}

/*
 * InitInstance - Create main window
 */
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hwnd;
    int screenWidth, screenHeight;
    int windowWidth, windowHeight;
    int windowX, windowY;

    /* Initialize color resources */
    InitializeColorResources();

    /* Create fonts */
    g_appState.hFontHeader = CreateFont(
        -FONT_SIZE_HEADER, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Tahoma");

    g_appState.hFontLabel = CreateFont(
        -FONT_SIZE_LABEL, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Tahoma");

    g_appState.hFontAmount = CreateFont(
        -FONT_SIZE_AMOUNT, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Tahoma");

    g_appState.hFontNormal = CreateFont(
        -FONT_SIZE_NORMAL, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Tahoma");

    /* Fallback to MS Sans Serif if Tahoma not available (NT 4.0) */
    if (!g_appState.hFontNormal) {
        g_appState.hFontNormal = CreateFont(
            -FONT_SIZE_NORMAL, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "MS Sans Serif");
    }

    /* Calculate window size and position (centered on screen) */
    screenWidth = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);
    windowWidth = 1024;   /* Minimum width for layout */
    windowHeight = 700;   /* Minimum height */

    windowX = (screenWidth - windowWidth) / 2;
    windowY = (screenHeight - windowHeight) / 2;

    /* Create main window */
    hwnd = CreateWindowEx(
        0,                              /* Extended styles */
        MAIN_WINDOW_CLASS,              /* Class name */
        APP_TITLE,                      /* Window title */
        WS_OVERLAPPEDWINDOW,            /* Window style */
        windowX, windowY,               /* Position */
        windowWidth, windowHeight,      /* Size */
        NULL,                           /* Parent window */
        NULL,                           /* Menu */
        hInstance,                      /* Instance handle */
        NULL);                          /* Additional data */

    if (!hwnd) {
        return FALSE;
    }

    /* Store window handle */
    g_appState.hwndMain = hwnd;
    g_appState.currentTab = IDC_TAB_PPH21;
    g_appState.lastResult = NULL;

    /* Show and update window */
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    return TRUE;
}
