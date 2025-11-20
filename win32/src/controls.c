/*
 * controls.c - Control creation and management
 * Creates input form and results display controls
 */

#include "../include/pphcalc.h"

/* Control positions and sizes */
#define LABEL_X 8
#define INPUT_X 8
#define INPUT_WIDTH (PANEL_WIDTH - 16)
#define COMBO_HEIGHT 21

/*
 * Helper: Create a static label
 */
static HWND CreateLabel(HWND hwndParent, const char* text, int x, int y, int width)
{
    HWND hwnd = CreateWindowEx(0, "STATIC", text,
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        x, y, width, LABEL_HEIGHT,
        hwndParent, NULL, GetModuleHandle(NULL), NULL);
    SendMessage(hwnd, WM_SETFONT, (WPARAM)g_appState.hFontLabel, TRUE);
    return hwnd;
}

/*
 * Helper: Create an edit control
 */
static HWND CreateEditControl(HWND hwndParent, int id, int x, int y, int width, DWORD extraStyle)
{
    HWND hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL | extraStyle,
        x, y, width, CONTROL_HEIGHT,
        hwndParent, (HMENU)(UINT_PTR)id, GetModuleHandle(NULL), NULL);
    SendMessage(hwnd, WM_SETFONT, (WPARAM)g_appState.hFontNormal, TRUE);
    return hwnd;
}

/*
 * Helper: Create a combo box
 */
static HWND CreateComboControl(HWND hwndParent, int id, int x, int y, int width)
{
    HWND hwnd = CreateWindowEx(0, "COMBOBOX", "",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | CBS_DROPDOWNLIST,
        x, y, width, 200,  /* Height is for dropdown list */
        hwndParent, (HMENU)(UINT_PTR)id, GetModuleHandle(NULL), NULL);
    SendMessage(hwnd, WM_SETFONT, (WPARAM)g_appState.hFontNormal, TRUE);
    return hwnd;
}

/*
 * Helper: Create a button
 */
static HWND CreateButton(HWND hwndParent, int id, const char* text, int x, int y, int width, int height)
{
    HWND hwnd = CreateWindowEx(0, "BUTTON", text,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        x, y, width, height,
        hwndParent, (HMENU)(UINT_PTR)id, GetModuleHandle(NULL), NULL);
    SendMessage(hwnd, WM_SETFONT, (WPARAM)g_appState.hFontNormal, TRUE);
    return hwnd;
}

/*
 * CreateInputControls - Create input form controls (left panel)
 */
