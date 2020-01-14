/*
------------------------------------------------------------------------------
Name:        digit_array.c

Author:      Michael Amrhein (michael@adrhinum.de)

Copyright:   (c) 2020 ff. Michael Amrhein
License:     This program is part of a larger application or library.
             For license details please read the file LICENSE provided
             together with the application or library.
------------------------------------------------------------------------------
$Source$
$Revision$
*/


#include "digit_array.h"

/*****************************************************************************
*  Macros
*****************************************************************************/

#define BITS_PER_DIGIT = 64
#define BYTES_PER_DIGIT = 8                     // BITS_PER_DIGIT / 8
#define DEC_DIGITS_PER_DIGIT = 19               // int(log10(2 ** 64))
#define RADIX = 10000000000000000000UL          // 10 ** DEC_DIGITS_PER_DIGIT
#define MAX_DIGIT = 9999999999999999999UL       // RADIX - 1
#define MAX_DIGIT_T = 18446744073709551615UL    // 2 ** BITS_PER_DIGIT - 1
