/*
 * PPH Money - Fixed-point arithmetic implementation
 * Copyright (c) 2025 OpenPajak Contributors
 */

#include <pph/pph_calculator.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================
   64-bit Multiplication Helper (Portable)
   ============================================ */

typedef struct {
    pph_uint64_t lo;
    pph_uint64_t hi;
} pph_uint128_t;

/* Portable 64x64 -> 128-bit multiplication (works on 32-bit platforms) */
static pph_uint128_t pph_mul64x64(pph_uint64_t a, pph_uint64_t b) {
    pph_uint64_t a_lo, a_hi, b_lo, b_hi;
    pph_uint64_t p0, p1, p2, p3;
    pph_uint64_t carry;
    pph_uint128_t result;

    /* Split into 32-bit halves */
    a_lo = a & 0xFFFFFFFFULL;
    a_hi = a >> 32;
    b_lo = b & 0xFFFFFFFFULL;
    b_hi = b >> 32;

    /* Compute partial products */
    p0 = a_lo * b_lo;
    p1 = a_lo * b_hi;
    p2 = a_hi * b_lo;
    p3 = a_hi * b_hi;

    /* Add middle products with carry */
    carry = ((p0 >> 32) + (p1 & 0xFFFFFFFFULL) + (p2 & 0xFFFFFFFFULL)) >> 32;

    result.lo = p0 + (p1 << 32) + (p2 << 32);
    result.hi = p3 + (p1 >> 32) + (p2 >> 32) + carry;

    return result;
}

/* ============================================
   Basic Arithmetic Operations
   ============================================ */

pph_money_t pph_money_add(pph_money_t a, pph_money_t b) {
    pph_money_t result;
    result.value = a.value + b.value;
    return result;
}

pph_money_t pph_money_sub(pph_money_t a, pph_money_t b) {
    pph_money_t result;
    result.value = a.value - b.value;
    return result;
}

pph_money_t pph_money_neg(pph_money_t a) {
    pph_money_t result;
    result.value = -a.value;
    return result;
}

/* ============================================
   Multiplication Operations
   ============================================ */

pph_money_t pph_money_mul(pph_money_t a, pph_money_t b) {
    int negative;
    pph_uint64_t abs_a, abs_b;
    pph_uint128_t product;
    pph_uint64_t result_u64;
    pph_money_t result;

    /* Handle sign */
    negative = (a.value < 0) ^ (b.value < 0);
    abs_a = (a.value < 0) ? (pph_uint64_t)(-a.value) : (pph_uint64_t)a.value;
    abs_b = (b.value < 0) ? (pph_uint64_t)(-b.value) : (pph_uint64_t)b.value;

    /* Multiply and scale down */
    product = pph_mul64x64(abs_a, abs_b);

    /* Divide by scale factor (10000) */
    /* Shift down by dividing: result = (hi * 2^64 + lo) / 10000 */
    /* Simplified: assume product.hi is small enough */
    result_u64 = (product.lo / PPH_SCALE_FACTOR);

    result.value = negative ? -(pph_int64_t)result_u64 : (pph_int64_t)result_u64;
    return result;
}

pph_money_t pph_money_mul_int(pph_money_t a, pph_int64_t scalar) {
    pph_money_t result;
    result.value = a.value * scalar;
    return result;
}

pph_money_t pph_money_percent(pph_money_t amount, pph_int64_t num, pph_int64_t den) {
    pph_money_t result;
    /* (amount * num) / den */
    result.value = (amount.value * num) / den;
    return result;
}

pph_money_t pph_money_div(pph_money_t a, pph_int64_t divisor) {
    pph_money_t result;
    result.value = a.value / divisor;
    return result;
}

/* ============================================
   Comparison Operations
   ============================================ */

int pph_money_cmp(pph_money_t a, pph_money_t b) {
    if (a.value < b.value) return -1;
    if (a.value > b.value) return 1;
    return 0;
}

pph_money_t pph_money_min(pph_money_t a, pph_money_t b) {
    return (a.value < b.value) ? a : b;
}

pph_money_t pph_money_max(pph_money_t a, pph_money_t b) {
    return (a.value > b.value) ? a : b;
}

/* ============================================
   Rounding Operations
   ============================================ */

pph_money_t pph_money_round_down_thousand(pph_money_t value) {
    pph_money_t result;
    pph_int64_t thousands;

    if (value.value < 0) {
        result.value = 0;
    } else {
        /* Divide by 1000 (in internal units: 1000 * 10000 = 10000000) */
        thousands = value.value / PPH_INT64_C(10000000);
        result.value = thousands * PPH_INT64_C(10000000);
    }

    return result;
}

pph_money_t pph_money_floor(pph_money_t value) {
    pph_money_t result;
    result.value = (value.value < 0) ? 0 : value.value;
    return result;
}

/* ============================================
   String Conversion
   ============================================ */

