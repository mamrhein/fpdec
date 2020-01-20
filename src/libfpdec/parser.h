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

#ifndef FPDEC_PARSER_H
#define FPDEC_PARSER_H

#include "common.h"

/*****************************************************************************
*  Types
*****************************************************************************/

typedef struct {
    uchar sign;
    int exp;
    size_t n_chars;
    uchar coeff[];
} dec_str_repr;

/*****************************************************************************
*  Functions
*****************************************************************************/

int
parse_ascii_dec_literal(dec_str_repr *result, const uchar *literal);

#endif //FPDEC_PARSER_H
