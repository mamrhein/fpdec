/* ---------------------------------------------------------------------------
Name:        compare_test.cpp

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
#include <vector>

#include "catch.hpp"
#include "helper_macros.h"
#include "fpdec.h"
#include "checks.hpp"


TEST_CASE("Compare") {

    char buf[255];

    SECTION("Respect sign") {

        std::vector<std::string> literals = {
                "-5.30951e42",
                "-12345678901234567890.12345",
                "-0.28364e-7",
                "-0.28363e-7",
                "0.00",
                "1e-83",
                "1",
                "12345678901234567890.12345",
                "5.30951e42",
                "5.309511e42",
        };
        error_t rc;

        for (int i = 0; i < literals.size(); ++i) {
            fpdec_t x = FPDEC_ZERO;
            rc = fpdec_from_ascii_literal(&x, literals[i].c_str());
            REQUIRE(rc == FPDEC_OK);

            for (int j = 0; j < literals.size(); ++j) {
                fpdec_t y = FPDEC_ZERO;
                rc = fpdec_from_ascii_literal(&y, literals[j].c_str());
                REQUIRE(rc == FPDEC_OK);

                snprintf((char *) (&buf), 255, "%s ? %s", literals[i].c_str(),
                         literals[j].c_str());
                SECTION(buf) {
                    CHECK(fpdec_compare(&x, &y, false) == CMP(i, j));
                }
                fpdec_reset_to_zero(&y, 0);
            }
            fpdec_reset_to_zero(&x, 0);
        }
    }

    SECTION("Ignore sign") {

        std::vector<std::string> literals = {
                "0.00",
                "1e-83",
                "-0.28363e-7",
                "-0.28364e-7",
                "1",
                "-12345678901234567890.12345",
                "5.30951e42",
                "-5.309511e42",
        };
        error_t rc;

        for (int i = 0; i < literals.size(); ++i) {
            fpdec_t x = FPDEC_ZERO;
            rc = fpdec_from_ascii_literal(&x, literals[i].c_str());
            REQUIRE(rc == FPDEC_OK);

            for (int j = 0; j < literals.size(); ++j) {
                fpdec_t y = FPDEC_ZERO;
                rc = fpdec_from_ascii_literal(&y, literals[j].c_str());
                REQUIRE(rc == FPDEC_OK);

                snprintf((char *) (&buf), 255, "%s ? %s", literals[i].c_str(),
                         literals[j].c_str());
                SECTION(buf) {
                    CHECK(fpdec_compare(&x, &y, true) == CMP(i, j));
                }
                fpdec_reset_to_zero(&y, 0);
            }
            fpdec_reset_to_zero(&x, 0);
        }
    }
}
