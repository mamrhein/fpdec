/*
------------------------------------------------------------------------------
Name:        fpdec.c
Purpose:     Fixed-point decimal arithmetic

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
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "fpdec.h"
#include "digit_array_.h"
#include "parser.h"
#include "shifted_int_.h"
#include "rounding_.h"

/*****************************************************************************
*  Macros
*****************************************************************************/

#define U128_FROM_SHINT(x) {x->lo, x->hi}

#define FPDEC_DYN_EXP(fpdec) (((fpdec_t*)fpdec)->exp)

#define FPDEC_DYN_N_DIGITS(fpdec) (((fpdec_t*)fpdec)->digit_array->n_signif)

#define FPDEC_DYN_DIGITS(fpdec) (((fpdec_t*)fpdec)->digit_array->digits)

#define FPDEC_DYN_MOST_SIGNIF_DIGIT(fpdec) \
        (FPDEC_DYN_DIGITS(fpdec)[FPDEC_DYN_N_DIGITS(fpdec) - 1])

#define FPDEC_IS_ZEROED(fpdec) (!FPDEC_IS_DYN_ALLOC(fpdec) && \
                                !FPDEC_IS_NORMALIZED(fpdec) && \
                                FPDEC_SIGN(fpdec) == 0 && \
                                FPDEC_DEC_PREC(fpdec) == 0 && \
                                ((fpdec_t*)fpdec)->hi == 0 && \
                                ((fpdec_t*)fpdec)->lo == 0)

#define ASSERT_FPDEC_IS_ZEROED(fpdec) assert(FPDEC_IS_ZEROED(fpdec))

#define DISPATCH_FUNC(vtab, fpdec) \
        (vtab[FPDEC_IS_DYN_ALLOC(fpdec)])(fpdec)

#define DISPATCH_FUNC_VA(vtab, fpdec, ...) \
        (vtab[FPDEC_IS_DYN_ALLOC(fpdec)])(fpdec, __VA_ARGS__)

#define DISPATCH_BIN_EXPR(vtab, x, y) \
        (vtab[((FPDEC_IS_DYN_ALLOC(x)) << 1U) + FPDEC_IS_DYN_ALLOC(y)])(x, y)

#define DISPATCH_BIN_OP(vtab, z, x, y) \
        (vtab[((FPDEC_IS_DYN_ALLOC(x)) << 1U) + FPDEC_IS_DYN_ALLOC(y)]) \
                (z, x, y)

#define DISPATCH_BIN_OP_VA(vtab, z, x, y, ...) \
        (vtab[((FPDEC_IS_DYN_ALLOC(x)) << 1U) + FPDEC_IS_DYN_ALLOC(y)]) \
                (z, x, y, __VA_ARGS__)

/*****************************************************************************
*  Functions
*****************************************************************************/

// For testing only!

void
fpdec_dump(const fpdec_t *fpdec) {
    digit_iter it = FPDEC_ITER_DIGITS(fpdec);

    printf("flags:\n  dyn_alloc: %d\n  normalized: %d\n",
           FPDEC_IS_DYN_ALLOC(fpdec), FPDEC_IS_NORMALIZED(fpdec));
    printf("sign: %d\n", FPDEC_SIGN(fpdec));
    printf("dec_prec: %u\n", FPDEC_DEC_PREC(fpdec));
    printf("exp: %d\n", FPDEC_EXP(fpdec));
    printf("n digits: %u\n", it.limit);
    printf("digits: ");
    while (it.next_idx < it.limit) {
        printf("%lu, ", it.next(&it));
    }
    printf("\n\n");
}

// Initializer

static inline error_t
fpdec_copy(fpdec_t *fpdec, const fpdec_t *src) {
    *fpdec = *src;
    if (src->dyn_alloc) {
        fpdec->digit_array = digits_copy(src->digit_array, 0, 0);
        if (fpdec->digit_array == NULL) MEMERROR
    }
    return FPDEC_OK;
}