void CreateInputControls(HWND hwndParent, RECT* rcPanel)
{
    int y = 0;
    int halfWidth = (INPUT_WIDTH - SPACING) / 2;

    /* Group box title */
    CreateWindowEx(0, "BUTTON", "Data Penghasilan",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        rcPanel->left, rcPanel->top, PANEL_WIDTH, rcPanel->bottom - rcPanel->top,
        hwndParent, NULL, GetModuleHandle(NULL), NULL);

    y = rcPanel->top + 20;

    /* Jenis subjek */
    CreateLabel(hwndParent, "Jenis subjek", rcPanel->left + LABEL_X, y, INPUT_WIDTH);
    y += LABEL_HEIGHT + 4;
    CreateComboControl(hwndParent, IDC_CMB_SUBJECT, rcPanel->left + INPUT_X, y, INPUT_WIDTH);
    y += COMBO_HEIGHT + SPACING;

    /* Penghasilan bruto per masa */
    CreateLabel(hwndParent, "Penghasilan bruto per masa", rcPanel->left + LABEL_X, y, INPUT_WIDTH);
    y += LABEL_HEIGHT + 4;
    CreateEditControl(hwndParent, IDC_EDIT_BRUTO, rcPanel->left + INPUT_X, y, INPUT_WIDTH, ES_RIGHT);
    y += CONTROL_HEIGHT + SPACING;

    /* Jumlah masa (with spinner) */
    CreateLabel(hwndParent, "Jumlah masa (bulan/hari)", rcPanel->left + LABEL_X, y, INPUT_WIDTH);
    y += LABEL_HEIGHT + 4;
    {
        HWND hwndEdit = CreateEditControl(hwndParent, IDC_EDIT_MONTHS,
            rcPanel->left + INPUT_X, y, INPUT_WIDTH - 20, ES_RIGHT | ES_NUMBER);

        /* Create up-down control */
        CreateWindowEx(0, UPDOWN_CLASS, "",
            WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_ARROWKEYS,
            0, 0, 0, 0,  /* Position set automatically */
            hwndParent, (HMENU)(UINT_PTR)IDC_SPIN_MONTHS,
            GetModuleHandle(NULL), NULL);

        SendDlgItemMessage(hwndParent, IDC_SPIN_MONTHS, UDM_SETBUDDY, (WPARAM)hwndEdit, 0);
        SendDlgItemMessage(hwndParent, IDC_SPIN_MONTHS, UDM_SETRANGE, 0, MAKELONG(12, 1));
        SendDlgItemMessage(hwndParent, IDC_SPIN_MONTHS, UDM_SETPOS, 0, 12);
    }
    y += CONTROL_HEIGHT + SPACING;

    /* Iuran pensiun per masa */
    CreateLabel(hwndParent, "Iuran pensiun per masa", rcPanel->left + LABEL_X, y, INPUT_WIDTH);
    y += LABEL_HEIGHT + 4;
    CreateEditControl(hwndParent, IDC_EDIT_PENSION, rcPanel->left + INPUT_X, y, INPUT_WIDTH, ES_RIGHT);
    y += CONTROL_HEIGHT + SPACING;

    /* Bonus/tantiem tahunan */
    CreateLabel(hwndParent, "Bonus/tantiem tahunan", rcPanel->left + LABEL_X, y, INPUT_WIDTH);
    y += LABEL_HEIGHT + 4;
    CreateEditControl(hwndParent, IDC_EDIT_BONUS, rcPanel->left + INPUT_X, y, INPUT_WIDTH, ES_RIGHT);
    y += CONTROL_HEIGHT + SPACING;

    /* Status PTKP */
    CreateLabel(hwndParent, "Status PTKP", rcPanel->left + LABEL_X, y, INPUT_WIDTH);
    y += LABEL_HEIGHT + 4;
    CreateComboControl(hwndParent, IDC_CMB_PTKP, rcPanel->left + INPUT_X, y, INPUT_WIDTH);
    y += COMBO_HEIGHT + SPACING;

    /* Zakat/Sumbangan (setahun) */
    CreateLabel(hwndParent, "Zakat/Sumbangan (setahun)", rcPanel->left + LABEL_X, y, INPUT_WIDTH);
    y += LABEL_HEIGHT + 4;
    CreateEditControl(hwndParent, IDC_EDIT_ZAKAT, rcPanel->left + INPUT_X, y, INPUT_WIDTH, ES_RIGHT);
    y += CONTROL_HEIGHT + SPACING;

    /* Skema */
    CreateLabel(hwndParent, "Skema", rcPanel->left + LABEL_X, y, INPUT_WIDTH);
    y += LABEL_HEIGHT + 4;
    CreateComboControl(hwndParent, IDC_CMB_SCHEME, rcPanel->left + INPUT_X, y, INPUT_WIDTH);
    y += COMBO_HEIGHT + SPACING;

    /* Kategori TER */
    CreateLabel(hwndParent, "Kategori TER", rcPanel->left + LABEL_X, y, INPUT_WIDTH);
    y += LABEL_HEIGHT + 4;
    CreateComboControl(hwndParent, IDC_CMB_TER_CATEGORY, rcPanel->left + INPUT_X, y, INPUT_WIDTH);
    y += COMBO_HEIGHT + SPACING * 2;

    /* Buttons */
    CreateButton(hwndParent, IDC_BTN_RESET, "Reset",
        rcPanel->left + INPUT_X, y, halfWidth, BUTTON_HEIGHT);
    CreateButton(hwndParent, IDC_BTN_EXAMPLE, "Gunakan Contoh",
        rcPanel->left + INPUT_X + halfWidth + SPACING, y, halfWidth, BUTTON_HEIGHT);
}

/*
 * CreateResultsControls - Create results display controls (right panel)
 */
