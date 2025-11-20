/*
 * mainwnd.c - Main window procedure and message handlers
 * Windows NT 4.0+ compatible
 */

#include "../include/pphcalc.h"
#include <stdio.h>

/*
 * MainWndProc - Main window procedure
 */
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
        case WM_CREATE:
            OnCreate(hwnd);
            return 0;

        case WM_SIZE:
            OnSize(hwnd, (UINT)wParam, LOWORD(lParam), HIWORD(lParam));
            return 0;

        case WM_PAINT:
            OnPaint(hwnd);
            return 0;

        case WM_COMMAND:
            OnCommand(hwnd, LOWORD(wParam), (HWND)lParam, HIWORD(wParam));
            return 0;

        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLOREDIT:
            /* Set background color for edit and static controls */
            SetBkMode((HDC)wParam, TRANSPARENT);
            return (LRESULT)GetStockObject(NULL_BRUSH);

        case WM_DESTROY:
            OnDestroy(hwnd);
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

/*
 * OnCreate - Handle WM_CREATE message
 */
void OnCreate(HWND hwnd)
{
    RECT rcClient;

    /* Get client area */
    GetClientRect(hwnd, &rcClient);

    /* Create input panel controls */
    {
        RECT rcPanel;
        rcPanel.left = MARGIN;
        rcPanel.top = HEADER_HEIGHT + MARGIN;
        rcPanel.right = rcPanel.left + PANEL_WIDTH;
        rcPanel.bottom = rcClient.bottom - MARGIN;
        CreateInputControls(hwnd, &rcPanel);
    }

    /* Create results panel controls */
    {
        RECT rcPanel;
        rcPanel.left = MARGIN + PANEL_WIDTH + MARGIN;
        rcPanel.top = HEADER_HEIGHT + MARGIN;
        rcPanel.right = rcClient.right - MARGIN;
        rcPanel.bottom = rcClient.bottom - MARGIN;
        CreateResultsControls(hwnd, &rcPanel);
    }

    /* Populate dropdowns with initial data */
    PopulateDropdowns(hwnd);

    /* Load example data by default */
    OnExampleClicked(hwnd);
}

/*
 * OnSize - Handle WM_SIZE message
 */
void OnSize(HWND hwnd, UINT state, int cx, int cy)
{
    /* Avoid unreferenced parameter warnings */
    (void)state;

    /* Layout controls based on new size */
    LayoutControls(hwnd, cx, cy);

    /* Invalidate to repaint header */
    {
        RECT rcHeader;
        rcHeader.left = 0;
        rcHeader.top = 0;
        rcHeader.right = cx;
        rcHeader.bottom = HEADER_HEIGHT;
        InvalidateRect(hwnd, &rcHeader, FALSE);
    }
}

/*
 * OnPaint - Handle WM_PAINT message
 */
void OnPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc;
    RECT rcClient, rcHeader, rcTab;
    int tabX, tabWidth;

    hdc = BeginPaint(hwnd, &ps);

    /* Get client area */
    GetClientRect(hwnd, &rcClient);

    /* Draw custom header */
    rcHeader.left = 0;
    rcHeader.top = 0;
    rcHeader.right = rcClient.right;
    rcHeader.bottom = HEADER_HEIGHT;
    DrawCustomHeader(hdc, &rcHeader);

    /* Draw tab buttons */
    tabWidth = 90;
    tabX = 220;  /* Start after app title */

    /* PPh 21/26 tab (active) */
    rcTab.left = tabX;
    rcTab.top = 10;
    rcTab.right = rcTab.left + tabWidth;
    rcTab.bottom = rcTab.top + 36;
    DrawTabButton(hdc, &rcTab, "PPh 21/26", TRUE);
    tabX = rcTab.right + 4;

    /* PPh 22 tab (inactive) */
    rcTab.left = tabX;
    rcTab.right = rcTab.left + tabWidth;
    DrawTabButton(hdc, &rcTab, "PPh 22", FALSE);
    tabX = rcTab.right + 4;

    /* PPh 23 tab (inactive) */
    rcTab.left = tabX;
    rcTab.right = rcTab.left + tabWidth;
    DrawTabButton(hdc, &rcTab, "PPh 23", FALSE);
    tabX = rcTab.right + 4;

    /* PPh 4(2) tab (inactive) */
    rcTab.left = tabX;
    rcTab.right = rcTab.left + tabWidth;
    DrawTabButton(hdc, &rcTab, "PPh 4(2)", FALSE);

    /* Draw "About" button on the right */
    rcTab.left = rcClient.right - 100 - MARGIN;
    rcTab.right = rcClient.right - MARGIN;
    DrawTabButton(hdc, &rcTab, "About", FALSE);

    EndPaint(hwnd, &ps);
}

/*
 * OnCommand - Handle WM_COMMAND message
 */
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    /* Avoid unreferenced parameter warnings */
    (void)hwndCtl;

    switch (id) {
        case IDC_BTN_RESET:
            OnResetClicked(hwnd);
            break;

        case IDC_BTN_EXAMPLE:
            OnExampleClicked(hwnd);
            break;

        case IDC_CMB_SCHEME:
            if (codeNotify == CBN_SELCHANGE) {
                OnSchemeChanged(hwnd);
            }
            break;

        case IDC_EDIT_BRUTO:
        case IDC_EDIT_MONTHS:
        case IDC_EDIT_PENSION:
        case IDC_EDIT_BONUS:
        case IDC_EDIT_ZAKAT:
        case IDC_CMB_SUBJECT:
        case IDC_CMB_PTKP:
        case IDC_CMB_TER_CATEGORY:
            /* Input changed - recalculate */
            if (codeNotify == EN_CHANGE || codeNotify == CBN_SELCHANGE) {
                PerformCalculation(hwnd);
            }
            break;

        case IDC_BTN_COPY:
            CopyResultsToClipboard(hwnd);
            break;

        case IDC_BTN_EXPORT:
            ExportResultsToCSV(hwnd);
            break;

        case IDC_BTN_PRINT:
            PrintResults(hwnd);
            break;

        case IDM_FILE_EXIT:
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            break;

        case IDM_HELP_ABOUT:
            {
                char aboutText[256];
                sprintf(aboutText,
                    "Kalkulator PPh 21/26\nVersion %s\n\n"
                    "PPHC Library Version: %s\n\n"
                    "Portable Indonesian Tax Calculator\n"
                    "Windows NT 4.0+ Compatible",
                    APP_VERSION, pph_get_version());
                MessageBox(hwnd, aboutText, "About", MB_OK | MB_ICONINFORMATION);
            }
            break;
    }
}

/*
 * OnDestroy - Handle WM_DESTROY message
 */
void OnDestroy(HWND hwnd)
{
    /* Avoid unreferenced parameter warning */
    (void)hwnd;

    /* Free last result if any */
    if (g_appState.lastResult) {
        pph_result_free(g_appState.lastResult);
        g_appState.lastResult = NULL;
    }

    /* Delete fonts */
    SAFE_DELETE_OBJECT(g_appState.hFontHeader);
    SAFE_DELETE_OBJECT(g_appState.hFontLabel);
    SAFE_DELETE_OBJECT(g_appState.hFontAmount);
    SAFE_DELETE_OBJECT(g_appState.hFontNormal);
}
