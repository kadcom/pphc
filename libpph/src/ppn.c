/*
 * PPN - PPN (VAT) Calculator
 * Copyright (c) 2025 OpenPajak Contributors
 */

#include <pph/pph_calculator.h>
#include "pph_internal.h"

pph_result_t* ppn_calculate(const ppn_input_t *input) {
    pph_result_t *result;
    pph_money_t dpp, ppn;

    if (input == NULL) {
        pph_set_last_error("Input is NULL");
        return NULL;
    }

    result = pph_result_create();
    if (!result) {
        pph_set_last_error("Memory allocation failed");
        return NULL;
    }

    if (input->mode == PPN_MODE_INCLUSIVE) {
        /* Extract DPP from inclusive price: DPP = price / (1 + rate) */
        pph_money_t divisor;
        divisor.value = PPH_SCALE_FACTOR + input->rate.value;
        dpp = pph_money_div(pph_money_mul_int(input->dpp, PPH_SCALE_FACTOR), divisor.value);
        ppn = pph_money_sub(input->dpp, dpp);
    } else {
        /* Exclusive: PPN = DPP * rate */
        dpp = input->dpp;
        ppn = pph_money_mul(dpp, input->rate);
    }

    pph_result_add_section(result, "PPN");
    pph_result_add_currency(result, "DPP", dpp, NULL);
    pph_result_add_percent(result, "Tarif PPN", input->rate, NULL);
    pph_result_add_total(result, "PPN", ppn);

    result->total_tax = ppn;
    return result;
}
