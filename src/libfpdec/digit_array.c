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
