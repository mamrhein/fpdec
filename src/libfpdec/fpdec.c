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

#define FPDEC_DYN_EXP(fpdec) (((fpdec_t*)fpdec)->exp)

#define FPDEC_DYN_N_DIGITS(fpdec) (((fpdec_t*)fpdec)->digit_array->n_signif)

#define FPDEC_IS_ZEROED(fpdec) (!FPDEC_IS_DYN_ALLOC(fpdec) && \
                                !FPDEC_IS_NORMALIZED(fpdec) && \
                                FPDEC_SIGN(fpdec) == 0 && \
                                FPDEC_DEC_PREC(fpdec) == 0 && \
                                ((fpdec_t*)fpdec)->hi == 0 && \
                                ((fpdec_t*)fpdec)->lo == 0)

#define ASSERT_FPDEC_IS_ZEROED(fpdec) assert(FPDEC_IS_ZEROED(fpdec))

/*****************************************************************************
*  Functions
*****************************************************************************/

// For testing only!

void
fpdec_dump(fpdec_t *fpdec) {
    digit_iter it = FPDEC_DIGITS(fpdec);

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
        fpdec->digit_array = digits_copy(src->digit_array);
        if (fpdec->digit_array == NULL) return ENOMEM;
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
    if (n_dec_digits <= MAX_N_DEC_DIGITS_IN_SHINT) {
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
    fpdec->dyn_alloc = true;
    fpdec->normalized = true;
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

// Converter

error_t
fpdec_neg(fpdec_t *fpdec, fpdec_t *src) {
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
    error_t rc;

    assert(!FPDEC_IS_DYN_ALLOC(fpdec));

    shint_to_digits(digits, 3, RADIX, fpdec->lo, fpdec->hi,
                    FPDEC_DEC_PREC(fpdec));
    rc = digits_from_digits(&fpdec->digit_array, digits, 3);
    if (rc == FPDEC_OK) {
        fpdec->dyn_alloc = true;
        fpdec->normalized = true;
        fpdec->exp = -CEIL(FPDEC_DEC_PREC(fpdec), DEC_DIGITS_PER_DIGIT);
    }
    return rc;
}

static error_t
fpdec_dyn_adjust_to_prec(fpdec_t *fpdec,
                         const fpdec_dec_prec_t dec_prec,
                         const enum FPDEC_ROUNDING_MODE rounding) {
    size_t radix_point_at = -FPDEC_DYN_EXP(fpdec) * DEC_DIGITS_PER_DIGIT;
    if (dec_prec <= FPDEC_DEC_PREC(fpdec) && dec_prec < radix_point_at) {
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
            fpdec_dealloc(fpdec);
            // *fpdec = FPDEC_ZERO
        }
        // else {
        // TODO: try to transform result to shifted int
        // }
    }
    FPDEC_DEC_PREC(fpdec) = dec_prec;
    return FPDEC_OK;
}

static error_t
fpdec_shint_adjust_to_prec(fpdec_t *fpdec,
                           const fpdec_dec_prec_t dec_prec,
                           const enum FPDEC_ROUNDING_MODE rounding) {
    error_t rc;
    int dec_shift = dec_prec - FPDEC_DEC_PREC(fpdec);
    uint128_t shifted = {fpdec->lo, fpdec->hi};

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

    if (FPDEC_IS_DYN_ALLOC(fpdec)) {
        return fpdec_dyn_adjust_to_prec(fpdec, dec_prec, rounding);
    }
    else {
        return fpdec_shint_adjust_to_prec(fpdec, dec_prec, rounding);
    }
}

// Deallocator

void
fpdec_dealloc(fpdec_t *fpdec) {
    if (FPDEC_IS_DYN_ALLOC(fpdec)) {
        free((void *) fpdec->digit_array);
    }
    memset((void *) fpdec, 0, sizeof(fpdec_t));
}
