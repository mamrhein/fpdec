/*
------------------------------------------------------------------------------
Name:        rounding.c

Author:      Michael Amrhein (michael@adrhinum.de)

Copyright:   (c) 2020 ff. Michael Amrhein
License:     This program is part of a larger application or library.
             For license details please read the file LICENSE provided
             together with the application or library.
------------------------------------------------------------------------------
$Source$
$Revision$
*/

#include <assert.h>

#include "rounding_.h"


static enum FPDEC_ROUNDING_MODE dflt_rounding_mode = FPDEC_ROUND_HALF_EVEN;


enum FPDEC_ROUNDING_MODE
fpdec_get_default_rounding_mode() {
    return dflt_rounding_mode;
}


enum FPDEC_ROUNDING_MODE
fpdec_set_default_rounding_mode(enum FPDEC_ROUNDING_MODE rnd) {
    assert(rnd > FPDEC_DEFAULT_ROUNDING_MODE);
    assert(rnd <= FPDEC_MAX_ROUNDING_MODE);

    dflt_rounding_mode = rnd;
    return rnd;
}


static inline fpdec_digit_t
round_qr(fpdec_sign_t sign, fpdec_digit_t quot, fpdec_digit_t rem,
         fpdec_digit_t quant, enum FPDEC_ROUNDING_MODE rnd) {
    fpdec_digit_t tie;

    assert(quant > 0);
    assert(rem < quant);
    assert(0 <= rnd && rnd <= FPDEC_MAX_ROUNDING_MODE);

    if (rnd == FPDEC_DEFAULT_ROUNDING_MODE) {
        rnd = FPDEC_ROUND_HALF_EVEN;
    }

    switch (rnd) {
        case FPDEC_ROUND_05UP:
            // Round down unless last digit is 0 or 5
            if (quot % 5 == 0)
                return quant - rem;
            break;
        case FPDEC_ROUND_CEILING:
            // Round towards Infinity (i. e. not towards 0 if non-negative)
            if (sign >= 0)
                return quant - rem;
            break;
        case FPDEC_ROUND_DOWN:
            // Round towards 0 (aka truncate)
            break;
        case FPDEC_ROUND_FLOOR:
            // Round towards -Infinity (i.e. not towards 0 if negative)
            if (sign < 0)
                return quant - rem;
            break;
        case FPDEC_ROUND_HALF_DOWN:
            // Round 5 down, rest to nearest
            if (rem > quant >> 1U)
                return quant - rem;
            break;
        case FPDEC_ROUND_HALF_EVEN:
            // Round 5 to nearest even, rest to nearest
            tie = quant >> 1U;
            if (rem > tie || (rem == tie && quot % 2 != 0))
                return quant - rem;
            break;
        case FPDEC_ROUND_HALF_UP:
            // Round 5 up (away from 0), rest to nearest
            if (rem >= quant >> 1U)
                return quant - rem;
            break;
        case FPDEC_ROUND_UP:
            // Round away from 0
            return quant - rem;
        default:
            return 0;
    }
    // fall-through: round towards 0
    return -rem;
}


fpdec_digit_t
round_to_multiple(fpdec_sign_t sign, fpdec_digit_t num, fpdec_digit_t quant,
                  enum FPDEC_ROUNDING_MODE rnd) {
    fpdec_digit_t quot, rem;

    rem = num % quant;
    if (rem == 0)
        return num;
    else {
        quot = num / quant;
        return num + round_qr(sign, quot, rem, quant, rnd);
    }
}
