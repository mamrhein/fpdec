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
