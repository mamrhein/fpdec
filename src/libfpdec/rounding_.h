/*
------------------------------------------------------------------------------
Name:        _rnd.h

Author:      Michael Amrhein (michael@adrhinum.de)

Copyright:   (c) 2020 ff. Michael Amrhein
License:     This program is part of a larger application or library.
             For license details please read the file LICENSE provided
             together with the application or library.
------------------------------------------------------------------------------
$Source:$
$Revision:$
*/

#ifndef FPDEC__RND_H
#define FPDEC__RND_H

#include <stdint.h>
#include "rounding.h"

static inline uint64_t
round_to_multiple(char sign, uint64_t num, uint64_t quant,
                  enum FPDEC_ROUNDING_MODE rnd);;

#endif //FPDEC__RND_H
