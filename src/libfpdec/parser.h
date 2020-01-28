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

#ifndef FPDEC_PARSER_H
#define FPDEC_PARSER_H

/*****************************************************************************
*  Types
*****************************************************************************/

typedef struct {
    char sign;
    int exp;
    size_t n_dec_digits;
    char coeff[];
} dec_repr_t;

/*****************************************************************************
*  Functions
*****************************************************************************/

dec_repr_t *
parse_ascii_dec_literal(const char *literal);

#endif //FPDEC_PARSER_H
