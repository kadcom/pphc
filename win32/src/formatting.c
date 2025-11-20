/*
 * formatting.c - Money formatting and parsing utilities
 * Handles conversion between strings and pph_money_t
 */

#include "../include/pphcalc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 * FormatRupiah - Format pph_money_t as "Rp 10,000,000"
 */
void FormatRupiah(pph_money_t money, char* buffer, size_t size)
{
    char temp[64];
    char formatted[64];

    /* Convert to string using PPHC library */
    pph_money_to_string(money, temp, sizeof(temp));

    /* Parse the value (format: "10000000.0000") */
    {
        char *dot = strchr(temp, '.');
        char wholePart[64];

        if (dot) {
            size_t len = (size_t)(dot - temp);
            strncpy(wholePart, temp, len);
            wholePart[len] = '\0';
        } else {
            strcpy(wholePart, temp);
        }

        /* Add thousand separators */
        FormatThousandSeparator(wholePart, formatted, sizeof(formatted));
    }

    /* Add "Rp " prefix */
    if (money.value < 0) {
        snprintf(buffer, size, "-Rp %s", formatted + 1);  /* Skip negative sign */
    } else {
        snprintf(buffer, size, "Rp %s", formatted);
    }
}

/*
 * FormatRupiahWithLabel - Format with label: "Rp 10,000,000.00"
 */
void FormatRupiahWithLabel(pph_money_t money, char* buffer, size_t size)
{
    FormatRupiah(money, buffer, size);
}

/*
 * FormatPercent - Format as percentage "5.00%"
 */
void FormatPercent(pph_money_t percent, char* buffer, size_t size)
{
    char temp[64];
    pph_percent_to_string(percent, temp, sizeof(temp));
    snprintf(buffer, size, "%s", temp);
}

/*
 * ParseRupiah - Parse string to pph_money_t
 * Accepts: "10000000", "10,000,000", "10.000.000"
 */
pph_money_t ParseRupiah(const char* str)
{
    char cleaned[256];
    RemoveThousandSeparator(str, cleaned, sizeof(cleaned));
    return pph_money_from_string(cleaned);
}

/*
 * FormatThousandSeparator - Add thousand separators
 * Input: "10000000" -> Output: "10,000,000"
 */
void FormatThousandSeparator(const char* input, char* output, size_t outSize)
{
    int len = (int)strlen(input);
    int outPos = 0;
    int count = 0;
    int i;
    int isNegative = 0;

    /* Check for negative */
    if (input[0] == '-') {
        output[outPos++] = '-';
        input++;
        len--;
        isNegative = 1;
    }

    /* Process from right to left */
    for (i = len - 1; i >= 0; i--) {
        if (count == 3) {
            output[outPos++] = ',';
            count = 0;
        }
        output[outPos++] = input[i];
        count++;
    }

    output[outPos] = '\0';

    /* Reverse the string */
    {
        int start = isNegative ? 1 : 0;
        int end = outPos - 1;
        while (start < end) {
            char temp = output[start];
            output[start] = output[end];
            output[end] = temp;
            start++;
            end--;
        }
    }
}

/*
 * RemoveThousandSeparator - Remove thousand separators and dots
 * Input: "10,000,000" or "10.000.000" -> Output: "10000000"
 */
void RemoveThousandSeparator(const char* input, char* output, size_t outSize)
{
    size_t outPos = 0;
    size_t i;

    for (i = 0; input[i] != '\0' && outPos < outSize - 1; i++) {
        char c = input[i];
        /* Skip commas, dots (thousand separators), and spaces */
        if (c != ',' && c != ' ' && !(c == '.' && isdigit(input[i+1]))) {
            output[outPos++] = c;
        }
    }
    output[outPos] = '\0';
}

/*
 * GetEditMoneyValue - Get pph_money_t from edit control
 */
void GetEditMoneyValue(HWND hwnd, int controlId, pph_money_t* outValue)
{
    char buffer[256];
    GetDlgItemText(hwnd, controlId, buffer, sizeof(buffer));

    if (buffer[0] == '\0') {
        *outValue = PPH_ZERO;
    } else {
        *outValue = ParseRupiah(buffer);
    }
}

/*
 * SetEditMoneyValue - Set edit control from pph_money_t
 */
void SetEditMoneyValue(HWND hwnd, int controlId, pph_money_t value)
{
    char buffer[64];
    char temp[64];

    /* Convert to string */
    pph_money_to_string(value, temp, sizeof(temp));

    /* Extract whole part only (before decimal point) */
    {
        char *dot = strchr(temp, '.');
        if (dot) {
            *dot = '\0';
        }
    }

    /* Add thousand separators */
    FormatThousandSeparator(temp, buffer, sizeof(buffer));

    /* Set to control */
    SetDlgItemText(hwnd, controlId, buffer);
}

/*
 * FormatMoneyForDisplay - Format for display in static controls
 * Returns: "Rp 10,000,000"
 */
void FormatMoneyForDisplay(pph_money_t money, char* buffer, size_t size)
{
    FormatRupiah(money, buffer, size);
}

/*
 * ParseIntSafe - Safely parse integer with default value
 */
int ParseIntSafe(const char* str, int defaultValue)
{
    char cleaned[64];
    int value;

    if (!str || str[0] == '\0') {
        return defaultValue;
    }

    /* Remove non-digits */
    RemoveThousandSeparator(str, cleaned, sizeof(cleaned));

    value = atoi(cleaned);
    return value > 0 ? value : defaultValue;
}
