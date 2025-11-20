/*
 * PPH23 - PPh 23 Calculator
 * Copyright (c) 2025 OpenPajak Contributors
 */

#include <pph/pph_calculator.h>
#include "pph_internal.h"

pph_result_t* pph23_calculate(const pph23_input_t *input) {
    pph_result_t *result;
    pph_money_t tax;

    if (input == NULL) {
        pph_set_last_error("Input is NULL");
        return NULL;
    }

    result = pph_result_create();
    if (!result) {
        pph_set_last_error("Memory allocation failed");
        return NULL;
    }

    tax = pph_money_mul(input->bruto, input->rate);

    pph_result_add_section(result, "PPh 23");
    pph_result_add_currency(result, "Penghasilan bruto", input->bruto, NULL);
    pph_result_add_percent(result, "Tarif", input->rate, NULL);
    pph_result_add_total(result, "PPh 23", tax);

    result->total_tax = tax;
    return result;
}
