/*
------------------------------------------------------------------------------
Name:        _rnd.h

Author:      Michael Amrhein (michael@adrhinum.de)

Copyright:   (c) 2020 ff. Michael Amrhein
License:     This program is part of a larger application or library.
             For license details please read the file LICENSE provided
             together with the application or library.
------------------------------------------------------------------------------
$Source$
$Revision$
*/

#ifndef FPDEC_ROUNDING__H
#define FPDEC_ROUNDING__H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "common.h"
#include "rounding.h"

fpdec_digit_t
round_to_multiple(fpdec_sign_t, fpdec_digit_t, fpdec_digit_t,
                  enum FPDEC_ROUNDING_MODE);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //FPDEC_ROUNDING__H