error_t
fpdec_from_ascii_literal(fpdec_t *fpdec, const char *literal) {
    size_t n_chars = strlen(literal);
    dec_repr_t st_dec_repr;
    dec_repr_t *dec_repr;
    size_t n_add_zeros, n_dec_digits;
    error_t rc;

    ASSERT_FPDEC_IS_ZEROED(fpdec);

    if (n_chars == 0) ERROR(FPDEC_INVALID_DECIMAL_LITERAL)

    if (n_chars <= COEFF_SIZE_THRESHOLD) {
        dec_repr = &st_dec_repr;
    }
    else {
        dec_repr = malloc(offsetof(dec_repr_t, coeff) + n_chars);
        if (dec_repr == NULL) MEMERROR
    }
    rc = parse_ascii_dec_literal(dec_repr, literal);
    if (rc != FPDEC_OK) ERROR(rc)

    fpdec->sign = dec_repr->negative ? FPDEC_SIGN_NEG : FPDEC_SIGN_POS;
    n_add_zeros = MAX(0, dec_repr->exp);
    n_dec_digits = dec_repr->n_dec_digits + n_add_zeros;
    if (n_dec_digits <= MAX_N_DEC_DIGITS_IN_SHINT &&
            -dec_repr->exp <= MAX_DEC_PREC_FOR_SHINT) {
        rc = shint_from_dec_coeff(&fpdec->lo, &fpdec->hi,
                                  dec_repr->coeff, dec_repr->n_dec_digits,
                                  n_add_zeros);
        if (rc == FPDEC_OK) {
            fpdec->dyn_alloc = false;
            fpdec->normalized = false;
            fpdec->dec_prec = MAX(0, -dec_repr->exp);
            if (fpdec->lo == 0 && fpdec->hi == 0) {
                fpdec->sign = FPDEC_SIGN_ZERO;
            }
            goto EXIT;
        }
    }
    rc = digits_from_dec_coeff_exp(&(fpdec->digit_array), &(fpdec->exp),
                                   dec_repr->n_dec_digits, dec_repr->coeff,
                                   dec_repr->exp);
    if (rc != FPDEC_OK)
        goto EXIT;
    fpdec->dyn_alloc = true;
    fpdec->exp += digits_eliminate_trailing_zeros(fpdec->digit_array);
    if (FPDEC_DYN_N_DIGITS(fpdec) > 0) {
        fpdec->normalized = true;
    }
    else {      // corner case: result == 0
        fpdec_reset_to_zero(fpdec);
    }
    fpdec->dec_prec = MAX(0, -dec_repr->exp);
EXIT:
    if (dec_repr != &st_dec_repr) {
        free(dec_repr);
    }
    return rc;
}

error_t
fpdec_from_long_long(fpdec_t *fpdec, const long long val) {
    ASSERT_FPDEC_IS_ZEROED(fpdec);

    if (val > 0) {
        fpdec->sign = FPDEC_SIGN_POS;
        fpdec->lo = val;
    }
    else if (val < 0) {
        fpdec->sign = FPDEC_SIGN_NEG;
        fpdec->lo = -val;
    }
    return FPDEC_OK;
}

// Properties

static int
fpdec_shint_magnitude(const fpdec_t *fpdec) {
    if (fpdec->hi == 0)
        return U64_MAGNITUDE(fpdec->lo) - fpdec->dec_prec;
    else
        return U128_MAGNITUDE(fpdec->lo, fpdec->hi) - fpdec->dec_prec;
}

static int
fpdec_dyn_magnitude(const fpdec_t *fpdec) {
    int rel_pos_radix_point = FPDEC_DYN_N_DIGITS(fpdec) + FPDEC_EXP(fpdec);
    return (rel_pos_radix_point - 1) * DEC_DIGITS_PER_DIGIT +
            U64_MAGNITUDE(FPDEC_DYN_MOST_SIGNIF_DIGIT(fpdec));
}

typedef int (*v_magnitude)(const fpdec_t *);

const v_magnitude vtab_magnitude[2] = {fpdec_shint_magnitude,
                                       fpdec_dyn_magnitude};

int
fpdec_magnitude(const fpdec_t *fpdec) {
    if (FPDEC_EQ_ZERO(fpdec)) ERROR_RETVAL(ERANGE, -1);
    return DISPATCH_FUNC(vtab_magnitude, fpdec);
}

// Comparison

// Pre-condition: magnitude(x) == magnitude(y)
static int
fpdec_cmp_abs_shint_to_shint(const fpdec_t *x, const fpdec_t *y) {
    uint128_t x_shint = U128_FROM_SHINT(x);
    uint128_t y_shint = U128_FROM_SHINT(y);

    return shint_cmp_abs(x_shint, x->dec_prec, y_shint, y->dec_prec);
}

