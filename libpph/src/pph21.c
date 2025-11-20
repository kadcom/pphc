/*
 * PPH21 - PPh 21/26 Calculator
 * Copyright (c) 2025 OpenPajak Contributors
 */

#include <pph/pph_calculator.h>
#include "pph_internal.h"
#include <string.h>
#include <stdio.h>

/* Helper: Clamp months to 1-12 */
static int clamp_months(int months) {
    if (months < 1) return 1;
    if (months > 12) return 12;
    return months;
}

/* ============================================
   Pegawai Tetap (Permanent Employee)
   ============================================ */

static pph_result_t* calculate_pegawai_tetap(const pph21_input_t *input) {
    pph_result_t *result;
    int months;
    pph_money_t bruto_tahun, iuran_tahun, biaya_jabatan, netto_setahun;
    pph_money_t ptkp, pkp_rounded, pajak_setahun;
    char note[128];

    result = pph_result_create();
    if (!result) {
        pph_set_last_error("Memory allocation failed");
        return NULL;
    }

    months = clamp_months(input->months_paid);

    /* Annual calculations */
    bruto_tahun = pph_money_mul_int(input->bruto_monthly, months);

    /* Add all bonuses to annual bruto */
    if (input->bonuses != NULL && input->bonus_count > 0) {
        int i;
        for (i = 0; i < input->bonus_count; i++) {
            bruto_tahun = pph_money_add(bruto_tahun, input->bonuses[i].amount);
        }
    }

    iuran_tahun = pph_money_mul_int(input->pension_contribution, months);

    /* Biaya jabatan: min(5% * bruto, 6 juta) */
    biaya_jabatan = pph_money_percent(bruto_tahun, 5, 100);
    biaya_jabatan = pph_money_min(biaya_jabatan, PPH_RUPIAH(6000000));

    netto_setahun = pph_money_sub(
        pph_money_sub(
            pph_money_sub(bruto_tahun, biaya_jabatan),
            iuran_tahun),
        input->zakat_or_donation);

    ptkp = pph_get_ptkp(input->ptkp_status);
    pkp_rounded = pph_money_round_down_thousand(
        pph_money_floor(pph_money_sub(netto_setahun, ptkp)));

    if (input->scheme == PPH21_SCHEME_TER) {
        /* TER scheme with month-by-month calculation */
        pph_money_t monthly_income[12];
        pph_money_t monthly_ter[12];
        pph_money_t ter_paid, pajak_setahun, adjustment;
        int i, m;

        /* Initialize monthly income with base salary */
        for (i = 0; i < 12; i++) {
            monthly_income[i] = (i < months) ? input->bruto_monthly : PPH_ZERO;
        }

        /* Add bonuses to appropriate months */
        if (input->bonuses != NULL && input->bonus_count > 0) {
            for (i = 0; i < input->bonus_count; i++) {
                m = input->bonuses[i].month - 1;  /* Convert to 0-indexed */
                if (m >= 0 && m < 12 && m < months) {
                    monthly_income[m] = pph_money_add(monthly_income[m], input->bonuses[i].amount);
                }
            }
        }

        /* Calculate TER for each month (months 1-11 only) */
        ter_paid = PPH_ZERO;
        for (i = 0; i < 11 && i < months; i++) {
            pph_money_t ter_rate, month_tax;
            ter_rate = pph_get_ter_bulanan_rate(input->ter_category, monthly_income[i]);
            month_tax = pph_money_mul(monthly_income[i], ter_rate);
            monthly_ter[i] = month_tax;
            ter_paid = pph_money_add(ter_paid, month_tax);
        }

        /* Annual progressive tax (Pasal 17) */
        pajak_setahun = pph_calculate_pasal17(pkp_rounded);

        /* Month 12 adjustment */
        adjustment = pph_money_sub(pajak_setahun, ter_paid);

        /* Show TER withholding breakdown */
        pph_result_add_section(result, "Pemotongan TER (Bulan 1-11)");

        /* Group months by income for cleaner display */
        {
            int regular_count = 0;
            pph_money_t regular_total = PPH_ZERO;

            for (i = 0; i < 11 && i < months; i++) {
                /* Check if this month has bonus */
                int has_bonus = 0;
                if (input->bonuses != NULL && input->bonus_count > 0) {
                    int j;
                    for (j = 0; j < input->bonus_count; j++) {
                        if (input->bonuses[j].month == i + 1) {
                            has_bonus = 1;
                            break;
                        }
                    }
                }

                if (has_bonus) {
                    /* Show bonus month separately */
                    pph_money_t ter_rate = pph_get_ter_bulanan_rate(input->ter_category, monthly_income[i]);
                    char bonus_names[256] = "";

                    /* Collect bonus names for this month */
                    {
                        int j, first = 1;
                        for (j = 0; j < input->bonus_count; j++) {
                            if (input->bonuses[j].month == i + 1) {
                                if (!first) strcat(bonus_names, ", ");
                                strcat(bonus_names, input->bonuses[j].name);
                                first = 0;
                            }
                        }
                    }

                    snprintf(note, sizeof(note), "Bulan %d (%s)", i + 1, bonus_names);
                    pph_result_add_currency(result, note, monthly_income[i], NULL);
                    pph_result_add_percent(result, "  Tarif TER", ter_rate, NULL);
                    pph_result_add_currency(result, "  PPh 21 TER", monthly_ter[i], NULL);
                } else {
                    /* Regular month */
                    regular_count++;
                    regular_total = pph_money_add(regular_total, monthly_ter[i]);
                }
            }

            /* Show regular months summary */
            if (regular_count > 0) {
                pph_money_t ter_rate = pph_get_ter_bulanan_rate(input->ter_category, input->bruto_monthly);
                pph_money_t monthly_ter = pph_money_mul(input->bruto_monthly, ter_rate);
                snprintf(note, sizeof(note), "%d bulan reguler", regular_count);
                pph_result_add_currency(result, note, input->bruto_monthly, "per bulan");
                pph_result_add_percent(result, "  Tarif TER", ter_rate, NULL);
                pph_result_add_currency(result, "  PPh 21 TER per bulan", monthly_ter, NULL);
                pph_result_add_currency(result, "  Total PPh 21 TER", regular_total, NULL);
            }
        }

        pph_result_add_currency(result, "Total TER bulan 1-11", ter_paid, NULL);

        /* Show annual progressive calculation */
        pph_result_add_section(result, "Perhitungan Tahunan (Pasal 17)");
        pph_result_add_currency(result, "Bruto setahun", bruto_tahun, NULL);
        pph_result_add_currency(result, "Biaya jabatan (5%, maks 6 jt)", biaya_jabatan, NULL);
        pph_result_add_currency(result, "Netto setahun", netto_setahun, NULL);
        pph_result_add_currency(result, "PTKP", ptkp, NULL);
        pph_result_add_currency(result, "PKP (dibulatkan ribuan)", pkp_rounded, NULL);
        pph_result_add_currency(result, "PPh 21 setahun (progresif)", pajak_setahun, NULL);

        /* Show month 12 adjustment */
        pph_result_add_section(result, "Penyesuaian Bulan 12");
        pph_result_add_currency(result, "PPh 21 setahun", pajak_setahun, NULL);
        pph_result_add_currency(result, "TER telah dipotong (bln 1-11)", ter_paid, NULL);
        pph_result_add_currency(result, "Kurang/(lebih) bayar bulan 12", adjustment, NULL);

        result->total_tax = pajak_setahun;
    } else {
        /* Traditional Pasal 17 scheme */
        pajak_setahun = pph_calculate_pasal17(pkp_rounded);

        pph_result_add_section(result, "Penghasilan Bruto");
        pph_result_add_currency(result, "Gaji per bulan", input->bruto_monthly, NULL);
        snprintf(note, sizeof(note), "%d bulan", months);
        pph_result_add_currency(result, "Gaji setahun", pph_money_mul_int(input->bruto_monthly, months), note);
        if (input->bonuses != NULL && input->bonus_count > 0) {
            int i;
            for (i = 0; i < input->bonus_count; i++) {
                pph_result_add_currency(result, input->bonuses[i].name, input->bonuses[i].amount, NULL);
            }
        }
        pph_result_add_subtotal(result, "Total bruto", bruto_tahun);

        pph_result_add_section(result, "Pengurang");
        pph_result_add_currency(result, "Biaya jabatan (5%, maks 6 jt)", biaya_jabatan, NULL);
        pph_result_add_currency(result, "Iuran pensiun", iuran_tahun, NULL);
        if (input->zakat_or_donation.value > 0) {
            pph_result_add_currency(result, "Zakat/sumbangan", input->zakat_or_donation, NULL);
        }
        pph_result_add_subtotal(result, "Netto setahun", netto_setahun);

        pph_result_add_section(result, "PKP dan Pajak");
        pph_result_add_currency(result, "PTKP", ptkp, NULL);
        pph_result_add_currency(result, "PKP (dibulatkan ribuan)", pkp_rounded, NULL);
        pph_result_add_total(result, "PPh 21 setahun", pajak_setahun);

        result->total_tax = pajak_setahun;
    }

    return result;
}

