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
do_div_test(const div_test_variant &variant, const div_test_data &test,
            FPDEC_ROUNDING_MODE rounding) {
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

    rc = fpdec_div(&q, &x, &y, test.prec_limit, rounding);
    REQUIRE(rc == FPDEC_OK);
    CHECK(FPDEC_IS_DYN_ALLOC(&q) == variant.dyn_quot);
    if (test.prec_limit == -1)
        CHECK(FPDEC_DEC_PREC(&q) == FPDEC_DEC_PREC(&quot));
    else
        CHECK(FPDEC_DEC_PREC(&q) == test.prec_limit);
    CHECK((int)FPDEC_SIGN(&q) == (int)FPDEC_SIGN(&quot));
    CHECK(fpdec_compare(&q, &quot, true) == 0);

    fpdec_reset_to_zero(&x, 0);
    fpdec_reset_to_zero(&y, 0);
    fpdec_reset_to_zero(&q, 0);
    fpdec_reset_to_zero(&quot, 0);
}

TEST_CASE("Div (w/o limit and default rounding)") {

    SECTION("shint / shint -> shint") {

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
                do_div_test(tv, test, FPDEC_ROUND_DEFAULT);
            }
        }
    }

    SECTION("shint / shint -> dyn") {

        const struct div_test_variant tv = {
            .dyn_x = false,
            .dyn_y = false,
            .dyn_quot = true,
        };


        struct div_test_data tests[] = {
            {
                .lit_x = "173.849428",
                .lit_y = "10000.00",
                .lit_quot = "0.0173849428",
                .prec_limit = -1,
            },
            {
                .lit_x = "3459999896e19",
                .lit_y = "-0.4199029",
                .lit_quot = "-82400000000000000000000000000",
                .prec_limit = -1,
            },
            {
                .lit_x = "7922816251426433759354394.7777",
                .lit_y = "0.00700002",
                .lit_quot = "1131827659267606915316584063.717",
                .prec_limit = 3,
            },
            {
                .lit_x = "12345678901234567e12",
                .lit_y = "0.000000005",
                .lit_quot = "24691357802469134e20",
                .prec_limit = 0,
            },
            {
                .lit_x = "172.9999948",
                .lit_y = "41.99029",
                .lit_quot = "4.1200000000",
                .prec_limit = 10,
            },
            {
                .lit_x = "222222222e18",
                .lit_y = "0.002",
                .lit_quot = "111111111e21",
                .prec_limit = -1,
            },
            {
                .lit_x = "876543210987654321e10",
                .lit_y = "0.000000003",
                .lit_quot = "292181070329218107e19",
                .prec_limit = -1,
            },
        };
        for (const auto &test : tests) {

            const std::string section_name = test.lit_x + " / " + test
                .lit_y;

            SECTION(section_name) {
                do_div_test(tv, test, FPDEC_ROUND_DEFAULT);
            }
        }
    }

    SECTION("shint / dyn -> shint") {

        const struct div_test_variant tv = {
            .dyn_x = false,
            .dyn_y = true,
            .dyn_quot = false,
        };

        struct div_test_data tests[] = {
            {
                .lit_x = "173.849428",
                .lit_y = "0.0173849428",
                .lit_quot = "10000",
                .prec_limit = -1,
            },
            {
                .lit_x = "3459999896e19",
                .lit_y = "-82400000000000000000000000000",
                .lit_quot = "-0.4199029",
                .prec_limit = -1,
            },
        };

        for (const auto &test : tests) {

            const std::string section_name = test.lit_x + " / " + test
                .lit_y;

            SECTION(section_name) {
                do_div_test(tv, test, FPDEC_ROUND_DEFAULT);
            }
        }
    }

    SECTION("dyn / dyn -> shint") {

        const struct div_test_variant tv = {
            .dyn_x = true,
            .dyn_y = true,
            .dyn_quot = false,
        };

        struct div_test_data tests[] = {
            {
                .lit_x = "617777772217777777221777777722.1774936",
                .lit_y = "123456789012345678901234567890.1234",
                .lit_quot = "5.004",
                .prec_limit = -1,
            },
            {
                .lit_x = "123456789012345678901234567890.1234",
                .lit_y = "-4938271560493827156049382715604.936",
                .lit_quot = "-0.025",
                .prec_limit = -1,
            },
            {
                .lit_x = "-0.1604196e-36",
                .lit_y = "-0.396e-37",
                .lit_quot = "4.051",
                .prec_limit = -1,
            },
        };
        for (const auto &test : tests) {

            const std::string section_name = test.lit_x + " / " + test.lit_y;

            SECTION(section_name) {
                do_div_test(tv, test, FPDEC_ROUND_DEFAULT);
            }
        }
    }

    SECTION("dyn / dyn -> dyn") {

        const struct div_test_variant tv = {
            .dyn_x = true,
            .dyn_y = true,
            .dyn_quot = true,
        };

        struct div_test_data tests[] = {
            {
                .lit_x = "2948980000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000000000000000000000000000000000"
                         "0000000000001498953",
                .lit_y = "1e459",
                .lit_quot = "2948980000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000000000000000000000000000000000"
                            "0000000000001498953e-459",
                .prec_limit = -1,
            },
        };
        for (const auto &test : tests) {

            const std::string section_name = test.lit_x.substr(0, 10) +
                "... / " + test.lit_y;

            SECTION(section_name) {
                do_div_test(tv, test, FPDEC_ROUND_DEFAULT);
            }
        }
    }
}

