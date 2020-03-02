/*
------------------------------------------------------------------------------
Name:        shifted_int.c

Author:      Michael Amrhein (michael@adrhinum.de)

Copyright:   (c) 2020 ff. Michael Amrhein
License:     This program is part of a larger application or library.
             For license details please read the file LICENSE provided
             together with the application or library.
------------------------------------------------------------------------------
$Source$
$Revision$
*/

#include <assert.h>

#include "shifted_int_.h"
#include "fpdec.h"

/*****************************************************************************
*  Macros
*****************************************************************************/


/*****************************************************************************
*  Functions
*****************************************************************************/

// Bit arithmetic

static inline unsigned
u64_most_signif_bit_pos(uint64_t x) {
    unsigned n = 0;
    uint64_t t = x >> 32U;
    if (t != 0) {
        n += 32;
        x = t;
    }
    t = x >> 16U;
    if (t != 0) {
        n += 16;
        x = t;
    }
    t = x >> 8U;
    if (t != 0) {
        n += 8;
        x = t;
    }
    t = x >> 4U;
    if (t != 0) {
        n += 4;
        x = t;
    }
    t = x >> 2U;
    if (t != 0) {
        n += 2;
        x = t;
    }
    t = x >> 1U;
    if (t != 0) {
        n += 1;
        x = t;
    }
    return n + (uint8_t) t;
}

static inline unsigned
u128_n_signif_u32(const uint128_t *x) {
    return (x->hi != 0) ?
           (U64_HI(x->hi) != 0 ? 4 : 3) :
           (U64_HI(x->lo) != 0 ? 2 : 1);
}

static inline unsigned
u64_n_leading_0_bits(uint64_t x) {
    if (x == 0) return 64;
    return 64 - u64_most_signif_bit_pos(x);
}

static inline void
u128_imul10(uint128_t *x) {
    x->hi = x->hi * 10 + U64_HI(U64_HI(x->lo) * 10);
    x->lo *= 10;
}

// Digit iterator

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

// Multiplication

static void
u64_mul_u64(uint128_t *z, const uint64_t x, const uint64_t y) {
    const uint64_t xl = U64_LO(x);
    const uint64_t xh = U64_HI(x);
    const uint64_t yl = U64_LO(y);
    const uint64_t yh = U64_HI(y);
    register uint64_t t;

    t = xl * yl;
    z->lo = U64_LO(t);
    t = xl * yh + U64_HI(t);
    z->hi = U64_HI(t);
    t = xh * yl + U64_LO(t);
    z->lo += (U64_LO(t) << 32U);
    z->hi += xh * yh + U64_HI(t);
}

static void
u128_imul_u64(uint128_t *x, const uint64_t y) {
    uint128_t t;

    u64_mul_u64(&t, x->hi, y);
    if (t.hi != 0) {
        SIGNAL_OVERFLOW(x);
        return;
    }
    x->hi = t.lo;
    u64_mul_u64(&t, x->lo, y);
    x->hi += t.hi;
    if (x->hi < t.hi) {
        SIGNAL_OVERFLOW(x);
        return;
    }
    x->lo = t.lo;
}

static inline void
u128_imul_10_pow_n(uint128_t *x, const uint8_t n) {
    for (uint64_t k = MIN(n, UINT64_10_POW_N_CUTOFF); k > 0; k = n - k) {
        if (x->hi == 0)
            u64_mul_u64(x, x->lo, _10_POW_N(k));
        else
            u128_imul_u64(x, _10_POW_N(k));
    }
}

// Division

// adapted from
// D. E. Knuth, The Art of Computer Programming, Vol. 2, Ch. 4.3.1,
// Exercise 16
static uint64_t
u128_idiv_u32(uint128_t *x, uint32_t y) {
    uint64_t th, tl, r;

    assert(y != 0);

    if (y == 1) return 0UL;

    th = U64_HI(x->hi);
    r = th % y;
    tl = (r << 32U) + U64_LO(x->hi);
    x->hi = ((th / y) << 32U) + tl / y;
    r = tl % y;
    th = (r << 32U) + U64_HI(x->lo);
    r = th % y;
    tl = (r << 32U) + U64_LO(x->lo);
    x->lo = ((th / y) << 32U) + tl / y;
    return tl % y;
}