// Pre-condition: magnitude(x) == magnitude(y)
static int
fpdec_cmp_abs_shint_to_dyn(const fpdec_t *x, const fpdec_t *y) {
    fpdec_digit_t x_digits[3];
    int n_trailing_zeros_skipped;
    fpdec_n_digits_t x_n_digits;

    x_n_digits = shint_to_digits(x_digits, &n_trailing_zeros_skipped, RADIX,
                                 x->lo, x->hi, FPDEC_DEC_PREC(x));
    return digits_cmp(x_digits, x_n_digits,
                      FPDEC_DYN_DIGITS(y), FPDEC_DYN_N_DIGITS(y));
}

// Pre-condition: magnitude(x) == magnitude(y)
static int
fpdec_cmp_abs_dyn_to_shint(const fpdec_t *x, const fpdec_t *y) {
    fpdec_digit_t y_digits[3];
    int n_trailing_zeros_skipped;
    fpdec_n_digits_t y_n_digits;

    y_n_digits = shint_to_digits(y_digits, &n_trailing_zeros_skipped, RADIX,
                                 y->lo, y->hi, FPDEC_DEC_PREC(y));
    return digits_cmp(FPDEC_DYN_DIGITS(x), FPDEC_DYN_N_DIGITS(x),
                      y_digits, y_n_digits);
}

// Pre-condition: magnitude(x) == magnitude(y)
static int
fpdec_cmp_abs_dyn_to_dyn(const fpdec_t *x, const fpdec_t *y) {
    return digits_cmp(FPDEC_DYN_DIGITS(x), FPDEC_DYN_N_DIGITS(x),
                      FPDEC_DYN_DIGITS(y), FPDEC_DYN_N_DIGITS(y));
}

typedef int (*v_cmp)(const fpdec_t *, const fpdec_t *);

const v_cmp vtab_cmp[4] = {fpdec_cmp_abs_shint_to_shint,
                           fpdec_cmp_abs_shint_to_dyn,
                           fpdec_cmp_abs_dyn_to_shint,
                           fpdec_cmp_abs_dyn_to_dyn};

int
fpdec_compare(const fpdec_t *x, const fpdec_t *y, const bool ignore_sign) {
    fpdec_sign_t x_sign, y_sign;
    int x_magn, y_magn;

    if (ignore_sign) {
        if (FPDEC_SIGN(x) == 0)
            return FPDEC_SIGN(y) ? -1 : 0;
        if (FPDEC_SIGN(y) == 0)
            return FPDEC_SIGN(x) != 0;
        x_sign = FPDEC_SIGN_POS;
        y_sign = FPDEC_SIGN_POS;
    }
    else {
        x_sign = FPDEC_SIGN(x);
        y_sign = FPDEC_SIGN(y);
        if (x_sign != y_sign)
            return CMP(x_sign, y_sign);
        if (x_sign == 0)
            return 0;
    }

    // here: x != 0 and y != 0
    x_magn = fpdec_magnitude(x);
    y_magn = fpdec_magnitude(y);
    if (x_magn != y_magn)
        return CMP(x_magn, y_magn) * x_sign;

    return DISPATCH_BIN_EXPR(vtab_cmp, x, y) * x_sign;
}

// Converter

error_t
fpdec_neg(fpdec_t *fpdec, const fpdec_t *src) {
    error_t rc;

    ASSERT_FPDEC_IS_ZEROED(fpdec);

    rc = fpdec_copy(fpdec, src);
    if (rc == ENOMEM) MEMERROR
    fpdec->sign = -src->sign;
    return FPDEC_OK;
}

static error_t
fpdec_shint_to_dyn(fpdec_t *fpdec) {
    fpdec_digit_t digits[3];
    int n_trailing_zeros;
    fpdec_n_digits_t n_digits;
    error_t rc;

    assert(!FPDEC_IS_DYN_ALLOC(fpdec));

    n_digits = shint_to_digits(digits, &n_trailing_zeros, RADIX, fpdec->lo,
                               fpdec->hi, FPDEC_DEC_PREC(fpdec));
    rc = digits_from_digits(&fpdec->digit_array, digits, n_digits);
    if (rc == FPDEC_OK) {
        fpdec->dyn_alloc = true;
        fpdec->normalized = true;
        fpdec->exp = n_trailing_zeros -
                CEIL(FPDEC_DEC_PREC(fpdec), DEC_DIGITS_PER_DIGIT);
    }
    return rc;
}

