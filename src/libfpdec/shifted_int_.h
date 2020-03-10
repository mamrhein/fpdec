/*
------------------------------------------------------------------------------
Name:        shifted_int_.h

Author:      Michael Amrhein (michael@adrhinum.de)

Copyright:   (c) 2020 ff. Michael Amrhein
License:     This program is part of a larger application or library.
             For license details please read the file LICENSE provided
             together with the application or library.
------------------------------------------------------------------------------
$Source$
$Revision$
*/


#ifndef FPDEC_SHIFTED_INT__H
#define FPDEC_SHIFTED_INT__H

#include <math.h>

#include "common_.h"
#include "shifted_int.h"
#include "rounding_.h"

/*****************************************************************************
*  Types
*****************************************************************************/

/*****************************************************************************
*  Macros
*****************************************************************************/

#define MAX_N_DEC_DIGITS_IN_SHINT 29
#define MAX_DEC_PREC_FOR_SHINT 9
#define UINT64_10_POW_N_CUTOFF 19

#define UINT128_SET_MAX(x) {x->lo = UINT64_MAX; x->hi = UINT64_MAX;}
#define SIGNAL_OVERFLOW(x) UINT128_SET_MAX(x)

#define U64_HI(x) (((uint64_t)x) >> 32U)
#define U64_LO(x) (((uint64_t)x) & 0xFFFFFFFF)

#define U128_FITS_SHINT(x) (U64_HI(x.hi) == 0)

#define U64_MAGNITUDE(x) ((int) log10(x))
#define U128_MAGNITUDE(lo, hi) ((int) log10(((double) hi) * 0x100000000UL \
                                * 0x100000000UL + (double) lo))

/*****************************************************************************
*  Functions
*****************************************************************************/

// Digit iterator

digit_iter
shint_iter_digits(fpdec_digit_t lo, fpdec_digit_t hi);

// Comparison

int
shint_cmp_abs(uint128_t x, fpdec_dec_prec_t x_prec,
              uint128_t y, fpdec_dec_prec_t y_prec);

// Converter

error_t
shint_from_dec_coeff(uint64_t *lo, uint32_t *hi, const dec_digit_t *coeff,
                     size_t n_dec_digits, size_t n_add_zeros);

fpdec_n_digits_t
shint_to_digits(fpdec_digit_t *digit, int *n_trailing_zeros_skipped,
                uint64_t b, uint64_t lo, uint32_t hi, int prec);

// Decimal shift

void
u128_idecshift(uint128_t *ui, fpdec_sign_t sign, int n_dec_digits,
               enum FPDEC_ROUNDING_MODE rounding);

// Comparison

static inline int
u128_cmp(const uint128_t *x, const uint128_t *y) {
    return ((x->hi > y->hi) || ((x->hi == y->hi) && (x->lo > y->lo))) -
            ((x->hi < y->hi) || ((x->hi == y->hi) && (x->lo < y->lo)));
}

// Addition

static inline void
u128_iadd_u64(uint128_t *x, const uint64_t y) {
    uint64_t t = x->lo + y;
    x->hi += (t < x->lo);
    x->lo = t;
}

static inline void
u128_incr(uint128_t *x) {
    u128_iadd_u64(x, 1UL);
}

static inline void
u128_iadd_u128(uint128_t *x, const uint128_t *y) {
    uint64_t t = x->lo + y->lo;
    x->hi += y->hi + (t < x->lo);
    x->lo = t;
}

// Subtraction

static inline void
u128_isub_u64(uint128_t *x, const uint64_t y) {
    uint64_t t = x->lo - y;
    x->hi -= (t > x->lo);
    x->lo = t;
}

static inline void
u128_decr(uint128_t *x) {
    u128_isub_u64(x, 1UL);
}

static inline void
u128_isub_u128(uint128_t *x, const uint128_t *y) {
    assert(u128_cmp(x, y) >= 0);
    uint64_t t = x->lo - y->lo;
    x->hi -= y->hi + (t > x->lo);
    x->lo = t;
}

static inline void
u128_sub_u128(uint128_t *z, const uint128_t *x, const uint128_t *y) {
    assert(u128_cmp(x, y) >= 0);
    uint64_t t = x->lo - y->lo;
    z->hi = x->hi - y->hi - (t > x->lo);
    z->lo = t;
}

#endif //FPDEC_SHIFTED_INT__H
