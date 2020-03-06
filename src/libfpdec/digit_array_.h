/*
------------------------------------------------------------------------------
Name:        digit_array_.h

Author:      Michael Amrhein (michael@adrhinum.de)

Copyright:   (c) 2020 ff. Michael Amrhein
License:     This program is part of a larger application or library.
             For license details please read the file LICENSE provided
             together with the application or library.
------------------------------------------------------------------------------
$Source$
$Revision$
*/


#ifndef FPDEC_DIGIT_ARRAY__H
#define FPDEC_DIGIT_ARRAY__H

#include "common_.h"
#include "digit_array.h"
#include "fpdec.h"
#include "rounding.h"


/*****************************************************************************
*  Macros
*****************************************************************************/

#define DEC_DIGITS_PER_DIGIT 19             // int(log10(2^64))
#define RADIX 10000000000000000000UL        // 10 ** DEC_DIGITS_PER_DIGIT
#define MAX_DIGIT 9999999999999999999UL     // RADIX - 1

/*****************************************************************************
*  Functions
*****************************************************************************/

// Digit getter

fpdec_digit_t
digits_get_digit(fpdec_digit_array_t *digit_array, fpdec_n_digits_t idx);

// Digit iterator

digit_iter
digits_iter_digits(fpdec_digit_array_t *digit_array);

// Constructors

fpdec_digit_array_t *
digits_copy(fpdec_digit_array_t *src, fpdec_n_digits_t n_shift,
            fpdec_n_digits_t n_add_leading_zeros);

error_t
digits_from_dec_coeff_exp(fpdec_digit_array_t **digit_array, fpdec_exp_t *exp,
                          size_t n_dec_digits, const dec_digit_t *coeff,
                          int dec_exp);

error_t
digits_from_digits(fpdec_digit_array_t **digit_array,
                   const fpdec_digit_t *digits, size_t n_digits);

// Normalization

fpdec_n_digits_t
digits_eliminate_trailing_zeros(fpdec_digit_array_t *digit_array);

// Rounding

bool
digits_round(fpdec_digit_array_t *digit_array, fpdec_sign_t sign,
             size_t n_dec_shift, enum FPDEC_ROUNDING_MODE rounding);

// Comparison

int digits_cmp(fpdec_digit_t *x_digits, fpdec_n_digits_t x_n_digits,
               fpdec_digit_t *y_digits, fpdec_n_digits_t y_n_digits);

#endif //FPDEC_DIGIT_ARRAY__H
