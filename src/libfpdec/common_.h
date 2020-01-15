/*
------------------------------------------------------------------------------
Name:        common_.h

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

#include <stddef.h>

#include "common.h"


/*****************************************************************************
*  Types
*****************************************************************************/

struct _it_t;

typedef fpdec_digit_t (*get_next)(struct _it_t *iter);

typedef struct _it_t  {
    fpdec_n_digits_t limit;
    size_t next_idx;
    get_next next;
    union {
        fpdec_digit_t *ptr_to_digits;
        fpdec_digit_t digits[2];
    };
} digit_iter;

#endif //FPDEC_COMMON__H
