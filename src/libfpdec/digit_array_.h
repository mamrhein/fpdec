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


/*****************************************************************************
*  Macros
*****************************************************************************/

#define DEC_DIGITS_PER_DIGIT 19             // int(log10(2^64))
#define RADIX 10000000000000000000UL        // 10 ** DEC_DIGITS_PER_DIGIT
#define MAX_DIGIT 9999999999999999999UL     // RADIX - 1

/*****************************************************************************
*  Functions
*****************************************************************************/

// digit getter

fpdec_digit_t
digits_get_digit(fpdec_digit_array_t *digit_array, fpdec_n_digits_t idx);

// digit iterator

digit_iter
digits_iter_digits(fpdec_digit_array_t *digit_array);

// converter

error_t
digits_from_dec_coeff_exp(fpdec_digit_array_t **digit_array, fpdec_exp_t *exp,
                          size_t n_dec_digits, const dec_digit_t *coeff,
                          int dec_exp);

#endif //FPDEC_DIGIT_ARRAY__H
