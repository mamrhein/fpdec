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
#include "rounding_.h"

/*****************************************************************************
*  Types
*****************************************************************************/

/*****************************************************************************
*  Macros
*****************************************************************************/

#define MAX_N_DEC_DIGITS_IN_SHINT 29
#define UINT64_10_POW_N_CUTOFF 19

#define UINT128_SET_MAX(x) {x->lo = UINT64_MAX; x->hi = UINT64_MAX;}
#define SIGNAL_OVERFLOW(x) UINT128_SET_MAX(x)

#define U64_HI(x) (((uint64_t)x) >> 32U)
#define U64_LO(x) (((uint64_t)x) & 0xFFFFFFFF)

#define U128_FITS_SHINT(x) (U64_HI(x.hi) == 0)

/*****************************************************************************
*  Functions
*****************************************************************************/

// Digit iterator

digit_iter
shint_iter_digits(fpdec_digit_t lo, fpdec_digit_t hi);

// Converter

error_t
shint_from_dec_coeff(uint64_t *lo, uint32_t *hi, const dec_digit_t *coeff,
                     size_t n_dec_digits, size_t n_add_zeros);

void
shint_to_digits(uint64_t *digit, size_t n_digits, uint64_t b,
                uint64_t lo, uint32_t hi, int prec);

// Decimal shift

void
u128_idecshift(uint128_t *ui, fpdec_sign_t sign, int n_dec_digits,
               enum FPDEC_ROUNDING_MODE rounding);

#endif //FPDEC_SHIFTED_INT__H
