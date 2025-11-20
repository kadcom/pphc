/*
 * PPH Internal API
 * Private functions and types shared between implementation files
 *
 * DO NOT include this header in public headers or user code.
 * This header is for internal use only within the PPHC library implementation.
 *
 * Copyright (c) 2025 OpenPajak Contributors
 */

#ifndef PPH_INTERNAL_H
#define PPH_INTERNAL_H

#include <pph/pph_calculator.h>
#include <stddef.h>  /* for size_t */

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================
   Result Management (pph_breakdown.c)
   ============================================ */

/**
 * Create a new tax calculation result structure
 * @return Pointer to new result, or NULL on allocation failure
 */
pph_result_t* pph_result_create(void);

/**
 * Add a section header to breakdown
 * @param result Result structure
 * @param label Section label text
 * @return 0 on success, -1 on error
 */
int pph_result_add_section(pph_result_t *result, const char *label);

/**
 * Add a currency value row to breakdown
 * @param result Result structure
 * @param label Row label text
 * @param value Money value to display
 * @param note Optional note text (can be NULL)
 * @return 0 on success, -1 on error
 */
int pph_result_add_currency(pph_result_t *result, const char *label,
                             pph_money_t value, const char *note);

/**
 * Add a percentage value row to breakdown
 * @param result Result structure
 * @param label Row label text
 * @param percent Percentage value (as money, e.g., 0.0500 for 5%)
 * @param note Optional note text (can be NULL)
 * @return 0 on success, -1 on error
 */
int pph_result_add_percent(pph_result_t *result, const char *label,
                            pph_money_t percent, const char *note);

/**
 * Add a subtotal row to breakdown
 * @param result Result structure
 * @param label Subtotal label text
 * @param value Money value to display
 * @return 0 on success, -1 on error
 */
int pph_result_add_subtotal(pph_result_t *result, const char *label,
                             pph_money_t value);

/**
 * Add a total row to breakdown
 * @param result Result structure
 * @param label Total label text
 * @param value Money value to display
 * @return 0 on success, -1 on error
 */
int pph_result_add_total(pph_result_t *result, const char *label,
                          pph_money_t value);

/* ============================================
   Error Handling (pph_breakdown.c)
   ============================================ */

/**
 * Set the last error message
 * @param error Error message string
 */
void pph_set_last_error(const char *error);

/* ============================================
   Tax Constants and Helpers (pph_constants.c)
   ============================================ */

/**
 * Get PTKP (Penghasilan Tidak Kena Pajak) amount for given status
 * @param status PTKP status (TK/0 through K/3)
 * @return PTKP amount
 */
pph_money_t pph_get_ptkp(pph_ptkp_status_t status);

/**
 * Calculate progressive tax using Pasal 17 rates (5%, 15%, 25%, 30%, 35%)
 * @param pkp Penghasilan Kena Pajak (taxable income after PTKP)
 * @return Tax amount
 */
pph_money_t pph_calculate_pasal17(pph_money_t pkp);

/**
 * Get TER (Tarif Efektif Rata-rata) monthly withholding rate
 * @param category TER category (A, B, or C)
 * @param bruto_monthly Gross monthly income
 * @return TER rate (as money, e.g., 0.0024 for 0.24%)
 */
pph_money_t pph_get_ter_bulanan_rate(pph21_ter_category_t category,
                                      pph_money_t bruto_monthly);

/**
 * Get TER (Tarif Efektif Rata-rata) daily withholding rate
 * @param category TER category (A, B, or C)
 * @param bruto Daily gross income
 * @return TER rate (as money, e.g., 0.0025 for 0.25%)
 */
pph_money_t pph_get_ter_harian_rate(pph21_ter_category_t category,
                                     pph_money_t bruto);

/* ============================================
   Custom Allocator Support
   ============================================ */

/**
 * Allocator function table for custom memory management
 * Allows library to work on platforms without standard malloc/free
 */
typedef struct {
    void* (*malloc_fn)(pph_size_t size);          /* Allocate memory */
    void* (*realloc_fn)(void* ptr, pph_size_t size); /* Reallocate memory */
    void  (*free_fn)(void* ptr);              /* Free memory */
} pph_allocator_t;

/**
 * Allocate memory using configured allocator
 * @param size Number of bytes to allocate
 * @return Pointer to allocated memory, or NULL on failure
 */
void* pph_malloc(pph_size_t size);

/**
 * Reallocate memory using configured allocator
 * @param ptr Existing allocation to resize
 * @param size New size in bytes
 * @return Pointer to reallocated memory, or NULL on failure
 */
void* pph_realloc(void* ptr, pph_size_t size);

/**
 * Free memory using configured allocator
 * @param ptr Memory to free (can be NULL)
 */
void pph_free(void* ptr);

#ifdef __cplusplus
}
#endif

#endif /* PPH_INTERNAL_H */
