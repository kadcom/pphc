/*
 * PPH Calculator - Public API
 * Indonesian Tax Calculator Library
 * Copyright (c) 2025 OpenPajak Contributors
 */

#ifndef PPH_CALCULATOR_H
#define PPH_CALCULATOR_H

#include "pph_types.h"
#include "pph_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================
   Version Information
   ============================================ */
#define PPH_VERSION_MAJOR 0
#define PPH_VERSION_MINOR 1
#define PPH_VERSION_PATCH 0
#define PPH_VERSION_BUILD 1
#define PPH_VERSION_STRING "0.1a"

/* ============================================
   Money Type (Fixed-Point Decimal)
   ============================================ */
#define PPH_DECIMAL_PLACES 4
#define PPH_SCALE_FACTOR PPH_INT64_C(10000)

typedef struct {
    pph_int64_t value;  /* Value * 10000 (4 decimal places) */
} pph_money_t;

/* Money constructors */
#define PPH_MONEY(whole, frac4) \
    ((pph_money_t){((pph_int64_t)(whole) * PPH_SCALE_FACTOR) + (pph_int64_t)(frac4)})

#define PPH_RUPIAH(whole) \
    ((pph_money_t){(pph_int64_t)(whole) * PPH_SCALE_FACTOR})

#define PPH_ZERO ((pph_money_t){0})

/* Money arithmetic operations */
PPH_EXPORT pph_money_t pph_money_add(pph_money_t a, pph_money_t b);
PPH_EXPORT pph_money_t pph_money_sub(pph_money_t a, pph_money_t b);
PPH_EXPORT pph_money_t pph_money_neg(pph_money_t a);
PPH_EXPORT pph_money_t pph_money_mul(pph_money_t a, pph_money_t b);
PPH_EXPORT pph_money_t pph_money_mul_int(pph_money_t a, pph_int64_t scalar);
PPH_EXPORT pph_money_t pph_money_percent(pph_money_t amount, pph_int64_t num, pph_int64_t den);
PPH_EXPORT pph_money_t pph_money_div(pph_money_t a, pph_int64_t divisor);
PPH_EXPORT pph_money_t pph_money_min(pph_money_t a, pph_money_t b);
PPH_EXPORT pph_money_t pph_money_max(pph_money_t a, pph_money_t b);
PPH_EXPORT int pph_money_cmp(pph_money_t a, pph_money_t b);

/* Rounding functions */
PPH_EXPORT pph_money_t pph_money_round_down_thousand(pph_money_t value);
PPH_EXPORT pph_money_t pph_money_floor(pph_money_t value);

/* String conversion */
PPH_EXPORT char* pph_money_to_string(pph_money_t money, char *buffer, pph_size_t size);
PPH_EXPORT char* pph_money_to_string_formatted(pph_money_t money, char *buffer, pph_size_t size);
PPH_EXPORT char* pph_percent_to_string(pph_money_t percent, char *buffer, pph_size_t size);
PPH_EXPORT pph_money_t pph_money_from_string(const char *str);
PPH_EXPORT pph_money_t pph_money_from_string_id(const char *str); /* Indonesian format: comma=decimal, dot=thousands */

/* ============================================
   Tax Breakdown Types
   ============================================ */
typedef enum {
    PPH_BREAKDOWN_NORMAL = 0,
    PPH_BREAKDOWN_SECTION,
    PPH_BREAKDOWN_SUBTOTAL,
    PPH_BREAKDOWN_TOTAL,
    PPH_BREAKDOWN_GROUP,
    PPH_BREAKDOWN_SPACER
} pph_breakdown_variant_t;

typedef enum {
    PPH_VALUE_CURRENCY = 0,
    PPH_VALUE_PERCENT,
    PPH_VALUE_TEXT
} pph_value_type_t;

typedef struct {
    char label[256];
    pph_money_t value;
    pph_value_type_t value_type;
    char note[128];
    pph_breakdown_variant_t variant;
} pph_breakdown_row_t;

typedef struct {
    pph_money_t total_tax;
    pph_breakdown_row_t *breakdown;
    pph_size_t breakdown_count;
    pph_size_t breakdown_capacity;
} pph_result_t;

/* Result management */
PPH_EXPORT void pph_result_free(pph_result_t *result);

/* ============================================
   PPh21/26 Types and Functions
   ============================================ */
