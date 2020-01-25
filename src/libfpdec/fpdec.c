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
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "fpdec.h"
#include "digit_array_.h"
#include "parser.h"
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

// Initializer

int
fpdec_from_asci_literal(fpdec_t *fpdec, const char *literal) {
    size_t n_chars = strlen(literal);
    dec_str_repr_t *dec_str_repr;
    int rc;
    size_t n_add_zeros, n_dec_digits;

    if (n_chars == 0) {
        return FPDEC_INVALID_DECIMAL_LITERAL;
    }
    dec_str_repr = malloc(offsetof(dec_str_repr_t, coeff) + n_chars + 1);
    dec_str_repr->n_chars = n_chars;
    dec_str_repr->coeff[0] = '\0';
    rc = parse_ascii_dec_literal(dec_str_repr, literal);
    if (rc != FPDEC_OK) {
        return rc;
    }
    fpdec->sign = dec_str_repr->sign == '-' ? FPDEC_SIGN_NEG : FPDEC_SIGN_POS;
    n_chars = strlen(dec_str_repr->coeff);
    n_add_zeros = MAX(0, dec_str_repr->exp);
    n_dec_digits = n_chars + n_add_zeros;
    if (n_dec_digits <= MAX_N_DEC_DIGITS_IN_SHINT) {
        rc = shint_from_coeff_exp(&fpdec->lo, &fpdec->hi, dec_str_repr->coeff,
                                  n_add_zeros);
        if (rc == FPDEC_OK) {
            fpdec->dec_prec = MAX(0, -dec_str_repr->exp);
            goto EXIT;
        }
    }
    // TODO: digit array variant
    rc = FPDEC_N_DIGITS_LIMIT_EXCEEDED;
EXIT:
    free(dec_str_repr);
    return rc;
}