static error_t
fpdec_dyn_adjust_to_prec(fpdec_t *fpdec,
                         const fpdec_dec_prec_t dec_prec,
                         const enum FPDEC_ROUNDING_MODE rounding) {
    size_t radix_point_at = -FPDEC_DYN_EXP(fpdec) * DEC_DIGITS_PER_DIGIT;

    if (dec_prec >= FPDEC_DEC_PREC(fpdec) || dec_prec >= radix_point_at) {
        // no need to adjust digits
        FPDEC_DEC_PREC(fpdec) = dec_prec;
    }
    else {
        // need to shorten / round digits
        size_t dec_shift = radix_point_at - dec_prec;
        if (dec_shift >
                FPDEC_DYN_N_DIGITS(fpdec) * DEC_DIGITS_PER_DIGIT) {
            fpdec_digit_t quant;
            FPDEC_DYN_EXP(fpdec) += dec_shift / DEC_DIGITS_PER_DIGIT;
            dec_shift %= DEC_DIGITS_PER_DIGIT;
            quant = _10_POW_N(dec_shift);
            if (round_qr(FPDEC_SIGN(fpdec), 0UL, 0UL, true, quant,
                         rounding) == 0UL) {
                FPDEC_DYN_N_DIGITS(fpdec) = 0;
            }
            else {
                FPDEC_DYN_N_DIGITS(fpdec) = 1;
                fpdec->digit_array->digits[0] = quant;
            }
        }
        else {
            bool carry = digits_round(fpdec->digit_array,
                                      FPDEC_SIGN(fpdec),
                                      dec_shift, rounding);
            if (carry) {
                // total carry-over
                FPDEC_DYN_EXP(fpdec) += FPDEC_DYN_N_DIGITS(fpdec);
                fpdec->digit_array->digits[0] = 1UL;
                FPDEC_DYN_N_DIGITS(fpdec) = 1;
            }
            else {
                FPDEC_DYN_EXP(fpdec) +=
                        digits_eliminate_trailing_zeros(
                                fpdec->digit_array);
            }
        }
        if (FPDEC_DYN_N_DIGITS(fpdec) == 0) {
            fpdec_reset_to_zero(fpdec);
            // *fpdec = FPDEC_ZERO
        }
        // else {
        // TODO: try to transform result to shifted int
        // }
        FPDEC_DEC_PREC(fpdec) = dec_prec;
    }
    return FPDEC_OK;
}

static error_t
fpdec_shint_adjust_to_prec(fpdec_t *fpdec,
                           const fpdec_dec_prec_t dec_prec,
                           const enum FPDEC_ROUNDING_MODE rounding) {
    error_t rc;
    int dec_shift = dec_prec - FPDEC_DEC_PREC(fpdec);
    uint128_t shifted = U128_FROM_SHINT(fpdec);

    u128_idecshift(&shifted, FPDEC_SIGN(fpdec), dec_shift, rounding);
    if (!U128_FITS_SHINT(shifted)) {
        rc = fpdec_shint_to_dyn(fpdec);
        if (rc != FPDEC_OK)
            return rc;
        return fpdec_dyn_adjust_to_prec(fpdec, dec_prec, rounding);
    }
    else {
        if (shifted.lo > 0 || shifted.hi > 0) {
            fpdec->lo = shifted.lo;
            fpdec->hi = shifted.hi;
        }
        else {
            FPDEC_SIGN(fpdec) = FPDEC_SIGN_ZERO;
            fpdec->lo = 0;
            fpdec->hi = 0;
        }
    }
    FPDEC_DEC_PREC(fpdec) = dec_prec;
    return FPDEC_OK;
}

typedef error_t (*v_adjust_to_prec)(fpdec_t *, fpdec_dec_prec_t,
                                    enum FPDEC_ROUNDING_MODE);

const v_adjust_to_prec vtab_adjust_to_prec[2] = {fpdec_shint_adjust_to_prec,
                                                 fpdec_dyn_adjust_to_prec};

