/*
 * PPH4_2 - PPh Final Pasal 4(2) Calculator
 * Copyright (c) 2025 OpenPajak Contributors
 */

#include <pph/pph_calculator.h>
#include "pph_internal.h"

pph_result_t* pph4_2_calculate(const pph4_2_input_t *input) {
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

    pph_result_add_section(result, "PPh Final Pasal 4(2)");
    pph_result_add_currency(result, "Penghasilan bruto", input->bruto, NULL);
    pph_result_add_percent(result, "Tarif", input->rate, NULL);
    pph_result_add_total(result, "PPh Final Pasal 4(2)", tax);

    result->total_tax = tax;
    return result;
}
