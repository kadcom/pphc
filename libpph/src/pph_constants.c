/*
 * PPH Constants - Tax tables and rates
 * Copyright (c) 2025 OpenPajak Contributors
 */

#include <pph/pph_calculator.h>

/* ============================================
   PTKP Table (Penghasilan Tidak Kena Pajak)
   ============================================ */

static const pph_money_t PPH_PTKP_TABLE[8] = {
    { PPH_INT64_C(540000000000) },  /* TK/0: 54,000,000 */
    { PPH_INT64_C(585000000000) },  /* TK/1: 58,500,000 */
    { PPH_INT64_C(630000000000) },  /* TK/2: 63,000,000 */
    { PPH_INT64_C(675000000000) },  /* TK/3: 67,500,000 */
    { PPH_INT64_C(585000000000) },  /* K/0: 58,500,000 */
    { PPH_INT64_C(630000000000) },  /* K/1: 63,000,000 */
    { PPH_INT64_C(675000000000) },  /* K/2: 67,500,000 */
    { PPH_INT64_C(720000000000) }   /* K/3: 72,000,000 */
};

pph_money_t pph_get_ptkp(pph_ptkp_status_t status) {
    if (status < 0 || status > 7) {
        return PPH_PTKP_TABLE[0];  /* Default to TK/0 */
    }
    return PPH_PTKP_TABLE[status];
}

/* ============================================
   Pasal 17 Progressive Tax Layers
   ============================================ */

typedef struct {
    pph_money_t limit;
    pph_money_t rate;  /* Stored as fraction (0.05 = 500/10000) */
} pasal17_layer_t;

#define PASAL17_LAYER_COUNT 5

static const pasal17_layer_t PPH_PASAL17_LAYERS[PASAL17_LAYER_COUNT] = {
    { { PPH_INT64_C(600000000000) },    { PPH_INT64_C(500) } },   /* 5% = 0.0500 */
    { { PPH_INT64_C(1900000000000) },   { PPH_INT64_C(1500) } },  /* 15% = 0.1500 */
    { { PPH_INT64_C(2500000000000) },   { PPH_INT64_C(2500) } },  /* 25% = 0.2500 */
    { { PPH_INT64_C(45000000000000) },  { PPH_INT64_C(3000) } },  /* 30% = 0.3000 */
    { { PPH_INT64_C(21474836470000) },  { PPH_INT64_C(3500) } }   /* 35% = 0.3500, use max int as infinity */
};

pph_money_t pph_calculate_pasal17(pph_money_t pkp) {
    pph_money_t tax;
    pph_money_t remaining = pkp;
    int i;

    tax.value = 0;

    for (i = 0; i < PASAL17_LAYER_COUNT; i++) {
        pph_money_t taxable, layer_tax;

        if (remaining.value <= 0) {
            break;
        }

        taxable = pph_money_min(remaining, PPH_PASAL17_LAYERS[i].limit);
        layer_tax = pph_money_mul(taxable, PPH_PASAL17_LAYERS[i].rate);

        tax = pph_money_add(tax, layer_tax);
        remaining = pph_money_sub(remaining, taxable);
    }

    return tax;
}

/* ============================================
   TER Bulanan (Monthly) Tables
   ============================================ */

typedef struct {
    pph_money_t ceiling;
    pph_money_t rate;
} ter_entry_t;

#define TER_BULANAN_A_COUNT 44
#define TER_BULANAN_B_COUNT 40
#define TER_BULANAN_C_COUNT 41