error_t
fpdec_adjusted(fpdec_t *fpdec, const fpdec_t *src,
               const fpdec_dec_prec_t dec_prec,
               const enum FPDEC_ROUNDING_MODE rounding) {
    error_t rc;

    ASSERT_FPDEC_IS_ZEROED(fpdec);

    rc = fpdec_copy(fpdec, src);
    if (rc == ENOMEM) MEMERROR

    if (FPDEC_DEC_PREC(fpdec) == dec_prec)
        return FPDEC_OK;

    return DISPATCH_FUNC_VA(vtab_adjust_to_prec, fpdec, dec_prec, rounding);
}

// Basic arithmetic operations

static inline fpdec_dec_prec_t
make_adjusted_shints(uint128_t *x_shint, uint128_t *y_shint,
                     const fpdec_dec_prec_t x_dec_prec,
                     const fpdec_dec_prec_t y_dec_prec) {
    int shift = x_dec_prec - y_dec_prec;
    fpdec_dec_prec_t prec;

    if (shift == 0)
        prec = x_dec_prec;
    else if (shift > 0) {
        prec = x_dec_prec;
        u128_imul_10_pow_n(y_shint, shift);
    }
    else {
        prec = y_dec_prec;
        u128_idecshift(x_shint, FPDEC_SIGN_POS, -shift, FPDEC_ROUND_DEFAULT);
    }
    return prec;
}

static error_t
fpdec_add_abs_shint_to_shint(fpdec_t *z, const fpdec_t *x, const fpdec_t *y) {
    uint128_t x_shint = U128_FROM_SHINT(x);
    uint128_t y_shint = U128_FROM_SHINT(y);

    FPDEC_DEC_PREC(z) = make_adjusted_shints(&x_shint, &y_shint,
                                             FPDEC_DEC_PREC(x),
                                             FPDEC_DEC_PREC(y));
    u128_iadd_u128(&x_shint, &y_shint);
    z->lo = x_shint.lo;
    z->hi = x_shint.hi;
    if (U128_FITS_SHINT(x_shint))
        return FPDEC_OK;
    else
        return fpdec_shint_to_dyn(z);
}

static error_t
fpdec_add_abs_dyn_to_dyn(fpdec_t *z, const fpdec_t *x, const fpdec_t *y) {
    fpdec_n_digits_t n_shift, n_add_zeros;
    fpdec_digit_array_t *z_digits;
    const fpdec_digit_array_t *s_digits;
    fpdec_exp_t z_exp;

    if (FPDEC_DYN_EXP(x) == FPDEC_DYN_EXP(y)) {
        n_shift = 0;                // no need to adjust exponents
        n_add_zeros = 1;            // provision for potential carry-over
        if (FPDEC_DYN_N_DIGITS(x) >= FPDEC_DYN_N_DIGITS(y)) {
            z_digits = digits_copy(x->digit_array, n_shift, n_add_zeros);
            if (z_digits == NULL) MEMERROR
            s_digits = y->digit_array;
        }
        else {
            z_digits = digits_copy(y->digit_array, n_shift, n_add_zeros);
            if (z_digits == NULL) MEMERROR
            s_digits = x->digit_array;
        }
        z_exp = FPDEC_DYN_EXP(x);
    }
    else if (FPDEC_DYN_EXP(x) > FPDEC_DYN_EXP(y)) {
        n_shift = FPDEC_DYN_EXP(x) - FPDEC_DYN_EXP(y);
        n_add_zeros = MAX((int) FPDEC_DYN_N_DIGITS(y) -
                                  (int) FPDEC_DYN_N_DIGITS(x) -
                                  (int) n_shift + 1,
                          1);
        z_digits = digits_copy(x->digit_array, n_shift, n_add_zeros);
        if (z_digits == NULL) MEMERROR
        s_digits = y->digit_array;
        z_exp = FPDEC_DYN_EXP(y);
    }
    else {
        n_shift = FPDEC_DYN_EXP(y) - FPDEC_DYN_EXP(x);
        n_add_zeros = MAX((int) FPDEC_DYN_N_DIGITS(x) -
                                  (int) FPDEC_DYN_N_DIGITS(y) -
                                  (int) n_shift + 1,
                          1);
        z_digits = digits_copy(y->digit_array, n_shift, n_add_zeros);
        if (z_digits == NULL) MEMERROR
        s_digits = x->digit_array;
        z_exp = FPDEC_DYN_EXP(x);
    }
    digits_iadd_digits(z_digits, s_digits);
    z->exp = z_exp + digits_eliminate_trailing_zeros(z_digits);
    z->digit_array = z_digits;
    z->dyn_alloc = true;
    z->normalized = true;
    // ???: try to convert to shifted int
    return FPDEC_OK;
}