// Specialized version adapted from
// Henry S. Warren, Hacker’s Delight,
// originally found at http://www.hackersdelight.org/HDcode/divlu.c.txt.
// That code is in turn based on Algorithm D from
// D. E. Knuth, The Art of Computer Programming, Vol. 2, Ch. 4.3.1,
// adapted to the special case m = 4 and n = 2 and x->hi < y (!).
// The link given above does not exist anymore, but the code can still be
// found at https://github.com/hcs0/Hackers-Delight/blob/master/divlu.c.txt.
static uint64_t
u128_idiv_u64_special(uint64_t *xlo, uint64_t *xhi, uint64_t y) {
    const uint64_t b = 1UL << 32U;
    unsigned n_bits;
    uint64_t xn0, xn1, xn10, xn32, yn0, yn1, t, rhat;

    assert(U64_HI(y) != 0);
    assert(*xhi < y);

    // Normalize dividend and divisor, so that y > 2^63 (i.e. highest bit set)
    n_bits = u64_n_leading_0_bits(y);
    y <<= n_bits;
    yn1 = U64_HI(y);
    yn0 = U64_LO(y);

    xn32 = (*xhi << n_bits) | (n_bits == 0 ? 0 : *xlo >> (64 - n_bits));
    xn10 = *xlo << n_bits;
    xn0 = U64_LO(xn10);
    xn1 = U64_HI(xn10);

    *xhi = xn32 / yn1;
    rhat = xn32 % yn1;
    // Now we have
    // xhi * yn1 + rhat = xn32
    // so that
    // xhi * yn1 * 2^32 + rhat * 2^32 + xn1 = xn32 * 2^32 + xn1
    while (*xhi >= b || *xhi * yn0 > (rhat << 32U) + xn1) {
        (*xhi)--;
        rhat += yn1;
        if (rhat >= b)
            break;
    }
    // The loop did not change the equation given above. It was terminated if
    // either xhi < 2^32 or rhat >= 2^32 or xhi * yn0 > rhat * 2^32 + xn1.
    // In these cases follows:
    // xhi * yn0 <= rhat * 2^32 + xn1, therefor
    // xhi * yn1 * 2^32 + xhi * yn0 <= xn32 * 2^32 + xn1, and
    // xhi * y <= xn32 * 2^32 + xn1, and
    // xn32 * 2^32 + xn1 - xhi * y >= 0.
    // That means that the add-back step in Knuth's algorithm is not required.

    // Since the final quotient is < 2^64, this must also be true for
    // xn32 * 2^32 + xn1 - xhi * y. Thus, in the following we can safely
    // ignore any possible overflow in xn32 * 2^32 or xhi * y.
    t = (xn32 << 32U) + xn1 - *xhi * y;

    *xlo = t / yn1;
    rhat = t % yn1;
    while (*xlo >= b ||
            *xlo * yn0 > (rhat << 32U) + xn0) {
        (*xlo)--;
        rhat += yn1;
        if (rhat >= b)
            break;
    }

    // Denormalize remainder
    return ((t << 32U) + xn0 - *xlo * y) >> n_bits;
}

static uint64_t
u128_idiv_u64(uint128_t *x, uint64_t y) {
    uint64_t t;

    assert(y != 0);

    if (U64_HI(y) == 0)
        return u128_idiv_u32(x, U64_LO(y));

    if (x->hi < y)
        return u128_idiv_u64_special(&(x->lo), &(x->hi), y);

    t = x->hi % y;
    x->hi /= y;
    return u128_idiv_u64_special(&(x->lo), &t, y);
}

// The following code is based on Algorithm D from
// D. E. Knuth, The Art of Computer Programming, Vol. 2, Ch. 4.3.1,
// adapted to base 2^64 and special case m = n = 2
static void
u128_idiv_u128_special(uint128_t *r, uint128_t *x, const uint128_t *y) {
    unsigned n_bits_left, n_bits_right;
    uint64_t xn[3], yn[2], q;
    uint128_t t = {0, 0};

    assert(y->hi != 0);
    assert(u128_cmp(x, y) >= 0);

    // Normalize dividend and divisor, so that y->hi > 2^63
    // (i.e. highest bit set)
    n_bits_left = u64_n_leading_0_bits(y->hi);   // n_bits_left < 64
    n_bits_right = 64 - n_bits_left;
    yn[0] = y->lo << n_bits_left;
    yn[1] = (y->hi << n_bits_left) + (y->lo >> n_bits_right);
    xn[0] = x->lo << n_bits_left;
    xn[1] = (x->hi << n_bits_left) + (x->lo >> n_bits_right);
    xn[2] = x->hi >> n_bits_right;

    // m = 2, n = 2
    // D2: Loop j not nessary because j = m - n = 0
    // D3: Calculate estimation of quotient
    // yn[1] > 2^63 and xn[2] < 2^63 => xn[2] < yn[1] => q < 2^64
    t.lo = xn[1];
    t.hi = xn[2];
    r->lo = u128_idiv_u64_special(&(t.lo), &(t.hi), yn[1]);
    assert(t.hi == 0);
    q = t.lo;
    // D4: Multiply and subtract
    t.lo = y->lo;
    t.hi = y->hi;
    u128_imul_u64(&t, q);
    u128_sub_u128(r, &t, x);
    // D5: Test remainder
    if (u128_cmp(r, y) <= 0) {
        x->lo = q;
        x->hi = 0;
    }
    else {
        // D6: Add back
        x->lo = ++q;
        x->hi = 0;
        u128_isub_u128(r, y);
    }
}

