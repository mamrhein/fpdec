/* ---------------------------------------------------------------------------
Name:        checks.hpp

Author:      Michael Amrhein (michael@adrhinum.de)

Copyright:   (c) 2020 ff. Michael Amrhein
License:     This program is part of a larger application. For license
             details please read the file LICENSE.TXT provided together
             with the application.
------------------------------------------------------------------------------
$Source$
$Revision$
*/

#ifndef FPDEC_CHECKS_HPP
#define FPDEC_CHECKS_HPP

#include "fpdec.h"

static inline bool
is_shint(fpdec_t *fpdec) {
    return !(fpdec->dyn_alloc && fpdec->normalized);
}

static inline bool
is_digit_array(fpdec_t *fpdec) {
    return (fpdec->dyn_alloc && fpdec->normalized);
}

static inline bool check_normalized(fpdec_t *fpdec) {
    fpdec_digit_array_t *digit_array = fpdec->digit_array;
    fpdec_n_digits_t n_signif = digit_array->n_signif;
    return n_signif == 0 ||
           (digit_array->digits[0] != 0 &&
            digit_array->digits[n_signif - 1] != 0);
}

#endif //FPDEC_CHECKS_HPP
