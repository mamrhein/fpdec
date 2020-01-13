/*
------------------------------------------------------------------------------
Name:        fpdec.c
Purpose:     Fixed-point decimal arithmetic

Author:      Michael Amrhein (michael@adrhinum.de)

Copyright:   (c) 2020 ff. Michael Amrhein
License:     This program is part of a larger application or library.
             For license details please read the file LICENSE provided
             together with the application or library.
------------------------------------------------------------------------------
$Source$
$Revision$
*/

#include <assert.h>
#include <stdio.h>

#include "fpdec.h"


/*****************************************************************************
*  Functions
*****************************************************************************/

// For testing only!

void
fpdec_dump(fpdec_t *fpdec) {
    printf("flags:\n  dyn_alloc: %d\n  normalized: %d\n",
           FPDEC_IS_DYN_ALLOC(fpdec), FPDEC_IS_NORMALIZED(fpdec));
    printf("sign: %d\n", FPDEC_SIGN(fpdec));
    printf("dec_prec: %u\n", FPDEC_DEC_PREC(fpdec));
    printf("exp: %d\n", FPDEC_EXP(fpdec));
    if FPDEC_IS_DYN_ALLOC(fpdec) {
        fpdec_n_digits_t n = FPDEC_N_DIGITS(fpdec);
        printf("n digits: %u\n", n);
        printf("digits: [");
        if (n > 0) {
            n -= 1;
            // TODO: use FPDEC_DIGITS when ready
            for (int i = 0; i < n; ++i) {
                printf("%lu, ", fpdec->digit_array->digits[i]);
            }
            printf("%lu", fpdec->digit_array->digits[n]);
        }
        printf("]\n");
    }
    else {
        printf("n digits: 2\n");
        printf("digits: [%u, %lu]\n", fpdec->hi, fpdec->lo);
    }
    printf("\n");
}
