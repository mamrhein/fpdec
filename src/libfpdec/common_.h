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

// error_t may or may not be available from errno.h
#ifndef __error_t_defined
    #define __error_t_defined 1
    typedef int error_t;
#endif

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


#endif //FPDEC_COMMON__H