static error_t
fpdec_add_abs_dyn_to_shint(fpdec_t *z, const fpdec_t *x, const fpdec_t *y) {
    error_t rc;
    fpdec_t s;

    rc = fpdec_copy(&s, x);
    rc = fpdec_shint_to_dyn(&s);
    if (rc == FPDEC_OK) {
        rc = fpdec_add_abs_dyn_to_dyn(z, &s, y);
        fpdec_reset_to_zero(&s);
    }
    return rc;
}

static error_t
fpdec_add_abs_shint_to_dyn(fpdec_t *z, const fpdec_t *x, const fpdec_t *y) {
    error_t rc;
    fpdec_t s;

    rc = fpdec_copy(&s, y);
    rc = fpdec_shint_to_dyn(&s);
    if (rc == FPDEC_OK) {
        rc = fpdec_add_abs_dyn_to_dyn(z, x, &s);
        fpdec_reset_to_zero(&s);
    }
    return rc;
}

typedef error_t (*v_math_op)(fpdec_t *, const fpdec_t *, const fpdec_t *);

const v_math_op vtab_add_abs[4] = {
        fpdec_add_abs_shint_to_shint,
        fpdec_add_abs_dyn_to_shint,
        fpdec_add_abs_shint_to_dyn,
        fpdec_add_abs_dyn_to_dyn
};

// pre-condition: x >= y
static error_t
fpdec_sub_abs_shint_from_shint(fpdec_t *z, const fpdec_t *x,
                               const fpdec_t *y) {
    uint128_t x_shint = U128_FROM_SHINT(x);
    uint128_t y_shint = U128_FROM_SHINT(y);

    FPDEC_DEC_PREC(z) = make_adjusted_shints(&x_shint, &y_shint,
                                             FPDEC_DEC_PREC(x),
                                             FPDEC_DEC_PREC(y));
    u128_isub_u128(&x_shint, &y_shint);
    z->lo = x_shint.lo;
    z->hi = x_shint.hi;
    return FPDEC_OK;
}

static error_t
fpdec_sub_abs_dyn_from_dyn(fpdec_t *z, const fpdec_t *x, const fpdec_t *y) {
    fpdec_n_digits_t n_shift;
    fpdec_digit_array_t *z_digits;
    fpdec_digit_array_t *s_digits;
    fpdec_exp_t z_exp;

    if (FPDEC_DYN_EXP(x) == FPDEC_DYN_EXP(y)) {
        n_shift = 0;                // no need to adjust exponents
        z_digits = digits_copy(x->digit_array, n_shift, 0);
        if (z_digits == NULL) MEMERROR
        s_digits = y->digit_array;
        z_exp = FPDEC_DYN_EXP(x);
    }
    else if (FPDEC_DYN_EXP(x) > FPDEC_DYN_EXP(y)) {
        n_shift = FPDEC_DYN_EXP(x) - FPDEC_DYN_EXP(y);
        z_digits = digits_copy(x->digit_array, n_shift, 0);
        if (z_digits == NULL) MEMERROR
        s_digits = y->digit_array;
        z_exp = FPDEC_DYN_EXP(y);
    }
    else {
        z_digits = digits_copy(x->digit_array, 0, 0);
        if (z_digits == NULL) MEMERROR
        n_shift = FPDEC_DYN_EXP(y) - FPDEC_DYN_EXP(x);
        s_digits = digits_copy(y->digit_array, n_shift, 0);
        if (s_digits == NULL) MEMERROR
        z_exp = FPDEC_DYN_EXP(x);
    }
    digits_isub_digits(z_digits, s_digits);
    z->exp = z_exp + digits_eliminate_trailing_zeros(z_digits);
    z->digit_array = z_digits;
    z->dyn_alloc = true;
    z->normalized = true;
    // ???: try to convert to shifted int
    return FPDEC_OK;
}

static error_t
fpdec_sub_abs_dyn_from_shint(fpdec_t *z, const fpdec_t *x, const fpdec_t *y) {
    error_t rc;
    fpdec_t s;

    rc = fpdec_copy(&s, x);
    rc = fpdec_shint_to_dyn(&s);
    if (rc == FPDEC_OK) {
        rc = fpdec_sub_abs_dyn_from_dyn(z, &s, y);
        fpdec_reset_to_zero(&s);
    }
    return rc;
}

