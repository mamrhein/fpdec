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
    CHECK((int) FPDEC_SIGN(&q) == (int) FPDEC_SIGN(&quot));
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
                        .lit_y = "6",
                        .lit_quot = "0.566666667",
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
        };

        for (const auto &test : tests) {

            const std::string section_name = test.lit_x + " / " + test
                    .lit_y;

            SECTION(section_name) {
                do_div_test(tv, test, FPDEC_ROUND_DEFAULT);
            }
        }
    }
}

TEST_CASE("Div (with limit and explicit rounding mode)") {

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

TEST_CASE("Div (w/o limit and explicit rounding mode)") {
}