static const ter_entry_t TER_BULANAN_A[TER_BULANAN_A_COUNT] = {
    { { PPH_INT64_C(54000000000) }, { PPH_INT64_C(0) } },
    { { PPH_INT64_C(56500000000) }, { PPH_INT64_C(25) } },
    { { PPH_INT64_C(59500000000) }, { PPH_INT64_C(50) } },
    { { PPH_INT64_C(63000000000) }, { PPH_INT64_C(75) } },
    { { PPH_INT64_C(67500000000) }, { PPH_INT64_C(100) } },
    { { PPH_INT64_C(75000000000) }, { PPH_INT64_C(125) } },
    { { PPH_INT64_C(85500000000) }, { PPH_INT64_C(150) } },
    { { PPH_INT64_C(96500000000) }, { PPH_INT64_C(175) } },
    { { PPH_INT64_C(100500000000) }, { PPH_INT64_C(200) } },
    { { PPH_INT64_C(103500000000) }, { PPH_INT64_C(225) } },
    { { PPH_INT64_C(107000000000) }, { PPH_INT64_C(250) } },
    { { PPH_INT64_C(110500000000) }, { PPH_INT64_C(300) } },
    { { PPH_INT64_C(116000000000) }, { PPH_INT64_C(350) } },
    { { PPH_INT64_C(125000000000) }, { PPH_INT64_C(400) } },
    { { PPH_INT64_C(137500000000) }, { PPH_INT64_C(500) } },
    { { PPH_INT64_C(151000000000) }, { PPH_INT64_C(600) } },
    { { PPH_INT64_C(169500000000) }, { PPH_INT64_C(700) } },
    { { PPH_INT64_C(197500000000) }, { PPH_INT64_C(800) } },
    { { PPH_INT64_C(241500000000) }, { PPH_INT64_C(900) } },
    { { PPH_INT64_C(264500000000) }, { PPH_INT64_C(1000) } },
    { { PPH_INT64_C(280000000000) }, { PPH_INT64_C(1100) } },
    { { PPH_INT64_C(300500000000) }, { PPH_INT64_C(1200) } },
    { { PPH_INT64_C(324000000000) }, { PPH_INT64_C(1300) } },
    { { PPH_INT64_C(354000000000) }, { PPH_INT64_C(1400) } },
    { { PPH_INT64_C(391000000000) }, { PPH_INT64_C(1500) } },
    { { PPH_INT64_C(438500000000) }, { PPH_INT64_C(1600) } },
    { { PPH_INT64_C(478000000000) }, { PPH_INT64_C(1700) } },
    { { PPH_INT64_C(514000000000) }, { PPH_INT64_C(1800) } },
    { { PPH_INT64_C(563000000000) }, { PPH_INT64_C(1900) } },
    { { PPH_INT64_C(622000000000) }, { PPH_INT64_C(2000) } },
    { { PPH_INT64_C(686000000000) }, { PPH_INT64_C(2100) } },
    { { PPH_INT64_C(775000000000) }, { PPH_INT64_C(2200) } },
    { { PPH_INT64_C(890000000000) }, { PPH_INT64_C(2300) } },
    { { PPH_INT64_C(1030000000000) }, { PPH_INT64_C(2400) } },
    { { PPH_INT64_C(1250000000000) }, { PPH_INT64_C(2500) } },
    { { PPH_INT64_C(1570000000000) }, { PPH_INT64_C(2600) } },
    { { PPH_INT64_C(2060000000000) }, { PPH_INT64_C(2700) } },
    { { PPH_INT64_C(3370000000000) }, { PPH_INT64_C(2800) } },
    { { PPH_INT64_C(4540000000000) }, { PPH_INT64_C(2900) } },
    { { PPH_INT64_C(5500000000000) }, { PPH_INT64_C(3000) } },
    { { PPH_INT64_C(6950000000000) }, { PPH_INT64_C(3100) } },
    { { PPH_INT64_C(9100000000000) }, { PPH_INT64_C(3200) } },
    { { PPH_INT64_C(14000000000000) }, { PPH_INT64_C(3300) } },
    { { PPH_INT64_C(21474836470000) }, { PPH_INT64_C(3400) } }  /* Infinity */
};

