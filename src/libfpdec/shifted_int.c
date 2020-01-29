/*
------------------------------------------------------------------------------
Name:        uint96.c

Author:      Michael Amrhein (michael@adrhinum.de)

Copyright:   (c) 2020 ff. Michael Amrhein
License:     This program is part of a larger application or library.
             For license details please read the file LICENSE provided
             together with the application or library.
------------------------------------------------------------------------------
$Source$
$Revision$
*/


#include "shifted_int_.h"

/*****************************************************************************
*  Functions
*****************************************************************************/

// base arithmetic

static inline void
uint128_imul10(uint128_t *x) {
    x->hi = x->hi * 10 + U64_HI(U64_HI(x->lo) * 10);
    x->lo *= 10;
}

static inline void
uint128_iadd_uint64(uint128_t *x, const uint64_t y) {
    uint64_t t = x->lo + y;
    x->hi += (t < x->lo);
    x->lo = t;
}

// digit iterator

fpdec_digit_t
_shint_get_next_digit(digit_iter *it) {
    return it->next_idx < it->limit ? it->digits[it->next_idx++] :
           (fpdec_digit_t) FPDEC_DIGIT_MAX;
}

digit_iter
shint_iter_digits(fpdec_digit_t lo, fpdec_digit_t hi) {
    digit_iter it = {
        .limit = 2,
        .next_idx = 0,
        .next = _shint_get_next_digit,
        .digits = {lo, hi}
    };
    return it;
}

// converter

error_t
shint_from_dec_coeff(uint64_t *lo, uint32_t *hi, const dec_digit_t *coeff,
                     const size_t n_dec_digits, const size_t n_add_zeros) {
    uint128_t accu = {0, 0};
    const dec_digit_t *stop = coeff + n_dec_digits;
    const dec_digit_t *cut = MIN(coeff + MAX_N_DEC_DIGITS_UINT64 - 1, stop);

    for (; coeff < cut; ++coeff) {
        accu.lo *= 10;
        accu.lo += *coeff;           // *coeff is < 10, so no overflow here
    }
    for (; coeff < stop; ++coeff) {
        uint128_imul10(&accu);
        accu.lo += *coeff;           // *coeff is < 10, so no overflow here
    }
    for (int i = 0; i < n_add_zeros; ++i) {
        uint128_imul10(&accu);
    }
    if (U64_HI(U128_HI(accu))) return FPDEC_N_DIGITS_LIMIT_EXCEEDED;
    *lo = U128_LO(accu);
    *hi = (uint32_t) U128_HI(accu);
    return FPDEC_OK;
}