char* pph_money_to_string(pph_money_t money, char *buffer, pph_size_t size) {
    pph_int64_t whole, frac;

    if (buffer == NULL || size < 24) {
        return NULL;
    }

    whole = money.value / PPH_SCALE_FACTOR;
    frac = money.value % PPH_SCALE_FACTOR;

    if (frac < 0) {
        frac = -frac;
    }

    #if defined(__WATCOMC__) || defined(_MSC_VER)
        snprintf(buffer, size, "%I64d.%04I64d", whole, frac);
    #else
        snprintf(buffer, size, "%lld.%04lld", (long long)whole, (long long)frac);
    #endif

    return buffer;
}

char* pph_money_to_string_formatted(pph_money_t money, char *buffer, pph_size_t size) {
    pph_int64_t whole, frac;
    char temp[64];
    char *p, *q;
    int i, len, count;
    int negative;

    if (buffer == NULL || size < 32) {
        return NULL;
    }

    whole = money.value / PPH_SCALE_FACTOR;
    frac = money.value % PPH_SCALE_FACTOR;

    negative = (whole < 0);
    if (negative) {
        whole = -whole;
    }
    if (frac < 0) {
        frac = -frac;
    }

    /* Format whole part with thousands separator */
    #if defined(__WATCOMC__) || defined(_MSC_VER)
        snprintf(temp, sizeof(temp), "%I64d", whole);
    #else
        snprintf(temp, sizeof(temp), "%lld", (long long)whole);
    #endif

    len = (int)strlen(temp);
    p = temp + len - 1;
    q = buffer;

    if (negative) {
        *q++ = '-';
    }

    count = 0;
    for (i = len - 1; i >= 0; i--) {
        if (count == 3) {
            *q++ = ',';
            count = 0;
        }
        *q++ = *p--;
        count++;
    }

    /* Reverse the digits */
    {
        char *start = negative ? buffer + 1 : buffer;
        char *end = q - 1;
        char tmp_char;
        while (start < end) {
            tmp_char = *start;
            *start = *end;
            *end = tmp_char;
            start++;
            end--;
        }
    }

    /* Add decimal part */
    #if defined(__WATCOMC__) || defined(_MSC_VER)
        snprintf(q, size - (pph_size_t)(q - buffer), ".%04I64d", frac);
    #else
        snprintf(q, size - (pph_size_t)(q - buffer), ".%04lld", (long long)frac);
    #endif

    return buffer;
}

char* pph_percent_to_string(pph_money_t percent, char *buffer, pph_size_t size) {
    /* Percent is stored same way: 5.25% = 52500 internal units */
    if (pph_money_to_string(percent, buffer, size)) {
        /* Append % sign */
        pph_size_t len = strlen(buffer);
        if (len + 2 < size) {
            buffer[len] = '%';
            buffer[len + 1] = '\0';
        }
        return buffer;
    }
    return NULL;
}

pph_money_t pph_money_from_string(const char *str) {
    pph_money_t result;
    pph_int64_t whole = 0, frac = 0;
    int negative = 0;
    const char *p = str;

    result.value = 0;

    if (str == NULL) {
        return result;
    }

    /* Skip whitespace */
    while (*p == ' ' || *p == '\t') {
        p++;
    }

    /* Check sign */
    if (*p == '-') {
        negative = 1;
        p++;
    } else if (*p == '+') {
        p++;
    }

    /* Parse whole part */
    while (*p >= '0' && *p <= '9') {
        whole = whole * 10 + (*p - '0');
        p++;
    }

    /* Parse decimal part */
    if (*p == '.') {
        int count = 0;
        p++;
        while (*p >= '0' && *p <= '9' && count < 4) {
            frac = frac * 10 + (*p - '0');
            p++;
            count++;
        }
        /* Pad with zeros if needed */
        while (count < 4) {
            frac *= 10;
            count++;
        }
    }

    result.value = whole * PPH_SCALE_FACTOR + frac;
    if (negative) {
        result.value = -result.value;
    }

    return result;
}

pph_money_t pph_money_from_string_id(const char *str) {
    pph_money_t result;
    pph_int64_t whole = 0, frac = 0;
    int negative = 0;
    const char *p = str;

    result.value = 0;

    if (str == NULL) {
        return result;
    }

    /* Skip whitespace */
    while (*p == ' ' || *p == '\t') {
        p++;
    }

    /* Check sign */
    if (*p == '-') {
        negative = 1;
        p++;
    } else if (*p == '+') {
        p++;
    }

    /* Parse whole part, skip thousands separators (dots) */
    while (*p != '\0') {
        if (*p >= '0' && *p <= '9') {
            whole = whole * 10 + (*p - '0');
            p++;
        } else if (*p == '.') {
            /* Skip thousands separator */
            p++;
        } else if (*p == ',') {
            /* Reached decimal separator */
            break;
        } else {
            /* Invalid character */
            pph_money_t zero;
            zero.value = 0;
            return zero;
        }
    }

    /* Parse decimal part (after comma) */
    if (*p == ',') {
        int count = 0;
        p++;
        while (*p >= '0' && *p <= '9' && count < 4) {
            frac = frac * 10 + (*p - '0');
            p++;
            count++;
        }
        /* Pad with zeros if needed */
        while (count < 4) {
            frac *= 10;
            count++;
        }
    }

    result.value = whole * PPH_SCALE_FACTOR + frac;
    if (negative) {
        result.value = -result.value;
    }

    return result;
}
