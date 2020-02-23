/*
------------------------------------------------------------------------------
Name:        rounding_.h

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


static inline fpdec_digit_t
round_qr(fpdec_sign_t sign, fpdec_digit_t quot, fpdec_digit_t rem,
         fpdec_digit_t divisor, enum FPDEC_ROUNDING_MODE rounding) {
    const uint64_t max_tie = 0x8000000000000000;
    fpdec_digit_t tie;

    // divisor == 0 means divisor == 2^64
    assert(divisor == 0 || rem < divisor);
    assert(0 <= rounding && rounding <= FPDEC_MAX_ROUNDING_MODE);

    if (rounding == FPDEC_ROUND_DEFAULT) {
        rounding = fpdec_get_default_rounding_mode();
    }

    switch (rounding) {
        case FPDEC_ROUND_05UP:
            // Round down unless last digit is 0 or 5
            if (quot % 5 == 0)
                return 1;
            break;
        case FPDEC_ROUND_CEILING:
            // Round towards Infinity (i. e. not towards 0 if non-negative)
            if (sign >= 0)
                return 1;
            break;
        case FPDEC_ROUND_DOWN:
            // Round towards 0 (aka truncate)
            break;
        case FPDEC_ROUND_FLOOR:
            // Round towards -Infinity (i.e. not towards 0 if negative)
            if (sign < 0)
                return 1;
            break;
        case FPDEC_ROUND_HALF_DOWN:
            // Round 5 down, rest to nearest
            tie = divisor > 0 ? divisor >> 1U : max_tie;
            if (rem > tie) {
                return 1;
            }
            break;
        case FPDEC_ROUND_HALF_EVEN:
            // Round 5 to nearest even, rest to nearest
            tie = divisor > 0 ? divisor >> 1U : max_tie;
            if (rem > tie || (rem == tie && quot % 2 != 0))
                return 1;
            break;
        case FPDEC_ROUND_HALF_UP:
            // Round 5 up (away from 0), rest to nearest
            tie = divisor > 0 ? divisor >> 1U : max_tie;
            if (rem >= tie)
                return 1;
            break;
        case FPDEC_ROUND_UP:
            // Round away from 0
            return 1;
        default:
            return 0;
    }
    // fall-through: round towards 0
    return 0;
}

fpdec_digit_t
round_to_multiple(fpdec_sign_t sign, fpdec_digit_t num, fpdec_digit_t quant,
                  enum FPDEC_ROUNDING_MODE rounding);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //FPDEC_ROUNDING__H
