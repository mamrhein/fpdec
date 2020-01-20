/* ---------------------------------------------------------------------------
Name:        parser
Purpose:     ${PURPOSE}

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
#include <stddef.h>
#include <string.h>

#include "parser.h"

/*****************************************************************************
*  Functions
*****************************************************************************/

// parse for a Decimal
// [+|-]<int>[.<frac>][<e|E>[+|-]<exp>] or
// [+|-].<frac>[<e|E>[+|-]<exp>].
int
parse_ascii_dec_literal(dec_str_repr *result, const uchar *literal) {
    const uchar *curr_char = literal;
    const uchar *int_part = NULL;
    const uchar *signif_int_part = NULL;
    ptrdiff_t len_int_part = 0;
    const uchar *frac_part = NULL;
    ptrdiff_t len_frac_part = 0;

    while isspace(*curr_char) {
        curr_char++;
    }
    if (*curr_char == 0) {
        return FPDEC_INVALID_DECIMAL_LITERAL;
    }
    if (*curr_char == '+' || *curr_char == '-') {
        result->sign = *curr_char;
        curr_char++;
    }
    else {
        result->sign = '+';
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
            return FPDEC_INVALID_DECIMAL_LITERAL;
        }
    }
    if (*curr_char == 'e' || *curr_char == 'E') {
        int sign = 1;
        int exp = 0;
        curr_char++;
        if (*curr_char == '+') {
            curr_char++;
        }
        if (*curr_char == '-') {
            sign = -1;
            curr_char++;
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
        return FPDEC_INVALID_DECIMAL_LITERAL;
    }
    assert(len_int_part + len_frac_part <= result->n_chars);
    strncat((char*)result->coeff, (char*)signif_int_part, len_int_part);
    strncat((char*)result->coeff, (char*)frac_part, len_frac_part);
    result->exp -= len_frac_part;
    return FPDEC_OK;
}
