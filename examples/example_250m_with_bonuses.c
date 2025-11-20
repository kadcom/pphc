/*
 * Example: PPh21 calculation for 250M IDR monthly salary with bonuses
 * - Monthly salary: 250,000,000 IDR
 * - THR (March): 250,000,000 IDR
 * - Annual bonus (April): 100,000,000 IDR
 * - PTKP: K/2 (Married with 2 dependents)
 * - TER Category: B (with NPWP)
 */

#include <pph/pph_calculator.h>
#include <stdio.h>

int main(void) {
    pph21_input_t input = {0};
    pph_result_t *result;
    char buffer[64];
    pph_size_t i;

    /* Define bonuses */
    pph21_bonus_t bonuses[] = {
        {3, PPH_RUPIAH(250000000), "THR"},
        {4, PPH_RUPIAH(100000000), "Bonus Tahunan"}
    };

    pph_init();

    /* Configure calculation */
    input.subject_type = PPH21_PEGAWAI_TETAP;
    input.bruto_monthly = PPH_RUPIAH(250000000);     /* 250 million IDR per month */
    input.months_paid = 12;
    input.pension_contribution = PPH_RUPIAH(100000); /* 100k IDR per month */
    input.zakat_or_donation = PPH_ZERO;
    input.ptkp_status = PPH_PTKP_K2;                 /* Married with 2 dependents */
    input.scheme = PPH21_SCHEME_TER;                 /* Use TER scheme */
    input.ter_category = PPH21_TER_CATEGORY_B;       /* With NPWP */
    input.bonuses = bonuses;
    input.bonus_count = 2;
    input.is_daily_worker = 0;

    /* Calculate */
    result = pph21_calculate(&input);
    if (!result) {
        fprintf(stderr, "Calculation failed: %s\n", pph_get_last_error());
        return 1;
    }

    printf("PPh21 Calculation Example\n");
    printf("=========================\n\n");

    /* Print breakdown */
    for (i = 0; i < result->breakdown_count; i++) {
        pph_breakdown_row_t *item = &result->breakdown[i];

        if (item->variant == PPH_BREAKDOWN_SECTION) {
            printf("\n%s\n", item->label);
        } else if (item->value_type == PPH_VALUE_CURRENCY) {
            pph_money_to_string_formatted(item->value, buffer, sizeof(buffer));
            if (item->note[0] != '\0') {
                printf("  %s: %s IDR (%s)\n", item->label, buffer, item->note);
            } else {
                printf("  %s: %s IDR\n", item->label, buffer);
            }
        } else if (item->value_type == PPH_VALUE_PERCENT) {
            pph_percent_to_string(item->value, buffer, sizeof(buffer));
            printf("  %s: %s\n", item->label, buffer);
        }
    }

    printf("\n");
    pph_money_to_string_formatted(result->total_tax, buffer, sizeof(buffer));
    printf("Total Tax: %s IDR\n", buffer);

    /* Clean up */
    pph_result_free(result);
    return 0;
}