void CreateResultsControls(HWND hwndParent, RECT* rcPanel)
{
    int y = rcPanel->top;
    int panelWidth = rcPanel->right - rcPanel->left;
    HWND hwndListView;
    LVCOLUMN lvc;

    /* Summary boxes will be drawn custom in OnPaint of a child window */
    /* For now, create static controls for totals */

    /* Total Tax label */
    CreateLabel(hwndParent, "RANGKUMAN PAJAK", rcPanel->left, y, panelWidth);
    y += LABEL_HEIGHT + 8;

    /* Total PPh 21/26 */
    CreateLabel(hwndParent, "Total PPh 21/26:", rcPanel->left, y, 150);
    CreateWindowEx(0, "STATIC", "Rp 0",
        WS_CHILD | WS_VISIBLE | SS_RIGHT,
        rcPanel->left + 150, y, 200, LABEL_HEIGHT,
        hwndParent, (HMENU)(UINT_PTR)IDC_STATIC_TOTAL_TAX,
        GetModuleHandle(NULL), NULL);
    SendDlgItemMessage(hwndParent, IDC_STATIC_TOTAL_TAX, WM_SETFONT,
        (WPARAM)g_appState.hFontAmount, TRUE);
    y += LABEL_HEIGHT + 4;

    /* TER bulanan */
    CreateLabel(hwndParent, "TER bulanan:", rcPanel->left, y, 150);
    CreateWindowEx(0, "STATIC", "Rp 0",
        WS_CHILD | WS_VISIBLE | SS_RIGHT,
        rcPanel->left + 150, y, 200, LABEL_HEIGHT,
        hwndParent, (HMENU)(UINT_PTR)IDC_STATIC_TER_MONTHLY,
        GetModuleHandle(NULL), NULL);
    SendDlgItemMessage(hwndParent, IDC_STATIC_TER_MONTHLY, WM_SETFONT,
        (WPARAM)g_appState.hFontNormal, TRUE);
    y += LABEL_HEIGHT + 4;

    /* Kurang bayar Desember */
    CreateLabel(hwndParent, "Kurang bayar Desember:", rcPanel->left, y, 150);
    CreateWindowEx(0, "STATIC", "Rp 0",
        WS_CHILD | WS_VISIBLE | SS_RIGHT,
        rcPanel->left + 150, y, 200, LABEL_HEIGHT,
        hwndParent, (HMENU)(UINT_PTR)IDC_STATIC_DEC_BALANCE,
        GetModuleHandle(NULL), NULL);
    SendDlgItemMessage(hwndParent, IDC_STATIC_DEC_BALANCE, WM_SETFONT,
        (WPARAM)g_appState.hFontNormal, TRUE);
    y += LABEL_HEIGHT + SPACING * 2;

    /* Take-home pay section */
    CreateLabel(hwndParent, "TAKE-HOME PAY", rcPanel->left, y, panelWidth);
    y += LABEL_HEIGHT + 8;

    /* THP setahun */
    CreateLabel(hwndParent, "THP setahun:", rcPanel->left, y, 150);
    CreateWindowEx(0, "STATIC", "Rp 0",
        WS_CHILD | WS_VISIBLE | SS_RIGHT,
        rcPanel->left + 150, y, 200, LABEL_HEIGHT,
        hwndParent, (HMENU)(UINT_PTR)IDC_STATIC_THP_ANNUAL,
        GetModuleHandle(NULL), NULL);
    SendDlgItemMessage(hwndParent, IDC_STATIC_THP_ANNUAL, WM_SETFONT,
        (WPARAM)g_appState.hFontAmount, TRUE);
    y += LABEL_HEIGHT + 4;

    /* THP per masa */
    CreateLabel(hwndParent, "THP per masa:", rcPanel->left, y, 150);
    CreateWindowEx(0, "STATIC", "Rp 0",
        WS_CHILD | WS_VISIBLE | SS_RIGHT,
        rcPanel->left + 150, y, 200, LABEL_HEIGHT,
        hwndParent, (HMENU)(UINT_PTR)IDC_STATIC_THP_MONTHLY,
        GetModuleHandle(NULL), NULL);
    SendDlgItemMessage(hwndParent, IDC_STATIC_THP_MONTHLY, WM_SETFONT,
        (WPARAM)g_appState.hFontNormal, TRUE);
    y += LABEL_HEIGHT + SPACING * 3;

    /* Breakdown table label */
    CreateLabel(hwndParent, "RINCIAN PERHITUNGAN", rcPanel->left, y, panelWidth);
    y += LABEL_HEIGHT + 8;

    /* Create ListView for breakdown */
    hwndListView = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, "",
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | WS_BORDER,
        rcPanel->left, y, panelWidth, rcPanel->bottom - y - BUTTON_HEIGHT - SPACING * 2,
        hwndParent, (HMENU)(UINT_PTR)IDC_LISTVIEW_BREAKDOWN,
        GetModuleHandle(NULL), NULL);

    SendMessage(hwndListView, WM_SETFONT, (WPARAM)g_appState.hFontNormal, TRUE);

    /* Set extended ListView styles */
    ListView_SetExtendedListViewStyle(hwndListView, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    /* Add columns */
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;

    /* Column 0: Komponen */
    lvc.fmt = LVCFMT_LEFT;
    lvc.cx = 250;
    lvc.pszText = "Komponen";
    ListView_InsertColumn(hwndListView, 0, &lvc);

    /* Column 1: Nilai (Rp) */
    lvc.fmt = LVCFMT_RIGHT;
    lvc.cx = 150;
    lvc.pszText = "Nilai (Rp)";
    ListView_InsertColumn(hwndListView, 1, &lvc);

    /* Column 2: Keterangan */
    lvc.fmt = LVCFMT_LEFT;
    lvc.cx = panelWidth - 250 - 150 - 20;  /* Remaining width */
    lvc.pszText = "Keterangan";
    ListView_InsertColumn(hwndListView, 2, &lvc);

    /* Action buttons at bottom */
    y = rcPanel->bottom - BUTTON_HEIGHT - SPACING;
    CreateButton(hwndParent, IDC_BTN_COPY, "Salin",
        rcPanel->left, y, BUTTON_WIDTH, BUTTON_HEIGHT);
    CreateButton(hwndParent, IDC_BTN_EXPORT, "Ekspor CSV",
        rcPanel->left + BUTTON_WIDTH + SPACING, y, BUTTON_WIDTH, BUTTON_HEIGHT);
    CreateButton(hwndParent, IDC_BTN_PRINT, "Cetak Bukti",
        rcPanel->left + (BUTTON_WIDTH + SPACING) * 2, y, BUTTON_WIDTH, BUTTON_HEIGHT);
}