TEST_CASE("Div (with limit and default rounding)") {

    SECTION("shint / shint -> shint") {

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
            {
                .lit_x = "3.4",
                .lit_y = "-6",
                .lit_quot = "-0.566666667",
                .prec_limit = 9,
            },
        };
        for (const auto &test : tests) {

            const std::string section_name = test.lit_x + " / " + test
                .lit_y;

            SECTION(section_name) {
                do_div_test(tv, test, FPDEC_ROUND_DEFAULT);
            }
        }
    }

    SECTION("shint / shint -> dyn") {

        const struct div_test_variant tv = {
            .dyn_x = false,
            .dyn_y = false,
            .dyn_quot = true,
        };


        struct div_test_data tests[] = {
            {
                .lit_x = "1",
                .lit_y = "3",
                .lit_quot = "0.333333333333",
                .prec_limit = 12,
            },
            {
                .lit_x = "2.00",
                .lit_y = "3.0000000",
                .lit_quot = "0.6666666666666666666666666666666667",
                .prec_limit = 34,
            },
            {
                .lit_x = "3.4",
                .lit_y = "6",
                .lit_quot = "0.566666666666666666666666666666667",
                .prec_limit = 33,
            },
            {
                .lit_x = "3.843",
                .lit_y = "6.3",
                .lit_quot = "0.610000000000",
                .prec_limit = 12,
            },
        };
        for (const auto &test : tests) {

            const std::string section_name = test.lit_x + " / " + test
                .lit_y;

            SECTION(section_name) {
                do_div_test(tv, test, FPDEC_ROUND_DEFAULT);
            }
        }
    }

    SECTION("shint / dyn -> shint") {

        const struct div_test_variant tv = {
            .dyn_x = false,
            .dyn_y = true,
            .dyn_quot = false,
        };

        struct div_test_data tests[] = {
            {
                .lit_x = "173.849428",
                .lit_y = "0.0173849428",
                .lit_quot = "10000.000",
                .prec_limit = 3,
            },
            {
                .lit_x = "3459999896e19",
                .lit_y = "-82400000000000000000000000000",
                .lit_quot = "-0.419903",
                .prec_limit = 6,
            },
            {
                .lit_x = "1",
                .lit_y = "4494232837155789769323262976972561834044"
                         "9424473557664318357520289433168951375",
                .lit_quot = "0",
                .prec_limit = 26,
            },
        };

        for (const auto &test : tests) {

            const std::string section_name = test.lit_x + " / " + test
                .lit_y;

            SECTION(section_name) {
                do_div_test(tv, test, FPDEC_ROUND_DEFAULT);
            }
        }
    }

    SECTION("dyn / dyn -> dyn") {

        const struct div_test_variant tv = {
            .dyn_x = true,
            .dyn_y = true,
            .dyn_quot = true,
        };

        struct div_test_data tests[] = {
            {
                .lit_x = "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1.44444444444444444444444444444444400000"
                         "00000000000000",
                .lit_y = "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111111111111111111"
                         "1111111111111111111111111.47474747474747"
                         "4747474747470000000000000000000000000000"
                         "0",
                .lit_quot = "1e296",
                .prec_limit = 0,
            },
        };
        for (const auto &test : tests) {

            const std::string section_name = test.lit_x.substr(0, 10) +
                "... / " + test.lit_y;

            SECTION(section_name) {
                do_div_test(tv, test, FPDEC_ROUND_DEFAULT);
            }
        }
    }
}

