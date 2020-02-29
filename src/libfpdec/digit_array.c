/*
------------------------------------------------------------------------------
Name:        digit_array.c

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
#include <stddef.h>
#include <string.h>

#include "digit_array_.h"
#include "rounding_.h"


/*****************************************************************************
*  Macros
*****************************************************************************/

#define BITS_PER_DIGIT 64
#define BYTES_PER_DIGIT 8                       // BITS_PER_DIGIT / 8
#define DEC_DIGITS_PER_DIGIT 19                 // int(log10(2 ** 64))
#define RADIX 10000000000000000000UL            // 10 ** DEC_DIGITS_PER_DIGIT
#define MAX_DIGIT 9999999999999999999UL         // RADIX - 1


/*****************************************************************************
*  Functions
*****************************************************************************/

// Digit array allocator

static inline fpdec_digit_array_t *
digits_alloc(size_t n_digits) {
    fpdec_digit_array_t *digit_array = (fpdec_digit_array_t *) \
        calloc(n_digits + 1, sizeof(fpdec_digit_t));
    if (digit_array != NULL) {
        digit_array->n_alloc = n_digits;
    }
    return digit_array;
}

// Digit getter

fpdec_digit_t
digits_get_digit(fpdec_digit_array_t *digit_array, fpdec_n_digits_t idx) {
    return idx < digit_array->n_signif ? digit_array->digits[idx] :
           (fpdec_digit_t) FPDEC_DIGIT_MAX;
}

// Digit iterator

fpdec_digit_t
_digits_get_next_digit(digit_iter *it) {
    return it->next_idx < it->limit ? it->ptr_to_digits[it->next_idx++] :
           (fpdec_digit_t) FPDEC_DIGIT_MAX;
}

digit_iter
digits_iter_digits(fpdec_digit_array_t *digit_array) {
    digit_iter it = {
            .limit = digit_array->n_signif,
            .next_idx = 0,
            .next = _digits_get_next_digit,
            .ptr_to_digits = digit_array->digits
    };
    return it;
}

// Tests

static inline bool
digits_all_zero(fpdec_digit_t *digits, fpdec_n_digits_t n) {
    for (fpdec_digit_t *digit = digits; digit < digits + n; ++digit)
        if (*digit != 0)
            return false;
    return true;
}

// Constructors

static inline void
digits_set_zero(fpdec_digit_t *digits, fpdec_n_digits_t n) {
    for (fpdec_digit_t *digit = digits; digit < digits + n; ++digit)
        *digit = 0;
}

fpdec_digit_array_t *
digits_copy(fpdec_digit_array_t *src) {
    fpdec_digit_array_t *result;

    assert(src != NULL);

    result = digits_alloc(src->n_signif);
    if (result != NULL) {
        result->n_signif = src->n_signif;
        memcpy(result->digits, src->digits,
               src->n_signif * sizeof(fpdec_digit_t));
    }
    return result;
}

static inline fpdec_digit_t
dec_digits_to_digit(const dec_digit_t *start, const dec_digit_t *stop) {
    fpdec_digit_t digit = 0;
    for (; start < stop; ++start) {
        digit *= 10;
        digit += *start;
    }
    return digit;
}

error_t
digits_from_dec_coeff_exp(fpdec_digit_array_t **digit_array, fpdec_exp_t *exp,
                          size_t n_dec_digits, const dec_digit_t *coeff,
                          int dec_exp) {
    size_t n_digits, n_dec_shift;
    fpdec_digit_t *digit;
    const dec_digit_t *chunk_start, *chunk_stop;

    assert(n_dec_digits > 0);

    *exp = FLOOR(dec_exp, DEC_DIGITS_PER_DIGIT);
    n_dec_shift = (size_t) MOD(dec_exp, DEC_DIGITS_PER_DIGIT);
    n_digits = CEIL(n_dec_digits + n_dec_shift, DEC_DIGITS_PER_DIGIT);
    *digit_array = digits_alloc(n_digits);
    if (*digit_array == NULL) MEMERROR

    digit = (*digit_array)->digits;
    chunk_stop = coeff + n_dec_digits;
    chunk_start = MAX(chunk_stop + n_dec_shift - DEC_DIGITS_PER_DIGIT, coeff);
    while (chunk_stop > coeff) {
        *digit = dec_digits_to_digit(chunk_start, chunk_stop);
        digit++;
        chunk_stop = chunk_start;
        chunk_start = MAX(chunk_start - DEC_DIGITS_PER_DIGIT, coeff);
    }
    // least significant digit to be shifted?
    if (n_dec_shift > 0) {
        (*digit_array)->digits[0] *= _10_POW_N(n_dec_shift);
    }
    // cut-off leading zeroes
    digit = (*digit_array)->digits + n_digits - 1;
    while (n_digits > 0 && *digit == 0) {
        n_digits--;
        digit--;
    }
    (*digit_array)->n_signif = n_digits;
    return FPDEC_OK;
}

