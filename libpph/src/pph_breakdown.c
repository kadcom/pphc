/*
 * PPH Breakdown - Breakdown generation and management
 * Copyright (c) 2025 OpenPajak Contributors
 */

#include <pph/pph_calculator.h>
#include "pph_internal.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Initial capacity for breakdown array */
#define INITIAL_BREAKDOWN_CAPACITY 64

/* ============================================
   Custom Allocator Support
   ============================================ */

/* Default allocator using standard library */
static void* default_malloc(pph_size_t size) {
    return malloc((size_t)size);
}

static void* default_realloc(void* ptr, pph_size_t size) {
    return realloc(ptr, (size_t)size);
}

static void default_free(void* ptr) {
    free(ptr);
}

static const pph_allocator_t default_allocator = {
    default_malloc,
    default_realloc,
    default_free
};

/* Current allocator (points to default by default) */
static const pph_allocator_t *current_allocator = &default_allocator;

/* Set custom allocator */
void pph_set_custom_allocator(
    void* (*malloc_fn)(pph_size_t),
    void* (*realloc_fn)(void*, pph_size_t),
    void  (*free_fn)(void*)
) {
    static pph_allocator_t custom_allocator;

    if (malloc_fn == NULL || realloc_fn == NULL || free_fn == NULL) {
        /* Reset to default if any NULL */
        current_allocator = &default_allocator;
        return;
    }

    custom_allocator.malloc_fn = malloc_fn;
    custom_allocator.realloc_fn = realloc_fn;
    custom_allocator.free_fn = free_fn;

    current_allocator = &custom_allocator;
}

/* Allocator wrapper functions */
void* pph_malloc(pph_size_t size) {
    return current_allocator->malloc_fn(size);
}

void* pph_realloc(void* ptr, pph_size_t size) {
    return current_allocator->realloc_fn(ptr, size);
}

void pph_free(void* ptr) {
    if (ptr != NULL) {
        current_allocator->free_fn(ptr);
    }
}

/* ============================================
   Result Management
   ============================================ */

pph_result_t* pph_result_create(void) {
    pph_result_t *result;

    result = (pph_result_t*)pph_malloc(sizeof(pph_result_t));
    if (result == NULL) {
        return NULL;
    }

    result->total_tax.value = 0;
    result->breakdown_count = 0;
    result->breakdown_capacity = INITIAL_BREAKDOWN_CAPACITY;

    result->breakdown = (pph_breakdown_row_t*)pph_malloc(
        sizeof(pph_breakdown_row_t) * result->breakdown_capacity);

    if (result->breakdown == NULL) {
        pph_free(result);
        return NULL;
    }

    return result;
}

void pph_result_free(pph_result_t *result) {
    if (result == NULL) {
        return;
    }

    if (result->breakdown != NULL) {
        pph_free(result->breakdown);
    }

    pph_free(result);
}

/* ============================================
   Breakdown Row Management
   ============================================ */

static int pph_result_ensure_capacity(pph_result_t *result) {
    pph_breakdown_row_t *new_breakdown;
    pph_size_t new_capacity;

    if (result->breakdown_count < result->breakdown_capacity) {
        return 1;  /* Success */
    }

    /* Double the capacity */
    new_capacity = result->breakdown_capacity * 2;
    new_breakdown = (pph_breakdown_row_t*)pph_realloc(
        result->breakdown,
        sizeof(pph_breakdown_row_t) * new_capacity);

    if (new_breakdown == NULL) {
        return 0;  /* Failure */
    }

    result->breakdown = new_breakdown;
    result->breakdown_capacity = new_capacity;

    return 1;  /* Success */
}

int pph_result_add_row(pph_result_t *result,
                       const char *label,
                       pph_money_t value,
                       pph_value_type_t value_type,
                       const char *note,
                       pph_breakdown_variant_t variant) {
    pph_breakdown_row_t *row;

    if (result == NULL) {
        return 0;
    }

    if (!pph_result_ensure_capacity(result)) {
        return 0;
    }

    row = &result->breakdown[result->breakdown_count];

    /* Copy label */
    strncpy(row->label, label ? label : "", sizeof(row->label) - 1);
    row->label[sizeof(row->label) - 1] = '\0';

    /* Set value */
    row->value = value;
    row->value_type = value_type;

    /* Copy note */
    strncpy(row->note, note ? note : "", sizeof(row->note) - 1);
    row->note[sizeof(row->note) - 1] = '\0';

    /* Set variant */
    row->variant = variant;

    result->breakdown_count++;

    return 1;  /* Success */
}

int pph_result_add_section(pph_result_t *result, const char *label) {
    pph_money_t zero;
    zero.value = 0;
    return pph_result_add_row(result, label, zero, PPH_VALUE_TEXT, NULL, PPH_BREAKDOWN_SECTION);
}

int pph_result_add_currency(pph_result_t *result, const char *label, pph_money_t value, const char *note) {
    return pph_result_add_row(result, label, value, PPH_VALUE_CURRENCY, note, PPH_BREAKDOWN_NORMAL);
}

int pph_result_add_percent(pph_result_t *result, const char *label, pph_money_t percent, const char *note) {
    return pph_result_add_row(result, label, percent, PPH_VALUE_PERCENT, note, PPH_BREAKDOWN_NORMAL);
}

int pph_result_add_subtotal(pph_result_t *result, const char *label, pph_money_t value) {
    return pph_result_add_row(result, label, value, PPH_VALUE_CURRENCY, NULL, PPH_BREAKDOWN_SUBTOTAL);
}

int pph_result_add_total(pph_result_t *result, const char *label, pph_money_t value) {
    return pph_result_add_row(result, label, value, PPH_VALUE_CURRENCY, NULL, PPH_BREAKDOWN_TOTAL);
}

int pph_result_add_spacer(pph_result_t *result) {
    pph_money_t zero;
    zero.value = 0;
    return pph_result_add_row(result, "", zero, PPH_VALUE_TEXT, NULL, PPH_BREAKDOWN_SPACER);
}

/* ============================================
   Library Initialization
   ============================================ */

static const char *last_error = NULL;

void pph_init(void) {
    /* Currently no initialization needed */
    last_error = NULL;
}

const char* pph_get_last_error(void) {
    return last_error ? last_error : "No error";
}

void pph_set_last_error(const char *error) {
    last_error = error;
}

const char* pph_get_version(void) {
    return PPH_VERSION_STRING;
}
