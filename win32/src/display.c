/*
 * display.c - Results display functions
 * Handles displaying calculation results in ListView and summary boxes
 */

#include "../include/pphcalc.h"
#include <stdio.h>
#include <string.h>

/*
 * DisplayResults - Display calculation results
 */
void DisplayResults(HWND hwnd, pph_result_t* result)
{
    if (!result) {
        ClearResults(hwnd);
        return;
    }

    /* Display summary boxes */
    DisplaySummary(hwnd, result);

    /* Display breakdown table */
    DisplayBreakdown(hwnd, result);
}

/*
 * DisplaySummary - Update summary static controls
 */
void DisplaySummary(HWND hwnd, pph_result_t* result)
{
    char buffer[128];

    /* Format total tax */
    FormatRupiah(result->total_tax, buffer, sizeof(buffer));
    SetDlgItemText(hwnd, IDC_STATIC_TOTAL_TAX, buffer);

    /* Calculate TER monthly and December balance from breakdown */
    /* For now, show total tax divided by 12 (simplified) */
    {
        pph_money_t monthly = pph_money_div(result->total_tax, 12);
        FormatRupiah(monthly, buffer, sizeof(buffer));
        SetDlgItemText(hwnd, IDC_STATIC_TER_MONTHLY, buffer);
    }

    /* December balance (simplified) */
    SetDlgItemText(hwnd, IDC_STATIC_DEC_BALANCE, "Rp 0");

    /* Calculate Take-Home Pay directly from bruto and tax */
    /* THP = Total Bruto - Total Tax */
    {
        pph_money_t brutoAnnual = PPH_ZERO;
        pph_money_t thpAnnual, thpMonthly;
        pph_size_t i;
        int months = 12;

        /* Find total bruto from breakdown - search more broadly */
        for (i = 0; i < result->breakdown_count; i++) {
            pph_breakdown_row_t *row = &result->breakdown[i];
            /* Look for any variation of "bruto" total */
            if ((strstr(row->label, "bruto") != NULL && strstr(row->label, "Total") != NULL) ||
                strstr(row->label, "Total bruto") != NULL ||
                strstr(row->label, "bruto setahun") != NULL ||
                strstr(row->label, "Gaji pokok") != NULL) {
                /* Take the largest value as total bruto */
                if (pph_money_cmp(row->value, brutoAnnual) > 0) {
                    brutoAnnual = row->value;
                }
            }
        }

        /* If still not found, try to get from penghasilan */
        if (pph_money_cmp(brutoAnnual, PPH_ZERO) == 0) {
            for (i = 0; i < result->breakdown_count; i++) {
                pph_breakdown_row_t *row = &result->breakdown[i];
                if (strstr(row->label, "Penghasilan") != NULL &&
                    pph_money_cmp(row->value, PPH_ZERO) > 0) {
                    if (pph_money_cmp(row->value, brutoAnnual) > 0) {
                        brutoAnnual = row->value;
                    }
                }
            }
        }

        /* If we found bruto, calculate THP */
        if (pph_money_cmp(brutoAnnual, PPH_ZERO) > 0) {
            /* THP = Bruto - Tax */
            thpAnnual = pph_money_sub(brutoAnnual, result->total_tax);
            thpMonthly = pph_money_div(thpAnnual, months);

            FormatRupiah(thpAnnual, buffer, sizeof(buffer));
            SetDlgItemText(hwnd, IDC_STATIC_THP_ANNUAL, buffer);

            FormatRupiah(thpMonthly, buffer, sizeof(buffer));
            SetDlgItemText(hwnd, IDC_STATIC_THP_MONTHLY, buffer);
        } else {
            SetDlgItemText(hwnd, IDC_STATIC_THP_ANNUAL, "Rp 0");
            SetDlgItemText(hwnd, IDC_STATIC_THP_MONTHLY, "Rp 0");
        }
    }
}

/*
 * DisplayBreakdown - Populate ListView with breakdown rows
 */
