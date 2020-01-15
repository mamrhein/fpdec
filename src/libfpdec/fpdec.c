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
#include "digit_array_.h"
#include "shifted_int_.h"


/*****************************************************************************
*  Functions
*****************************************************************************/

// For testing only!

void
fpdec_dump(fpdec_t *fpdec) {
    digit_iter it = FPDEC_DIGITS(fpdec);

    printf("flags:\n  dyn_alloc: %d\n  normalized: %d\n",
           FPDEC_IS_DYN_ALLOC(fpdec), FPDEC_IS_NORMALIZED(fpdec));
    printf("sign: %d\n", FPDEC_SIGN(fpdec));
    printf("dec_prec: %u\n", FPDEC_DEC_PREC(fpdec));
    printf("exp: %d\n", FPDEC_EXP(fpdec));
    printf("n digits: %u\n", it.limit);
    printf("digits: ");
    while (it.next_idx < it.limit) {
        printf("%lu, ", it.next(&it));
    }
    printf("\n\n");
}
