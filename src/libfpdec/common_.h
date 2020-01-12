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


#ifndef FPDEC_COMMON__H
#define FPDEC_COMMON__H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdint.h>

/*****************************************************************************
*  Common internal types
*****************************************************************************/

typedef int8_t fpdec_sign_t;                // sign indicator:
                                            // =0 -> zero
                                            // <0 -> negative
                                            // >0 -> positive
typedef uint64_t fpdec_digit_t;             // single digit
                                            // (base 2 ** 64 or 10 ** 19)

/*****************************************************************************
*  Constants
*****************************************************************************/

// sign
#define FPDEC_SIGN_ZERO 0
#define FPDEC_SIGN_NEG -1
#define FPDEC_SIGN_POS 1


#ifdef __cplusplus
}
#endif // __cplusplus


#endif //FPDEC_COMMON__H