static error_t
fpdec_sub_abs_shint_from_dyn(fpdec_t *z, const fpdec_t *x, const fpdec_t *y) {
    error_t rc;
    fpdec_t s;

    rc = fpdec_copy(&s, y);
    rc = fpdec_shint_to_dyn(&s);
    if (rc == FPDEC_OK) {
        rc = fpdec_sub_abs_dyn_from_dyn(z, x, &s);
        fpdec_reset_to_zero(&s);
    }
    return rc;
}

const v_math_op vtab_sub_abs[4] = {
        fpdec_sub_abs_shint_from_shint,
        fpdec_sub_abs_dyn_from_shint,
        fpdec_sub_abs_shint_from_dyn,
        fpdec_sub_abs_dyn_from_dyn
};

error_t
fpdec_add(fpdec_t *z, const fpdec_t *x, const fpdec_t *y) {
    int cmp;

    ASSERT_FPDEC_IS_ZEROED(z);

    if (FPDEC_EQ_ZERO(x))
        return fpdec_copy(z, y);
    if (FPDEC_EQ_ZERO(y))
        return fpdec_copy(z, x);

    FPDEC_DEC_PREC(z) = MAX(FPDEC_DEC_PREC(x), FPDEC_DEC_PREC(y));
    if (FPDEC_SIGN(x) == FPDEC_SIGN(y)) {
        // same sign => x + y = sign(x) (|x| + |y|)
        FPDEC_SIGN(z) = FPDEC_SIGN(x);
        return DISPATCH_BIN_OP(vtab_add_abs, z, x, y);
    }
    // sign(x) != sign(y) ...
    cmp = fpdec_compare(x, y, true);
    if (cmp == 1) {
        // ... and |x| > |y| => x + y = sign(x) (|x| - |y|)
        FPDEC_SIGN(z) = FPDEC_SIGN(x);
        return DISPATCH_BIN_OP(vtab_sub_abs, z, x, y);
    }
    if (cmp == -1) {
        // ... and |x| < |y| => x + y = sign(y) (|y| - |x|)
        FPDEC_SIGN(z) = FPDEC_SIGN(y);
        return DISPATCH_BIN_OP(vtab_sub_abs, z, y, x);
    }
    // ... and |x| = |y| => x + y = 0
    return FPDEC_OK;
}

error_t
fpdec_sub(fpdec_t *z, const fpdec_t *x, const fpdec_t *y) {
    int cmp;

    ASSERT_FPDEC_IS_ZEROED(z);

    if (FPDEC_EQ_ZERO(x))
        return fpdec_neg(z, y);
    if (FPDEC_EQ_ZERO(y))
        return fpdec_copy(z, x);

    FPDEC_DEC_PREC(z) = MAX(FPDEC_DEC_PREC(x), FPDEC_DEC_PREC(y));
    if (FPDEC_SIGN(x) != FPDEC_SIGN(y)) {
        // sign(x) != sign(y) => x - y = sign(x) (|x| + |y|)
        FPDEC_SIGN(z) = FPDEC_SIGN(x);
        return DISPATCH_BIN_OP(vtab_add_abs, z, x, y);
    }
    // same sign ...
    cmp = fpdec_compare(x, y, true);
    if (cmp == 1) {
        // ... and |x| > |y| => x - y = sign(x) (|x| - |y|)
        FPDEC_SIGN(z) = FPDEC_SIGN(x);
        return DISPATCH_BIN_OP(vtab_sub_abs, z, x, y);
    }
    if (cmp == -1) {
        // ... and |x| < |y| => x - y = ~sign(y) (|y| - |x|)
        FPDEC_SIGN(z) = FPDEC_SIGN(y) * -1;
        return DISPATCH_BIN_OP(vtab_sub_abs, z, y, x);
    }
    // ... and |x| = |y| => x - y = 0
    return FPDEC_OK;

}

// Deallocator

void
fpdec_reset_to_zero(fpdec_t *fpdec) {
    if (FPDEC_IS_DYN_ALLOC(fpdec)) {
        free((void *) fpdec->digit_array);
    }
    memset((void *) fpdec, 0, sizeof(fpdec_t));
}
