/*
 * clipboard.c - Clipboard copy and export functionality
 * Handles copying results to clipboard and exporting to CSV
 */

#include "../include/pphcalc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commdlg.h>

/* Maximum text buffer size */
#define MAX_TEXT_SIZE (64 * 1024)  /* 64 KB */

/*
 * CopyResultsToClipboard - Copy breakdown table to clipboard
 */
void CopyResultsToClipboard(HWND hwnd)
{
    pph_result_t *result = g_appState.lastResult;
    char *text;
    size_t offset;
    pph_size_t i;
    HGLOBAL hMem;
    char *pMem;

    if (!result || result->breakdown_count == 0) {
        MessageBox(hwnd, "Tidak ada hasil untuk disalin", "Info", MB_OK | MB_ICONINFORMATION);
        return;
    }

    /* Allocate buffer for text */
    text = (char*)malloc(MAX_TEXT_SIZE);
    if (!text) {
        MessageBox(hwnd, "Gagal mengalokasi memori", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    /* Build text representation */
    offset = 0;
    offset += sprintf(text + offset, "Kalkulator PPh 21/26\r\n");
    offset += sprintf(text + offset, "========================================\r\n\r\n");

    /* Add breakdown rows */
    for (i = 0; i < result->breakdown_count && offset < MAX_TEXT_SIZE - 256; i++) {
        pph_breakdown_row_t *row = &result->breakdown[i];
        char valueStr[128];

        /* Format value */
        valueStr[0] = '\0';
        if (row->value_type == PPH_VALUE_CURRENCY) {
            if (pph_money_cmp(row->value, PPH_ZERO) != 0) {
                FormatRupiah(row->value, valueStr, sizeof(valueStr));
            }
        } else if (row->value_type == PPH_VALUE_PERCENT) {
            FormatPercent(row->value, valueStr, sizeof(valueStr));
        }

        /* Add row to text */
        if (row->variant == PPH_BREAKDOWN_SECTION) {
            offset += sprintf(text + offset, "\r\n%s\r\n", row->label);
        } else {
            if (valueStr[0] != '\0') {
                offset += sprintf(text + offset, "  %-40s %20s", row->label, valueStr);
                if (row->note[0] != '\0') {
                    offset += sprintf(text + offset, "  (%s)", row->note);
                }
                offset += sprintf(text + offset, "\r\n");
            } else {
                offset += sprintf(text + offset, "  %s\r\n", row->label);
            }
        }
    }

    /* Add total */
    {
        char totalStr[128];
        FormatRupiah(result->total_tax, totalStr, sizeof(totalStr));
        offset += sprintf(text + offset, "\r\n========================================\r\n");
        offset += sprintf(text + offset, "Total PPh 21/26: %s\r\n", totalStr);
    }

    /* Copy to clipboard */
    if (OpenClipboard(hwnd)) {
        EmptyClipboard();

        hMem = GlobalAlloc(GMEM_MOVEABLE, offset + 1);
        if (hMem) {
            pMem = (char*)GlobalLock(hMem);
            if (pMem) {
                memcpy(pMem, text, offset + 1);
                GlobalUnlock(hMem);
                SetClipboardData(CF_TEXT, hMem);
            }
        }

        CloseClipboard();
        MessageBox(hwnd, "Hasil berhasil disalin ke clipboard", "Sukses", MB_OK | MB_ICONINFORMATION);
    } else {
        MessageBox(hwnd, "Gagal membuka clipboard", "Error", MB_OK | MB_ICONERROR);
    }

    free(text);
}

/*
 * ExportResultsToCSV - Export breakdown to CSV file
 */
void ExportResultsToCSV(HWND hwnd)
{
    pph_result_t *result = g_appState.lastResult;
    OPENFILENAME ofn;
    char szFile[MAX_PATH];
    FILE *fp;
    pph_size_t i;

    if (!result || result->breakdown_count == 0) {
        MessageBox(hwnd, "Tidak ada hasil untuk diekspor", "Info", MB_OK | MB_ICONINFORMATION);
        return;
    }

    /* Initialize OPENFILENAME */
    ZeroMemory(&ofn, sizeof(ofn));
    szFile[0] = '\0';

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "CSV Files (*.csv)\0*.csv\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = "Ekspor ke CSV";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt = "csv";

    /* Show save dialog */
    if (!GetSaveFileName(&ofn)) {
        return;  /* User cancelled */
    }

    /* Open file for writing */
    fp = fopen(szFile, "w");
    if (!fp) {
        MessageBox(hwnd, "Gagal membuat file", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    /* Write CSV header */
    fprintf(fp, "Komponen,Nilai (Rp),Keterangan\r\n");

    /* Write breakdown rows */
    for (i = 0; i < result->breakdown_count; i++) {
        pph_breakdown_row_t *row = &result->breakdown[i];
        char valueStr[128];

        /* Format value */
        valueStr[0] = '\0';
        if (row->value_type == PPH_VALUE_CURRENCY) {
            if (pph_money_cmp(row->value, PPH_ZERO) != 0) {
                pph_money_to_string(row->value, valueStr, sizeof(valueStr));
            }
        } else if (row->value_type == PPH_VALUE_PERCENT) {
            FormatPercent(row->value, valueStr, sizeof(valueStr));
        }

        /* Write CSV row - escape commas in text */
        fprintf(fp, "\"%s\",\"%s\",\"%s\"\r\n", row->label, valueStr, row->note);
    }

    /* Write total */
    {
        char totalStr[128];
        pph_money_to_string(result->total_tax, totalStr, sizeof(totalStr));
        fprintf(fp, "\"Total PPh 21/26\",\"%s\",\"\"\r\n", totalStr);
    }

    fclose(fp);

    MessageBox(hwnd, "Hasil berhasil diekspor", "Sukses", MB_OK | MB_ICONINFORMATION);
}

/*
 * PrintResults - Print results (placeholder)
 */
void PrintResults(HWND hwnd)
{
    /* TODO: Implement print functionality */
    MessageBox(hwnd, "Fitur cetak belum diimplementasikan", "Info", MB_OK | MB_ICONINFORMATION);

    /* For full implementation, would use:
     * - CreateDC with WINSPOOL printer
     * - StartDoc, StartPage, EndPage, EndDoc
     * - Draw breakdown table to printer DC
     */

    (void)hwnd;  /* Avoid warning */
}
