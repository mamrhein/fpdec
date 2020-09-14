/* ---------------------------------------------------------------------------
Name:        converter_test.cpp

Author:      Michael Amrhein (michael@adrhinum.de)

Copyright:   (c) 2020 ff. Michael Amrhein
License:     This program is part of a larger application. For license
             details please read the file LICENSE.TXT provided together
             with the application.
------------------------------------------------------------------------------
$Source$
$Revision$
*/

#include <cstring>

#include "catch.hpp"
#include "fpdec.h"
#include "checks.hpp"


TEST_CASE("Negate") {

    SECTION("Shifted int variant") {

        std::string literals[3] = {
            "82345678901234567890e-7",
            "0",
            "-7e4"
        };

        for (const auto &literal : literals) {
            fpdec_t src = FPDEC_ZERO;
            fpdec_t negated = FPDEC_ZERO;
            error_t rc;

            SECTION(literal) {
                rc = fpdec_from_ascii_literal(&src, literal.c_str());
                REQUIRE(rc == FPDEC_OK);
                rc = fpdec_neg(&negated, &src);
                REQUIRE(rc == FPDEC_OK);
                CHECK(is_shint(&negated));
                CHECK(FPDEC_SIGN(&negated) == -FPDEC_SIGN(&src));
                CHECK(FPDEC_DEC_PREC(&negated) == FPDEC_DEC_PREC(&src));
                CHECK(negated.lo == src.lo);
                CHECK(negated.hi == src.hi);
            }
        }
    }

    SECTION("Digit array variant") {

        std::string literals[2] = {
            "1234567890123456789012345678901234567890e-12",
            "-11702439610000.0000002938162540"
        };

        for (const auto &literal : literals) {
            fpdec_t src = FPDEC_ZERO;
            fpdec_t negated = FPDEC_ZERO;
            error_t rc;

            SECTION(literal) {
                rc = fpdec_from_ascii_literal(&src, literal.c_str());
                REQUIRE(rc == FPDEC_OK);
                rc = fpdec_neg(&negated, &src);
                REQUIRE(rc == FPDEC_OK);
                CHECK(is_digit_array(&negated));
                CHECK(FPDEC_SIGN(&negated) == -FPDEC_SIGN(&src));
                CHECK(FPDEC_DEC_PREC(&negated) == FPDEC_DEC_PREC(&src));
                REQUIRE(FPDEC_DYN_N_DIGITS(&negated) ==
                        FPDEC_DYN_N_DIGITS(&src));
                for (int i = 0; i < FPDEC_DYN_N_DIGITS(&src); ++i) {
                    CHECK(negated.digit_array->digits[i] ==
                          src.digit_array->digits[i]);
                }
            }
            fpdec_reset_to_zero(&src, 0);
            fpdec_reset_to_zero(&negated, 0);
        }
    }

    SECTION("Zero") {
        fpdec_t zero = FPDEC_ZERO;
        fpdec_t negated = FPDEC_ZERO;
        error_t rc = fpdec_neg(&negated, &zero);
        REQUIRE(rc == FPDEC_OK);
        CHECK(is_shint(&negated));
        CHECK(FPDEC_SIGN(&negated) == 0);
        CHECK(FPDEC_DEC_PREC(&negated) == 0);
        CHECK(negated.lo == 0);
        CHECK(negated.hi == 0);
    }
}

TEST_CASE("As ASCII literal") {

    std::string literals[] = {
        // shifted int
        "15.00",
        "823456789012345.67890",
        "-0.00200",
        "-7000",
        "0.000000001",
        "0",
        "0.0000",
        // digit arrray
        "1234567890123456789012345.678901234567890",
        "-11702439610000.000000293816254000",
        "700000000000000000000000000000",
        "-0.0000000004",
        "-28.50000000000000000000000000000000000000000000000000"
    };

    for (const auto &literal : literals) {
        fpdec_t dec = FPDEC_ZERO;
        error_t rc;
        char *lit = NULL;
        size_t n;
        std::string stripped;

        n = literal.size();
        if (!(literal.find('.') == std::string::npos)) {
            while (n > 0 && literal[n - 1] == '0')
                n--;
            if (literal[n -1] == '.')
                n--;
        }
        stripped = literal.substr(0, n);

        SECTION(literal) {

            rc = fpdec_from_ascii_literal(&dec, literal.c_str());
            REQUIRE(rc == FPDEC_OK);

            SECTION("with trailing zeroes") {
                lit = fpdec_as_ascii_literal(&dec, false);
                CHECK(strcmp(lit, literal.c_str()) == 0);
                fpdec_mem_free(lit);
                lit = NULL;
            }

            SECTION("w/o trailing zeroes") {
                lit = fpdec_as_ascii_literal(&dec, true);
                CHECK(strcmp(lit, stripped.c_str()) == 0);
                fpdec_mem_free(lit);
                lit = NULL;
            }

            fpdec_reset_to_zero(&dec, 0);
        }
    }
}