TEST_CASE("Div (with limit and explicit rounding mode)") {

    SECTION("shint / shint -> shint [ROUND_HALF_UP]") {

        const struct div_test_variant tv = {
            .dyn_x = false,
            .dyn_y = false,
            .dyn_quot = false,
        };

        struct div_test_data tests[] = {
            {
                .lit_x = "7",
                .lit_y = "3",
                .lit_quot = "2",
                .prec_limit = 0,
            },
            {
                .lit_x = "12.34",
                .lit_y = "10",
                .lit_quot = "1.23",
                .prec_limit = 2,
            },
            {
                .lit_x = "61.7",
                .lit_y = "5",
                .lit_quot = "12.3",
                .prec_limit = 1,
            },
            {
                .lit_x = "61.75",
                .lit_y = "5",
                .lit_quot = "12.4",
                .prec_limit = 1,
            },
        };
        for (const auto &test : tests) {

            const std::string section_name = test.lit_x + " / " + test.lit_y +
                " [ROUND_HALF_UP]";

            SECTION(section_name) {
                do_div_test(tv, test, FPDEC_ROUND_HALF_UP);
            }
        }
    }

    SECTION("shint / shint -> shint [ROUND_HALF_EVEN]") {

        const struct div_test_variant tv = {
            .dyn_x = false,
            .dyn_y = false,
            .dyn_quot = false,
        };

        struct div_test_data tests[] = {
            {
                .lit_x = "7",
                .lit_y = "3",
                .lit_quot = "2",
                .prec_limit = 0,
            },
            {
                .lit_x = "12.34",
                .lit_y = "10",
                .lit_quot = "1.23",
                .prec_limit = 2,
            },
            {
                .lit_x = "61.7",
                .lit_y = "5",
                .lit_quot = "12.3",
                .prec_limit = 1,
            },
            {
                .lit_x = "61.75",
                .lit_y = "5",
                .lit_quot = "12.4",
                .prec_limit = 1,
            },
            {
                .lit_x = "617.5",
                .lit_y = "10",
                .lit_quot = "61.8",
                .prec_limit = 1,
            },
            {
                .lit_x = "-2.5",
                .lit_y = "5",
                .lit_quot = "0",
                .prec_limit = 0,
            },
        };
        for (const auto &test : tests) {

            const std::string section_name = test.lit_x + " / " + test.lit_y +
                " [ROUND_HALF_EVEN]";

            SECTION(section_name) {
                do_div_test(tv, test, FPDEC_ROUND_HALF_EVEN);
            }
        }
    }

    SECTION("shint / shint -> shint [ROUND_CEILING]") {

        const struct div_test_variant tv = {
            .dyn_x = false,
            .dyn_y = false,
            .dyn_quot = false,
        };

        struct div_test_data tests[] = {
            {
                .lit_x = "12.34",
                .lit_y = "10",
                .lit_quot = "1.24",
                .prec_limit = 2,
            },
            {
                .lit_x = "61.7",
                .lit_y = "-5",
                .lit_quot = "-12.3",
                .prec_limit = 1,
            },
            {
                .lit_x = "-2.5",
                .lit_y = "5",
                .lit_quot = "0",
                .prec_limit = 0,
            },
            {
                .lit_x = "3.4",
                .lit_y = "-6",
                .lit_quot = "-0.566666666",
                .prec_limit = 9,
            },
        };
        for (const auto &test : tests) {

            const std::string section_name = test.lit_x + " / " + test.lit_y +
                                             " [ROUND_CEILING]";

            SECTION(section_name) {
                do_div_test(tv, test, FPDEC_ROUND_CEILING);
            }
        }
    }

    SECTION("dyn / shint -> shint [ROUND_UP]") {

        const struct div_test_variant tv = {
            .dyn_x = true,
            .dyn_y = false,
            .dyn_quot = false,
        };

        struct div_test_data tests[] = {
            {
                .lit_x = "-0.160419683452",
                .lit_y = "1",
                .lit_quot = "-1",
                .prec_limit = 0,
            },
            {
                .lit_x = "0.00000000000606",
                .lit_y = "3.03",
                .lit_quot = "0.001",
                .prec_limit = 3,
            },
        };
        for (const auto &test : tests) {

            const std::string section_name = test.lit_x + " / " + test.lit_y +
                                             " [ROUND_UP]";

            SECTION(section_name) {
                do_div_test(tv, test, FPDEC_ROUND_UP);
            }
        }
    }

    SECTION("dyn / shint -> dyn [ROUND_UP]") {

        const struct div_test_variant tv = {
            .dyn_x = true,
            .dyn_y = false,
            .dyn_quot = true,
        };

        struct div_test_data tests[] = {
            {
                .lit_x = "-0.4999683452",
                .lit_y = "-1e17",
                .lit_quot = "1e-12",
                .prec_limit = 12,
            },
            {
                .lit_x = "0.000000000006",
                .lit_y = "3e20",
                .lit_quot = "1e-23",
                .prec_limit = 23,
            },
        };
        for (const auto &test : tests) {

            const std::string section_name = test.lit_x + " / " + test.lit_y +
                                             " [ROUND_UP]";

            SECTION(section_name) {
                do_div_test(tv, test, FPDEC_ROUND_UP);
            }
        }
    }

    SECTION("dyn / dyn -> shint [ROUND_HALF_UP]") {

        const struct div_test_variant tv = {
            .dyn_x = true,
            .dyn_y = true,
            .dyn_quot = false,
        };

        struct div_test_data tests[] = {
            {
                .lit_x = "617777772217777777221777777722.1774936",
                .lit_y = "123456789012345678901234567890.1234",
                .lit_quot = "5.00400",
                .prec_limit = 5,
            },
            {
                .lit_x = "123456789012345678901234567890.1234",
                .lit_y = "331039480691409992146997641.561269171764",
                .lit_quot = "372.936752905",
                .prec_limit = 9,
            },
            {
                .lit_x = "0.1604196e-36",
                .lit_y = "0.396e-37",
                .lit_quot = "4.1",
                .prec_limit = 1,
            },
            {
                .lit_x = "0.000000000007",
                .lit_y = "0.000000000003",
                .lit_quot = "2",
                .prec_limit = 0,
            },
        };
        for (const auto &test : tests) {

            const std::string section_name = test.lit_x + " / " + test.lit_y +
                " [ROUND_HALF_UP]";

            SECTION(section_name) {
                do_div_test(tv, test, FPDEC_ROUND_HALF_UP);
            }
        }
    }

    SECTION("dyn / dyn -> shint [ROUND_HALF_EVEN]") {

        const struct div_test_variant tv = {
            .dyn_x = true,
            .dyn_y = true,
            .dyn_quot = false,
        };

        struct div_test_data tests[] = {
            {
                .lit_x = "617777772217777777221777777722.1774936",
                .lit_y = "123456789012345678901234567890.1234",
                .lit_quot = "5.00",
                .prec_limit = 2,
            },
            {
                .lit_x = "123456789012345678901234567890.1234",
                .lit_y = "-4938271560493827156049382715604.936",
                .lit_quot = "-0.02",
                .prec_limit = 2,
            },
            {
                .lit_x = "-0.1604196e-36",
                .lit_y = "-0.396e-37",
                .lit_quot = "4.051",
                .prec_limit = 3,
            },
            {
                .lit_x = "0.000000000007",
                .lit_y = "0.000000000003",
                .lit_quot = "2",
                .prec_limit = 0,
            },
            {
                .lit_x = "0.0000000000025",
                .lit_y = "0.000000000005",
                .lit_quot = "0",
                .prec_limit = 0,
            },
        };
        for (const auto &test : tests) {

            const std::string section_name = test.lit_x + " / " + test.lit_y +
                " [ROUND_HALF_EVEN]";

            SECTION(section_name) {
                do_div_test(tv, test, FPDEC_ROUND_HALF_EVEN);
            }
        }
    }
}
