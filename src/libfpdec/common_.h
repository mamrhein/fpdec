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

typedef struct _it_t {
    fpdec_n_digits_t limit;
    size_t next_idx;
    get_next next;
    union {
        fpdec_digit_t *ptr_to_digits;
        fpdec_digit_t digits[2];
    };
} digit_iter;

/*****************************************************************************
*  Macros
*****************************************************************************/

// error return
#define ERROR(err, retval) {errno = err; return retval;}
#define MEMERROR(retval) ERROR(ENOMEM, retval)
#define FREE_N_ERROR(buf, err, retval) {free(buf); ERROR(err, retval)}

// max / min
#define MAX(a, b) (a >= b ? a : b)
#define MIN(a, b) (a <= b ? a : b)

// modulo arith
#define CEIL(a, b) ((a % b) <= 0 ? a / b : a / b + 1)
#define FLOOR(a, b) ((a % b) < 0 ? a / b - 1 : a / b)
#define MOD(a, b) (a - FLOOR(a, b) * b)

// powers of 10
static uint64_t _10_pows[20] = {
        1UL,
        10UL,
        100UL,
        1000UL,
        10000UL,
        100000UL,
        1000000UL,
        10000000UL,
        100000000UL,
        1000000000UL,
        10000000000UL,
        100000000000UL,
        1000000000000UL,
        10000000000000UL,
        100000000000000UL,
        1000000000000000UL,
        10000000000000000UL,
        100000000000000000UL,
        1000000000000000000UL,
        10000000000000000000UL
};
#define _10_POW_N(exp) (_10_pows[exp])

#endif //FPDEC_COMMON__H
