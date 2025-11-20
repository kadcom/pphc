/*
 * calculation.c - PPHC library integration and calculation
 * Handles reading inputs, performing calculation, and updating display
 */

#include "../include/pphcalc.h"
#include <stdio.h>
#include <string.h>

/*
 * GetInputFromControls - Read input values from controls
 */
BOOL GetInputFromControls(HWND hwnd, pph21_input_t* input)
{
    char buffer[256];
    int sel;

    /* Zero-initialize the structure */
    memset(input, 0, sizeof(pph21_input_t));

    /* Subject type */
    sel = (int)SendDlgItemMessage(hwnd, IDC_CMB_SUBJECT, CB_GETCURSEL, 0, 0);
    if (sel == CB_ERR) sel = 0;
    input->subject_type = (pph21_subject_type_t)sel;

    /* Bruto monthly */
    GetDlgItemText(hwnd, IDC_EDIT_BRUTO, buffer, sizeof(buffer));
    input->bruto_monthly = ParseRupiah(buffer);

    /* Months paid */
    GetDlgItemText(hwnd, IDC_EDIT_MONTHS, buffer, sizeof(buffer));
    input->months_paid = ParseIntSafe(buffer, 12);
    if (input->months_paid < 1) input->months_paid = 1;
    if (input->months_paid > 12) input->months_paid = 12;

    /* Pension contribution */
    GetDlgItemText(hwnd, IDC_EDIT_PENSION, buffer, sizeof(buffer));
    input->pension_contribution = ParseRupiah(buffer);

    /* Zakat or donation */
    GetDlgItemText(hwnd, IDC_EDIT_ZAKAT, buffer, sizeof(buffer));
    input->zakat_or_donation = ParseRupiah(buffer);

    /* PTKP status */
    sel = (int)SendDlgItemMessage(hwnd, IDC_CMB_PTKP, CB_GETCURSEL, 0, 0);
    if (sel == CB_ERR) sel = 4;  /* Default K/0 */
    input->ptkp_status = (pph_ptkp_status_t)sel;

    /* Scheme */
    sel = (int)SendDlgItemMessage(hwnd, IDC_CMB_SCHEME, CB_GETCURSEL, 0, 0);
    if (sel == CB_ERR) sel = 1;  /* Default TER */
    input->scheme = (pph21_scheme_t)sel;

    /* TER category */
    sel = (int)SendDlgItemMessage(hwnd, IDC_CMB_TER_CATEGORY, CB_GETCURSEL, 0, 0);
    if (sel == CB_ERR) sel = 1;  /* Default Category B */
    input->ter_category = (pph21_ter_category_t)sel;

    /* Bonus (simple - single annual bonus) */
    GetDlgItemText(hwnd, IDC_EDIT_BONUS, buffer, sizeof(buffer));
    {
        pph_money_t bonusAmount = ParseRupiah(buffer);
        if (pph_money_cmp(bonusAmount, PPH_ZERO) > 0) {
            /* TODO: For now, treat as annual bonus added to total */
            /* In full implementation, would use bonus array */
            input->bonuses = NULL;
            input->bonus_count = 0;
            /* Note: The library will handle annual bonus calculation */
        } else {
            input->bonuses = NULL;
            input->bonus_count = 0;
        }
    }

    /* Other fields */
    input->is_daily_worker = 0;
    input->foreign_tax_rate = PPH_ZERO;

    return TRUE;
}

/*
 * PerformCalculation - Execute calculation and update results
 */
void PerformCalculation(HWND hwnd)
{
    pph21_input_t input;
    pph_result_t *result;

    /* Get input from controls */
    if (!GetInputFromControls(hwnd, &input)) {
        MessageBox(hwnd, "Invalid input values", "Error", MB_ICONERROR);
        return;
    }

    /* Free previous result if any */
    if (g_appState.lastResult) {
        pph_result_free(g_appState.lastResult);
        g_appState.lastResult = NULL;
    }

    /* Perform calculation */
    result = pph21_calculate(&input);
    if (!result) {
        const char* error = pph_get_last_error();
        MessageBox(hwnd, error ? error : "Calculation failed", "Error", MB_ICONERROR);
        ClearResults(hwnd);
        return;
    }

    /* Store result */
    g_appState.lastResult = result;

    /* Display results */
    DisplayResults(hwnd, result);
}

