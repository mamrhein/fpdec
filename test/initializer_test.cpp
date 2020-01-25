/* ---------------------------------------------------------------------------
Name:        initializer_test.cpp

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


bool
check_shint(fpdec_t *fpdec) {
    return (fpdec->dyn_alloc == 0 && fpdec->normalized == 0);
}

TEST_CASE("Initialize from string") {

    SECTION("Coeff <= MAX_N_DEC_DIGITS_IN_SHINT") {
        std::string literals[6] = {
                "  1926.83 \n",
                "-12345678901234567890e-7",
                "82345678901234567890e-12",
                "+007e28",
                "   +0.00e-0",
                "-000000",
        };
        fpdec_sign_t res_sign[6] = {1, -1, 1, 1, 0, 0};
        fpdec_dec_prec_t res_prec[6] = {2, 7, 12, 0, 2, 0};

        for (int i = 0; i < 6; ++i) {
            fpdec_t fpdec;
            int rc = fpdec_from_asci_literal(&fpdec, literals[i].c_str());

            REQUIRE(rc == 0);
            REQUIRE(check_shint(&fpdec));
            REQUIRE(fpdec.sign == res_sign[i]);
            REQUIRE(fpdec.dec_prec == res_prec[i]);
        }
    }

    SECTION("Coeff > MAX_N_DEC_DIGITS_IN_SHINT") {

    }

    SECTION("Invalid input") {

    }
}