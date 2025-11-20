/*
 * PPNBM - PPnBM (Luxury VAT) Calculator
 * Copyright (c) 2025 OpenPajak Contributors
 */

#include <pph/pph_calculator.h>
#include "pph_internal.h"

pph_result_t* ppnbm_calculate(const ppnbm_input_t *input) {
    pph_result_t *result;
    pph_money_t ppn, ppnbm, total;

    if (input == NULL) {
        pph_set_last_error("Input is NULL");
        return NULL;
    }

    result = pph_result_create();
    if (!result) {
        pph_set_last_error("Memory allocation failed");
        return NULL;
    }

    ppn = pph_money_mul(input->dpp, input->ppn_rate);
    ppnbm = pph_money_mul(input->dpp, input->ppnbm_rate);
    total = pph_money_add(ppn, ppnbm);

    pph_result_add_section(result, "PPN dan PPnBM");
    pph_result_add_currency(result, "DPP", input->dpp, NULL);
    pph_result_add_percent(result, "Tarif PPN", input->ppn_rate, NULL);
    pph_result_add_currency(result, "PPN", ppn, NULL);
    pph_result_add_percent(result, "Tarif PPnBM", input->ppnbm_rate, NULL);
    pph_result_add_currency(result, "PPnBM", ppnbm, NULL);
    pph_result_add_total(result, "Total PPN + PPnBM", total);

    result->total_tax = total;
    return result;
}
