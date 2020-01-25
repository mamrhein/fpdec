/*
------------------------------------------------------------------------------
Name:        common.h

Author:      Michael Amrhein (michael@adrhinum.de)

Copyright:   (c) 2020 ff. Michael Amrhein
License:     This program is part of a larger application or library.
             For license details please read the file LICENSE provided
             together with the application or library.
------------------------------------------------------------------------------
$Source$
$Revision$
*/

#ifndef FPDEC_COMMON_H
#define FPDEC_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdint.h>


/*****************************************************************************
*  Common internal types
*****************************************************************************/

// sign indicator: =0 -> zero, <0 -> negative, >0 -> positive
typedef int8_t fpdec_sign_t;

// single digit (base 2 ** 64 or 10 ** 19)
typedef uint64_t fpdec_digit_t;

// digit counter
typedef uint32_t fpdec_n_digits_t;

// one byte character
typedef uint8_t uchar;

/*****************************************************************************
*  Macros
*****************************************************************************/

// limits
#define FPDEC_DIGIT_MAX UINT64_MAX

// sign
#define FPDEC_SIGN_ZERO 0
#define FPDEC_SIGN_NEG -1
#define FPDEC_SIGN_POS 1

// error codes
#define FPDEC_OK 0
#define FPDEC_PREC_LIMIT_EXCEEDED -1
#define FPDEC_EXP_LIMIT_EXCEEDED -2
#define FPDEC_N_DIGITS_LIMIT_EXCEEDED -3
#define FPDEC_INVALID_DECIMAL_LITERAL -4

// max / min
#define MAX(a, b) (a >= b ? a : b)
#define MIN(a, b) (a <= b ? a : b)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //FPDEC_COMMON_H