typedef enum {
    PPH21_PEGAWAI_TETAP = 0,
    PPH21_PENSIUNAN,
    PPH21_PEGAWAI_TIDAK_TETAP,
    PPH21_BUKAN_PEGAWAI,
    PPH21_PESERTA_KEGIATAN,
    PPH21_PROGRAM_PENSIUN,
    PPH21_MANTAN_PEGAWAI,
    PPH21_WPLN
} pph21_subject_type_t;

typedef enum {
    PPH21_SCHEME_LAMA = 0,
    PPH21_SCHEME_TER
} pph21_scheme_t;

typedef enum {
    PPH21_TER_CATEGORY_A = 0,
    PPH21_TER_CATEGORY_B,
    PPH21_TER_CATEGORY_C
} pph21_ter_category_t;

typedef enum {
    PPH_PTKP_TK0 = 0,
    PPH_PTKP_TK1,
    PPH_PTKP_TK2,
    PPH_PTKP_TK3,
    PPH_PTKP_K0,
    PPH_PTKP_K1,
    PPH_PTKP_K2,
    PPH_PTKP_K3
} pph_ptkp_status_t;

/* Bonus entry for flexible multi-bonus support */
typedef struct {
    int month;              /* Month when bonus is paid (1-12) */
    pph_money_t amount;     /* Bonus amount */
    char name[64];          /* Bonus name (e.g., "THR", "Bonus Natal", "Insentif") */
} pph21_bonus_t;

typedef struct {
    pph21_subject_type_t subject_type;
    pph_money_t bruto_monthly;
    int months_paid;
    pph_money_t pension_contribution;
    pph_money_t zakat_or_donation;
    pph_ptkp_status_t ptkp_status;
    pph21_scheme_t scheme;
    pph21_ter_category_t ter_category;

    /* Flexible multi-bonus system */
    const pph21_bonus_t *bonuses;  /* Array of bonuses */
    int bonus_count;               /* Number of bonuses in array */

    pph_money_t foreign_tax_rate;
    int is_daily_worker;
} pph21_input_t;

PPH_EXPORT pph_result_t* pph21_calculate(const pph21_input_t *input);

/* ============================================
   PPh22 Types and Functions
   ============================================ */
typedef struct {
    pph_money_t dpp;
    pph_money_t rate;
} pph22_input_t;

PPH_EXPORT pph_result_t* pph22_calculate(const pph22_input_t *input);

/* ============================================
   PPh23 Types and Functions
   ============================================ */
typedef struct {
    pph_money_t bruto;
    pph_money_t rate;
} pph23_input_t;

PPH_EXPORT pph_result_t* pph23_calculate(const pph23_input_t *input);

/* ============================================
   PPh Final Pasal 4(2) Types and Functions
   ============================================ */
typedef struct {
    pph_money_t bruto;
    pph_money_t rate;
} pph4_2_input_t;

PPH_EXPORT pph_result_t* pph4_2_calculate(const pph4_2_input_t *input);

/* ============================================
   PPN Types and Functions
   ============================================ */
typedef enum {
    PPN_MODE_EXCLUSIVE = 0,
    PPN_MODE_INCLUSIVE
} ppn_mode_t;

typedef struct {
    pph_money_t dpp;
    pph_money_t rate;
    ppn_mode_t mode;
} ppn_input_t;

PPH_EXPORT pph_result_t* ppn_calculate(const ppn_input_t *input);

/* ============================================
   PPNBM Types and Functions
   ============================================ */
typedef struct {
    pph_money_t dpp;
    pph_money_t ppn_rate;
    pph_money_t ppnbm_rate;
} ppnbm_input_t;

PPH_EXPORT pph_result_t* ppnbm_calculate(const ppnbm_input_t *input);

/* ============================================
   Library Initialization and Error Handling
   ============================================ */
PPH_EXPORT void pph_init(void);
PPH_EXPORT const char* pph_get_last_error(void);
PPH_EXPORT const char* pph_get_version(void);

/* ============================================
   Custom Memory Allocator

   For extreme portability on embedded/bare-metal platforms without malloc:
   - Set custom allocator before any calculations
   - Pass NULL to all three parameters to reset to default allocator
   - All parameters must be non-NULL or all NULL

   Example (embedded system with memory pool):
     pph_set_custom_allocator(my_malloc, my_realloc, my_free);

   Example (reset to default):
     pph_set_custom_allocator(NULL, NULL, NULL);
   ============================================ */
PPH_EXPORT void pph_set_custom_allocator(
    void* (*malloc_fn)(size_t size),
    void* (*realloc_fn)(void* ptr, size_t size),
    void  (*free_fn)(void* ptr)
);

#ifdef __cplusplus
}
#endif

#endif /* PPH_CALCULATOR_H */