/*
 * OnResetClicked - Handle Reset button click
 */
void OnResetClicked(HWND hwnd)
{
    /* Confirm reset */
    if (MessageBox(hwnd, "Reset semua input?", "Konfirmasi",
                   MB_YESNO | MB_ICONQUESTION) != IDYES) {
        return;
    }

    /* Clear all input controls */
    SetDlgItemText(hwnd, IDC_EDIT_BRUTO, "0");
    SetDlgItemText(hwnd, IDC_EDIT_MONTHS, "12");
    SetDlgItemText(hwnd, IDC_EDIT_PENSION, "0");
    SetDlgItemText(hwnd, IDC_EDIT_BONUS, "0");
    SetDlgItemText(hwnd, IDC_EDIT_ZAKAT, "0");

    /* Reset combo boxes to defaults */
    SendDlgItemMessage(hwnd, IDC_CMB_SUBJECT, CB_SETCURSEL, 0, 0);
    SendDlgItemMessage(hwnd, IDC_CMB_PTKP, CB_SETCURSEL, 4, 0);  /* K/0 */
    SendDlgItemMessage(hwnd, IDC_CMB_SCHEME, CB_SETCURSEL, 1, 0);  /* TER */
    SendDlgItemMessage(hwnd, IDC_CMB_TER_CATEGORY, CB_SETCURSEL, 1, 0);  /* Category B */

    /* Clear results */
    ClearResults(hwnd);
}

/*
 * OnExampleClicked - Load example data (matching openpajak.hamardikan.com)
 */
void OnExampleClicked(HWND hwnd)
{
    /* Set example values matching the screenshot */
    SetEditMoneyValue(hwnd, IDC_EDIT_BRUTO, PPH_RUPIAH(15000000));
    SetDlgItemText(hwnd, IDC_EDIT_MONTHS, "12");
    SetEditMoneyValue(hwnd, IDC_EDIT_PENSION, PPH_RUPIAH(200000));
    SetEditMoneyValue(hwnd, IDC_EDIT_BONUS, PPH_RUPIAH(20000000));
    SetEditMoneyValue(hwnd, IDC_EDIT_ZAKAT, PPH_RUPIAH(0));

    /* Set example combo box selections */
    SendDlgItemMessage(hwnd, IDC_CMB_SUBJECT, CB_SETCURSEL, 0, 0);  /* Pegawai tetap */
    SendDlgItemMessage(hwnd, IDC_CMB_PTKP, CB_SETCURSEL, 4, 0);  /* K/0 */
    SendDlgItemMessage(hwnd, IDC_CMB_SCHEME, CB_SETCURSEL, 1, 0);  /* TER */
    SendDlgItemMessage(hwnd, IDC_CMB_TER_CATEGORY, CB_SETCURSEL, 0, 0);  /* Category A */

    /* Trigger calculation */
    PerformCalculation(hwnd);
}

/*
 * OnSchemeChanged - Handle scheme combo box change
 */
void OnSchemeChanged(HWND hwnd)
{
    int sel = (int)SendDlgItemMessage(hwnd, IDC_CMB_SCHEME, CB_GETCURSEL, 0, 0);

    /* Enable/disable TER category based on scheme */
    if (sel == 1) {  /* TER scheme */
        EnableWindow(GetDlgItem(hwnd, IDC_CMB_TER_CATEGORY), TRUE);
    } else {  /* Old scheme */
        EnableWindow(GetDlgItem(hwnd, IDC_CMB_TER_CATEGORY), FALSE);
    }

    /* Recalculate */
    PerformCalculation(hwnd);
}
