/* ---------------------------------------------------------------------------
Name:        mul_test.cpp

Author:      Michael Amrhein (michael@adrhinum.de)

Copyright:   (c) 2020 ff. Michael Amrhein
License:     This program is part of a larger application. For license
             details please read the file LICENSE.TXT provided together
             with the application.
------------------------------------------------------------------------------
$Source$
$Revision$
*/

#include <cstdio>

#include "catch.hpp"
#include "fpdec.h"
#include "checks.hpp"


struct mul_test_variant {
    bool dyn_x;
    bool dyn_y;
    bool dyn_mult;
};

struct mul_test_data {
    std::string lit_x;
    std::string lit_y;
    std::string lit_mult;
};

static void
do_mul_test(const mul_test_variant &variant, const mul_test_data &test) {
    error_t rc;
    fpdec_t x = FPDEC_ZERO;
    fpdec_t y = FPDEC_ZERO;
    fpdec_t z = FPDEC_ZERO;
    fpdec_t m = FPDEC_ZERO;

    rc = fpdec_from_ascii_literal(&x, test.lit_x.c_str());
    REQUIRE(rc == FPDEC_OK);
    REQUIRE(FPDEC_IS_DYN_ALLOC(&x) == variant.dyn_x);
    rc = fpdec_from_ascii_literal(&y, test.lit_y.c_str());
    REQUIRE(rc == FPDEC_OK);
    REQUIRE(FPDEC_IS_DYN_ALLOC(&y) == variant.dyn_y);
    rc = fpdec_from_ascii_literal(&m, test.lit_mult.c_str());
    REQUIRE(rc == FPDEC_OK);
    REQUIRE(FPDEC_IS_DYN_ALLOC(&m) == variant.dyn_mult);

    rc = fpdec_mul(&z, &x, &y);
    REQUIRE(rc == FPDEC_OK);
    CHECK(FPDEC_IS_DYN_ALLOC(&z) == variant.dyn_mult);
    CHECK(fpdec_compare(&z, &m, false) == 0);
    fpdec_reset_to_zero(&z, 0);
    rc = fpdec_mul(&z, &y, &x);
    REQUIRE(rc == FPDEC_OK);
    CHECK(FPDEC_IS_DYN_ALLOC(&z) == variant.dyn_mult);
    CHECK(fpdec_compare(&z, &m, false) == 0);

    fpdec_reset_to_zero(&x, 0);
    fpdec_reset_to_zero(&y, 0);
    fpdec_reset_to_zero(&z, 0);
    fpdec_reset_to_zero(&m, 0);
}

TEST_CASE("Multiplication") {

    SECTION("shint * shint -> shint") {

        const struct mul_test_variant tv = {
                .dyn_x = false,
                .dyn_y = false,
                .dyn_mult = false
        };

        struct mul_test_data tests[] = {
                {
                        .lit_x = "17.84",
                        .lit_y = "0.00",
                        .lit_mult = "0",
                },
                {
                        .lit_x = "-123456789.0123",
                        .lit_y = "-1.00",
                        .lit_mult = "123456789.012300",
                },
                {
                        .lit_x = "17.35",
                        .lit_y = "10.00",
                        .lit_mult = "173.5000",
                },
                {
                        .lit_x = "1000000000000000000000000005",
                        .lit_y = "-0.01",
                        .lit_mult = "-10000000000000000000000000.05",
                },
        };

        for (const auto &test : tests) {

            const std::string section_name = test.lit_x + " * " + test.lit_y;

            SECTION(section_name) {
                do_mul_test(tv, test);
            }
        }
    }

    SECTION("shint * shint -> dyn") {

        const struct mul_test_variant tv = {
                .dyn_x = false,
                .dyn_y = false,
                .dyn_mult = true
        };

        struct mul_test_data tests[] = {
                {
                        .lit_x = "-123456789.012345678",
                        .lit_y = "-1.00",
                        .lit_mult = "123456789.01234567800",
                },
                {
                        .lit_x = "-17.05",
                        .lit_y = "0.00000002",
                        .lit_mult = "-.0000003410",
                },
                {
                        .lit_x = "1000000000000000000000000005",
                        .lit_y = "792281625142643.11",
                        .lit_mult =
                        "792281625142643110000000003961408125713215.55",
                },
        };

        for (const auto &test : tests) {

            const std::string section_name = test.lit_x + " * " + test.lit_y;

            SECTION(section_name) {
                do_mul_test(tv, test);
            }
        }
    }

    SECTION("shint * dyn -> dyn") {

        const struct mul_test_variant tv = {
                .dyn_x = false,
                .dyn_y = true,
                .dyn_mult = true
        };

        struct mul_test_data tests[] = {
                {
                        .lit_x = "-0.00000001",
                        .lit_y = "-123456789012345678901234567890",
                        .lit_mult = "1234567890123456789012.34567890",
                },
                {
                        .lit_x = "-17.05",
                        .lit_y = "0.0000000002",
                        .lit_mult = "-.000000003410",
                },
                {
                        .lit_x = "100000000000000000000000000.5",
                        .lit_y = "792281625142643.1100000000",
                        .lit_mult =
                        "79228162514264311000000000396140812571321"
                        ".55500000000",
                },
        };

        for (const auto &test : tests) {

            const std::string section_name = test.lit_x + " * " + test.lit_y;

            SECTION(section_name) {
                do_mul_test(tv, test);
            }
        }
    }

    SECTION("dyn * dyn -> dyn") {

        const struct mul_test_variant tv = {
                .dyn_x = true,
                .dyn_y = true,
                .dyn_mult = true
        };

        struct mul_test_data tests[] = {
                {
                        .lit_x = "-0.0000000001",
                        .lit_y = "-123456789012345678901234567890",
                        .lit_mult = "12345678901234567890.1234567890",
                },
                {
                        .lit_x = "-0.0000000175",
                        .lit_y = "0.0000000002",
                        .lit_mult = "-0.00000000000000000350",
                },
                {
                        .lit_x = "10000000000000000000000000000.5",
                        .lit_y = "792281625142643.1111111111",
                        .lit_mult =
                        "7922816251426431111111111000396140812571321"
                        ".55555555555",
                },
        };

        for (const auto &test : tests) {

            const std::string section_name = test.lit_x + " * " + test.lit_y;

            SECTION(section_name) {
                do_mul_test(tv, test);
            }
        }
    }
}

TEST_CASE("Multiplication: Limits exceeded") {

    SECTION("Exponent limit excedded") {

        error_t rc;
        fpdec_t x = FPDEC_ZERO;
        fpdec_t z = FPDEC_ZERO;

        rc = fpdec_from_ascii_literal(&x, "1e20401094656");
        REQUIRE(rc == FPDEC_OK);
        rc = fpdec_mul(&z, &x, &x);
        CHECK(rc == FPDEC_EXP_LIMIT_EXCEEDED);
    }

    SECTION("Precision limit excedded") {

        error_t rc;
        fpdec_t x = FPDEC_ZERO;
        fpdec_t z = FPDEC_ZERO;

        rc = fpdec_from_ascii_literal(&x, "1e-32775");
        REQUIRE(rc == FPDEC_OK);
        rc = fpdec_mul(&z, &x, &x);
        CHECK(rc == FPDEC_PREC_LIMIT_EXCEEDED);
    }
}
