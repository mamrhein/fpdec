/* ---------------------------------------------------------------------------
Name:        properties_test.cpp

Author:      Michael Amrhein (michael@adrhinum.de)

Copyright:   (c) 2020 ff. Michael Amrhein
License:     This program is part of a larger application. For license
             details please read the file LICENSE.TXT provided together
             with the application.
------------------------------------------------------------------------------
$Source$
$Revision$
*/

#include "catch.hpp"
#include "fpdec.h"
#include "checks.hpp"


TEST_CASE("Magnitude") {

    struct test_data {
        std::string literal;
        int magnitude;
    };

    SECTION("Shifted int variant") {

        test_data tests[5] = {
                {"-1234567890e8",           17},
                {"82345678901234567890e-9", 10},
                {"9",                       0},
                {"-7e-4",                   -4},
                {"0.000000005",             -9},
        };

        for (const auto &test : tests) {
            fpdec_t fpdec = FPDEC_ZERO;
            error_t rc;

            SECTION(test.literal) {
                rc = fpdec_from_ascii_literal(&fpdec, test.literal.c_str());
                REQUIRE(rc == FPDEC_OK);
                REQUIRE(is_shint(&fpdec));
                CHECK(fpdec_magnitude(&fpdec) == test.magnitude);
            }
        }
    }

    SECTION("Digit array variant") {

        test_data tests[7] = {
                {"-1234567890e83",                       92},
                {"82345678901234567890e-12",             7},
                {"123456789012345678901234567890e-12",   17},
                {"999999999999999999999999999999999e-4", 28},
                {"-7e-33",                               -33},
                {"0.0000000000000000000000000000000005", -34},
                {"-0.0000000000000000000000000000000005", -34},
        };

        for (const auto &test : tests) {
            fpdec_t fpdec = FPDEC_ZERO;
            error_t rc;

            SECTION(test.literal) {
                rc = fpdec_from_ascii_literal(&fpdec, test.literal.c_str());
                REQUIRE(rc == FPDEC_OK);
                REQUIRE(is_digit_array(&fpdec));
                CHECK(fpdec_magnitude(&fpdec) == test.magnitude);
            }
            fpdec_reset_to_zero(&fpdec, 0);
        }
    }

    SECTION("Zero") {
        fpdec_t zero = FPDEC_ZERO;
        CHECK(fpdec_magnitude(&zero) == -1);
        CHECK(errno == ERANGE);
    }
}