static void
u128_idiv_u128(uint128_t *r, uint128_t *x, const uint128_t *y) {
    int cmp;

    assert(y->hi != 0);

    cmp = u128_cmp(x, y);

    // Special cases
    if (cmp == 0) {
        x->lo = 1;
        x->hi = 0;
        r->lo = 0;
        r->hi = 0;
    }
    else if (cmp < 0) {
        r->lo = x->lo;
        r->hi = x->hi;
        x->lo = 0;
        x->hi = 0;
    }
    else {
        u128_idiv_u128_special(r, x, y);
    }
}

static void
u128_idivr_10_pow_n(uint128_t *x, const fpdec_sign_t sign, const uint8_t n,
                    const enum FPDEC_ROUNDING_MODE rounding) {
    uint64_t rem, divisor;

    assert(n <= UINT64_10_POW_N_CUTOFF);

    divisor = _10_POW_N(n);
    if (x->hi != 0)
            rem = u128_idiv_u64(x, divisor);
    else {
            rem = x->lo % divisor;
            x->lo /= divisor;
    }
    if (round_qr(sign, x->lo, rem, false, divisor, rounding) > 0) {
        u128_incr(x);
    }
}

// Decimal shift

void
u128_idecshift(uint128_t *ui, fpdec_sign_t sign, int n_dec_digits,
               enum FPDEC_ROUNDING_MODE rounding) {
    assert(n_dec_digits != 0);
    assert(n_dec_digits > -MAX_N_DEC_DIGITS_IN_SHINT);
    assert(n_dec_digits < MAX_N_DEC_DIGITS_IN_SHINT);

    if (n_dec_digits >= 0)
        u128_imul_10_pow_n(ui, n_dec_digits);
    else
        u128_idivr_10_pow_n(ui, sign, -n_dec_digits, rounding);
}

// Converter

error_t
shint_from_dec_coeff(uint64_t *lo, uint32_t *hi, const dec_digit_t *coeff,
                     const size_t n_dec_digits, const size_t n_add_zeros) {
    uint128_t accu = {0, 0};
    const dec_digit_t *stop = coeff + n_dec_digits;
    const dec_digit_t *cut = MIN(coeff + UINT64_10_POW_N_CUTOFF, stop);

    for (; coeff < cut; ++coeff) {
        accu.lo *= 10;
        accu.lo += *coeff;           // *coeff is < 10, so no overflow here
    }
    for (; coeff < stop; ++coeff) {
        u128_imul10(&accu);
        accu.lo += *coeff;           // *coeff is < 10, so no overflow here
    }
    for (int i = 0; i < n_add_zeros; ++i) {
        u128_imul10(&accu);
    }
    if (U64_HI(accu.hi)) return FPDEC_N_DIGITS_LIMIT_EXCEEDED;
    *lo = accu.lo;
    *hi = (uint32_t) accu.hi;
    return FPDEC_OK;
}

void
shint_to_digits(uint64_t *digit, size_t n_digits, uint64_t b,
                uint64_t lo, uint32_t hi, int prec) {
    uint128_t t = {lo, hi};

    assert(prec <= UINT64_10_POW_N_CUTOFF);

    if (prec > 0) {
        *digit = u128_idiv_u64(&t, _10_POW_N(prec)) *
                _10_POW_N(UINT64_10_POW_N_CUTOFF - prec);
        digit++;
    }
    *digit = u128_idiv_u64(&t, b);
    digit++;
    *digit = t.lo;
}
