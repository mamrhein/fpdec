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

#include "digit_array_.h"


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

// digit array allocator

static inline fpdec_digit_array_t *
digits_alloc(size_t n_digits) {
    fpdec_digit_array_t *digit_array = (fpdec_digit_array_t *) \
        calloc(n_digits + 1, sizeof(fpdec_digit_t));
    if (digit_array != NULL) {
        digit_array->n_alloc = n_digits;
    }
    return digit_array;
}

// digit getter

fpdec_digit_t
digits_get_digit(fpdec_digit_array_t *digit_array, fpdec_n_digits_t idx) {
    return idx < digit_array->n_signif ? digit_array->digits[idx] :
           (fpdec_digit_t) FPDEC_DIGIT_MAX;
}

// digit iterator

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

// converter

static inline fpdec_digit_t
dec_digits_to_digit(char *start, const char *stop) {
    fpdec_digit_t digit = 0;
    for (; start < stop; ++start) {
        digit = digit * 10 + *start - '0';
    }
    return digit;
}

error_t
digits_from_dec_coeff_exp(fpdec_digit_array_t **digit_array, fpdec_exp_t *exp,
                          const size_t n_dec_digits, const char *coeff,
                          const int dec_exp) {
    size_t n_digits, n_dec_shift;
    fpdec_digit_t *digit;
    char *chunk_start, *chunk_stop;

    assert(n_dec_digits > 0);

    n_digits = CEIL(n_dec_digits, DEC_DIGITS_PER_DIGIT);
    *digit_array = digits_alloc(n_digits);
    if (*digit_array == NULL) MEMERROR(ENOMEM)

    *exp = FLOOR(dec_exp, DEC_DIGITS_PER_DIGIT);
    n_dec_shift = MOD(dec_exp, DEC_DIGITS_PER_DIGIT);
    digit = (*digit_array)->digits;
    chunk_stop = (char *) coeff + n_dec_digits;
    chunk_start = MAX(chunk_stop + n_dec_shift - DEC_DIGITS_PER_DIGIT,
                      (char *) coeff);
    while (chunk_stop > coeff) {
        *digit = dec_digits_to_digit(chunk_start, chunk_stop);
        if (*digit == 0) {
            (*exp)++;
            n_digits--;
        } else {
            digit++;
        }
        chunk_stop = chunk_start;
        chunk_start = MAX(chunk_start - DEC_DIGITS_PER_DIGIT, (char *) coeff);
    }
    // least significant digit to be shifted?
    if (n_dec_shift > 0) {
        (*digit_array)->digits[0] *= _10_POW_N(n_dec_shift);
    }
    (*digit_array)->n_signif = n_digits;
    return FPDEC_OK;
}