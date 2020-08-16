/*
------------------------------------------------------------------------------
Name:        fpdec_struct.h

Author:      Michael Amrhein (michael@adrhinum.de)

Copyright:   (c) 2020 ff. Michael Amrhein
License:     This program is part of a larger application or library.
             For license details please read the file LICENSE provided
             together with the application or library.
------------------------------------------------------------------------------
$Source$
$Revision$
*/

#ifndef FPDEC_STRUCT_H
#define FPDEC_STRUCT_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "common.h"


/*****************************************************************************
*  Types
*****************************************************************************/

struct fpdec_struct {
    bool dyn_alloc: 1;           // true indicates digit array
    bool normalized: 1;          // true if digit array is normalized
    fpdec_sign_t sign;          // sign indicator
    fpdec_dec_prec_t dec_prec;  // number of decimal fractional digits
    //                             variants:
    union {                     // shifted int          digit_array
        uint32_t hi;            // high 32 bits
        fpdec_exp_t exp;        //                      exponent (base 2**64)
    };
    union {
        fpdec_digit_t lo;       // low  64 bits
        fpdec_digit_array_t *digit_array;   //          pointer to digit array
    };
};

/*****************************************************************************
*  Macros
*****************************************************************************/

// Properties

#define FPDEC_IS_DYN_ALLOC(fpdec) (((fpdec_t*)fpdec)->dyn_alloc)

#define FPDEC_IS_NORMALIZED(fpdec) (((fpdec_t*)fpdec)->normalized)

#define FPDEC_EQ_ZERO(fpdec) (((fpdec_t*)fpdec)->sign == FPDEC_SIGN_ZERO)

#define FPDEC_LT_ZERO(fpdec) (((fpdec_t*)fpdec)->sign == FPDEC_SIGN_NEG)

#define FPDEC_GT_ZERO(fpdec) (((fpdec_t*)fpdec)->sign == FPDEC_SIGN_POS)

// Access to members

#define FPDEC_SIGN(fpdec) (((fpdec_t*)fpdec)->sign)

#define FPDEC_DEC_PREC(fpdec) (((fpdec_t*)fpdec)->dec_prec)

#define FPDEC_EXP(fpdec) \
        (FPDEC_IS_DYN_ALLOC(fpdec) ? ((fpdec_t*)fpdec)->exp : 0)

#define FPDEC_N_DIGITS(fpdec) \
        (FPDEC_IS_DYN_ALLOC(fpdec) ? \
            ((fpdec_t*)fpdec)->digit_array->n_signif : 2)


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // FPDEC_STRUCT_H