static const ter_entry_t TER_BULANAN_B[TER_BULANAN_B_COUNT] = {
    { { PPH_INT64_C(62000000000) }, { PPH_INT64_C(0) } },
    { { PPH_INT64_C(65000000000) }, { PPH_INT64_C(25) } },
    { { PPH_INT64_C(68500000000) }, { PPH_INT64_C(50) } },
    { { PPH_INT64_C(73000000000) }, { PPH_INT64_C(75) } },
    { { PPH_INT64_C(92000000000) }, { PPH_INT64_C(100) } },
    { { PPH_INT64_C(107500000000) }, { PPH_INT64_C(150) } },
    { { PPH_INT64_C(112500000000) }, { PPH_INT64_C(200) } },
    { { PPH_INT64_C(116000000000) }, { PPH_INT64_C(250) } },
    { { PPH_INT64_C(126000000000) }, { PPH_INT64_C(300) } },
    { { PPH_INT64_C(136000000000) }, { PPH_INT64_C(400) } },
    { { PPH_INT64_C(149500000000) }, { PPH_INT64_C(500) } },
    { { PPH_INT64_C(164000000000) }, { PPH_INT64_C(600) } },
    { { PPH_INT64_C(184500000000) }, { PPH_INT64_C(700) } },
    { { PPH_INT64_C(218500000000) }, { PPH_INT64_C(800) } },
    { { PPH_INT64_C(260000000000) }, { PPH_INT64_C(900) } },
    { { PPH_INT64_C(277000000000) }, { PPH_INT64_C(1000) } },
    { { PPH_INT64_C(293500000000) }, { PPH_INT64_C(1100) } },
    { { PPH_INT64_C(314500000000) }, { PPH_INT64_C(1200) } },
    { { PPH_INT64_C(339500000000) }, { PPH_INT64_C(1300) } },
    { { PPH_INT64_C(371000000000) }, { PPH_INT64_C(1400) } },
    { { PPH_INT64_C(411000000000) }, { PPH_INT64_C(1500) } },
    { { PPH_INT64_C(458000000000) }, { PPH_INT64_C(1600) } },
    { { PPH_INT64_C(495000000000) }, { PPH_INT64_C(1700) } },
    { { PPH_INT64_C(538000000000) }, { PPH_INT64_C(1800) } },
    { { PPH_INT64_C(585000000000) }, { PPH_INT64_C(1900) } },
    { { PPH_INT64_C(640000000000) }, { PPH_INT64_C(2000) } },
    { { PPH_INT64_C(710000000000) }, { PPH_INT64_C(2100) } },
    { { PPH_INT64_C(800000000000) }, { PPH_INT64_C(2200) } },
    { { PPH_INT64_C(930000000000) }, { PPH_INT64_C(2300) } },
    { { PPH_INT64_C(1090000000000) }, { PPH_INT64_C(2400) } },
    { { PPH_INT64_C(1290000000000) }, { PPH_INT64_C(2500) } },
    { { PPH_INT64_C(1630000000000) }, { PPH_INT64_C(2600) } },
    { { PPH_INT64_C(2110000000000) }, { PPH_INT64_C(2700) } },
    { { PPH_INT64_C(3740000000000) }, { PPH_INT64_C(2800) } },
    { { PPH_INT64_C(4590000000000) }, { PPH_INT64_C(2900) } },
    { { PPH_INT64_C(5550000000000) }, { PPH_INT64_C(3000) } },
    { { PPH_INT64_C(7040000000000) }, { PPH_INT64_C(3100) } },
    { { PPH_INT64_C(9570000000000) }, { PPH_INT64_C(3200) } },
    { { PPH_INT64_C(14050000000000) }, { PPH_INT64_C(3300) } },
    { { PPH_INT64_C(21474836470000) }, { PPH_INT64_C(3400) } }
};

static const ter_entry_t TER_BULANAN_C[TER_BULANAN_C_COUNT] = {
    { { PPH_INT64_C(66000000000) }, { PPH_INT64_C(0) } },
    { { PPH_INT64_C(69500000000) }, { PPH_INT64_C(25) } },
    { { PPH_INT64_C(73500000000) }, { PPH_INT64_C(50) } },
    { { PPH_INT64_C(78000000000) }, { PPH_INT64_C(75) } },
    { { PPH_INT64_C(88500000000) }, { PPH_INT64_C(100) } },
    { { PPH_INT64_C(98000000000) }, { PPH_INT64_C(125) } },
    { { PPH_INT64_C(109500000000) }, { PPH_INT64_C(150) } },
    { { PPH_INT64_C(112000000000) }, { PPH_INT64_C(175) } },
    { { PPH_INT64_C(120500000000) }, { PPH_INT64_C(200) } },
    { { PPH_INT64_C(129500000000) }, { PPH_INT64_C(300) } },
    { { PPH_INT64_C(141500000000) }, { PPH_INT64_C(400) } },
    { { PPH_INT64_C(155500000000) }, { PPH_INT64_C(500) } },
    { { PPH_INT64_C(170500000000) }, { PPH_INT64_C(600) } },
    { { PPH_INT64_C(195000000000) }, { PPH_INT64_C(700) } },
    { { PPH_INT64_C(227000000000) }, { PPH_INT64_C(800) } },
    { { PPH_INT64_C(266000000000) }, { PPH_INT64_C(900) } },
    { { PPH_INT64_C(281000000000) }, { PPH_INT64_C(1000) } },
    { { PPH_INT64_C(301000000000) }, { PPH_INT64_C(1100) } },
    { { PPH_INT64_C(326000000000) }, { PPH_INT64_C(1200) } },
    { { PPH_INT64_C(354000000000) }, { PPH_INT64_C(1300) } },
    { { PPH_INT64_C(389000000000) }, { PPH_INT64_C(1400) } },
    { { PPH_INT64_C(430000000000) }, { PPH_INT64_C(1500) } },
    { { PPH_INT64_C(474000000000) }, { PPH_INT64_C(1600) } },
    { { PPH_INT64_C(512000000000) }, { PPH_INT64_C(1700) } },
    { { PPH_INT64_C(558000000000) }, { PPH_INT64_C(1800) } },
    { { PPH_INT64_C(604000000000) }, { PPH_INT64_C(1900) } },
    { { PPH_INT64_C(667000000000) }, { PPH_INT64_C(2000) } },
    { { PPH_INT64_C(745000000000) }, { PPH_INT64_C(2100) } },
    { { PPH_INT64_C(832000000000) }, { PPH_INT64_C(2200) } },
    { { PPH_INT64_C(956000000000) }, { PPH_INT64_C(2300) } },
    { { PPH_INT64_C(1100000000000) }, { PPH_INT64_C(2400) } },
    { { PPH_INT64_C(1340000000000) }, { PPH_INT64_C(2500) } },
    { { PPH_INT64_C(1690000000000) }, { PPH_INT64_C(2600) } },
    { { PPH_INT64_C(2210000000000) }, { PPH_INT64_C(2700) } },
    { { PPH_INT64_C(3900000000000) }, { PPH_INT64_C(2800) } },
    { { PPH_INT64_C(4630000000000) }, { PPH_INT64_C(2900) } },
    { { PPH_INT64_C(5610000000000) }, { PPH_INT64_C(3000) } },
    { { PPH_INT64_C(7090000000000) }, { PPH_INT64_C(3100) } },
    { { PPH_INT64_C(9650000000000) }, { PPH_INT64_C(3200) } },
    { { PPH_INT64_C(14190000000000) }, { PPH_INT64_C(3300) } },
    { { PPH_INT64_C(21474836470000) }, { PPH_INT64_C(3400) } }
};

