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
*  Types
*****************************************************************************/

typedef struct {
    uint64_t lo;
    uint64_t hi;
} uint128_t;

/*****************************************************************************
*  Macros
*****************************************************************************/

#define MAX_N_DEC_DIGITS_IN_SHINT 29
#define MAX_N_DEC_DIGITS_UINT64 20

#define U128_HI(x) (((uint128_t)x).hi)
#define U128_LO(x) (((uint128_t)x).lo)

#define U64_HI(x) (((uint64_t)x) >> 32U)
#define U64_LO(x) (((uint64_t)x) & 0xFFFFFFFF)

/*****************************************************************************
*  Functions
*****************************************************************************/

// digit iterator

digit_iter
shint_iter_digits(fpdec_digit_t lo, fpdec_digit_t hi);

// converter

error_t
shint_from_coeff_exp(uint64_t *lo, uint32_t *hi, const char *coeff, int exp);

#endif //FPDEC_SHIFTED_INT__H
