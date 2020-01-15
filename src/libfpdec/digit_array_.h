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


/*****************************************************************************
*  Functions
*****************************************************************************/

// digit getter

fpdec_digit_t
digits_get_digit(fpdec_digit_array_t *digit_array, fpdec_n_digits_t idx);

// digit iterator

digit_iter
digits_iter_digits(fpdec_digit_array_t *digit_array);

#endif //FPDEC_DIGIT_ARRAY__H
