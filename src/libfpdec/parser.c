/* ---------------------------------------------------------------------------
Name:        parser

Author:      Michael Amrhein (michael@adrhinum.de)

Copyright:   (c) 2020 ff. Michael Amrhein
License:     This program is part of a larger application. For license
             details please read the file LICENSE.TXT provided together
             with the application.
------------------------------------------------------------------------------
$Source$
$Revision$
*/

#include <assert.h>
#include <ctype.h>
#include <malloc.h>
#include <stddef.h>
#include <string.h>

#include "parser.h"

/*****************************************************************************
*  Functions
*****************************************************************************/

// parse for a Decimal
// [+|-]<int>[.<frac>][<e|E>[+|-]<exp>] or
// [+|-].<frac>[<e|E>[+|-]<exp>].
dec_str_repr_t *
parse_ascii_dec_literal(const char *literal) {
    dec_str_repr_t *result;
    size_t n_chars = strlen(literal);
    const char *curr_char = literal;
    const char *int_part = NULL;
    const char *signif_int_part = NULL;
    ptrdiff_t len_int_part = 0;
    const char *frac_part = NULL;
    ptrdiff_t len_frac_part = 0;

    if (n_chars == 0) {
        return NULL;
    }
    while isspace(*curr_char) {
        curr_char++;
    }
    if (*curr_char == 0) {
        return NULL;
    }

    result = malloc(offsetof(dec_str_repr_t, coeff) + n_chars + 1);
    result->sign = '+';
    result->exp = 0;
    result->n_chars = 0;
    result->coeff[0] = '\0';

    if (*curr_char == '-' || *curr_char == '+') {
        result->sign = *curr_char;
        curr_char++;
    }
    int_part = curr_char;
    while (*curr_char == '0') {
        curr_char++;
    }
    signif_int_part = curr_char;
    while (isdigit(*curr_char)) {
        curr_char++;
    }
    len_int_part = curr_char - signif_int_part;
    if (*curr_char == '.') {
        curr_char++;
        frac_part = curr_char;
        while (isdigit(*curr_char)) {
            curr_char++;
        }
        len_frac_part = curr_char - frac_part;
    }
    if (len_int_part == 0 && len_frac_part == 0) {
        if (*int_part == '0') {
            signif_int_part = int_part;
            len_int_part = 1;
        }
        else {
            free(result);
            return NULL;
        }
    }
    if (*curr_char == 'e' || *curr_char == 'E') {
        int sign = 1;
        int exp = 0;
        curr_char++;
        switch (*curr_char) {
            case '-':
                sign = -1;
            case '+':
                curr_char++;
                break;
            default:
                if (!isdigit(*curr_char)) {
                    return NULL;
                }
        }
        while isdigit(*curr_char) {
            // TODO: check overflow
            exp = exp * 10 + (*curr_char - '0');
            curr_char++;
        }
        result->exp = sign * exp;
    }
    while isspace(*curr_char) {
        curr_char++;
    }
    if (*curr_char != 0) {
        free(result);
        return NULL;
    }
    assert(len_int_part + len_frac_part <= n_chars);
    strncat(result->coeff, signif_int_part, len_int_part);
    strncat(result->coeff, frac_part, len_frac_part);
    result->n_chars = len_int_part + len_frac_part;
    result->exp -= len_frac_part;
    return result;
}