void DisplayBreakdown(HWND hwnd, pph_result_t* result)
{
    HWND hwndListView;
    LVITEM lvi;
    pph_size_t i;
    char valueStr[128];
    int itemIndex;

    hwndListView = GetDlgItem(hwnd, IDC_LISTVIEW_BREAKDOWN);
    if (!hwndListView) return;

    /* Clear existing items */
    ListView_DeleteAllItems(hwndListView);

    /* Populate with breakdown rows */
    for (i = 0; i < result->breakdown_count; i++) {
        pph_breakdown_row_t *row = &result->breakdown[i];

        /* Format value based on type */
        valueStr[0] = '\0';
        if (row->value_type == PPH_VALUE_CURRENCY) {
            if (pph_money_cmp(row->value, PPH_ZERO) != 0) {
                FormatRupiah(row->value, valueStr, sizeof(valueStr));
            }
        } else if (row->value_type == PPH_VALUE_PERCENT) {
            FormatPercent(row->value, valueStr, sizeof(valueStr));
        }
        /* else PPH_VALUE_TEXT - leave empty */

        /* Insert item - Column 0: Label */
        memset(&lvi, 0, sizeof(lvi));
        lvi.mask = LVIF_TEXT | LVIF_PARAM;
        lvi.iItem = (int)i;
        lvi.iSubItem = 0;
        lvi.pszText = row->label;
        lvi.lParam = (LPARAM)row->variant;  /* Store variant for custom draw */

        itemIndex = ListView_InsertItem(hwndListView, &lvi);

        /* Column 1: Value */
        if (valueStr[0] != '\0') {
            ListView_SetItemText(hwndListView, itemIndex, 1, valueStr);
        }

        /* Column 2: Note */
        if (row->note[0] != '\0') {
            ListView_SetItemText(hwndListView, itemIndex, 2, row->note);
        }

        /* Apply styling based on variant */
        /* Note: Custom draw would be needed for full styling */
        /* For simplicity, we'll use item state for basic styling */
        if (row->variant == PPH_BREAKDOWN_SECTION ||
            row->variant == PPH_BREAKDOWN_TOTAL) {
            /* Mark for bold rendering (would need custom draw) */
        }
    }
}

/*
 * ClearResults - Clear all result displays
 */
void ClearResults(HWND hwnd)
{
    HWND hwndListView;

    /* Clear summary */
    SetDlgItemText(hwnd, IDC_STATIC_TOTAL_TAX, "Rp 0");
    SetDlgItemText(hwnd, IDC_STATIC_TER_MONTHLY, "Rp 0");
    SetDlgItemText(hwnd, IDC_STATIC_DEC_BALANCE, "Rp 0");
    SetDlgItemText(hwnd, IDC_STATIC_THP_ANNUAL, "Rp 0");
    SetDlgItemText(hwnd, IDC_STATIC_THP_MONTHLY, "Rp 0");

    /* Clear ListView */
    hwndListView = GetDlgItem(hwnd, IDC_LISTVIEW_BREAKDOWN);
    if (hwndListView) {
        ListView_DeleteAllItems(hwndListView);
    }

    /* Free stored result */
    if (g_appState.lastResult) {
        pph_result_free(g_appState.lastResult);
        g_appState.lastResult = NULL;
    }
}

/*
 * DrawSummaryBox - Custom draw a summary box (for future use)
 */
void DrawSummaryBox(HDC hdc, RECT* rc, const char* label, const char* value)
{
    HBRUSH hBrush;
    HPEN hPen;
    RECT rcText;
    HFONT hOldFont;
    int oldBkMode;

    /* Draw background */
    hBrush = CreateSolidBrush(COLOR_SUMMARY_BG);
    hPen = CreatePen(PS_SOLID, 2, COLOR_SUMMARY_BORDER);

    SelectObject(hdc, hBrush);
    SelectObject(hdc, hPen);
    Rectangle(hdc, rc->left, rc->top, rc->right, rc->bottom);

    /* Draw label */
    oldBkMode = SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, COLOR_TEXT_LABEL);
    hOldFont = (HFONT)SelectObject(hdc, g_appState.hFontLabel);

    rcText = *rc;
    rcText.top += 8;
    rcText.left += 12;
    DrawText(hdc, label, -1, &rcText, DT_LEFT | DT_TOP | DT_SINGLELINE);

    /* Draw value */
    SetTextColor(hdc, COLOR_TEXT_AMOUNT);
    SelectObject(hdc, g_appState.hFontAmount);

    rcText.top += 20;
    DrawText(hdc, value, -1, &rcText, DT_LEFT | DT_TOP | DT_SINGLELINE);

    /* Cleanup */
    SelectObject(hdc, hOldFont);
    SetBkMode(hdc, oldBkMode);
    DeleteObject(hBrush);
    DeleteObject(hPen);
}
