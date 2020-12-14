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
        {
            .x_lit = "12345678901234567e12",
            .q_lit = "0.025",
            .z_lit = "12345678901234567000000000000.000",
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


TEST_CASE("Quantize / ROUND_UP") {

    struct test_data {
        std::string x_lit;
        std::string q_lit;
        std::string z_lit;
    };

    struct test_data tests[] = {
        {
            .x_lit = "-0.14",
            .q_lit = "1",
            .z_lit = "-1",
        },
        {
            .x_lit = "-0.142857142857143",
            .q_lit = "1",
            .z_lit = "-1",
        },
        {
            .x_lit = "-24.00000000007",
            .q_lit = "2.00",
            .z_lit = "-26.00",
        },
        {
            .x_lit = "36e-23",
            .q_lit = "1e-19",
            .z_lit = "1e-19",
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
            rc = fpdec_quantize(&x, &q, FPDEC_ROUND_UP);
            REQUIRE(rc == FPDEC_OK);
            CHECK(fpdec_compare(&x, &z, false) == 0);
            CHECK(FPDEC_DEC_PREC(&x) == FPDEC_DEC_PREC(&z));
        }
        fpdec_reset_to_zero(&x, 0);
        fpdec_reset_to_zero(&q, 0);
        fpdec_reset_to_zero(&z, 0);
    }
}


TEST_CASE("Quantize vs. adjust") {

    std::string rnd_mode_names[] = {
        "FPDEC_ROUND_DEFAULT",
        "FPDEC_ROUND_05UP",
        "FPDEC_ROUND_CEILING",
        "FPDEC_ROUND_DOWN",
        "FPDEC_ROUND_FLOOR",
        "FPDEC_ROUND_HALF_DOWN",
        "FPDEC_ROUND_HALF_EVEN",
        "FPDEC_ROUND_HALF_UP",
        "FPDEC_ROUND_UP",
    };

    std::string test_data[] = {
        "-0.14",
        "-0.142857142857143",
        "-24.00000000007",
        "36e-23",
    };

    fpdec_t q = FPDEC_ONE;
    fpdec_t x = FPDEC_ZERO;
    fpdec_t y = FPDEC_ZERO;
    fpdec_t z = FPDEC_ZERO;
    error_t rc;

    for (const auto &test_val : test_data) {
        const char *x_lit = test_val.c_str();

        SECTION(test_val) {
            rc = fpdec_from_ascii_literal(&x, x_lit);
            REQUIRE(rc == FPDEC_OK);

            for (int i = 0; i <= FPDEC_MAX_ROUNDING_MODE; ++i) {
                FPDEC_ROUNDING_MODE rnd = static_cast<FPDEC_ROUNDING_MODE>(i);
                SECTION(rnd_mode_names[i].c_str()) {
                    rc = fpdec_quantized(&y, &x, &q, rnd);
                    REQUIRE(rc == FPDEC_OK);
                    rc = fpdec_adjusted(&z, &x, 0, rnd);
                    REQUIRE(rc == FPDEC_OK);
                    CHECK(fpdec_compare(&y, &z, false) == 0);
                }
                fpdec_reset_to_zero(&y, 0);
                fpdec_reset_to_zero(&z, 0);
            }
            fpdec_reset_to_zero(&x, 0);
        }
    }
}