/* ============================================
   Other Subject Types (Simplified)
   ============================================ */

static pph_result_t* calculate_simple(const pph21_input_t *input, const char *subject_name) {
    pph_result_t *result;
    pph_money_t tax;

    result = pph_result_create();
    if (!result) {
        pph_set_last_error("Memory allocation failed");
        return NULL;
    }

    /* Simple 5% flat rate for demonstration */
    tax = pph_money_percent(input->bruto_monthly, 5, 100);

    pph_result_add_section(result, subject_name);
    pph_result_add_currency(result, "Penghasilan bruto", input->bruto_monthly, NULL);
    pph_result_add_percent(result, "Tarif", PPH_MONEY(0, 500), "5%");
    pph_result_add_total(result, "PPh 21", tax);

    result->total_tax = tax;
    return result;
}

/* ============================================
   Main Entry Point
   ============================================ */

pph_result_t* pph21_calculate(const pph21_input_t *input) {
    if (input == NULL) {
        pph_set_last_error("Input is NULL");
        return NULL;
    }

    switch (input->subject_type) {
        case PPH21_PEGAWAI_TETAP:
            return calculate_pegawai_tetap(input);

        case PPH21_PENSIUNAN:
            return calculate_simple(input, "Pensiunan");

        case PPH21_PEGAWAI_TIDAK_TETAP:
            return calculate_simple(input, "Pegawai Tidak Tetap");

        case PPH21_BUKAN_PEGAWAI:
            return calculate_simple(input, "Bukan Pegawai");

        case PPH21_PESERTA_KEGIATAN:
            return calculate_simple(input, "Peserta Kegiatan");

        case PPH21_PROGRAM_PENSIUN:
            return calculate_simple(input, "Program Pensiun");

        case PPH21_MANTAN_PEGAWAI:
            return calculate_simple(input, "Mantan Pegawai");

        case PPH21_WPLN:
            return calculate_simple(input, "WPLN (PPh 26)");

        default:
            pph_set_last_error("Unknown subject type");
            return NULL;
    }
}
