/* ---------------------------------------------------------------------------
Name:        divmod_test.cpp

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


struct divmod_test_variant {
    bool dyn_x;
    bool dyn_y;
    bool dyn_quot;
    bool dyn_rem;
};

struct divmod_test_data {
    std::string lit_x;
    std::string lit_y;
    std::string lit_quot;
    std::string lit_rem;
};

static void
do_divmod_test(const divmod_test_variant &variant,
               const divmod_test_data &test) {
    error_t rc;
    fpdec_t x = FPDEC_ZERO;
    fpdec_t y = FPDEC_ZERO;
    fpdec_t q = FPDEC_ZERO;
    fpdec_t quot = FPDEC_ZERO;
    fpdec_t r = FPDEC_ZERO;
    fpdec_t rem = FPDEC_ZERO;

    rc = fpdec_from_ascii_literal(&x, test.lit_x.c_str());
    REQUIRE(rc == FPDEC_OK);
    REQUIRE(FPDEC_IS_DYN_ALLOC(&x) == variant.dyn_x);
    rc = fpdec_from_ascii_literal(&y, test.lit_y.c_str());
    REQUIRE(rc == FPDEC_OK);
    REQUIRE(FPDEC_IS_DYN_ALLOC(&y) == variant.dyn_y);
    rc = fpdec_from_ascii_literal(&quot, test.lit_quot.c_str());
    REQUIRE(rc == FPDEC_OK);
    REQUIRE(FPDEC_IS_DYN_ALLOC(&quot) == variant.dyn_quot);
    rc = fpdec_from_ascii_literal(&rem, test.lit_rem.c_str());
    REQUIRE(rc == FPDEC_OK);
    REQUIRE(FPDEC_IS_DYN_ALLOC(&rem) == variant.dyn_rem);

    rc = fpdec_divmod(&q, &r, &x, &y);
    REQUIRE(rc == FPDEC_OK);
    CHECK(FPDEC_IS_DYN_ALLOC(&q) == variant.dyn_quot);
    CHECK(FPDEC_DEC_PREC(&q) == 0);
    CHECK((int)FPDEC_SIGN(&q) == (int)FPDEC_SIGN(&quot));
    CHECK(fpdec_compare(&q, &quot, true) == 0);
    CHECK(FPDEC_IS_DYN_ALLOC(&r) == variant.dyn_rem);
    CHECK((int)FPDEC_SIGN(&r) == (int)FPDEC_SIGN(&rem));
    CHECK(fpdec_compare(&r, &rem, true) == 0);

    fpdec_reset_to_zero(&x, 0);
    fpdec_reset_to_zero(&y, 0);
    fpdec_reset_to_zero(&q, 0);
    fpdec_reset_to_zero(&quot, 0);
    fpdec_reset_to_zero(&r, 0);
    fpdec_reset_to_zero(&rem, 0);
}

TEST_CASE("Divmod") {

    SECTION("shint /% shint -> shint, shint") {

        const struct divmod_test_variant tv = {
            .dyn_x = false,
            .dyn_y = false,
            .dyn_quot = false,
            .dyn_rem = false,
        };

        struct divmod_test_data tests[] = {
            {
                .lit_x = "0.00",
                .lit_y = "173.849428",
                .lit_quot = "0",
                .lit_rem = "0.000000",
            },
            {
                .lit_x = "-13.4",
                .lit_y = "-123456789.0123",
                .lit_quot = "0",
                .lit_rem = "-13.4",
            },
            {
                .lit_x = "-13.4",
                .lit_y = "123456789.0123",
                .lit_quot = "-1",
                .lit_rem = "123456775.6123",
            },
            {
                .lit_x = "13.4",
                .lit_y = "-123456789.0123",
                .lit_quot = "-1",
                .lit_rem = "-123456775.6123",
            },
            {
                .lit_x = "123456789.0123",
                .lit_y = "100.39",
                .lit_quot = "1229771",
                .lit_rem = "78.3223",
            },
            {
                .lit_x = "123456789.0123",
                .lit_y = "-100.39",
                .lit_quot = "-1229772",
                .lit_rem = "-22.0677",
            },
            {
                .lit_x = "-123456789.0123",
                .lit_y = "100.39",
                .lit_quot = "-1229772",
                .lit_rem = "22.0677",
            },
            {
                .lit_x = "-123456789.0123",
                .lit_y = "-100.39",
                .lit_quot = "1229771",
                .lit_rem = "-78.3223",
            },
            {
                .lit_x = "17.358",
                .lit_y = "17.358",
                .lit_quot = "1",
                .lit_rem = "0",
            },
            {
                .lit_x = "-17.358",
                .lit_y = "17.358",
                .lit_quot = "-1",
                .lit_rem = "0",
            },
            {
                .lit_x = "17.358",
                .lit_y = "-17.358",
                .lit_quot = "-1",
                .lit_rem = "0",
            },
            {
                .lit_x = "-17.358",
                .lit_y = "-17.358",
                .lit_quot = "1",
                .lit_rem = "0",
            },
            {
                .lit_x = "10000000000000000000.5",
                .lit_y = "-0.03",
                .lit_quot = "-333333333333333333350",
                .lit_rem = "0.00",
            },
            {
                .lit_x = "1235931852938539958205",
                .lit_y = "313594649253062377455",
                .lit_quot = "3",
                .lit_rem = "295147905179352825840",
            },
            {
                .lit_x = "1235931852938539958272",
                .lit_y = "313594649253062377472",
                .lit_quot = "3",
                .lit_rem = "295147905179352825856",
            },
        };

        for (const auto &test : tests) {

            const std::string section_name = test.lit_x + " /% " + test
                .lit_y;

            SECTION(section_name) {
                do_divmod_test(tv, test);
            }
        }
    }

    SECTION("shint /% shint -> dyn, shint") {

        const struct divmod_test_variant tv = {
            .dyn_x = false,
            .dyn_y = false,
            .dyn_quot = true,
            .dyn_rem = false,
        };

        struct divmod_test_data tests[] = {
            {
                .lit_x = "1000000000000000000000000005",
                .lit_y = "-0.01",
                .lit_quot = "-100000000000000000000000000500",
                .lit_rem = "0.00",
            },
            {
                .lit_x = "10000000000000000000000005.06",
                .lit_y = "-0.00002",
                .lit_quot = "-500000000000000000000000253000",
                .lit_rem = "0.00000",
            },
        };

        for (const auto &test : tests) {

            const std::string section_name = test.lit_x + " /% " + test.lit_y;

            SECTION(section_name) {
                do_divmod_test(tv, test);
            }
        }
    }

    SECTION("shint /% dyn -> dyn, shint") {

        const struct divmod_test_variant tv = {
            .dyn_x = false,
            .dyn_y = true,
            .dyn_quot = true,
            .dyn_rem = false,
        };

        struct divmod_test_data tests[] = {
            {
                .lit_x = "80000000000000000005",
                .lit_y = "-0.0000000001",
                .lit_quot = "-800000000000000000050000000000",
                .lit_rem = "0",
            },
            {
                .lit_x = "100000000000000000005.06",
                .lit_y = "-0.00000000002",
                .lit_quot = "-5000000000000000000253000000000",
                .lit_rem = "0",
            },
        };

        for (const auto &test : tests) {

            const std::string section_name = test.lit_x + " /% " + test.lit_y;

            SECTION(section_name) {
                do_divmod_test(tv, test);
            }
        }
    }

    SECTION("shint /% dyn -> shint, dyn") {

        const struct divmod_test_variant tv = {
            .dyn_x = false,
            .dyn_y = true,
            .dyn_quot = false,
            .dyn_rem = true,
        };

        struct divmod_test_data tests[] = {
            {
                .lit_x = "80000000000000000005",
                .lit_y = "-4.0003000001",
                .lit_quot = "-19998500111991638126",
                .lit_rem = "-1.6369638126",
            },
            {
                .lit_x = "-70005.062",
                .lit_y = "33.00000000002",
                .lit_quot = "-2122",
                .lit_rem = "20.93800004244",
            },
        };

        for (const auto &test : tests) {

            const std::string section_name = test.lit_x + " /% " + test.lit_y;

            SECTION(section_name) {
                do_divmod_test(tv, test);
            }
        }
    }

    SECTION("dyn /% dyn -> shint, dyn") {

        const struct divmod_test_variant tv = {
            .dyn_x = true,
            .dyn_y = true,
            .dyn_quot = false,
            .dyn_rem = true,
        };

        struct divmod_test_data tests[] = {
            {
                .lit_x = "8000000000.0000000005",
                .lit_y = "-4.0003000001",
                .lit_quot = "-1999850012",
                .lit_rem = "-3.2035850007",
            },
            {
                .lit_x = "-3942886002727763569672998513142755570005"
                         ".6623753387100284387111578339445",
                .lit_y = "82395454444289831743300580.002",
                .lit_quot = "-47853198083806",
                .lit_rem = "51173284002954286839233641"
                           ".9496246612899715612888421660555",
            },
        };

        for (const auto &test : tests) {

            const std::string section_name = test.lit_x + " /% " + test.lit_y;

            SECTION(section_name) {
                do_divmod_test(tv, test);
            }
        }
    }

    SECTION("dyn /% dyn -> shint, shint") {

        const struct divmod_test_variant tv = {
            .dyn_x = true,
            .dyn_y = true,
            .dyn_quot = false,
            .dyn_rem = false,
        };

        struct divmod_test_data tests[] = {
            {
                .lit_x = "79965997001999000000000000000000089.0004",
                .lit_y = "-39982998500999500000000000000000.0446",
                .lit_quot = "-2000",
                .lit_rem = "-0.1996",
            },
            {
                .lit_x = "-39428860027277635768970151219636620948297"
                         ".06659184",
                .lit_y = "-80239545444428983174330.0580002",
                .lit_quot = "491389374265394399",
                .lit_rem = "-222153261802213102898.84571204",
            },
        };

        for (const auto &test : tests) {

            const std::string section_name = test.lit_x + " /% " + test.lit_y;

            SECTION(section_name) {
                do_divmod_test(tv, test);
            }
        }
    }

    SECTION("dyn /% dyn -> dyn, dyn") {

        const struct divmod_test_variant tv = {
            .dyn_x = true,
            .dyn_y = true,
            .dyn_quot = true,
            .dyn_rem = true,
        };

        struct divmod_test_data tests[] = {
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
                .lit_quot = "9999999999999999999999999999999999999999"
                            "9999999999999999999999999999999999999999"
                            "9999999999999999999999999999999999999999"
                            "9999999999999999999999999999999999999999"
                            "9999999999999999999999999999999999999999"
                            "9999999999999999999999999999999999999999"
                            "9999999999999999999999999999999999999999"
                            "9999999999999999",
                .lit_rem = "1111111111111111111111111111111111111111"
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
                           "1111111107474747474747474747474747522222"
                           "2222222222222222222222222222222222222222"
                           "2222222222222222222222222222222222222222"
                           "2222222222222222222222222222222222222222"
                           "2222222222222222222222222222222222222222"
                           "2222222222222222222222222222222222222222"
                           "2222222222222222222222222222222222222222"
                           "2222222222222222222222222.91919191919191"
                           "9191919191914444444",
            },
            //{
            //    .lit_x = "-39428860027277635768970151219636620948297"
            //             ".06659184",
            //    .lit_y = "-80239545444428983174330.0580002",
            //    .lit_quot = "491389374265394399",
            //    .lit_rem = "-222153261802213102898.84571204",
            //},
        };

        for (const auto &test : tests) {

            const std::string section_name = test.lit_x + " /% " + test.lit_y;

            SECTION(section_name) {
                do_divmod_test(tv, test);
            }
        }
    }
}
