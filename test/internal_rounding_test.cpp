/*
------------------------------------------------------------------------------
Name:        internal_rounding_test.cpp

Author:      Michael Amrhein (michael@adrhinum.de)

Copyright:   (c) 2020 ff. Michael Amrhein
License:     This program is part of a larger application or library.
             For license details please read the file LICENSE provided
             together with the application or library.
------------------------------------------------------------------------------
$Source$
$Revision$
*/

#include <cstdio>

#include "catch.hpp"
#include "rounding_.h"


void check_rounding(fpdec_digit_t num, fpdec_digit_t quant,
                    enum FPDEC_ROUNDING_MODE rnd, fpdec_digit_t res_neg,
                    fpdec_digit_t res_pos) {
    //SECTION("neg") {
        REQUIRE(round_to_multiple(-1, num, quant, rnd) == res_neg);
    //}
    //SECTION("pos") {
        REQUIRE(round_to_multiple(1, num, quant, rnd) == res_pos);
    //}
}

TEST_CASE("Round number to multiple") {

    fpdec_digit_t numbers[6] = {1UL, 10UL, 292UL, 294UL, 123456789UL,
                                1111111111111111UL};
    size_t n = sizeof(numbers) / sizeof(fpdec_digit_t);
    fpdec_digit_t quant = 5;
    enum FPDEC_ROUNDING_MODE rnd;
    char buf[30];

    SECTION("ROUND_05UP") {
        rnd = FPDEC_ROUND_05UP;
        fpdec_digit_t results[6] = {5UL, 10UL, 290UL, 290UL, 123456785UL,
                                    1111111111111110UL};
        for (int i = 0; i < n; ++i) {
            // rnd included here to make section name unique
            snprintf((char *) (&buf), 30, "%d: %lu", rnd, numbers[i]);
            SECTION(buf) {
                check_rounding(numbers[i], quant, rnd, results[i],
                               results[i]);
            }
        }
    }

    SECTION("ROUND_CEILING") {
        rnd = FPDEC_ROUND_CEILING;
        fpdec_digit_t res_neg[6] = {0UL, 10UL, 290UL, 290UL, 123456785UL,
                                    1111111111111110UL,};
        fpdec_digit_t res_pos[6] = {5UL, 10UL, 295UL, 295UL, 123456790UL,
                                    1111111111111115UL,};
        for (int i = 0; i < n; ++i) {
            // rnd included here to make section name unique
            snprintf((char *) (&buf), 30, "%d: %lu", rnd, numbers[i]);
            SECTION(buf) {
                check_rounding(numbers[i], quant, rnd, res_neg[i],
                               res_pos[i]);
            }
        }
    }

    SECTION("ROUND_DOWN") {
        rnd = FPDEC_ROUND_DOWN;
        fpdec_digit_t results[6] = {0UL, 10UL, 290UL, 290UL, 123456785UL,
                                    1111111111111110UL,};
        for (int i = 0; i < n; ++i) {
            // rnd included here to make section name unique
            snprintf((char *) (&buf), 30, "%d: %lu", rnd, numbers[i]);
            SECTION(buf) {
                check_rounding(numbers[i], quant, rnd, results[i],
                               results[i]);
            }
        }
    }

    SECTION("ROUND_FLOOR") {
        rnd = FPDEC_ROUND_FLOOR;
        fpdec_digit_t res_neg[6] = {5UL, 10UL, 295UL, 295UL, 123456790UL,
                                    1111111111111115UL,};
        fpdec_digit_t res_pos[6] = {0UL, 10UL, 290UL, 290UL, 123456785UL,
                                    1111111111111110UL,};
        for (int i = 0; i < n; ++i) {
            // rnd included here to make section name unique
            snprintf((char *) (&buf), 30, "%d: %lu", rnd, numbers[i]);
            SECTION(buf) {
                check_rounding(numbers[i], quant, rnd, res_neg[i],
                               res_pos[i]);
            }
        }
    }

    SECTION("ROUND_HALF_DOWN") {
        rnd = FPDEC_ROUND_HALF_DOWN;
        fpdec_digit_t results[6] = {0UL, 10UL, 290UL, 295UL, 123456790UL,
                                    1111111111111110UL,};
        for (int i = 0; i < n; ++i) {
            // rnd included here to make section name unique
            snprintf((char *) (&buf), 30, "%d: %lu", rnd, numbers[i]);
            SECTION(buf) {
                check_rounding(numbers[i], quant, rnd, results[i],
                               results[i]);
            }
        }
    }

    SECTION("ROUND_HALF_EVEN") {
        rnd = FPDEC_ROUND_HALF_EVEN;
        fpdec_digit_t results[6] = {0UL, 10UL, 290UL, 295UL, 123456790UL,
                                    1111111111111110UL,};
        for (int i = 0; i < n; ++i) {
            // rnd included here to make section name unique
            snprintf((char *) (&buf), 30, "%d: %lu", rnd, numbers[i]);
            SECTION(buf) {
                check_rounding(numbers[i], quant, rnd, results[i],
                               results[i]);
            }
        }
    }

    SECTION("ROUND_HALF_UP") {
        rnd = FPDEC_ROUND_HALF_UP;
        fpdec_digit_t results[6] = {0UL, 10UL, 295UL, 295UL, 123456790UL,
                                    1111111111111110UL,};
        for (int i = 0; i < n; ++i) {
            // rnd included here to make section name unique
            snprintf((char *) (&buf), 30, "%d: %lu", rnd, numbers[i]);
            SECTION(buf) {
                check_rounding(numbers[i], quant, rnd, results[i],
                               results[i]);
            }
        }
    }

    SECTION("ROUND_UP") {
        rnd = FPDEC_ROUND_UP;
        fpdec_digit_t results[6] = {5UL, 10UL, 295UL, 295UL, 123456790UL,
                                    1111111111111115UL,};
        for (int i = 0; i < n; ++i) {
            // rnd included here to make section name unique
            snprintf((char *) (&buf), 30, "%d: %lu", rnd, numbers[i]);
            SECTION(buf) {
                check_rounding(numbers[i], quant, rnd, results[i],
                               results[i]);
            }
        }
    }
}