error_t
digits_from_digits(fpdec_digit_array_t **digit_array,
                   const fpdec_digit_t *digits, size_t n_digits) {
    assert(n_digits > 0);
    assert(*digits != 0);

    for (; n_digits > 0 && digits[n_digits - 1] == 0; --n_digits);
    *digit_array = digits_alloc(n_digits);
    if (*digit_array == NULL) MEMERROR

    (*digit_array)->n_signif = n_digits;
    for (size_t i = 0; i < n_digits; ++i)
        (*digit_array)->digits[i] = digits[i];
    return FPDEC_OK;
}

// Normalization

static inline fpdec_digit_t *
least_signif_digit(fpdec_digit_array_t *digit_array) {
    fpdec_digit_t *digit = digit_array->digits;
    fpdec_digit_t *stop = digit_array->digits + digit_array->n_signif;

    while (digit < stop && *digit == 0)
        digit++;
    return digit;
}

fpdec_n_digits_t
digits_eliminate_trailing_zeros(fpdec_digit_array_t *digit_array) {
    fpdec_digit_t *lsd = least_signif_digit(digit_array);
    fpdec_n_digits_t n_trailing_zeros = lsd - digit_array->digits;

    if (n_trailing_zeros > 0) {
        digit_array->n_signif -= n_trailing_zeros;
        for (int i = 0; i < digit_array->n_signif; ++i) {
            digit_array->digits[i] = lsd[i];
        }
    }
    return n_trailing_zeros;
}

// Rounding

bool
digits_round(fpdec_digit_array_t *digit_array, fpdec_sign_t sign,
             size_t n_dec_shift,
             enum FPDEC_ROUNDING_MODE rounding) {
    const fpdec_digit_t *stop = digit_array->digits + digit_array->n_signif;
    fpdec_n_digits_t digit_idx;
    fpdec_digit_t *digit;
    size_t n_dec_digits;

    assert(digit_array->n_signif > 0);
    assert(n_dec_shift > 0);
    assert(n_dec_shift <= digit_array->n_signif * DEC_DIGITS_PER_DIGIT);

    /* digit points to the digit to be rounded, n_dec_digits is the number
       of decimal digits to be rounded */
    digit_idx = FLOOR(n_dec_shift, DEC_DIGITS_PER_DIGIT);
    digit = digit_array->digits + digit_idx;
    n_dec_digits = MOD(n_dec_shift, DEC_DIGITS_PER_DIGIT);
    if (n_dec_digits == 0) {
        // need to round the previous digit ...
        fpdec_digit_t quot = (digit_idx < digit_array->n_signif) ? *digit : 0;
        digit--;
        digit_idx--;
        // ... but only if there is a remainder
        if (*digit != 0 || digit_idx > 0) {
            *digit = RADIX * round_qr(sign, quot, *digit, digit_idx > 0,
                                      RADIX, rounding);
        }
    }
    else {
        *digit = round_to_multiple(sign, *digit, digit_idx > 0,
                                   _10_POW_N(n_dec_digits),
                                   rounding);
    }
    // set all skiped digits to zero
    if (digit_idx > 0)
        digits_set_zero(digit_array->digits, digit_idx);
    // check and propagate carry
    if (*digit == RADIX) {
        do
            *digit = 0;
        while (++digit < stop && ++(*digit) == RADIX);
    }
    return (digit == stop);
}
