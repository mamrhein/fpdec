/*
------------------------------------------------------------------------------
Name:        fpdec.h
Purpose:     Fixed-point decimal arithmetic

Author:      Michael Amrhein (michael@adrhinum.de)

Copyright:   (c) 2020 ff. Michael Amrhein
License:     This program is part of a larger application or library.
             For license details please read the file LICENSE provided
             together with the application or library.
------------------------------------------------------------------------------
$Source$
$Revision$
*/

#ifndef FPDEC_H
#define FPDEC_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "common.h"
#include "rounding.h"
#include "digit_array.h"
#include "shifted_int.h"


typedef struct {
    union {
        struct {                                // common elements
            union {
                uint8_t flags;
                struct {
                    uint8_t dyn_alloc:1,        // 1 indicates digit array
                            normalized:1;       // 1 if digit array is
                };                              // normalized
            };
            int8_t sign;                        // sign indicator:
                                                // =0 -> zero
                                                // <0 -> negative
                                                // >0 -> positive
            uint16_t dec_prec;                  // number of decimal
                                                // fractional digits
            uint32_t _filler_1;
            uint64_t _filler_2;
        };
        struct {                                // shifted int variant
            uint32_t _filler_3;
            uint32_t hi;                        // high 32 bits
            uint64_t lo;                        // low  64 bits
        };
        struct {                                // digit array variant
            uint32_t _filler_4;
            int32_t exp;                        // exponent (base 10 ** 19)
            fpdec_digit_array_t *digit_array;   // pointer to digit array
        };
    };
} fpdec_t;

/*****************************************************************************
*  Macros
*****************************************************************************/

// Limits

#define FPDEC_MAX_DEC_PREC 65535                    // 2 ** 16 - 1
#define FPDEC_MIN_EXP -FPDEC_MAX_DEC_PREC / 19 + 1
#define FPDEC_MAX_EXP 2147483647                    // 2 ** 31 - 1

// Properties

#define FPDEC_IS_DYN_ALLOC(fpdec) (((fpdec_t*)fpdec)->dyn_alloc)

#define FPDEC_IS_NORMALIZED(fpdec) \
        (!FPDEC_IS_DYN_ALLOC(fpdec) || ((fpdec_t*)fpdec)->normalized)

/* TODO: shifted int variant
#define FPDEC_IS_INT(fpdec) (((fpdec_t*)fpdec)->dec_prec == 0 || \
                             FPDEC_IS_DYN_ALLOC(fpdec) && \
                             ((fpdec_t*)fpdec)->normalized && \
                             ((fpdec_t*)fpdec)->exp >= 0)
*/

#define FPDEC_EQ_ZERO(fpdec) (((fpdec_t*)fpdec)->sign == FPDEC_SIGN_ZERO)

#define FPDEC_IS_NEGATIVE(fpdec) (((fpdec_t*)fpdec)->sign == FPDEC_SIGN_NEG)

// Access to members

#define FPDEC_SIGN(fpdec) (((fpdec_t*)fpdec)->sign)

#define FPDEC_DEC_PREC(fpdec) (((fpdec_t*)fpdec)->dec_prec)

#define FPDEC_EXP(fpdec) \
        (FPDEC_IS_DYN_ALLOC(fpdec) ? ((fpdec_t*)fpdec)->exp : 0)

#define FPDEC_N_DIGITS(fpdec) \
        (FPDEC_IS_DYN_ALLOC(fpdec) ? \
            ((fpdec_t*)fpdec)->digit_array->n_signif : 2)

// #define FPDEC_DIGITS(fpdec, idx) TODO

/*****************************************************************************
*  Constants
*****************************************************************************/

static fpdec_t FPDEC_ZERO;
static fpdec_t FPDEC_ONE = {{0, 1, 0, 0, 1}};
static fpdec_t FPDEC_MINUS_ONE = {{0, -1, 0, 0, 1}};

/*****************************************************************************
*  Functions
*****************************************************************************/

// For testing only!

void
fpdec_dump(fpdec_t *);



#ifdef __cplusplus
}
#endif // __cplusplus

#endif // FPDEC_H
