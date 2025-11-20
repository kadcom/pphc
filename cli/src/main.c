/*
 * PPHC - Command-line tax calculator
 * Copyright (c) 2025 OpenPajak Contributors
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pph/pph_calculator.h>

static void print_version(void) {
    printf(
        "pphc version %s\n"
        "Indonesian Tax Calculator Library\n", 
        pph_get_version());
}

static void print_usage(void) {
    printf(
        "Usage: pphc <command> [options]\n\n"
        "Commands:\n"
        "  pph21    Calculate PPh 21/26\n"
        "  pph22    Calculate PPh 22\n"
        "  pph23    Calculate PPh 23\n"
        "  pph4-2   Calculate PPh Final Pasal 4(2)\n"
        "  ppn      Calculate PPN\n"
        "  ppnbm    Calculate PPnBM\n"
        "  version  Show version information\n"
        "  help     Show this help message\n"
    );
}

static void print_breakdown(pph_result_t *result) {
    char buf[64];
    pph_size_t i;

    printf(
        "\n========================================\n"
        "  Tax Calculation Result\n"
        "========================================\n\n"
    );

    for (i = 0; i < result->breakdown_count; i++) {
        pph_breakdown_row_t *row = &result->breakdown[i];

        if (row->variant == PPH_BREAKDOWN_SECTION) {
            printf("\n>>> %s\n", row->label);
        } else {
            printf("  %-40s ", row->label);

            if (row->value_type == PPH_VALUE_CURRENCY) {
                pph_money_to_string_formatted(row->value, buf, sizeof(buf));
                printf("%15s", buf);
            } else if (row->value_type == PPH_VALUE_PERCENT) {
                pph_percent_to_string(row->value, buf, sizeof(buf));
                printf("%15s", buf);
            }

            if (row->note[0] != '\0') {
                printf("  (%s)", row->note);
            }
            printf("\n");

            if (row->variant == PPH_BREAKDOWN_TOTAL) {
                printf("========================================\n");
            }
        }
    }

    printf("\nTotal Tax: ");
    pph_money_to_string_formatted(result->total_tax, buf, sizeof(buf));
    printf("%s IDR\n\n", buf);
}

int main(int argc, char *argv[]) {
    pph_result_t *result;

    pph_init();

    if (argc < 2) {
        print_usage();
        return 1;
    }

    if (strcmp(argv[1], "version") == 0) {
        print_version();
        return 0;
    }

    if (strcmp(argv[1], "help") == 0) {
        print_usage();
        return 0;
    }

    if (strcmp(argv[1], "pph21") == 0) {
        /* Example PPh21 calculation */
        pph21_input_t input;

        memset(&input, 0, sizeof(input));
        input.subject_type = PPH21_PEGAWAI_TETAP;
        input.bruto_monthly = PPH_RUPIAH(10000000);
        input.months_paid = 12;
        input.pension_contribution = PPH_RUPIAH(100000);
        input.zakat_or_donation = PPH_ZERO;
        input.ptkp_status = PPH_PTKP_TK0;
        input.scheme = PPH21_SCHEME_TER;
        input.ter_category = PPH21_TER_CATEGORY_A;
        input.bonuses = NULL;
        input.bonus_count = 0;
        input.is_daily_worker = 0;

        result = pph21_calculate(&input);
        if (result) {
            print_breakdown(result);
            pph_result_free(result);
            return 0;
        } else {
            fprintf(stderr, "Error: %s\n", pph_get_last_error());
            return 1;
        }
    }

    fprintf(stderr, "Unknown command: %s\n", argv[1]);
    print_usage();
    return 1;
}
