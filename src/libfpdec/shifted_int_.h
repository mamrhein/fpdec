/*
------------------------------------------------------------------------------
Name:        shifted_int_.h

Author:      Michael Amrhein (michael@adrhinum.de)

Copyright:   (c) 2020 ff. Michael Amrhein
License:     This program is part of a larger application or library.
             For license details please read the file LICENSE provided
             together with the application or library.
------------------------------------------------------------------------------
$Source$
$Revision$
*/


#ifndef FPDEC_SHIFTED_INT__H
#define FPDEC_SHIFTED_INT__H


#include "common_.h"
#include "shifted_int.h"


/*****************************************************************************
*  Functions
*****************************************************************************/

// digit iterator

digit_iter
shint_iter_digits(fpdec_digit_t lo, fpdec_digit_t hi);

#endif //FPDEC_SHIFTED_INT__H