pph_money_t pph_get_ter_bulanan_rate(pph21_ter_category_t category, pph_money_t bruto_monthly) {
    const ter_entry_t *table;
    int count, i;
    pph_money_t fallback_rate;

    switch (category) {
        case PPH21_TER_CATEGORY_B:
            table = TER_BULANAN_B;
            count = TER_BULANAN_B_COUNT;
            break;
        case PPH21_TER_CATEGORY_C:
            table = TER_BULANAN_C;
            count = TER_BULANAN_C_COUNT;
            break;
        case PPH21_TER_CATEGORY_A:
        default:
            table = TER_BULANAN_A;
            count = TER_BULANAN_A_COUNT;
            break;
    }

    fallback_rate = table[count - 1].rate;

    for (i = 0; i < count; i++) {
        if (pph_money_cmp(bruto_monthly, table[i].ceiling) <= 0) {
            return table[i].rate;
        }
    }

    return fallback_rate;
}

/* ============================================
   TER Harian (Daily) Tables
   ============================================ */

#define TER_HARIAN_COUNT 3

static const ter_entry_t TER_HARIAN_A[TER_HARIAN_COUNT] = {
    { { PPH_INT64_C(7500000000) }, { PPH_INT64_C(25) } },
    { { PPH_INT64_C(25000000000) }, { PPH_INT64_C(150) } },
    { { PPH_INT64_C(21474836470000) }, { PPH_INT64_C(200) } }
};

static const ter_entry_t TER_HARIAN_B[TER_HARIAN_COUNT] = {
    { { PPH_INT64_C(7500000000) }, { PPH_INT64_C(25) } },
    { { PPH_INT64_C(25000000000) }, { PPH_INT64_C(125) } },
    { { PPH_INT64_C(21474836470000) }, { PPH_INT64_C(175) } }
};

static const ter_entry_t TER_HARIAN_C[TER_HARIAN_COUNT] = {
    { { PPH_INT64_C(7500000000) }, { PPH_INT64_C(25) } },
    { { PPH_INT64_C(25000000000) }, { PPH_INT64_C(100) } },
    { { PPH_INT64_C(21474836470000) }, { PPH_INT64_C(150) } }
};

pph_money_t pph_get_ter_harian_rate(pph21_ter_category_t category, pph_money_t bruto) {
    const ter_entry_t *table;
    int i;

    switch (category) {
        case PPH21_TER_CATEGORY_B:
            table = TER_HARIAN_B;
            break;
        case PPH21_TER_CATEGORY_C:
            table = TER_HARIAN_C;
            break;
        case PPH21_TER_CATEGORY_A:
        default:
            table = TER_HARIAN_A;
            break;
    }

    for (i = 0; i < TER_HARIAN_COUNT; i++) {
        if (pph_money_cmp(bruto, table[i].ceiling) <= 0) {
            return table[i].rate;
        }
    }

    return table[TER_HARIAN_COUNT - 1].rate;
}
