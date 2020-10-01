/* ---------------------------------------------------------------------------
Name:        quantize_test.cpp

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


TEST_CASE("Quantize / dflt rounding") {

    struct test_data {
        std::string x_lit;
        std::string q_lit;
        std::string z_lit;
    };

    struct test_data tests[] = {
        {
            .x_lit = "17.8",
            .q_lit = "0.5",
            .z_lit = "18.0",
        },
        {
            .x_lit = "17.4000000001",
            .q_lit = "0.5",
            .z_lit = "17.5",
        },
        {
            .x_lit = "10000000000400000000007",
            .q_lit = "6.00",
            .z_lit = "10000000000400000000004.00",
        },
    };

    fpdec_t x = FPDEC_ZERO;
    fpdec_t q = FPDEC_ZERO;
    fpdec_t z = FPDEC_ZERO;
    error_t rc;

    for (const auto &test : tests) {
        const char *x_lit = test.x_lit.c_str();
        const char *q_lit = test.q_lit.c_str();
        const char *z_lit = test.z_lit.c_str();

        SECTION(test.x_lit) {
            rc = fpdec_from_ascii_literal(&x, x_lit);
            REQUIRE(rc == FPDEC_OK);
            rc = fpdec_from_ascii_literal(&q, q_lit);
            REQUIRE(rc == FPDEC_OK);
            rc = fpdec_from_ascii_literal(&z, z_lit);
            REQUIRE(rc == FPDEC_OK);
            rc = fpdec_quantize(&x, &q, FPDEC_ROUND_DEFAULT);
            REQUIRE(rc == FPDEC_OK);
            CHECK(fpdec_compare(&x, &z, false) == 0);
            CHECK(FPDEC_DEC_PREC(&x) == FPDEC_DEC_PREC(&z));
        }
        fpdec_reset_to_zero(&x, 0);
        fpdec_reset_to_zero(&q, 0);
        fpdec_reset_to_zero(&z, 0);
    }
}


TEST_CASE("Quantize / ROUND_HALF_UP") {

    struct test_data {
        std::string x_lit;
        std::string q_lit;
        std::string z_lit;
    };

    struct test_data tests[] = {
        {
            .x_lit = "17.8",
            .q_lit = "0.5",
            .z_lit = "18.0",
        },
        {
            .x_lit = "17.4000000001",
            .q_lit = "0.5",
            .z_lit = "17.5",
        },
        {
            .x_lit = "10000000000400000000007",
            .q_lit = "6.00",
            .z_lit = "10000000000400000000010.00",
        },
    };

    fpdec_t x = FPDEC_ZERO;
    fpdec_t q = FPDEC_ZERO;
    fpdec_t z = FPDEC_ZERO;
    error_t rc;

    for (const auto &test : tests) {
        const char *x_lit = test.x_lit.c_str();
        const char *q_lit = test.q_lit.c_str();
        const char *z_lit = test.z_lit.c_str();

        SECTION(test.x_lit) {
            rc = fpdec_from_ascii_literal(&x, x_lit);
            REQUIRE(rc == FPDEC_OK);
            rc = fpdec_from_ascii_literal(&q, q_lit);
            REQUIRE(rc == FPDEC_OK);
            rc = fpdec_from_ascii_literal(&z, z_lit);
            REQUIRE(rc == FPDEC_OK);
            rc = fpdec_quantize(&x, &q, FPDEC_ROUND_HALF_UP);
            REQUIRE(rc == FPDEC_OK);
            CHECK(fpdec_compare(&x, &z, false) == 0);
            CHECK(FPDEC_DEC_PREC(&x) == FPDEC_DEC_PREC(&z));
        }
        fpdec_reset_to_zero(&x, 0);
        fpdec_reset_to_zero(&q, 0);
        fpdec_reset_to_zero(&z, 0);
    }
}
