/*
 * pphcalc.h - Main application header
 * Windows NT 4.0+ compatible PPh 21 calculator
 */

#ifndef PPHCALC_H
#define PPHCALC_H

#include <windows.h>
#include <commctrl.h>
#include <pph/pph_calculator.h>
#include "resource.h"
#include "colors.h"

/* Application state structure */
typedef struct {
    HWND hwndMain;
    HWND hwndInputPanel;
    HWND hwndResultsPanel;
    HFONT hFontHeader;
    HFONT hFontLabel;
    HFONT hFontAmount;
    HFONT hFontNormal;
    pph_result_t* lastResult;
    int currentTab;
} AppState;

/* Global app state */
extern AppState g_appState;

/* Function prototypes - main.c */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow);
BOOL InitApplication(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);

/* Function prototypes - mainwnd.c */
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void OnCreate(HWND hwnd);
void OnSize(HWND hwnd, UINT state, int cx, int cy);
void OnPaint(HWND hwnd);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void OnDestroy(HWND hwnd);

/* Function prototypes - controls.c */
void CreateInputControls(HWND hwndParent, RECT* rcPanel);
void CreateResultsControls(HWND hwndParent, RECT* rcPanel);
void CreateHeaderTabs(HWND hwndParent);
void PopulateDropdowns(HWND hwndParent);
void LayoutControls(HWND hwnd, int width, int height);

/* Function prototypes - calculation.c */
void PerformCalculation(HWND hwnd);
BOOL GetInputFromControls(HWND hwnd, pph21_input_t* input);
void OnResetClicked(HWND hwnd);
void OnExampleClicked(HWND hwnd);
void OnSchemeChanged(HWND hwnd);

/* Function prototypes - display.c */
void DisplayResults(HWND hwnd, pph_result_t* result);
void DisplaySummary(HWND hwnd, pph_result_t* result);
void DisplayBreakdown(HWND hwnd, pph_result_t* result);
void ClearResults(HWND hwnd);
void DrawSummaryBox(HDC hdc, RECT* rc, const char* label, const char* value);

/* Function prototypes - formatting.c */
void FormatRupiah(pph_money_t money, char* buffer, size_t size);
void FormatRupiahWithLabel(pph_money_t money, char* buffer, size_t size);
void FormatPercent(pph_money_t percent, char* buffer, size_t size);
pph_money_t ParseRupiah(const char* str);
void FormatThousandSeparator(const char* input, char* output, size_t outSize);
void RemoveThousandSeparator(const char* input, char* output, size_t outSize);
void GetEditMoneyValue(HWND hwnd, int controlId, pph_money_t* outValue);
void SetEditMoneyValue(HWND hwnd, int controlId, pph_money_t value);

/* Function prototypes - colors.c */
void InitializeColorResources(void);
void CleanupColorResources(void);
void DrawCustomHeader(HDC hdc, RECT* rc);
void DrawTabButton(HDC hdc, RECT* rc, const char* text, BOOL isActive);

/* Function prototypes - clipboard/export */
void CopyResultsToClipboard(HWND hwnd);
void ExportResultsToCSV(HWND hwnd);
void PrintResults(HWND hwnd);

/* Helper macros */
#define SAFE_FREE(p) if(p) { free(p); p = NULL; }
#define SAFE_DELETE_OBJECT(h) if(h) { DeleteObject(h); h = NULL; }

/* Subject type dropdown items */
static const char* SUBJECT_TYPE_NAMES[] = {
    "Pegawai tetap",
    "Pensiunan",
    "Pegawai tidak tetap",
    "Bukan pegawai",
    "Peserta kegiatan",
    "Program pensiun",
    "Mantan pegawai",
    "WPLN (PPh 26)"
};

/* PTKP status dropdown items */
static const char* PTKP_STATUS_NAMES[] = {
    "TK/0",
    "TK/1",
    "TK/2",
    "TK/3",
    "K/0",
    "K/1",
    "K/2",
    "K/3"
};

/* Scheme dropdown items */
static const char* SCHEME_NAMES[] = {
    "Skema lama (Pasal 17)",
    "TER (Tarif Efektif)"
};

/* TER category dropdown items */
static const char* TER_CATEGORY_NAMES[] = {
    "Kategori A",
    "Kategori B",
    "Kategori C"
};

#endif /* PPHCALC_H */
