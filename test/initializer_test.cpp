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
is_shint(fpdec_t *fpdec) {
    return (fpdec->dyn_alloc == 0 && fpdec->normalized == 0);
}

bool
is_digit_array(fpdec_t *fpdec) {
    return (fpdec->dyn_alloc == 1 && fpdec->normalized == 1);
}

bool check_normalized(fpdec_t *fpdec) {
    fpdec_digit_array_t *digit_array = fpdec->digit_array;
    fpdec_n_digits_t n_signif = digit_array->n_signif;
    return n_signif == 0 ||
           (digit_array->digits[0] != 0 &&
            digit_array->digits[n_signif] != 0);
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
        uint64_t res_digits[6][2] = {
                {192683UL,               0UL},
                {12345678901234567890UL, 0UL},
                {8558702606396361426UL,  4UL},
                {12899172069043863552UL, 3794707603UL},
                {0UL,                    0UL},
                {0UL,                    0UL}
        };

        for (int i = 0; i < 6; ++i) {
            fpdec_t fpdec;
            int rc = fpdec_from_asci_literal(&fpdec, literals[i].c_str());

            REQUIRE(rc == 0);
            REQUIRE(is_shint(&fpdec));
            REQUIRE(fpdec.sign == res_sign[i]);
            REQUIRE(fpdec.dec_prec == res_prec[i]);
            REQUIRE(fpdec.lo == res_digits[i][0]);
            REQUIRE(fpdec.hi == res_digits[i][1]);
        }
    }

    SECTION("Coeff > MAX_N_DEC_DIGITS_IN_SHINT") {

    }

    SECTION("Invalid input") {
        std::string literals[7] = {
                " 1.23.5", "1.24e", "--4.92", "", "   ", "3,49E-3",
                "\t+   \r\n"
        };
        for (const auto &literal : literals) {
            fpdec_t fpdec;

            SECTION(literal) {
                REQUIRE(fpdec_from_asci_literal(&fpdec, literal.c_str()) ==
                        FPDEC_INVALID_DECIMAL_LITERAL);
            }
        }
    }
}