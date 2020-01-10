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


#include "catch.hpp"
#include "rounding_.h"


void check_rounding(uint64_t num, uint64_t quant,
                    enum FPDEC_ROUNDING_MODE rnd) {
    uint64_t res;

    for (char sign = -1; sign <= 1 ; ++sign) {
        res = round_to_multiple(sign, num, quant, rnd);
        REQUIRE(res % quant == 0);
    };
};

TEST_CASE("Round number to multiple") {

    uint64_t numbers[5] = {1UL, 10UL, 294UL, 123456789UL, 1111111111111111UL};
    uint64_t quant = 5;
    enum FPDEC_ROUNDING_MODE rnd;

    SECTION("ROUND_05UP") {
        rnd = FPDEC_ROUND_05UP;
        for (int i = 0; i < sizeof(numbers); ++i) {
            check_rounding(numbers[i], quant, rnd);
        };
    };

    SECTION("ROUND_CEILING") {
        rnd = FPDEC_ROUND_CEILING;
        for (int i = 0; i < sizeof(numbers); ++i) {
            check_rounding(numbers[i], quant, rnd);
        };
    };

    SECTION("ROUND_DOWN") {
        rnd = FPDEC_ROUND_DOWN;
        for (int i = 0; i < sizeof(numbers); ++i) {
            check_rounding(numbers[i], quant, rnd);
        };
    };

    SECTION("ROUND_FLOOR") {
        rnd = FPDEC_ROUND_FLOOR;
        for (int i = 0; i < sizeof(numbers); ++i) {
            check_rounding(numbers[i], quant, rnd);
        };
    };

    SECTION("ROUND_HALF_DOWN") {
        rnd = FPDEC_ROUND_HALF_DOWN;
        for (int i = 0; i < sizeof(numbers); ++i) {
            check_rounding(numbers[i], quant, rnd);
        };
    };

    SECTION("ROUND_HALF_EVEN") {
        rnd = FPDEC_ROUND_HALF_EVEN;
        for (int i = 0; i < sizeof(numbers); ++i) {
            check_rounding(numbers[i], quant, rnd);
        };
    };

    SECTION("ROUND_HALF_UP") {
        rnd = FPDEC_ROUND_HALF_UP;
        for (int i = 0; i < sizeof(numbers); ++i) {
            check_rounding(numbers[i], quant, rnd);
        };
    };

    SECTION("ROUND_UP") {
        rnd = FPDEC_ROUND_UP;
        for (int i = 0; i < sizeof(numbers); ++i) {
            check_rounding(numbers[i], quant, rnd);
        };
    };
};
