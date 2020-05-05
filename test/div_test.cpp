/* ---------------------------------------------------------------------------
Name:        div_test.cpp

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


struct div_test_variant {
    bool dyn_x;
    bool dyn_y;
    bool dyn_quot;
};

struct div_test_data {
    std::string lit_x;
    std::string lit_y;
    std::string lit_quot;
    int prec_limit;
};

static void
do_div_test(const div_test_variant &variant,
            const div_test_data &test) {
    error_t rc;
    fpdec_t x = FPDEC_ZERO;
    fpdec_t y = FPDEC_ZERO;
    fpdec_t q = FPDEC_ZERO;
    fpdec_t quot = FPDEC_ZERO;

    rc = fpdec_from_ascii_literal(&x, test.lit_x.c_str());
    REQUIRE(rc == FPDEC_OK);
    REQUIRE(FPDEC_IS_DYN_ALLOC(&x) == variant.dyn_x);
    rc = fpdec_from_ascii_literal(&y, test.lit_y.c_str());
    REQUIRE(rc == FPDEC_OK);
    REQUIRE(FPDEC_IS_DYN_ALLOC(&y) == variant.dyn_y);
    rc = fpdec_from_ascii_literal(&quot, test.lit_quot.c_str());
    REQUIRE(rc == FPDEC_OK);
    REQUIRE(FPDEC_IS_DYN_ALLOC(&quot) == variant.dyn_quot);

    rc = fpdec_div(&q, &x, &y, test.prec_limit, FPDEC_ROUND_DEFAULT);
    REQUIRE(rc == FPDEC_OK);
    CHECK(FPDEC_IS_DYN_ALLOC(&q) == variant.dyn_quot);
    if (test.prec_limit != -1)
        CHECK(FPDEC_DEC_PREC(&q) == test.prec_limit);
    CHECK((int) FPDEC_SIGN(&q) == (int) FPDEC_SIGN(&quot));
    CHECK(fpdec_compare(&q, &quot, true) == 0);

    fpdec_reset_to_zero(&x, 0);
    fpdec_reset_to_zero(&y, 0);
    fpdec_reset_to_zero(&q, 0);
    fpdec_reset_to_zero(&quot, 0);
}

TEST_CASE("Div (w/o limit and default rounding)") {

    SECTION("shint /% shint -> shint") {

        const struct div_test_variant tv = {
                .dyn_x = false,
                .dyn_y = false,
                .dyn_quot = false,
        };


        struct div_test_data tests[] = {
                {
                        .lit_x = "0.00",
                        .lit_y = "173.849428",
                        .lit_quot = "0",
                        .prec_limit = -1,
                },
                {
                        .lit_x = "172.9999948",
                        .lit_y = "41.99029",
                        .lit_quot = "4.12",
                        .prec_limit = -1,
                },
        };
        for (const auto &test : tests) {

            const std::string section_name = test.lit_x + " / " + test
                    .lit_y;

            SECTION(section_name) {
                do_div_test(tv, test);
            }
        }
    }
}

TEST_CASE("Div (with limit and default rounding)") {

    SECTION("shint /% shint -> shint") {

        const struct div_test_variant tv = {
                .dyn_x = false,
                .dyn_y = false,
                .dyn_quot = false,
        };


        struct div_test_data tests[] = {
                {
                        .lit_x = "172.9999948",
                        .lit_y = "41.99029",
                        .lit_quot = "4.1",
                        .prec_limit = 1,
                },
                {
                        .lit_x = "173.0143363",
                        .lit_y = "41.99",
                        .lit_quot = "4.1204",
                        .prec_limit = 4,
                },
        };
        for (const auto &test : tests) {

            const std::string section_name = test.lit_x + " / " + test
                    .lit_y;

            SECTION(section_name) {
                do_div_test(tv, test);
            }
        }
    }
}