/*
 * PopulateDropdowns - Fill combo boxes with initial data
 */
void PopulateDropdowns(HWND hwndParent)
{
    int i;

    /* Populate Subject Type combo */
    for (i = 0; i < 8; i++) {
        SendDlgItemMessage(hwndParent, IDC_CMB_SUBJECT, CB_ADDSTRING,
            0, (LPARAM)SUBJECT_TYPE_NAMES[i]);
    }
    SendDlgItemMessage(hwndParent, IDC_CMB_SUBJECT, CB_SETCURSEL, 0, 0);  /* Default: Pegawai tetap */

    /* Populate PTKP Status combo */
    for (i = 0; i < 8; i++) {
        SendDlgItemMessage(hwndParent, IDC_CMB_PTKP, CB_ADDSTRING,
            0, (LPARAM)PTKP_STATUS_NAMES[i]);
    }
    SendDlgItemMessage(hwndParent, IDC_CMB_PTKP, CB_SETCURSEL, 4, 0);  /* Default: K/0 */

    /* Populate Scheme combo */
    for (i = 0; i < 2; i++) {
        SendDlgItemMessage(hwndParent, IDC_CMB_SCHEME, CB_ADDSTRING,
            0, (LPARAM)SCHEME_NAMES[i]);
    }
    SendDlgItemMessage(hwndParent, IDC_CMB_SCHEME, CB_SETCURSEL, 1, 0);  /* Default: TER */

    /* Populate TER Category combo */
    for (i = 0; i < 3; i++) {
        SendDlgItemMessage(hwndParent, IDC_CMB_TER_CATEGORY, CB_ADDSTRING,
            0, (LPARAM)TER_CATEGORY_NAMES[i]);
    }
    SendDlgItemMessage(hwndParent, IDC_CMB_TER_CATEGORY, CB_SETCURSEL, 1, 0);  /* Default: Kategori B */
}

/*
 * LayoutControls - Adjust control positions on resize
 */
void LayoutControls(HWND hwnd, int width, int height)
{
    RECT rcClient;
    int resultsLeft, resultsWidth;

    /* Avoid unreferenced parameter warnings */
    (void)hwnd;
    (void)width;
    (void)height;

    /* Calculate new positions */
    GetClientRect(hwnd, &rcClient);
    resultsLeft = MARGIN + PANEL_WIDTH + MARGIN;
    resultsWidth = rcClient.right - resultsLeft - MARGIN;

    /* TODO: Move and resize controls as needed */
    /* For now, controls stay at their original positions */
}
