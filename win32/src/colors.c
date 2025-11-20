/*
 * colors.c - Color scheme implementation
 * Helper functions for creating color resources
 */

#include "../include/pphcalc.h"

/* Global color resources */
static HBRUSH g_hBrushHeaderBg = NULL;
static HBRUSH g_hBrushWindowBg = NULL;
static HBRUSH g_hBrushPanelBg = NULL;
static HBRUSH g_hBrushSummaryBg = NULL;
static HBRUSH g_hBrushSectionHeader = NULL;
static HPEN g_hPenBorder = NULL;
static HPEN g_hPenSummaryBorder = NULL;

/*
 * Initialize color resources (brushes and pens)
 * Call this once at application startup
 */
void InitializeColorResources(void)
{
    g_hBrushHeaderBg = CreateSolidBrush(COLOR_HEADER_BG);
    g_hBrushWindowBg = CreateSolidBrush(COLOR_WINDOW_BG);
    g_hBrushPanelBg = CreateSolidBrush(COLOR_PANEL_BG);
    g_hBrushSummaryBg = CreateSolidBrush(COLOR_SUMMARY_BG);
    g_hBrushSectionHeader = CreateSolidBrush(COLOR_SECTION_HEADER);

    g_hPenBorder = CreatePen(PS_SOLID, 1, COLOR_BORDER);
    g_hPenSummaryBorder = CreatePen(PS_SOLID, 2, COLOR_SUMMARY_BORDER);
}

/*
 * Clean up color resources
 * Call this at application shutdown
 */
void CleanupColorResources(void)
{
    SAFE_DELETE_OBJECT(g_hBrushHeaderBg);
    SAFE_DELETE_OBJECT(g_hBrushWindowBg);
    SAFE_DELETE_OBJECT(g_hBrushPanelBg);
    SAFE_DELETE_OBJECT(g_hBrushSummaryBg);
    SAFE_DELETE_OBJECT(g_hBrushSectionHeader);
    SAFE_DELETE_OBJECT(g_hPenBorder);
    SAFE_DELETE_OBJECT(g_hPenSummaryBorder);
}

/*
 * Create a solid brush with specified color
 */
HBRUSH CreateColorBrush(COLORREF color)
{
    return CreateSolidBrush(color);
}

/*
 * Create a pen with specified color and width
 */
HPEN CreateColorPen(COLORREF color, int width)
{
    return CreatePen(PS_SOLID, width, color);
}

/*
 * Draw the custom header bar with dark navy background
 */
void DrawCustomHeader(HDC hdc, RECT* rc)
{
    HFONT hOldFont;
    COLORREF oldTextColor;
    int oldBkMode;
    RECT rcText;

    /* Fill header background */
    FillRect(hdc, rc, g_hBrushHeaderBg);

    /* Setup text drawing */
    oldBkMode = SetBkMode(hdc, TRANSPARENT);
    oldTextColor = SetTextColor(hdc, COLOR_HEADER_TEXT);
    hOldFont = (HFONT)SelectObject(hdc, g_appState.hFontHeader);

    /* Draw app title on the left */
    rcText = *rc;
    rcText.left += MARGIN;
    rcText.right = rcText.left + 200;
    DrawText(hdc, "PPH KALKULATOR", -1, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    /* Draw subtitle below */
    SelectObject(hdc, g_appState.hFontLabel);
    SetTextColor(hdc, COLOR_TAB_TEXT);
    rcText.top += 18;
    DrawText(hdc, "PAJAK INDONESIA", -1, &rcText, DT_LEFT | DT_TOP | DT_SINGLELINE);

    /* Restore DC */
    SelectObject(hdc, hOldFont);
    SetTextColor(hdc, oldTextColor);
    SetBkMode(hdc, oldBkMode);
}

/*
 * Draw a tab button in the header
 */
void DrawTabButton(HDC hdc, RECT* rc, const char* text, BOOL isActive)
{
    HBRUSH hBrush;
    HFONT hOldFont;
    COLORREF oldTextColor;
    COLORREF bgColor, textColor;
    int oldBkMode;

    /* Select colors based on active state */
    if (isActive) {
        bgColor = COLOR_ACTIVE_TAB;
        textColor = COLOR_TEXT_PRIMARY;
    } else {
        bgColor = COLOR_INACTIVE_TAB;
        textColor = COLOR_TAB_TEXT;
    }

    /* Draw background */
    hBrush = CreateSolidBrush(bgColor);
    FillRect(hdc, rc, hBrush);
    DeleteObject(hBrush);

    /* Draw text */
    oldBkMode = SetBkMode(hdc, TRANSPARENT);
    oldTextColor = SetTextColor(hdc, textColor);
    hOldFont = (HFONT)SelectObject(hdc, g_appState.hFontNormal);

    DrawText(hdc, text, -1, rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    /* Restore DC */
    SelectObject(hdc, hOldFont);
    SetTextColor(hdc, oldTextColor);
    SetBkMode(hdc, oldBkMode);
}

/*
 * Get the appropriate background brush for controls
 */
HBRUSH GetPanelBackgroundBrush(void)
{
    return g_hBrushPanelBg;
}

/*
 * Get the window background brush
 */
HBRUSH GetWindowBackgroundBrush(void)
{
    return g_hBrushWindowBg;
}

/*
 * Get the section header brush for ListView
 */
HBRUSH GetSectionHeaderBrush(void)
{
    return g_hBrushSectionHeader;
}

/*
 * Get the summary box background brush
 */
HBRUSH GetSummaryBackgroundBrush(void)
{
    return g_hBrushSummaryBg;
}
